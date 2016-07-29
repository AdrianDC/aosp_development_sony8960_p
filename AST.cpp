#include "AST.h"

#include "Coordinator.h"
#include "Formatter.h"
#include "FQName.h"
#include "HandleType.h"
#include "RefType.h"
#include "Scope.h"

#include <android-base/logging.h>
#include <stdlib.h>

namespace android {

AST::AST(Coordinator *coordinator)
    : mCoordinator(coordinator),
      mScanner(NULL),
      mRootScope(new Scope("root")) {
    enterScope(mRootScope);
}

AST::~AST() {
    CHECK(scope() == mRootScope);
    leaveScope();

    delete mRootScope;
    mRootScope = NULL;

    CHECK(mScanner == NULL);

    // Ownership of "coordinator" was NOT transferred.
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

bool AST::addImport(const char *import) {
    FQName fqName(import);
    CHECK(fqName.isValid());

    fqName.applyDefaults(mPackage.package(), mPackage.version());

    LOG(INFO) << "importing " << fqName.debugString();
    const std::string packagePath = Coordinator::GetPackagePath(fqName);

    if (fqName.name().empty()) {
        // TODO: Import the whole package.
        CHECK(!"Should not be here");
        return false;
    }

    std::string path = packagePath;
    path.append(fqName.name());
    path.append(".hal");

    AST *importAST = mCoordinator->parse(path.c_str());

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

Type *AST::lookupType(const char *name) const {
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

    // LOG(INFO) << "lookupType now looking for " << fqName.debugString();

    return mCoordinator->lookupType(fqName);
}

Type *AST::lookupTypeInternal(const std::string &namePath) const {
    Scope *scope = mRootScope;

    size_t startPos = 0;
    for (;;) {
        size_t dotPos = namePath.find('.', startPos);

        std::string component;
        if (dotPos == std::string::npos) {
            component = namePath.substr(startPos);
        } else {
            component = namePath.substr(startPos, dotPos - startPos);
        }

        Type *type = scope->lookupType(component.c_str());

        if (type == NULL) {
            return NULL;
        }

        if (dotPos == std::string::npos) {
            return type;
        }

        if (!type->isScope()) {
            return NULL;
        }

        scope = static_cast<Scope *>(type);
        startPos = dotPos + 1;
    }
}

void AST::dump(Formatter &out) const {
    mRootScope->dump(out);
}

}  // namespace android;
