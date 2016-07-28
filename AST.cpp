#include "AST.h"

#include "Formatter.h"
#include "Scope.h"

#include <assert.h>
#include <stdlib.h>

extern void parseFile(android::AST *ast, const char *path);

namespace android {

AST::AST()
    : mScanner(NULL),
      mRootScope(new Scope("root")) {
    enterScope(mRootScope);
}

AST::~AST() {
    assert(scope() == mRootScope);
    leaveScope();

    delete mRootScope;
    mRootScope = NULL;

    assert(mScanner == NULL);
}

// static
AST *AST::Parse(const char *path) {
    AST *ast = new AST;
    parseFile(ast, path);

    return ast;
}

void *AST::scanner() {
    return mScanner;
}

void AST::setScanner(void *scanner) {
    mScanner = scanner;
}

void AST::enterScope(Scope *container) {
    mScopePath.push_back(container);
}

void AST::leaveScope() {
    mScopePath.pop();
}

Scope *AST::scope() {
    assert(!mScopePath.empty());
    return mScopePath.top();
}

Type *AST::lookupType(const char *name) {
    for (size_t i = mScopePath.size(); i-- > 0;) {
        Type *type = mScopePath[i]->lookupType(name);

        if (type != NULL) {
            return type;
        }
    }

    return NULL;
}

void AST::dump(Formatter &out) const {
    mRootScope->dump(out);
}

}  // namespace android;
