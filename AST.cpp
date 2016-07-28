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

void AST::setVersion(const char *major, const char *minor) {
    mVersion = "@";
    mVersion.append(major);
    mVersion.append(".");
    mVersion.append(minor);
}

bool AST::setPackage(const char *package) {
    FQName fqName(package);
    CHECK(fqName.isValid());

    if (!fqName.package().empty() || !fqName.version().empty()) {
        return false;
    }

    mPackage = package;

    return true;
}

bool AST::addImport(const char *import) {
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
    LOG(INFO) << "lookupType " << name;

    FQName fqName(name);
    CHECK(fqName.isValid());

    if (fqName.package().empty() && fqName.version().empty()) {
        // This is just a plain identifier, resolve locally first if possible.

        for (size_t i = mScopePath.size(); i-- > 0;) {
            Type *type = mScopePath[i]->lookupType(name);

            if (type != NULL) {
                return new RefType(name, type);
            }
        }
    }

    fqName.applyDefaults(mPackage, mVersion);

    LOG(INFO) << "lookupType now looking for " << fqName.debugString();

    return NULL;
}

void AST::dump(Formatter &out) const {
    mRootScope->dump(out);
}

}  // namespace android;
