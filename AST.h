#ifndef AST_H_

#define AST_H_

#include <android-base/macros.h>
#include <string>
#include <utils/Vector.h>

#include "FQName.h"

namespace android {

struct Coordinator;
struct Formatter;
struct Type;
struct Scope;

struct AST {
    AST(Coordinator *coordinator);
    ~AST();

    static AST *Parse(const char *path);

    bool setPackage(const char *package);
    bool addImport(const char *import);

    void enterScope(Scope *container);
    void leaveScope();
    Scope *scope();

    void *scanner();
    void setScanner(void *scanner);

    // Look up a type by FQName, "pure" names, i.e. those without package
    // or version are first looked up in the current scope chain.
    // After that lookup proceeds to imports.
    Type *lookupType(const char *name) const;

    // Takes dot-separated path components to a type possibly inside this AST.
    // Name resolution goes from root scope downwards, i.e. the path must be
    // absolute.
    Type *lookupTypeInternal(const std::string &namePath) const;

    void dump(Formatter &out) const;

private:
    Coordinator *mCoordinator;
    Vector<Scope *> mScopePath;

    void *mScanner;
    Scope *mRootScope;

    FQName mPackage;

    DISALLOW_COPY_AND_ASSIGN(AST);
};

}  // namespace android

#endif  // AST_H_
