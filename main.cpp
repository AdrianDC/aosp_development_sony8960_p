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
#include "Formatter.h"
#include "FQName.h"
#include "Scope.h"

#include <android-base/logging.h>
#include <set>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <vector>

using namespace android;

struct OutputHandler {
    std::string mKey;
    bool mNeedsOutputDir;
    enum ValRes {
        FAILED,
        PASS_PACKAGE,
        PASS_FULL
    };
    ValRes (*validate)(const FQName &, const std::string &language);
    status_t (*generate)(const FQName &fqName,
                         const char *hidl_gen,
                         Coordinator *coordinator,
                         const std::string &outputDir);
};

static status_t generateSourcesForFile(
        const FQName &fqName,
        const char *,
        Coordinator *coordinator,
        const std::string &outputDir,
        const std::string &lang) {
    CHECK(fqName.isFullyQualified());

    AST *ast;
    const char *limitToType = nullptr;

    if (fqName.name().find("types.") == 0) {
        CHECK(lang == "java");  // Already verified in validate().

        limitToType = fqName.name().c_str() + strlen("types.");

        FQName typesName(fqName.package(), fqName.version(), "types");
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

    if (lang == "c++") {
        return ast->generateCpp(outputDir);
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
        coordinator->appendPackageInterfacesToSet(packageFQName,
                                                  &packageInterfaces);

    if (err != OK) {
        return err;
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

static std::string makeLibraryName(const FQName &packageFQName) {
    return packageFQName.string();
}

static void generateMakefileSectionForLanguageAndType(
        Formatter &out,
        Coordinator *coordinator,
        const FQName &packageFQName,
        const FQName &fqName,
        const char *typeName,
        bool forJava) {
    out << "\n"
        << "\n#"
        << "\n# Build " << fqName.name() << ".hal";

    if (forJava && typeName != nullptr) {
        out << " (" << typeName << ")";
    }

    out << "\n#"
        << "\nGEN := $(intermediates)/"
        << coordinator->convertPackageRootToPath(packageFQName)
        << coordinator->getPackagePath(packageFQName, true /* relative */);
    if (!forJava) {
        CHECK(typeName == nullptr);

        if (fqName.name() == "types") {
            out << "types.cpp";
        } else {
            out << fqName.name().substr(1) << "All.cpp";
        }
    } else if (typeName == nullptr) {
        out << fqName.name() << ".java";
    } else {
        out << typeName << ".java";
    }

    out << "\n$(GEN): $(HIDL)";
    out << "\n$(GEN): PRIVATE_HIDL := $(HIDL)";
    out << "\n$(GEN): PRIVATE_DEPS := $(LOCAL_PATH)/"
        << fqName.name() << ".hal";
    out << "\n$(GEN): PRIVATE_OUTPUT_DIR := $(intermediates)"
        << "\n$(GEN): PRIVATE_CUSTOM_TOOL = \\";
    out.indent();
    out.indent();
    out << "\n$(PRIVATE_HIDL) -o $(PRIVATE_OUTPUT_DIR) \\"
        << "\n-L"
        << (forJava ? "java" : "c++")
        << " -r"
        << coordinator->getPackageRoot(packageFQName) << ":"
        << coordinator->getPackageRootPath(packageFQName) << " \\\n";

    out << packageFQName.string()
        << "::"
        << fqName.name();

    if (forJava && typeName != nullptr) {
        out << "." << typeName;
    }

    out << "\n";

    out.unindent();
    out.unindent();

    out << "\n$(GEN): $(LOCAL_PATH)/" << fqName.name() << ".hal";
    out << "\n\t$(transform-generated-source)";
    out << "\nLOCAL_GENERATED_SOURCES += $(GEN)";
}

static void generateMakefileSectionForLanguage(
        Formatter &out,
        Coordinator *coordinator,
        const FQName &packageFQName,
        const std::vector<FQName> &packageInterfaces,
        AST *typesAST,
        bool forJava) {
    for (const auto &fqName : packageInterfaces) {
        if (forJava && fqName.name() == "types") {
            CHECK(typesAST != nullptr);

            Scope *rootScope = typesAST->scope();
            for (size_t i = 0; i < rootScope->countTypes(); ++i) {
                std::string typeName;
                rootScope->typeAt(i, &typeName);

                generateMakefileSectionForLanguageAndType(
                        out,
                        coordinator,
                        packageFQName,
                        fqName,
                        typeName.c_str(),
                        forJava);
            }

            continue;
        }

        generateMakefileSectionForLanguageAndType(
                out,
                coordinator,
                packageFQName,
                fqName,
                nullptr /* typeName */,
                forJava);
    }
}

static status_t generateMakefileForPackage(
        const FQName &packageFQName,
        const char *hidl_gen,
        Coordinator *coordinator,
        const std::string &) {

    CHECK(packageFQName.isValid() &&
          !packageFQName.isFullyQualified() &&
          packageFQName.name().empty());

    std::vector<FQName> packageInterfaces;

    status_t err =
        coordinator->appendPackageInterfacesToSet(packageFQName,
                                                  &packageInterfaces);

    if (err != OK) {
        return err;
    }

    std::set<FQName> importedPackages;
    bool packageIsJavaCompatible = true;
    AST *typesAST = nullptr;

    for (const auto &fqName : packageInterfaces) {
        AST *ast = coordinator->parse(fqName);

        if (ast == NULL) {
            fprintf(stderr,
                    "ERROR: Could not parse %s. Aborting.\n",
                    fqName.string().c_str());

            return UNKNOWN_ERROR;
        }

        if (packageIsJavaCompatible && !ast->isJavaCompatible()) {
            packageIsJavaCompatible = false;
        }

        if (fqName.name() == "types") {
            typesAST = ast;
        }

        ast->getImportedPackages(&importedPackages);
    }

    std::string path =
        coordinator->getPackagePath(packageFQName, false /* relative */);

    path.append("Android.mk");

    CHECK(Coordinator::MakeParentHierarchy(path));
    FILE *file = fopen(path.c_str(), "w");

    if (file == NULL) {
        return -errno;
    }

    const std::string libraryName = makeLibraryName(packageFQName);

    Formatter out(file);

    out << "LOCAL_PATH := $(call my-dir)\n"
        << "include $(CLEAR_VARS)\n\n"
        << "LOCAL_MODULE := "
        << libraryName
        << "\n"
        << "LOCAL_MODULE_CLASS := SHARED_LIBRARIES\n\n"
        << "intermediates := $(local-generated-sources-dir)\n\n"
        << "HIDL := $(HOST_OUT_EXECUTABLES)/"
        << hidl_gen << "$(HOST_EXECUTABLE_SUFFIX)";

    generateMakefileSectionForLanguage(
            out,
            coordinator,
            packageFQName,
            packageInterfaces,
            typesAST,
            false /* forJava */);

    out << "\n"
        << "\nLOCAL_EXPORT_C_INCLUDE_DIRS := $(intermediates)"
        << "\nLOCAL_SHARED_LIBRARIES := \\";
    out.indent();
    out << "\nlibhidl \\"
        << "\nlibhwbinder \\"
        << "\nlibutils \\";

    for (const auto &importedPackage : importedPackages) {
        out << "\n" << makeLibraryName(importedPackage) << " \\";
    }
    out << "\n";
    out.unindent();
    out << "\nLOCAL_MULTILIB := both";

    out << "\ninclude $(BUILD_SHARED_LIBRARY)\n";

    if (packageIsJavaCompatible) {
        out << "\n"
            << "########################################"
            << "########################################\n\n";

        out << "include $(CLEAR_VARS)\n"
            << "LOCAL_MODULE := "
            << libraryName
            << "-java\n"
            << "LOCAL_MODULE_CLASS := JAVA_LIBRARIES\n\n"
            << "intermediates := $(local-generated-sources-dir)\n\n"
            << "HIDL := $(HOST_OUT_EXECUTABLES)/"
            << hidl_gen
            << "$(HOST_EXECUTABLE_SUFFIX)";

        generateMakefileSectionForLanguage(
                out,
                coordinator,
                packageFQName,
                packageInterfaces,
                typesAST,
                true /* forJava */);

        out << "\ninclude $(BUILD_JAVA_LIBRARY)\n";
    }

    return OK;
}

OutputHandler::ValRes validateForMakefile(
        const FQName &fqName, const std::string & /* language */) {
    if (fqName.package().empty()) {
        fprintf(stderr, "ERROR: Expecting package name\n");
        return OutputHandler::FAILED;
    }

    if (fqName.version().empty()) {
        fprintf(stderr, "ERROR: Expecting package version\n");
        return OutputHandler::FAILED;
    }

    if (!fqName.name().empty()) {
        fprintf(stderr,
                "ERROR: Expecting only package name and version.\n");
        return OutputHandler::FAILED;
    }

    return OutputHandler::PASS_PACKAGE;
}

OutputHandler::ValRes validateForSource(
        const FQName &fqName, const std::string &language) {
    if (fqName.package().empty()) {
        fprintf(stderr, "ERROR: Expecting package name\n");
        return OutputHandler::FAILED;
    }

    if (fqName.version().empty()) {
        fprintf(stderr, "ERROR: Expecting package version\n");
        return OutputHandler::FAILED;
    }

    const std::string &name = fqName.name();
    if (!name.empty()) {
        if (name.find('.') == std::string::npos) {
            return OutputHandler::PASS_FULL;
        }

        if (language != "java" || name.find("types.") != 0) {
            // When generating java sources for "types.hal", output can be
            // constrained to just one of the top-level types declared
            // by using the extended syntax
            // android.hardware.Foo@1.0::types.TopLevelTypeName.
            // In all other cases (different language, not 'types') the dot
            // notation in the name is illegal in this context.
            return OutputHandler::FAILED;
        }

        return OutputHandler::PASS_FULL;
    }

    return OutputHandler::PASS_PACKAGE;
}

static std::vector<OutputHandler> formats = {
    {"c++",
     true /* mNeedsOutputDir */,
     validateForSource,
     [](const FQName &fqName,
        const char *hidl_gen, Coordinator *coordinator,
        const std::string &outputDir) -> status_t {
            if (fqName.isFullyQualified()) {
                        return generateSourcesForFile(fqName,
                                                      hidl_gen,
                                                      coordinator,
                                                      outputDir,
                                                      "c++");
            } else {
                        return generateSourcesForPackage(fqName,
                                                         hidl_gen,
                                                         coordinator,
                                                         outputDir,
                                                         "c++");
            }
        }
    },

    {"java",
     true /* mNeedsOutputDir */,
     validateForSource,
     [](const FQName &fqName,
        const char *hidl_gen, Coordinator *coordinator,
        const std::string &outputDir) -> status_t {
            if (fqName.isFullyQualified()) {
                return generateSourcesForFile(fqName,
                                              hidl_gen,
                                              coordinator,
                                              outputDir,
                                              "java");
            }
            else {
                return generateSourcesForPackage(fqName,
                                                 hidl_gen,
                                                 coordinator,
                                                 outputDir,
                                                 "java");
            }
        }
    },

    {"vts",
     true,
     validateForSource,
     [](const FQName &fqName,
        const char * hidl_gen,
        Coordinator *coordinator,
        const std::string &outputDir) -> status_t {
            if (fqName.isFullyQualified()) {
                return generateSourcesForFile(fqName,
                                              hidl_gen,
                                              coordinator,
                                              outputDir, "vts");
            } else {
                return generateSourcesForPackage(fqName,
                                                 hidl_gen,
                                                 coordinator,
                                                 outputDir, "vts");
            }
       }
    },

    {"makefile",
     false /* mNeedsOutputDir */,
     validateForMakefile,
     generateMakefileForPackage,
    },
};

static void usage(const char *me) {
    fprintf(stderr,
            "usage: %s -o output-path -L <language> (-r interface-root)+ fqname+\n",
            me);

    fprintf(stderr, "         -o output path\n");

    fprintf(stderr, "         -L <language> (one of");
    for (auto &e : formats) {
        fprintf(stderr, " %s", e.mKey.c_str());
    }
    fprintf(stderr, ")\n");

    fprintf(stderr,
            "         -r package:path root "
            "(e.g., android.hardware:hardware/interfaces)\n");
}

int main(int argc, char **argv) {
    std::string outputDir;
    std::vector<std::string> packageRootPaths;
    std::vector<std::string> packageRoots;

    const char *me = argv[0];
    OutputHandler *outputFormat = nullptr;

    int res;
    while ((res = getopt(argc, argv, "ho:r:L:")) >= 0) {
        switch (res) {
            case 'o':
            {
                outputDir = optarg;
                break;
            }

            case 'r':
            {
                std::string val(optarg);
                auto index = val.find_first_of(':');
                CHECK(index != std::string::npos);

                auto package = val.substr(0, index);
                auto path = val.substr(index + 1);
                packageRootPaths.push_back(path);
                packageRoots.push_back(package);
                break;
            }

            case 'L':
            {
                CHECK(outputFormat == nullptr); // only one -L option
                for (auto &e : formats) {
                    if (e.mKey == optarg) {
                        outputFormat = &e;
                        break;
                    }
                }
                CHECK(outputFormat != nullptr);
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
        usage(me);
        exit(1);
    }

    argc -= optind;
    argv += optind;

    if (packageRootPaths.empty()) {
        // Pick reasonable defaults.

        packageRoots.push_back("android.hardware");

        const char *TOP = getenv("TOP");
        CHECK(TOP != NULL);

        std::string path = TOP;
        path.append("/hardware/interfaces");

        packageRootPaths.push_back(path);
    }

    // Valid options are now in argv[0] .. argv[argc - 1].

    if (!outputFormat->mNeedsOutputDir) {
        outputDir.clear();  // Unused.
    } else if (outputDir.empty()) {
        usage(me);
        exit(1);
    } else {
        const size_t len = outputDir.size();
        if (outputDir[len - 1] != '/') {
            outputDir += "/";
        }
    }

    Coordinator coordinator(packageRootPaths, packageRoots);

    for (int i = 0; i < argc; ++i) {
        FQName fqName(argv[i]);

        if (!fqName.isValid()) {
            fprintf(stderr,
                    "ERROR: Invalid fully-qualified name.\n");
            exit(1);
        }

        OutputHandler::ValRes valid =
            outputFormat->validate(fqName, outputFormat->mKey);

        if (valid == OutputHandler::FAILED) {
            exit(1);
        }

        status_t err =
            outputFormat->generate(fqName, me, &coordinator, outputDir);

        if (err != OK) {
            exit(1);
        }
    }

    return 0;
}
