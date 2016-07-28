#ifndef AST_H_

#define AST_H_

#include <android-base/macros.h>
#include <string>
#include <utils/Vector.h>

namespace android {

struct Formatter;
struct Type;
struct Scope;

struct AST {
    AST();
    ~AST();

    static AST *Parse(const char *path);

    void setVersion(const char *major, const char *minor);
    bool setPackage(const char *package);
    bool addImport(const char *import);

    void enterScope(Scope *container);
    void leaveScope();
    Scope *scope();

    void *scanner();
    void setScanner(void *scanner);

    Type *lookupType(const char *name);

    void dump(Formatter &out) const;

private:
    Vector<Scope *> mScopePath;

    void *mScanner;
    Scope *mRootScope;

    std::string mVersion;
    std::string mPackage;

    DISALLOW_COPY_AND_ASSIGN(AST);
};

}  // namespace android

#endif  // AST_H_
