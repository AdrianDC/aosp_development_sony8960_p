#include "AST.h"

#include "Formatter.h"

#include <stdio.h>

using namespace android;

int main(int argc, const char *const argv[]) {
    AST *ast = AST::Parse(argv[1]);

    Formatter out;

    printf("========================================\n");

    ast->dump(out);

    delete ast;
    ast = NULL;

    return 0;
}
