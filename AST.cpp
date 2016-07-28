#include "AST.h"

#include "Formatter.h"
#include "FQName.h"
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

bool AST::setPackage(const char *package) {
    mPackage.setTo(package);
    CHECK(mPackage.isValid());

    if (mPackage.package().empty()
            || mPackage.version().empty()
            || !mPackage.name().empty()) {
        return false;
    }

    return true;
}

bool AST::addImport(const char * /* import */) {
#if 0
    CHECK(!importPath->empty());

    std::string leaf = importPath->itemAt(importPath->size() - 1);
    scope()->addType(new RefType(leaf.c_str(), new HandleType));
#endif

    return true;
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
    FQName fqName(name);
    CHECK(fqName.isValid());

    if (fqName.name().empty()) {
        // Given a package and version???
        return NULL;
    }

    if (fqName.package().empty() && fqName.version().empty()) {
        // This is just a plain identifier, resolve locally first if possible.

        for (size_t i = mScopePath.size(); i-- > 0;) {
            Type *type = mScopePath[i]->lookupType(name);

            if (type != NULL) {
                return new RefType(name, type);
            }
        }
    }

    fqName.applyDefaults(mPackage.package(), mPackage.version());

    LOG(INFO) << "lookupType now looking for " << fqName.debugString();

    return NULL;
}

void AST::dump(Formatter &out) const {
    mRootScope->dump(out);
}

}  // namespace android;
