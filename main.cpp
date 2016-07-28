#include "AST.h"

#include "Formatter.h"
#include "FQName.h"

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

#if 0
    FQName("a.b.c.d@2.3::foo").print();
    FQName("a.b.c.d::foo").print();
    FQName("@3.4::foo").print();
    FQName("foo").print();
    FQName("::foo").print();
    FQName("some.package.somewhere@1.2").print();
#endif

    return 0;
}
