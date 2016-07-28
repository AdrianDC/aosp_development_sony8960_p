#include "AST.h"

#include "Formatter.h"
#include "HandleType.h"
#include "RefType.h"
#include "Scope.h"

#include <android-base/logging.h>
#include <stdlib.h>

extern void parseFile(android::AST *ast, const char *path);

namespace android {

AST::AST()
    : mScanner(NULL),
      mRootScope(new Scope("root")) {
    enterScope(mRootScope);
}

AST::~AST() {
    CHECK(scope() == mRootScope);
    leaveScope();

    delete mRootScope;
    mRootScope = NULL;

    CHECK(mScanner == NULL);
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

void AST::setVersion(const char *, const char *) {
}

void AST::setPackage(Vector<std::string> *) {
}

void AST::addImport(Vector<std::string> *importPath) {
    CHECK(!importPath->empty());

    std::string leaf = importPath->itemAt(importPath->size() - 1);
    scope()->addType(new RefType(leaf.c_str(), new HandleType));
}

void AST::enterScope(Scope *container) {
    mScopePath.push_back(container);
}

void AST::leaveScope() {
    mScopePath.pop();
}

Scope *AST::scope() {
    CHECK(!mScopePath.empty());
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
