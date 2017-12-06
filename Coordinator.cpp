/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Coordinator.h"

#include <dirent.h>
#include <sys/stat.h>

#include <algorithm>
#include <iterator>

#include <android-base/logging.h>
#include <hidl-hash/Hash.h>
#include <hidl-util/StringHelper.h>
#include <iostream>

#include "AST.h"
#include "Interface.h"

extern android::status_t parseFile(android::AST *ast);

static bool existdir(const char *name) {
    DIR *dir = opendir(name);
    if (dir == NULL) {
        return false;
    }
    closedir(dir);
    return true;
}

namespace android {

const std::string &Coordinator::getRootPath() const {
    return mRootPath;
}

void Coordinator::setRootPath(const std::string &rootPath) {
    mRootPath = rootPath;

    if (!mRootPath.empty() && !StringHelper::EndsWith(mRootPath, "/")) {
        mRootPath += "/";
    }
}

void Coordinator::setVerbose(bool verbose) {
    mVerbose = verbose;
}

bool Coordinator::isVerbose() const {
    return mVerbose;
}

const std::string& Coordinator::getOwner() const {
    return mOwner;
}
void Coordinator::setOwner(const std::string& owner) {
    mOwner = owner;
}

status_t Coordinator::addPackagePath(const std::string& root, const std::string& path, std::string* error) {
    FQName package = FQName(root, "0.0", "");
    for (const PackageRoot &packageRoot : mPackageRoots) {
        if (packageRoot.root.inPackage(root) || package.inPackage(packageRoot.root.package())) {
            if (error != nullptr) {
                *error = "ERROR: conflicting package roots " +
                         packageRoot.root.package() +
                         " and " +
                         root;
            }

            return UNKNOWN_ERROR;
        }
    }

    mPackageRoots.push_back({path, package});
    return OK;
}
void Coordinator::addDefaultPackagePath(const std::string& root, const std::string& path) {
    addPackagePath(root, path, nullptr /* error */);
}

Formatter Coordinator::getFormatter(const std::string& outputPath, const FQName& fqName,
                                    Location location, const std::string& fileName) const {
    std::string filepath = getFilepath(outputPath, fqName, location, fileName);

    onFileAccess(filepath, "w");

    if (!Coordinator::MakeParentHierarchy(filepath)) {
        fprintf(stderr, "ERROR: could not make directories for %s.\n", filepath.c_str());
        return Formatter::invalid();
    }

    FILE* file = fopen(filepath.c_str(), "w");

    if (file == nullptr) {
        fprintf(stderr, "ERROR: could not open file %s: %d\n", filepath.c_str(), errno);
        return Formatter::invalid();
    }

    return Formatter(file);
}

std::string Coordinator::getFilepath(const std::string& outputPath, const FQName& fqName,
                                     Location location, const std::string& fileName) const {
    std::string path = outputPath;

    switch (location) {
        case Location::DIRECT: { /* nothing */
        } break;
        case Location::PACKAGE_ROOT: {
            path.append(getPackagePath(fqName, false /* relative */));
        } break;
        case Location::GEN_OUTPUT: {
            path.append(convertPackageRootToPath(fqName));
            path.append(getPackagePath(fqName, true /* relative */, false /* sanitized */));
        } break;
        case Location::GEN_SANITIZED: {
            path.append(convertPackageRootToPath(fqName));
            path.append(getPackagePath(fqName, true /* relative */, true /* sanitized */));
        } break;
        default: { CHECK(false) << "Invalid location: " << static_cast<size_t>(location); }
    }

    path.append(fileName);
    return path;
}

void Coordinator::onFileAccess(const std::string& path, const std::string& mode) const {
    if (!mVerbose) {
        return;
    }

    fprintf(stderr,
            "VERBOSE: file access %s %s\n", path.c_str(), mode.c_str());
}


AST* Coordinator::parse(const FQName& fqName, std::set<AST*>* parsedASTs,
                        Enforce enforcement) const {
    CHECK(fqName.isFullyQualified());

    auto it = mCache.find(fqName);
    if (it != mCache.end()) {
        AST *ast = (*it).second;

        if (ast != nullptr && parsedASTs != nullptr) {
            parsedASTs->insert(ast);
        }

        return ast;
    }

    // Add this to the cache immediately, so we can discover circular imports.
    mCache[fqName] = nullptr;

    AST *typesAST = nullptr;

    if (fqName.name() != "types") {
        // Any interface file implicitly imports its package's types.hal.
        FQName typesName = fqName.getTypesForPackage();
        // Do not enforce on imports. Do not add imports' imports to this AST.
        typesAST = parse(typesName, nullptr, Enforce::NONE);

        // fall through.
    }

    std::string path = makeAbsolute(getPackagePath(fqName));

    path.append(fqName.name());
    path.append(".hal");

    AST *ast = new AST(this, path);

    if (typesAST != NULL) {
        // If types.hal for this AST's package existed, make it's defined
        // types available to the (about to be parsed) AST right away.
        ast->addImportedAST(typesAST);
    }

    onFileAccess(ast->getFilename(), "r");
    if (parseFile(ast) != OK || ast->postParse() != OK) {
        delete ast;
        ast = nullptr;

        return nullptr;
    }

    status_t err = OK;
    if (ast->package().package() != fqName.package()
            || ast->package().version() != fqName.version()) {
        fprintf(stderr,
                "ERROR: File at '%s' does not match expected package and/or "
                "version.\n",
                path.c_str());

        err = UNKNOWN_ERROR;
    } else {
        if (ast->isInterface()) {
            if (fqName.name() == "types") {
                fprintf(stderr,
                        "ERROR: File at '%s' declares an interface '%s' "
                        "instead of the expected types common to the package.\n",
                        path.c_str(),
                        ast->getInterface()->localName().c_str());

                err = UNKNOWN_ERROR;
            } else if (ast->getInterface()->localName() != fqName.name()) {
                fprintf(stderr,
                        "ERROR: File at '%s' does not declare interface type "
                        "'%s'.\n",
                        path.c_str(),
                        fqName.name().c_str());

                err = UNKNOWN_ERROR;
            }
        } else if (fqName.name() != "types") {
            fprintf(stderr,
                    "ERROR: File at '%s' declares types rather than the "
                    "expected interface type '%s'.\n",
                    path.c_str(),
                    fqName.name().c_str());

            err = UNKNOWN_ERROR;
        } else if (ast->containsInterfaces()) {
            fprintf(stderr,
                    "ERROR: types.hal file at '%s' declares at least one "
                    "interface type.\n",
                    path.c_str());

            err = UNKNOWN_ERROR;
        }
    }

    if (err != OK) {
        delete ast;
        ast = nullptr;

        return nullptr;
    }

    if (parsedASTs != nullptr) { parsedASTs->insert(ast); }

    // put it into the cache now, so that enforceRestrictionsOnPackage can
    // parse fqName.
    mCache[fqName] = ast;

    // For each .hal file that hidl-gen parses, the whole package will be checked.
    err = enforceRestrictionsOnPackage(fqName, enforcement);
    if (err != OK) {
        mCache[fqName] = nullptr;
        delete ast;
        ast = nullptr;
        return nullptr;
    }

    return ast;
}

const Coordinator::PackageRoot &Coordinator::findPackageRoot(const FQName &fqName) const {
    CHECK(!fqName.package().empty());

    // Find the right package prefix and path for this FQName.  For
    // example, if FQName is "android.hardware.nfc@1.0::INfc", and the
    // prefix:root is set to [ "android.hardware:hardware/interfaces",
    // "vendor.qcom.hardware:vendor/qcom"], then we will identify the
    // prefix "android.hardware" and the package root
    // "hardware/interfaces".

    auto ret = mPackageRoots.end();
    for (auto it = mPackageRoots.begin(); it != mPackageRoots.end(); it++) {
        if (!fqName.inPackage(it->root.package())) {
            continue;
        }

        CHECK(ret == mPackageRoots.end())
            << "Multiple package roots found for "<< fqName.string()
            << " (" << it->root.package() << " and "
            << ret->root.package() << ")";

        ret = it;
    }
    CHECK(ret != mPackageRoots.end())
        << "Unable to find package root for " << fqName.string();

    return *ret;
}

std::string Coordinator::makeAbsolute(const std::string& path) const {
    if (StringHelper::StartsWith(path, "/") || mRootPath.empty()) {
        return path;
    }

    return mRootPath + path;
}

std::string Coordinator::getPackageRoot(const FQName &fqName) const {
    return findPackageRoot(fqName).root.package();
}

std::string Coordinator::getPackageRootPath(const FQName &fqName) const {
    return findPackageRoot(fqName).path;
}

std::string Coordinator::getPackageRootOption(const FQName &fqName) const {
    return getPackageRoot(fqName) + ":" + getPackageRootPath(fqName);
}

std::string Coordinator::getPackagePath(
        const FQName& fqName, bool relative, bool sanitized) const {

    const PackageRoot& packageRoot = findPackageRoot(fqName);

    // Given FQName of "android.hardware.nfc.test@1.0::IFoo" and a prefix
    // "android.hardware", the suffix is "nfc.test".
    std::string suffix = StringHelper::LTrim(fqName.package(), packageRoot.root.package());
    suffix = StringHelper::LTrim(suffix, ".");

    std::vector<std::string> suffixComponents;
    StringHelper::SplitString(suffix, '.', &suffixComponents);

    std::vector<std::string> components;
    if (!relative) {
        components.push_back(StringHelper::RTrimAll(packageRoot.path, "/"));
    }
    components.insert(components.end(), suffixComponents.begin(), suffixComponents.end());
    components.push_back(sanitized ? fqName.sanitizedVersion() : fqName.version());

    return StringHelper::JoinStrings(components, "/") + "/";
}

status_t Coordinator::getPackageInterfaceFiles(
        const FQName &package,
        std::vector<std::string> *fileNames) const {
    fileNames->clear();

    const std::string packagePath = makeAbsolute(getPackagePath(package));

    DIR *dir = opendir(packagePath.c_str());

    if (dir == NULL) {
        fprintf(stderr,
                "ERROR: Could not open package path %s for package %s:\n%s\n",
                getPackagePath(package).c_str(),
                package.string().c_str(),
                packagePath.c_str());
        return -errno;
    }

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type != DT_REG) {
            continue;
        }

        const auto suffix = ".hal";
        const auto suffix_len = std::strlen(suffix);
        const auto d_namelen = strlen(ent->d_name);

        if (d_namelen < suffix_len
                || strcmp(ent->d_name + d_namelen - suffix_len, suffix)) {
            continue;
        }

        fileNames->push_back(std::string(ent->d_name, d_namelen - suffix_len));
    }

    closedir(dir);
    dir = NULL;

    std::sort(fileNames->begin(), fileNames->end(),
              [](const std::string& lhs, const std::string& rhs) -> bool {
                  if (lhs == "types") {
                      return true;
                  }
                  if (rhs == "types") {
                      return false;
                  }
                  return lhs < rhs;
              });

    return OK;
}

status_t Coordinator::appendPackageInterfacesToVector(
        const FQName &package,
        std::vector<FQName> *packageInterfaces) const {
    packageInterfaces->clear();

    std::vector<std::string> fileNames;
    status_t err = getPackageInterfaceFiles(package, &fileNames);

    if (err != OK) {
        return err;
    }

    for (const auto &fileName : fileNames) {
        FQName subFQName(
                package.package() + package.atVersion() + "::" + fileName);

        if (!subFQName.isValid()) {
            std::cerr << "ERROR: Whole-package import encountered invalid filename '" << fileName
                      << "' in package " << package.package() << package.atVersion() << std::endl;

            return UNKNOWN_ERROR;
        }

        packageInterfaces->push_back(subFQName);
    }

    return OK;
}

std::string Coordinator::convertPackageRootToPath(const FQName &fqName) const {
    std::string packageRoot = getPackageRoot(fqName);

    if (*(packageRoot.end()--) != '.') {
        packageRoot += '.';
    }

    std::replace(packageRoot.begin(), packageRoot.end(), '.', '/');

    return packageRoot; // now converted to a path
}

status_t Coordinator::isTypesOnlyPackage(const FQName& package, bool* result) const {
    std::vector<FQName> packageInterfaces;

    status_t err = appendPackageInterfacesToVector(package, &packageInterfaces);

    if (err != OK) {
        *result = false;
        return err;
    }

    *result = packageInterfaces.size() == 1 && packageInterfaces[0].name() == "types";
    return OK;
}

status_t Coordinator::enforceRestrictionsOnPackage(const FQName& fqName,
                                                   Enforce enforcement) const {
    CHECK(enforcement == Enforce::FULL || enforcement == Enforce::NO_HASH ||
          enforcement == Enforce::NONE);

    // need fqName to be something like android.hardware.foo@1.0.
    // name and valueName is ignored.
    if (fqName.package().empty() || fqName.version().empty()) {
        std::cerr << "ERROR: Cannot enforce restrictions on package " << fqName.string()
                  << ": package or version is missing." << std::endl;
        return BAD_VALUE;
    }

    if (enforcement == Enforce::NONE) {
        return OK;
    }

    FQName package = fqName.getPackageAndVersion();
    // look up cache.
    if (mPackagesEnforced.find(package) != mPackagesEnforced.end()) {
        return OK;
    }

    // enforce all rules.
    status_t err;

    err = enforceMinorVersionUprevs(package);
    if (err != OK) {
        return err;
    }

    if (enforcement != Enforce::NO_HASH) {
        err = enforceHashes(package);
        if (err != OK) {
            return err;
        }
    }

    // cache it so that it won't need to be enforced again.
    mPackagesEnforced.insert(package);
    return OK;
}

status_t Coordinator::enforceMinorVersionUprevs(const FQName &currentPackage) const {
    if(!currentPackage.hasVersion()) {
        std::cerr << "ERROR: Cannot enforce minor version uprevs for " << currentPackage.string()
                  << ": missing version." << std::endl;
        return UNKNOWN_ERROR;
    }

    if (currentPackage.getPackageMinorVersion() == 0) {
        return OK; // ignore for @x.0
    }

    bool hasPrevPackage = false;
    FQName prevPackage = currentPackage;
    while (prevPackage.getPackageMinorVersion() > 0) {
        prevPackage = prevPackage.downRev();
        const std::string prevPackagePath = makeAbsolute(getPackagePath(prevPackage));
        if (existdir(prevPackagePath.c_str())) {
            hasPrevPackage = true;
            break;
        }
    }
    if (!hasPrevPackage) {
        // no @x.z, where z < y, exist.
        return OK;
    }

    if (prevPackage != currentPackage.downRev()) {
        std::cerr << "ERROR: Cannot enforce minor version uprevs for " << currentPackage.string()
                  << ": Found package " << prevPackage.string() << " but missing "
                  << currentPackage.downRev().string() << "; you cannot skip a minor version."
                  << std::endl;
        return UNKNOWN_ERROR;
    }

    bool prevIsTypesOnly;
    status_t err = isTypesOnlyPackage(prevPackage, &prevIsTypesOnly);
    if (err != OK) return err;

    if (prevIsTypesOnly) {
        // A types only package can be extended in any way.
        return OK;
    }

    std::vector<FQName> packageInterfaces;
    err = appendPackageInterfacesToVector(currentPackage, &packageInterfaces);
    if (err != OK) {
        return err;
    }

    bool extendedInterface = false;
    for (const FQName &currentFQName : packageInterfaces) {
        if (currentFQName.name() == "types") {
            continue; // ignore types.hal
        }

        const Interface *iface = nullptr;
        AST *currentAST = parse(currentFQName);
        if (currentAST != nullptr) {
            iface = currentAST->getInterface();
        }
        if (iface == nullptr) {
            if (currentAST == nullptr) {
                std::cerr << "WARNING: Skipping " << currentFQName.string()
                          << " because it could not be found or parsed"
                          << " or " << currentPackage.string() << " doesn't pass all requirements."
                          << std::endl;
            } else {
                std::cerr << "WARNING: Skipping " << currentFQName.string()
                          << " because the file might contain more than one interface."
                          << std::endl;
            }
            continue;
        }

        if (iface->superType() == nullptr) {
            CHECK(iface->isIBase());
            continue;
        }

        // Assume that currentFQName == android.hardware.foo@2.2::IFoo.
        FQName lastFQName(prevPackage.package(), prevPackage.version(),
                currentFQName.name());
        AST *lastAST = parse(lastFQName);

        for (; lastFQName.getPackageMinorVersion() > 0 &&
               (lastAST == nullptr || lastAST->getInterface() == nullptr)
             ; lastFQName = lastFQName.downRev(), lastAST = parse(lastFQName)) {
            // nothing
        }

        // Then lastFQName == android.hardware.foo@2.1::IFoo or
        //      lastFQName == android.hardware.foo@2.0::IFoo if 2.1 doesn't exist.

        bool lastFQNameExists = lastAST != nullptr && lastAST->getInterface() != nullptr;

        if (!lastFQNameExists) {
            continue;
        }

        if (iface->superType()->fqName() != lastFQName) {
            std::cerr << "ERROR: Cannot enforce minor version uprevs for "
                      << currentPackage.string() << ": " << iface->fqName().string() << " extends "
                      << iface->superType()->fqName().string()
                      << ", which is not allowed. It must extend " << lastFQName.string()
                      << std::endl;
            return UNKNOWN_ERROR;
        }

        // at least one interface must extend the previous version
        // @2.0::IFoo does not work. It must be @2.1::IFoo for at least one interface.
        if (lastFQName.getPackageAndVersion() == prevPackage.getPackageAndVersion()) {
            extendedInterface = true;
        }

        if (mVerbose) {
            std::cout << "VERBOSE: EnforceMinorVersionUprevs: " << currentFQName.string()
                      << " passes." << std::endl;
        }
    }

    if (!extendedInterface) {
        // No interface extends the interface with the same name in @x.(y-1).
        std::cerr << "ERROR: " << currentPackage.string()
                  << " doesn't pass minor version uprev requirement. "
                  << "Requires at least one interface to extend an interface with the same name "
                  << "from " << prevPackage.string() << "." << std::endl;
        return UNKNOWN_ERROR;
    }

    return OK;
}

status_t Coordinator::enforceHashes(const FQName &currentPackage) const {
    status_t err = OK;
    std::vector<FQName> packageInterfaces;
    err = appendPackageInterfacesToVector(currentPackage, &packageInterfaces);
    if (err != OK) {
        return err;
    }

    for (const FQName &currentFQName : packageInterfaces) {
        AST *ast = parse(currentFQName);

        if (ast == nullptr) {
            err = UNKNOWN_ERROR;
            continue;
        }

        std::string hashPath = makeAbsolute(getPackageRootPath(currentFQName)) + "/current.txt";
        std::string error;
        std::vector<std::string> frozen = Hash::lookupHash(hashPath, currentFQName.string(), &error);

        if (error.size() > 0) {
            std::cerr << "ERROR: " << error << std::endl;
            err = UNKNOWN_ERROR;
            continue;
        }

        // hash not defined, interface not frozen
        if (frozen.size() == 0) {
            // This ensures that it can be detected.
            Hash::clearHash(ast->getFilename());
            continue;
        }

        std::string currentHash = Hash::getHash(ast->getFilename()).hexString();

        if(std::find(frozen.begin(), frozen.end(), currentHash) == frozen.end()) {
            std::cerr << "ERROR: " << currentFQName.string() << " has hash " << currentHash
                      << " which does not match hash on record. This interface has "
                      << "been frozen. Do not change it!" << std::endl;
            err = UNKNOWN_ERROR;
            continue;
        }
    }

    return err;
}

bool Coordinator::MakeParentHierarchy(const std::string &path) {
    static const mode_t kMode = 0755;

    size_t start = 1;  // Ignore leading '/'
    size_t slashPos;
    while ((slashPos = path.find('/', start)) != std::string::npos) {
        std::string partial = path.substr(0, slashPos);

        struct stat st;
        if (stat(partial.c_str(), &st) < 0) {
            if (errno != ENOENT) {
                return false;
            }

            int res = mkdir(partial.c_str(), kMode);
            if (res < 0) {
                return false;
            }
        } else if (!S_ISDIR(st.st_mode)) {
            return false;
        }

        start = slashPos + 1;
    }

    return true;
}

}  // namespace android

