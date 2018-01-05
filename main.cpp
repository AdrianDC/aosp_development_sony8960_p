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

#include "AST.h"
#include "Coordinator.h"
#include "Scope.h"

#include <android-base/logging.h>
#include <hidl-hash/Hash.h>
#include <hidl-util/FQName.h>
#include <hidl-util/Formatter.h>
#include <hidl-util/StringHelper.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace android;

struct OutputHandler {
    std::string mKey;
    std::string mDescription;
    enum OutputMode {
        NEEDS_DIR,
        NEEDS_FILE,
        NEEDS_SRC, // for changes inside the source tree itself
        NOT_NEEDED
    } mOutputMode;

    const std::string& name() { return mKey; }
    const std::string& description() { return mDescription; }

    using ValidationFunction = std::function<bool(const FQName &, const std::string &language)>;
    using GenerationFunction = std::function<status_t(const FQName &fqName,
                                                      const char *hidl_gen,
                                                      Coordinator *coordinator,
                                                      const std::string &outputDir)>;

    ValidationFunction validate;
    GenerationFunction generate;
};

static status_t generateSourcesForFile(
        const FQName &fqName,
        const char *,
        Coordinator *coordinator,
        const std::string &outputDir,
        const std::string &lang) {
    CHECK(fqName.isFullyQualified());

    AST *ast;
    std::string limitToType;

    if (fqName.name().find("types.") == 0) {
        CHECK(lang == "java");  // Already verified in validate().

        limitToType = fqName.name().substr(strlen("types."));

        FQName typesName = fqName.getTypesForPackage();
        ast = coordinator->parse(typesName);
    } else {
        ast = coordinator->parse(fqName);
    }

    if (ast == NULL) {
        fprintf(stderr,
                "ERROR: Could not parse %s. Aborting.\n",
                fqName.string().c_str());

        return UNKNOWN_ERROR;
    }

    if (lang == "check") {
        return OK; // only parsing, not generating
    }
    if (lang == "c++") {
        return ast->generateCpp(outputDir);
    }
    if (lang == "c++-headers") {
        return ast->generateCppHeaders(outputDir);
    }
    if (lang == "c++-sources") {
        return ast->generateCppSources(outputDir);
    }
    if (lang == "c++-impl") {
        return ast->generateCppImpl(outputDir);
    }
    if (lang == "c++-impl-headers") {
        return ast->generateCppImplHeader(outputDir);
    }
    if (lang == "c++-impl-sources") {
        return ast->generateCppImplSource(outputDir);
    }
    if (lang == "c++-adapter") {
        return ast->generateCppAdapter(outputDir);
    }
    if (lang == "c++-adapter-headers") {
        return ast->generateCppAdapterHeader(outputDir);
    }
    if (lang == "c++-adapter-sources") {
        return ast->generateCppAdapterSource(outputDir);
    }
    if (lang == "java") {
        return ast->generateJava(outputDir, limitToType);
    }
    if (lang == "vts") {
        return ast->generateVts(outputDir);
    }
    // Unknown language.
    return UNKNOWN_ERROR;
}

static status_t dumpDefinedButUnreferencedTypeNames(
        const std::vector<FQName> &packageInterfaces,
        Coordinator *coordinator) {
    std::set<FQName> packageDefinedTypes;
    std::set<FQName> packageReferencedTypes;
    std::set<FQName> packageImportedTypes;
    for (const auto &fqName : packageInterfaces) {
        AST *ast = coordinator->parse(fqName);
        if (!ast) {
            std::cerr
                << "ERROR: Could not parse " << fqName.string() << ". Aborting."
                << std::endl;

            return UNKNOWN_ERROR;
        }

        ast->addDefinedTypes(&packageDefinedTypes);
        ast->addReferencedTypes(&packageReferencedTypes);
        ast->getAllImportedNamesGranular(&packageImportedTypes);
    }

#if 0
    for (const auto &fqName : packageDefinedTypes) {
        std::cout << "DEFINED: " << fqName.string() << std::endl;
    }

    for (const auto &fqName : packageImportedTypes) {
        std::cout << "IMPORTED: " << fqName.string() << std::endl;
    }

    for (const auto &fqName : packageReferencedTypes) {
        std::cout << "REFERENCED: " << fqName.string() << std::endl;
    }
#endif

    for (const auto &fqName : packageReferencedTypes) {
        packageDefinedTypes.erase(fqName);
        packageImportedTypes.erase(fqName);
    }

    // A package implicitly imports its own types, only track them in one set.
    for (const auto &fqName : packageDefinedTypes) {
        packageImportedTypes.erase(fqName);
    }

    for (const auto &fqName : packageDefinedTypes) {
        std::cerr
            << "VERBOSE: DEFINED-BUT-NOT-REFERENCED "
            << fqName.string()
            << std::endl;
    }

    for (const auto &fqName : packageImportedTypes) {
        std::cerr
            << "VERBOSE: IMPORTED-BUT-NOT-REFERENCED "
            << fqName.string()
            << std::endl;
    }

    return OK;
}

static status_t generateSourcesForPackage(
        const FQName &packageFQName,
        const char *hidl_gen,
        Coordinator *coordinator,
        const std::string &outputDir,
        const std::string &lang) {
    CHECK(packageFQName.isValid() &&
        !packageFQName.isFullyQualified() &&
        packageFQName.name().empty());

    std::vector<FQName> packageInterfaces;

    status_t err =
        coordinator->appendPackageInterfacesToVector(packageFQName,
                                                     &packageInterfaces);

    if (err != OK) {
        return err;
    }

    if (coordinator->isVerbose()) {
        err = dumpDefinedButUnreferencedTypeNames(
                packageInterfaces, coordinator);

        if (err != OK) {
            return err;
        }
    }

    for (const auto &fqName : packageInterfaces) {
        err = generateSourcesForFile(
                fqName, hidl_gen, coordinator, outputDir, lang);
        if (err != OK) {
            return err;
        }
    }

    return OK;
}

OutputHandler::GenerationFunction generationFunctionForFileOrPackage(const std::string &language) {
    return [language](const FQName &fqName,
              const char *hidl_gen, Coordinator *coordinator,
              const std::string &outputDir) -> status_t {
        if (fqName.isFullyQualified()) {
                    return generateSourcesForFile(fqName,
                                                  hidl_gen,
                                                  coordinator,
                                                  outputDir,
                                                  language);
        } else {
                    return generateSourcesForPackage(fqName,
                                                     hidl_gen,
                                                     coordinator,
                                                     outputDir,
                                                     language);
        }
    };
}

static std::string makeLibraryName(const FQName &packageFQName) {
    return packageFQName.string();
}

static status_t isPackageJavaCompatible(
        const FQName &packageFQName,
        Coordinator *coordinator,
        bool *compatible) {
    std::vector<FQName> todo;
    status_t err =
        coordinator->appendPackageInterfacesToVector(packageFQName, &todo);

    if (err != OK) {
        return err;
    }

    std::set<FQName> seen;
    for (const auto &iface : todo) {
        seen.insert(iface);
    }

    // Form the transitive closure of all imported interfaces (and types.hal-s)
    // If any one of them is not java compatible, this package isn't either.
    while (!todo.empty()) {
        const FQName fqName = todo.back();
        todo.pop_back();

        AST *ast = coordinator->parse(fqName);

        if (ast == nullptr) {
            return UNKNOWN_ERROR;
        }

        if (!ast->isJavaCompatible()) {
            *compatible = false;
            return OK;
        }

        std::set<FQName> importedPackages;
        ast->getImportedPackages(&importedPackages);

        for (const auto &package : importedPackages) {
            std::vector<FQName> packageInterfaces;
            status_t err = coordinator->appendPackageInterfacesToVector(
                    package, &packageInterfaces);

            if (err != OK) {
                return err;
            }

            for (const auto &iface : packageInterfaces) {
                if (seen.find(iface) != seen.end()) {
                    continue;
                }

                todo.push_back(iface);
                seen.insert(iface);
            }
        }
    }

    *compatible = true;
    return OK;
}

static bool packageNeedsJavaCode(
        const std::vector<FQName> &packageInterfaces, AST *typesAST) {
    if (packageInterfaces.size() == 0) {
        return false;
    }

    // If there is more than just a types.hal file to this package we'll
    // definitely need to generate Java code.
    if (packageInterfaces.size() > 1
            || packageInterfaces[0].name() != "types") {
        return true;
    }

    CHECK(typesAST != nullptr);

    // We'll have to generate Java code if types.hal contains any non-typedef
    // type declarations.

    Scope* rootScope = typesAST->getRootScope();
    std::vector<NamedType *> subTypes = rootScope->getSubTypes();

    for (const auto &subType : subTypes) {
        if (!subType->isTypeDef()) {
            return true;
        }
    }

    return false;
}

bool validateIsPackage(
        const FQName &fqName, const std::string & /* language */) {
    if (fqName.package().empty()) {
        fprintf(stderr, "ERROR: Expecting package name\n");
        return false;
    }

    if (fqName.version().empty()) {
        fprintf(stderr, "ERROR: Expecting package version\n");
        return false;
    }

    if (!fqName.name().empty()) {
        fprintf(stderr,
                "ERROR: Expecting only package name and version.\n");
        return false;
    }

    return true;
}

bool isHidlTransportPackage(const FQName& fqName) {
    return fqName.package() == gIBasePackageFqName.string() ||
           fqName.package() == gIManagerPackageFqName.string();
}

bool isSystemProcessSupportedPackage(const FQName& fqName) {
    // Technically, so is hidl IBase + IServiceManager, but
    // these are part of libhidltransport.
    return fqName.string() == "android.hardware.graphics.allocator@2.0" ||
           fqName.string() == "android.hardware.graphics.common@1.0" ||
           fqName.string() == "android.hardware.graphics.mapper@2.0" ||
           fqName.string() == "android.hardware.graphics.mapper@2.1" ||
           fqName.string() == "android.hardware.renderscript@1.0" ||
           fqName.string() == "android.hidl.memory.token@1.0" ||
           fqName.string() == "android.hidl.memory@1.0";
}

bool isSystemPackage(const FQName &package) {
    return package.inPackage("android.hidl") ||
           package.inPackage("android.system") ||
           package.inPackage("android.frameworks") ||
           package.inPackage("android.hardware");
}

// TODO(b/69862859): remove special case
bool isTestPackage(const FQName& fqName, Coordinator* coordinator, const std::string& outputPath) {
    const auto fileExists = [](const std::string& file) {
        struct stat buf;
        return stat(file.c_str(), &buf) == 0;
    };

    const std::string path = coordinator->getFilepath(
        outputPath, fqName, Coordinator::Location::PACKAGE_ROOT, ".hidl_for_test");
    const bool exists = fileExists(path);

    if (exists) {
        coordinator->onFileAccess(path, "r");
    }

    return exists;
}

static status_t generateAdapterMainSource(
        const FQName & packageFQName,
        const char* /* hidl_gen */,
        Coordinator* coordinator,
        const std::string &outputPath) {
    Formatter out = coordinator->getFormatter(outputPath, packageFQName,
                                              Coordinator::Location::DIRECT, "main.cpp");

    if (!out.isValid()) {
        return UNKNOWN_ERROR;
    }

    std::vector<FQName> packageInterfaces;
    status_t err =
        coordinator->appendPackageInterfacesToVector(packageFQName,
                                                     &packageInterfaces);
    if (err != OK) {
        return err;
    }

    out << "#include <hidladapter/HidlBinderAdapter.h>\n";

    for (auto &interface : packageInterfaces) {
        if (interface.name() == "types") {
            continue;
        }
        AST::generateCppPackageInclude(out, interface, interface.getInterfaceAdapterName());
    }

    out << "int main(int argc, char** argv) ";
    out.block([&] {
        out << "return ::android::hardware::adapterMain<\n";
        out.indent();
        for (auto &interface : packageInterfaces) {
            if (interface.name() == "types") {
                continue;
            }
            out << interface.getInterfaceAdapterFqName().cppName();

            if (&interface != &packageInterfaces.back()) {
                out << ",\n";
            }
        }
        out << ">(\"" << packageFQName.string() << "\", argc, argv);\n";
        out.unindent();
    }).endl();
    return OK;
}

static status_t generateAndroidBpForPackage(const FQName& packageFQName, const char* /* hidl_gen */,
                                            Coordinator* coordinator,
                                            const std::string& outputPath) {
    CHECK(packageFQName.isValid() && !packageFQName.isFullyQualified() &&
          packageFQName.name().empty());

    std::vector<FQName> packageInterfaces;

    status_t err = coordinator->appendPackageInterfacesToVector(packageFQName, &packageInterfaces);

    if (err != OK) {
        return err;
    }

    std::set<FQName> importedPackagesHierarchy;
    std::vector<const Type *> exportedTypes;
    AST* typesAST = nullptr;

    for (const auto& fqName : packageInterfaces) {
        AST* ast = coordinator->parse(fqName);

        if (ast == NULL) {
            fprintf(stderr, "ERROR: Could not parse %s. Aborting.\n", fqName.string().c_str());

            return UNKNOWN_ERROR;
        }

        if (fqName.name() == "types") {
            typesAST = ast;
        }

        ast->getImportedPackagesHierarchy(&importedPackagesHierarchy);
        ast->appendToExportedTypesVector(&exportedTypes);
    }

    bool needsJavaCode = packageNeedsJavaCode(packageInterfaces, typesAST);

    bool genJavaConstants = needsJavaCode && !exportedTypes.empty();

    bool isJavaCompatible;
    err = isPackageJavaCompatible(packageFQName, coordinator, &isJavaCompatible);
    if (err != OK) return err;
    bool genJavaLibrary = needsJavaCode && isJavaCompatible;

    bool generateForTest = isTestPackage(packageFQName, coordinator, outputPath);
    bool isVndk = !generateForTest && isSystemPackage(packageFQName);
    bool isVndkSp = isVndk && isSystemProcessSupportedPackage(packageFQName);

    Formatter out = coordinator->getFormatter(outputPath, packageFQName,
                                              Coordinator::Location::PACKAGE_ROOT, "Android.bp");

    if (!out.isValid()) {
        return UNKNOWN_ERROR;
    }

    out << "// This file is autogenerated by hidl-gen -Landroidbp.\n\n";


    out << "hidl_interface ";
    out.block([&] {
        out << "name: \"" << makeLibraryName(packageFQName) << "\",\n";
        if (!coordinator->getOwner().empty()) {
            out << "owner: \"" << coordinator->getOwner() << "\",\n";
        }
        out << "root: \"" << coordinator->getPackageRoot(packageFQName) << "\",\n";
        if (isHidlTransportPackage(packageFQName)) {
            out << "core_interface: true,\n";
        }
        if (isVndk) {
            out << "vndk: ";
            out.block([&]() {
                out << "enabled: true,\n";
                if (isVndkSp) {
                    out << "support_system_process: true,\n";
                }
            }) << ",\n";
        }
        (out << "srcs: [\n").indent([&] {
           for (const auto& fqName : packageInterfaces) {
               out << "\"" << fqName.name() << ".hal\",\n";
           }
        }) << "],\n";
        if (!importedPackagesHierarchy.empty()) {
            (out << "interfaces: [\n").indent([&] {
               for (const auto& fqName : importedPackagesHierarchy) {
                   out << "\"" << fqName.string() << "\",\n";
               }
            }) << "],\n";
        }
        if (typesAST != nullptr) {
            (out << "types: [\n").indent([&] {
                std::vector<NamedType *> subTypes = typesAST->getRootScope()->getSubTypes();
                std::sort(
                        subTypes.begin(),
                        subTypes.end(),
                        [](const NamedType *a, const NamedType *b) -> bool {
                            return a->fqName() < b->fqName();
                        });

                for (const auto &type : subTypes) {
                    if (type->isTypeDef()) {
                        continue;
                    }

                    out << "\"" << type->localName() << "\",\n";
                }
            }) << "],\n";
        }
        // Explicity call this out for developers.
        out << "gen_java: " << (genJavaLibrary ? "true" : "false") << ",\n";
        if (genJavaConstants) {
            out << "gen_java_constants: true,\n";
        }
   }).endl().endl();

    return OK;
}

static status_t generateAndroidBpImplForPackage(const FQName& packageFQName, const char*,
                                                Coordinator* coordinator,
                                                const std::string& outputPath) {
    const std::string libraryName = makeLibraryName(packageFQName) + "-impl";

    std::vector<FQName> packageInterfaces;

    status_t err =
        coordinator->appendPackageInterfacesToVector(packageFQName,
                                                     &packageInterfaces);

    if (err != OK) {
        return err;
    }

    std::set<FQName> importedPackages;

    for (const auto &fqName : packageInterfaces) {
        AST *ast = coordinator->parse(fqName);

        if (ast == NULL) {
            fprintf(stderr,
                    "ERROR: Could not parse %s. Aborting.\n",
                    fqName.string().c_str());

            return UNKNOWN_ERROR;
        }

        ast->getImportedPackages(&importedPackages);
    }

    Formatter out = coordinator->getFormatter(outputPath, packageFQName,
                                              Coordinator::Location::DIRECT, "Android.bp");

    if (!out.isValid()) {
        return UNKNOWN_ERROR;
    }

    out << "cc_library_shared {\n";
    out.indent([&] {
        out << "// FIXME: this should only be -impl for a passthrough hal.\n"
            << "// In most cases, to convert this to a binderized implementation, you should:\n"
            << "// - change '-impl' to '-service' here and make it a cc_binary instead of a\n"
            << "//   cc_library_shared.\n"
            << "// - add a *.rc file for this module.\n"
            << "// - delete HIDL_FETCH_I* functions.\n"
            << "// - call configureRpcThreadpool and registerAsService on the instance.\n"
            << "// You may also want to append '-impl/-service' with a specific identifier like\n"
            << "// '-vendor' or '-<hardware identifier>' etc to distinguish it.\n";
        out << "name: \"" << libraryName << "\",\n";
        if (!coordinator->getOwner().empty()) {
            out << "owner: \"" << coordinator->getOwner() << "\",\n";
        }
        out << "relative_install_path: \"hw\",\n";
        if (coordinator->getOwner().empty()) {
            out << "// FIXME: this should be 'vendor: true' for modules that will eventually be\n"
                   "// on AOSP.\n";
        }
        out << "proprietary: true,\n";
        out << "srcs: [\n";
        out.indent([&] {
            for (const auto &fqName : packageInterfaces) {
                if (fqName.name() == "types") {
                    continue;
                }
                out << "\"" << fqName.getInterfaceBaseName() << ".cpp\",\n";
            }
        });
        out << "],\n"
            << "shared_libs: [\n";
        out.indent([&] {
            out << "\"libhidlbase\",\n"
                << "\"libhidltransport\",\n"
                << "\"libutils\",\n"
                << "\"" << makeLibraryName(packageFQName) << "\",\n";

            for (const auto &importedPackage : importedPackages) {
                if (isHidlTransportPackage(importedPackage)) {
                    continue;
                }

                out << "\"" << makeLibraryName(importedPackage) << "\",\n";
            }
        });
        out << "],\n";
    });
    out << "}\n";

    return OK;
}

bool validateForSource(
        const FQName &fqName, const std::string &language) {
    if (fqName.package().empty()) {
        fprintf(stderr, "ERROR: Expecting package name\n");
        return false;
    }

    if (fqName.version().empty()) {
        fprintf(stderr, "ERROR: Expecting package version\n");
        return false;
    }

    const std::string &name = fqName.name();
    if (!name.empty()) {
        if (name.find('.') == std::string::npos) {
            return true;
        }

        if (language != "java" || name.find("types.") != 0) {
            // When generating java sources for "types.hal", output can be
            // constrained to just one of the top-level types declared
            // by using the extended syntax
            // android.hardware.Foo@1.0::types.TopLevelTypeName.
            // In all other cases (different language, not 'types') the dot
            // notation in the name is illegal in this context.
            return false;
        }

        return true;
    }

    return true;
}

OutputHandler::GenerationFunction generateExportHeaderForPackage(bool forJava) {
    return [forJava](const FQName &packageFQName,
                     const char * /* hidl_gen */,
                     Coordinator *coordinator,
                     const std::string &outputPath) -> status_t {
        CHECK(packageFQName.isValid()
                && !packageFQName.package().empty()
                && !packageFQName.version().empty()
                && packageFQName.name().empty());

        std::vector<FQName> packageInterfaces;

        status_t err = coordinator->appendPackageInterfacesToVector(
                packageFQName, &packageInterfaces);

        if (err != OK) {
            return err;
        }

        std::vector<const Type *> exportedTypes;

        for (const auto &fqName : packageInterfaces) {
            AST *ast = coordinator->parse(fqName);

            if (ast == NULL) {
                fprintf(stderr,
                        "ERROR: Could not parse %s. Aborting.\n",
                        fqName.string().c_str());

                return UNKNOWN_ERROR;
            }

            ast->appendToExportedTypesVector(&exportedTypes);
        }

        if (exportedTypes.empty()) {
            return OK;
        }

        // C++ filename is specified in output path
        const std::string filename = forJava ? "Constants.java" : "";
        const Coordinator::Location location =
            forJava ? Coordinator::Location::GEN_SANITIZED : Coordinator::Location::DIRECT;

        Formatter out = coordinator->getFormatter(outputPath, packageFQName, location, filename);

        if (!out.isValid()) {
            return UNKNOWN_ERROR;
        }

        out << "// This file is autogenerated by hidl-gen. Do not edit manually.\n"
            << "// Source: " << packageFQName.string() << "\n"
            << "// Root: " << coordinator->getPackageRootOption(packageFQName) << "\n\n";

        std::string guard;
        if (forJava) {
            out << "package " << packageFQName.javaPackage() << ";\n\n";
            out << "public class Constants {\n";
            out.indent();
        } else {
            guard = "HIDL_GENERATED_";
            guard += StringHelper::Uppercase(packageFQName.tokenName());
            guard += "_";
            guard += "EXPORTED_CONSTANTS_H_";

            out << "#ifndef "
                << guard
                << "\n#define "
                << guard
                << "\n\n#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n";
        }

        for (const auto &type : exportedTypes) {
            type->emitExportedHeader(out, forJava);
        }

        if (forJava) {
            out.unindent();
            out << "}\n";
        } else {
            out << "#ifdef __cplusplus\n}\n#endif\n\n#endif  // "
                << guard
                << "\n";
        }

        return OK;
    };
}

static status_t generateHashOutput(const FQName &fqName,
        const char* /*hidl_gen*/,
        Coordinator *coordinator,
        const std::string & /*outputDir*/) {

    status_t err;
    std::vector<FQName> packageInterfaces;

    if (fqName.isFullyQualified()) {
        packageInterfaces = {fqName};
    } else {
        err = coordinator->appendPackageInterfacesToVector(
                fqName, &packageInterfaces);
        if (err != OK) {
            return err;
        }
    }

    for (const auto &currentFqName : packageInterfaces) {
        AST* ast = coordinator->parse(currentFqName, {} /* parsed */,
                                      Coordinator::Enforce::NO_HASH /* enforcement */);

        if (ast == NULL) {
            fprintf(stderr,
                    "ERROR: Could not parse %s. Aborting.\n",
                    currentFqName.string().c_str());

            return UNKNOWN_ERROR;
        }

        printf("%s %s\n",
                Hash::getHash(ast->getFilename()).hexString().c_str(),
                currentFqName.string().c_str());
    }

    return OK;
}

// clang-format off
static std::vector<OutputHandler> formats = {
    {"check",
     "Parses the interface to see if valid but doesn't write any files.",
     OutputHandler::NOT_NEEDED /* mOutputMode */,
     validateForSource,
     generationFunctionForFileOrPackage("check")
    },

    {"c++",
     "(internal) (deprecated) Generates C++ interface files for talking to HIDL interfaces.",
     OutputHandler::NEEDS_DIR /* mOutputMode */,
     validateForSource,
     generationFunctionForFileOrPackage("c++")
    },
    {"c++-headers",
     "(internal) Generates C++ headers for interface files for talking to HIDL interfaces.",
     OutputHandler::NEEDS_DIR /* mOutputMode */,
     validateForSource,
     generationFunctionForFileOrPackage("c++-headers")
    },
    {"c++-sources",
     "(internal) Generates C++ sources for interface files for talking to HIDL interfaces.",
     OutputHandler::NEEDS_DIR /* mOutputMode */,
     validateForSource,
     generationFunctionForFileOrPackage("c++-sources")
    },

    {"export-header",
     "Generates a header file from @export enumerations to help maintain legacy code.",
     OutputHandler::NEEDS_FILE /* mOutputMode */,
     validateIsPackage,
     generateExportHeaderForPackage(false /* forJava */)
    },

    {"c++-impl",
     "Generates boilerplate implementation of a hidl interface in C++ (for convenience).",
     OutputHandler::NEEDS_DIR /* mOutputMode */,
     validateForSource,
     generationFunctionForFileOrPackage("c++-impl")
    },
    {"c++-impl-headers",
     "c++-impl but headers only",
     OutputHandler::NEEDS_DIR /* mOutputMode */,
     validateForSource,
     generationFunctionForFileOrPackage("c++-impl-headers")
    },
    {"c++-impl-sources",
     "c++-impl but sources only",
     OutputHandler::NEEDS_DIR /* mOutputMode */,
     validateForSource,
     generationFunctionForFileOrPackage("c++-impl-sources")
    },

    {"c++-adapter",
     "Takes a x.(y+n) interface and mocks an x.y interface.",
     OutputHandler::NEEDS_DIR /* mOutputMode */,
     validateForSource,
     generationFunctionForFileOrPackage("c++-adapter")
    },
    {"c++-adapter-headers",
     "c++-adapter but helper headers only",
     OutputHandler::NEEDS_DIR /* mOutputMode */,
     validateForSource,
     generationFunctionForFileOrPackage("c++-adapter-headers")
    },
    {"c++-adapter-sources",
     "c++-adapter but helper sources only",
     OutputHandler::NEEDS_DIR /* mOutputMode */,
     validateForSource,
     generationFunctionForFileOrPackage("c++-adapter-sources")
    },
    {"c++-adapter-main",
     "c++-adapter but the adapter binary source only",
     OutputHandler::NEEDS_DIR /* mOutputMode */,
     validateIsPackage,
     generateAdapterMainSource,
    },

    {"java",
     "(internal) Generates Java library for talking to HIDL interfaces in Java.",
     OutputHandler::NEEDS_DIR /* mOutputMode */,
     validateForSource,
     generationFunctionForFileOrPackage("java")
    },

    {"java-constants",
     "(internal) Like export-header but for Java (always created by -Lmakefile if @export exists).",
     OutputHandler::NEEDS_DIR /* mOutputMode */,
     validateIsPackage,
     generateExportHeaderForPackage(true /* forJava */)
    },

    {"vts",
     "(internal) Generates vts proto files for use in vtsd.",
     OutputHandler::NEEDS_DIR /* mOutputMode */,
     validateForSource,
     generationFunctionForFileOrPackage("vts")
    },

    {"makefile",
     "(removed) Used to generate makefiles for -Ljava and -Ljava-constants.",
     OutputHandler::NEEDS_SRC /* mOutputMode */,
     [](const FQName &, const std::string &) {
        fprintf(stderr, "ERROR: makefile output is not supported. Use -Landroidbp for all build file generation.\n");
        return false;
     },
     nullptr,
    },

    {"androidbp",
     "(internal) Generates Soong bp files for -Lc++-headers, -Lc++-sources, -Ljava, -Ljava-constants, and -Lc++-adapter.",
     OutputHandler::NEEDS_SRC /* mOutputMode */,
     validateIsPackage,
     generateAndroidBpForPackage,
    },

    {"androidbp-impl",
     "Generates boilerplate bp files for implementation created with -Lc++-impl.",
     OutputHandler::NEEDS_DIR /* mOutputMode */,
     validateIsPackage,
     generateAndroidBpImplForPackage,
    },

    {"hash",
     "Prints hashes of interface in `current.txt` format to standard out.",
     OutputHandler::NOT_NEEDED /* mOutputMode */,
     validateForSource,
     generateHashOutput,
    },
};
// clang-format on

static void usage(const char *me) {
    fprintf(stderr,
            "usage: %s [-p <root path>] -o <output path> -L <language> [-O <owner>] (-r <interface "
            "root>)+ [-v] fqname+\n",
            me);

    fprintf(stderr, "         -h: Prints this menu.\n");
    fprintf(stderr, "         -L <language>: The following options are available:\n");
    for (auto &e : formats) {
        fprintf(stderr, "            %-16s: %s\n", e.name().c_str(), e.description().c_str());
    }
    fprintf(stderr, "         -O <owner>: The owner of the module for -Landroidbp(-impl)?.\n");
    fprintf(stderr, "         -o <output path>: Location to output files.\n");
    fprintf(stderr, "         -p <root path>: Android build root, defaults to $ANDROID_BUILD_TOP or pwd.\n");
    fprintf(stderr, "         -r <package:path root>: E.g., android.hardware:hardware/interfaces.\n");
    fprintf(stderr, "         -v: verbose output (locations of touched files).\n");
}

// hidl is intentionally leaky. Turn off LeakSanitizer by default.
extern "C" const char *__asan_default_options() {
    return "detect_leaks=0";
}

int main(int argc, char **argv) {
    const char *me = argv[0];
    if (argc == 1) {
        usage(me);
        exit(1);
    }

    OutputHandler *outputFormat = nullptr;
    Coordinator coordinator;
    std::string outputPath;

    const char *ANDROID_BUILD_TOP = getenv("ANDROID_BUILD_TOP");
    if (ANDROID_BUILD_TOP != nullptr) {
        coordinator.setRootPath(ANDROID_BUILD_TOP);
    }

    int res;
    while ((res = getopt(argc, argv, "hp:o:O:r:L:v")) >= 0) {
        switch (res) {
            case 'p':
            {
                coordinator.setRootPath(optarg);
                break;
            }

            case 'v':
            {
                coordinator.setVerbose(true);
                break;
            }

            case 'o':
            {
                outputPath = optarg;
                break;
            }

            case 'O': {
                if (!coordinator.getOwner().empty()) {
                    fprintf(stderr, "ERROR: -O <owner> can only be specified once.\n");
                    exit(1);
                }
                coordinator.setOwner(optarg);
                break;
            }

            case 'r':
            {
                std::string val(optarg);
                auto index = val.find_first_of(':');
                if (index == std::string::npos) {
                    fprintf(stderr, "ERROR: -r option must contain ':': %s\n", val.c_str());
                    exit(1);
                }

                auto root = val.substr(0, index);
                auto path = val.substr(index + 1);

                std::string error;
                status_t err = coordinator.addPackagePath(root, path, &error);
                if (err != OK) {
                    fprintf(stderr, "%s\n", error.c_str());
                    exit(1);
                }

                break;
            }

            case 'L':
            {
                if (outputFormat != nullptr) {
                    fprintf(stderr,
                            "ERROR: only one -L option allowed. \"%s\" already specified.\n",
                            outputFormat->name().c_str());
                    exit(1);
                }
                for (auto &e : formats) {
                    if (e.name() == optarg) {
                        outputFormat = &e;
                        break;
                    }
                }
                if (outputFormat == nullptr) {
                    fprintf(stderr,
                            "ERROR: unrecognized -L option: \"%s\".\n",
                            optarg);
                    exit(1);
                }
                break;
            }

            case '?':
            case 'h':
            default:
            {
                usage(me);
                exit(1);
                break;
            }
        }
    }

    if (outputFormat == nullptr) {
        fprintf(stderr,
            "ERROR: no -L option provided.\n");
        exit(1);
    }

    argc -= optind;
    argv += optind;

    if (argc == 0) {
        fprintf(stderr, "ERROR: no fqname specified.\n");
        usage(me);
        exit(1);
    }

    // Valid options are now in argv[0] .. argv[argc - 1].

    switch (outputFormat->mOutputMode) {
        case OutputHandler::NEEDS_DIR:
        case OutputHandler::NEEDS_FILE:
        {
            if (outputPath.empty()) {
                usage(me);
                exit(1);
            }

            if (outputFormat->mOutputMode == OutputHandler::NEEDS_DIR) {
                if (outputPath.back() != '/') {
                    outputPath += "/";
                }
            }
            break;
        }
        case OutputHandler::NEEDS_SRC:
        {
            if (outputPath.empty()) {
                outputPath = coordinator.getRootPath();
            }
            if (outputPath.back() != '/') {
                outputPath += "/";
            }

            break;
        }

        default:
            outputPath.clear();  // Unused.
            break;
    }

    coordinator.addDefaultPackagePath("android.hardware", "hardware/interfaces");
    coordinator.addDefaultPackagePath("android.hidl", "system/libhidl/transport");
    coordinator.addDefaultPackagePath("android.frameworks", "frameworks/hardware/interfaces");
    coordinator.addDefaultPackagePath("android.system", "system/hardware/interfaces");

    for (int i = 0; i < argc; ++i) {
        FQName fqName(argv[i]);

        // TODO(b/65200821): remove
        gCurrentCompileName = "_" + StringHelper::Uppercase(fqName.tokenName());

        if (!fqName.isValid()) {
            fprintf(stderr,
                    "ERROR: Invalid fully-qualified name.\n");
            exit(1);
        }

        if (!outputFormat->validate(fqName, outputFormat->name())) {
            fprintf(stderr,
                    "ERROR: output handler failed.\n");
            exit(1);
        }

        status_t err =
            outputFormat->generate(fqName, me, &coordinator, outputPath);

        if (err != OK) {
            exit(1);
        }
    }

    return 0;
}
