#include "AST.h"

#include "Formatter.h"

#include <stdio.h>

using namespace android;

int main(int argc, const char *const argv[]) {
    for (int i = 1; i < argc; ++i) {
        AST *ast = AST::Parse(argv[i]);

        Formatter out;

        printf("========================================\n");

        ast->dump(out);

        delete ast;
        ast = NULL;
    }

    return 0;
}
