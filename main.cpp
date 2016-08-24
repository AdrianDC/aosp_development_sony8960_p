#include "AST.h"
#include "Coordinator.h"
#include "Formatter.h"
#include "FQName.h"

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
    ValRes (*validate)(const FQName &);
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

    AST *ast = coordinator->parse(fqName);

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
        return ast->generateJava(outputDir);
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

    for (const auto &fqName : packageInterfaces) {

        out << "\n"
            << "\n#"
            << "\n# Build " << fqName.name() << ".hal"
            << "\n#";
        out << "\nGEN := $(intermediates)/"
            << coordinator->convertPackageRootToPath(packageFQName)
            << coordinator->getPackagePath(packageFQName, true /* relative */);
        if (fqName.name() == "types") {
            out << "types.cpp";
        } else {
            out << fqName.name().substr(1) << "All.cpp";
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
            << "\n-Lc++ -r"
            << coordinator->getPackageRoot(packageFQName) << ":"
            << coordinator->getPackageRootPath(packageFQName) << "\\";
        out << "\n"
            << packageFQName.string()
            << "::$(patsubst %.hal,%,$(notdir $(PRIVATE_DEPS)))"
            << "\n";
        out.unindent();
        out.unindent();

        out << "\n$(GEN): $(LOCAL_PATH)/" << fqName.name() << ".hal";
        out << "\n\t$(transform-generated-source)";
        out << "\nLOCAL_GENERATED_SOURCES += $(GEN)";
    }

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

    if (path.find("hardware/interfaces/tests/") != std::string::npos) {
        out << "\nLOCAL_COMPATIBILITY_SUITE := vts"
            << "\n-include test/vts/tools/build/Android.packaging_sharedlib.mk";
        // TODO(yim): b/30589200 remove the above -include line after the
        // build rule change is merged.
    }
    out << "\ninclude $(BUILD_SHARED_LIBRARY)\n";

    return OK;
}

OutputHandler::ValRes validateForMakefile(const FQName &fqName) {
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

OutputHandler::ValRes validateForSource(const FQName &fqName) {
    if (fqName.package().empty()) {
        fprintf(stderr, "ERROR: Expecting package name\n");
        return OutputHandler::FAILED;
    }

    if (fqName.version().empty()) {
        fprintf(stderr, "ERROR: Expecting package version\n");
        return OutputHandler::FAILED;
    }

    return fqName.name().empty() ?
        OutputHandler::PASS_PACKAGE :
        OutputHandler::PASS_FULL;
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
                                                      outputDir, "c++");
            } else {
                        return generateSourcesForPackage(fqName,
                                                         hidl_gen,
                                                         coordinator,
                                                         outputDir, "c++");
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
                                              outputDir, "java");
            }
            else {
                return generateSourcesForPackage(fqName,
                                                 hidl_gen,
                                                 coordinator,
                                                 outputDir, "java");
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

        OutputHandler::ValRes valid = outputFormat->validate(fqName);
        if (valid == OutputHandler::FAILED) {
            exit(1);
        }

        status_t err =
            outputFormat->generate(fqName, me, &coordinator, outputDir);

        if (err != OK) {
            break;
        }
    }

    return 0;
}
