#include "AST.h"
#include "Coordinator.h"
#include "FQName.h"

#include <android-base/logging.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <vector>

using namespace android;

static void usage(const char *me) {
    fprintf(stderr,
            "usage: %s -o output-path (-r interface-root)+ fqname+\n",
            me);

    fprintf(stderr, "         -o output path\n");

    fprintf(stderr,
            "         -r package:path root "
            "(e.g., android.hardware:hardware/interfaces)\n");
}

int main(int argc, char **argv) {
    std::string outputDir;
    std::vector<std::string> packageRootPaths;
    std::vector<std::string> packageRoots;

    const char *me = argv[0];

    int res;
    while ((res = getopt(argc, argv, "ho:r:")) >= 0) {
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

    if (outputDir.empty()) {
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
        CHECK(fqName.isValid() && fqName.isFullyQualified());

        AST *ast = coordinator.parse(fqName);

        if (ast == NULL) {
            fprintf(stderr,
                    "Could not parse %s. Aborting.\n",
                    fqName.string().c_str());

            exit(1);
        }
    }

    // Now that we've found the transitive hull of all necessary interfaces
    // and types to process, go ahead and do the work.
    status_t err = coordinator.forEachAST(
            [&](const AST *ast) {
                return ast->generateCpp(outputDir);
            });

    return (err == OK) ? 0 : 1;
}
