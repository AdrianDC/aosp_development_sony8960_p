#include "AST.h"
#include "Coordinator.h"
#include "Formatter.h"
#include "FQName.h"

#include <android-base/logging.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <vector>

using namespace android;

static void usage(const char *me) {
    fprintf(stderr,
            "usage: %s -o output-path [-m] (-r interface-root)+ fqname+\n",
            me);

    fprintf(stderr, "         -o output path\n");
    fprintf(stderr, "         -m generate makefile instead of sources\n");

    fprintf(stderr,
            "         -r package:path root "
            "(e.g., android.hardware:hardware/interfaces)\n");
}

static std::string makeLibraryName(const FQName &packageFQName) {
    std::vector<std::string> components;
    packageFQName.getPackageAndVersionComponents(
            &components, true /* cpp_compatible */);

    const std::string libraryName =
        "lib_" + FQName::JoinStrings(components, "_");

    return libraryName;
}

static status_t generateMakefileOrSourcesForPackage(
        Coordinator *coordinator,
        const std::string &outputDir,
        bool createMakefile,
        const FQName &packageFQName) {
    std::vector<FQName> packageInterfaces;

    status_t err =
        coordinator->getPackageInterfaces(packageFQName, &packageInterfaces);

    if (err != OK) {
        return err;
    }

    std::set<FQName> importedPackages;
    for (const auto &fqName : packageInterfaces) {
        AST *ast = coordinator->parse(fqName);

        if (ast == NULL) {
            fprintf(stderr,
                    "Could not parse %s. Aborting.\n",
                    fqName.string().c_str());

            return UNKNOWN_ERROR;
        }

        if (!createMakefile) {
            status_t err = ast->generateCpp(outputDir);

            if (err != OK) {
                return err;
            }
        } else {
            ast->addImportedPackages(&importedPackages);
        }
    }

    if (!createMakefile) {
        return OK;
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
        << "GEN := \\\n";

    out.indent();
    for (const auto &fqName : packageInterfaces) {
        out << "$(intermediates)/"
            << coordinator->convertPackageRootToPath(packageFQName)
            << coordinator->getPackagePath(packageFQName, true /* relative */);

        if (fqName.name() == "types") {
            out << "types.cpp";
        } else {
            out << fqName.name().substr(1) << "All.cpp";
        }

        out << " \\\n";
    }

    out.unindent();

    out << "\n$(GEN): PRIVATE_OUTPUT_DIR := $(intermediates)\n\n"
        << "$(GEN): PRIVATE_CUSTOM_TOOL = \\\n";

    out.indent();

    out << "nuhidl-gen -o $(PRIVATE_OUTPUT_DIR) "
        << packageFQName.string()
        << "\n";

    out.unindent();

    out << "\n$(GEN): ";

    bool first = true;
    for (const auto &fqName : packageInterfaces) {
        if (!first) {
            out << " ";
        }

        out << "$(LOCAL_PATH)/" << fqName.name() << ".hal";

        first = false;
    }
    out << "\n\t$(transform-generated-source)\n\n";

    out << "LOCAL_GENERATED_SOURCES += $(GEN)\n\n"
        << "LOCAL_EXPORT_C_INCLUDE_DIRS := $(intermediates)\n\n"
        << "LOCAL_SHARED_LIBRARIES := \\\n";

    out.indent();
    out << "libhwbinder \\\n"
        << "libutils \\\n";

    for (const auto &importedPackage : importedPackages) {
        out << makeLibraryName(importedPackage) << " \\\n";
    }

    out.unindent();

    out << "\n"
        << "include $(BUILD_SHARED_LIBRARY)\n";

    return OK;
}

int main(int argc, char **argv) {
    std::string outputDir;
    std::vector<std::string> packageRootPaths;
    std::vector<std::string> packageRoots;
    bool createMakefile = false;

    const char *me = argv[0];

    int res;
    while ((res = getopt(argc, argv, "hmo:r:")) >= 0) {
        switch (res) {
            case 'm':
            {
                createMakefile = true;
                break;
            }

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

    if (createMakefile) {
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

        if (!fqName.isValid()
                || fqName.package().empty()
                || fqName.version().empty()
                || !fqName.name().empty()) {
            fprintf(stderr,
                    "Each fqname argument should specify a valid package.\n");

            exit(1);
        }

        status_t err =
            generateMakefileOrSourcesForPackage(
                    &coordinator, outputDir, createMakefile, fqName);

        if (err != OK) {
            break;
        }
    }

    return 0;
}
