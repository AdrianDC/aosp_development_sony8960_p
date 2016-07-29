#include "AST.h"
#include "Coordinator.h"
#include "Formatter.h"
#include "FQName.h"

#include <android-base/logging.h>
#include <stdio.h>

using namespace android;

int main(int argc, const char *const argv[]) {
    Coordinator coordinator;

    for (int i = 1; i < argc; ++i) {
        FQName fqName(argv[i]);
        CHECK(fqName.isValid() && fqName.isFullyQualified());

        AST *ast = coordinator.parse(fqName);

        if (ast == NULL) {
            continue;
        }

        Formatter out;

        printf("========================================\n");

        ast->dump(out);

        delete ast;
        ast = NULL;
    }

    return 0;
}
