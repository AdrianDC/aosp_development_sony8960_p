#include "AST.h"

#include "Coordinator.h"
#include "Formatter.h"
#include "FQName.h"
#include "HandleType.h"
#include "Interface.h"
#include "Scope.h"
#include "TypeDef.h"

#include <android-base/logging.h>
#include <iostream>
#include <stdlib.h>

namespace android {

AST::AST(Coordinator *coordinator, const std::string &path)
    : mCoordinator(coordinator),
      mPath(path),
      mScanner(NULL),
      mRootScope(new Scope("" /* localName */)) {
    enterScope(mRootScope);
}

AST::~AST() {
    delete mRootScope;
    mRootScope = NULL;

    CHECK(mScanner == NULL);

    // Ownership of "coordinator" was NOT transferred.
}

void *AST::scanner() {
    return mScanner;
}

void AST::setScanner(void *scanner) {
    mScanner = scanner;
}

const std::string &AST::getFilename() const {
    return mPath;
}

bool AST::setPackage(const char *package) {
    mPackage.setTo(package);
    CHECK(mPackage.isValid());

    if (mPackage.package().empty()
            || mPackage.version().empty()
            || !mPackage.name().empty()) {
        return false;
    }

    return true;
}

FQName AST::package() const {
    return mPackage;
}

bool AST::isInterface(std::string *ifaceName) const {
    return mRootScope->containsSingleInterface(ifaceName);
}

bool AST::addImport(const char *import) {
    FQName fqName(import);
    CHECK(fqName.isValid());

    fqName.applyDefaults(mPackage.package(), mPackage.version());

    // LOG(INFO) << "importing " << fqName.string();

    if (fqName.name().empty()) {
        std::vector<FQName> packageInterfaces;

        status_t err =
            mCoordinator->appendPackageInterfacesToSet(fqName,
                                                       &packageInterfaces);

        if (err != OK) {
            return false;
        }

        for (const auto &subFQName : packageInterfaces) {
            AST *ast = mCoordinator->parse(subFQName, &mImportedASTs);
            if (ast == NULL) {
                return false;
            }
        }

        return true;
    }

    AST *importAST = mCoordinator->parse(fqName, &mImportedASTs);

    if (importAST == NULL) {
        return false;
    }

    return true;
}

void AST::addImportedAST(AST *ast) {
    mImportedASTs.insert(ast);
}

void AST::enterScope(Scope *container) {
    mScopePath.push_back(container);
}

void AST::leaveScope() {
    mScopePath.pop();
}

Scope *AST::scope() {
    CHECK(!mScopePath.empty());
    return mScopePath.top();
}

bool AST::addTypeDef(
        const char *localName, Type *type, std::string *errorMsg) {
    // The reason we wrap the given type in a TypeDef is simply to suppress
    // emitting any type definitions later on, since this is just an alias
    // to a type defined elsewhere.
    return addScopedTypeInternal(
            localName, new TypeDef(type), errorMsg, true /* isTypeDef */);
}

bool AST::addScopedType(NamedType *type, std::string *errorMsg) {
    return addScopedTypeInternal(
            type->localName().c_str(), type, errorMsg, false /* isTypeDef */);
}

bool AST::addScopedTypeInternal(
        const char *localName,
        Type *type,
        std::string *errorMsg,
        bool isTypeDef) {
    if (!isTypeDef) {
        // Resolve typeDefs to the target type.
        while (type->isTypeDef()) {
            type = static_cast<TypeDef *>(type)->referencedType();
        }
    }

    // LOG(INFO) << "adding scoped type '" << localName << "'";

    bool success = scope()->addType(localName, type,  errorMsg);
    if (!success) {
        return false;
    }

    std::string path;
    for (size_t i = 1; i < mScopePath.size(); ++i) {
        path.append(mScopePath[i]->localName());
        path.append(".");
    }
    path.append(localName);

    FQName fqName(mPackage.package(), mPackage.version(), path);

    if (!isTypeDef) {
        CHECK(type->isNamedType());

        NamedType *namedType = static_cast<NamedType *>(type);
        namedType->setFullName(fqName);
    }

    mDefinedTypesByFullName.add(fqName, type);

    return true;
}

Type *AST::lookupType(const char *name) {
    FQName fqName(name);
    CHECK(fqName.isValid());

    if (fqName.name().empty()) {
        // Given a package and version???
        return NULL;
    }

    if (fqName.package().empty() && fqName.version().empty()) {
        // This is just a plain identifier, resolve locally first if possible.

        for (size_t i = mScopePath.size(); i-- > 0;) {
            Type *type = mScopePath[i]->lookupType(name);

            if (type != NULL) {
                // Resolve typeDefs to the target type.
                while (type->isTypeDef()) {
                    type = static_cast<TypeDef *>(type)->referencedType();
                }

                return type->ref();
            }
        }
    }

    Type *resolvedType = nullptr;
    FQName resolvedName;

    for (const auto &importedAST : mImportedASTs) {
        FQName matchingName;
        Type *match = importedAST->findDefinedType(fqName, &matchingName);

        if (match != nullptr) {
            if (resolvedType != nullptr) {
                std::cerr << "ERROR: Unable to resolve type name '"
                          << fqName.string()
                          << "', multiple matches found:\n";

                std::cerr << "  " << resolvedName.string() << "\n";
                std::cerr << "  " << matchingName.string() << "\n";

                return NULL;
            }

            resolvedType = match;
            resolvedName = matchingName;

            // Keep going even after finding a match.
        }
    }

    if (resolvedType) {
#if 0
        LOG(INFO) << "found '"
                  << resolvedName.string()
                  << "' after looking for '"
                  << fqName.string()
                  << "'.";
#endif

        // Resolve typeDefs to the target type.
        while (resolvedType->isTypeDef()) {
            resolvedType =
                static_cast<TypeDef *>(resolvedType)->referencedType();
        }

        if (!resolvedType->isInterface()) {
            // Non-interface types are declared in the associated types header.
            FQName typesName(
                    resolvedName.package(), resolvedName.version(), "types");

            mImportedNames.insert(typesName);
        } else {
            // Do _not_ use fqName, i.e. the name we used to look up the type,
            // but instead use the name of the interface we found.
            // This is necessary because if fqName pointed to a typedef which
            // in turn referenced the found interface we'd mistakenly use the
            // name of the typedef instead of the proper name of the interface.

            mImportedNames.insert(
                    static_cast<Interface *>(resolvedType)->fqName());
        }
    }

    return resolvedType->ref();
}

Type *AST::findDefinedType(const FQName &fqName, FQName *matchingName) const {
    for (size_t i = 0; i < mDefinedTypesByFullName.size(); ++i) {
        const FQName &key = mDefinedTypesByFullName.keyAt(i);

        if (key.endsWith(fqName)) {
            *matchingName = key;
            return mDefinedTypesByFullName.valueAt(i);
        }
    }

    return nullptr;
}

void AST::getImportedPackages(std::set<FQName> *importSet) const {
    for (const auto &fqName : mImportedNames) {
        FQName packageName(fqName.package(), fqName.version(), "");

        if (packageName == mPackage) {
            // We only care about external imports, not our own package.
            continue;
        }

        importSet->insert(packageName);
    }
}

}  // namespace android;
