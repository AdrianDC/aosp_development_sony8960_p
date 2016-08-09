#ifndef AST_H_

#define AST_H_

#include <android-base/macros.h>
#include <set>
#include <string>
#include <utils/KeyedVector.h>
#include <utils/RefBase.h>
#include <utils/Vector.h>

#include "FQName.h"
#include "Type.h"

namespace android {

struct Coordinator;
struct Formatter;
struct Interface;
struct Method;
struct NamedType;
struct TypedVar;
struct Scope;

struct AST {
    AST(Coordinator *coordinator);
    ~AST();

    bool setPackage(const char *package);
    bool addImport(const char *import);

    // package and version really.
    FQName package() const;
    bool isInterface(std::string *ifaceName) const;

    void enterScope(Scope *container);
    void leaveScope();
    Scope *scope();

    // Returns true iff successful.
    bool addScopedType(const char *localName, NamedType *type);

    void *scanner();
    void setScanner(void *scanner);

    // Look up a type by FQName, "pure" names, i.e. those without package
    // or version are first looked up in the current scope chain.
    // After that lookup proceeds to imports.
    Type *lookupType(const char *name);

    // Takes dot-separated path components to a type possibly inside this AST.
    // Name resolution goes from root scope downwards, i.e. the path must be
    // absolute.
    Type *lookupTypeInternal(const std::string &namePath) const;

    status_t generateCpp(const std::string &outputPath) const;

    void addImportedPackages(std::set<FQName> *importSet) const;

private:
    Coordinator *mCoordinator;
    Vector<Scope *> mScopePath;

    void *mScanner;
    Scope *mRootScope;

    FQName mPackage;

    std::set<FQName> mImportedNames;

    void getPackageComponents(std::vector<std::string> *components) const;

    void getPackageAndVersionComponents(
            std::vector<std::string> *components, bool cpp_compatible) const;

    std::string makeHeaderGuard(const std::string &baseName) const;
    void enterLeaveNamespace(Formatter &out, bool enter) const;

    status_t generateInterfaceHeader(const std::string &outputPath) const;
    status_t generateStubHeader(const std::string &outputPath) const;
    status_t generateProxyHeader(const std::string &outputPath) const;
    status_t generateAllSource(const std::string &outputPath) const;

    status_t generateTypeSource(
            Formatter &out, const std::string &ifaceName) const;

    status_t generateProxySource(
            Formatter &out, const std::string &baseName) const;

    status_t generateStubSource(
            Formatter &out, const std::string &baseName) const;

    status_t generateStubSourceForMethod(
            Formatter &out, const Interface *iface, const Method *method) const;

    void emitCppReaderWriter(
            Formatter &out,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            const TypedVar *arg,
            bool isReader,
            Type::ErrorMode mode) const;

    status_t emitTypeDeclarations(Formatter &out) const;

    DISALLOW_COPY_AND_ASSIGN(AST);
};

}  // namespace android

#endif  // AST_H_
