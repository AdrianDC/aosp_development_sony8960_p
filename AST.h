#ifndef AST_H_

#define AST_H_

#include <android-base/macros.h>
#include <set>
#include <string>
#include <utils/KeyedVector.h>
#include <utils/RefBase.h>
#include <vector>

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
    AST(Coordinator *coordinator, const std::string &path);
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
    bool addTypeDef(const char *localName, Type *type, std::string *errorMsg);

    // Returns true iff successful.
    bool addScopedType(NamedType *type, std::string *errorMsg);

    void *scanner();
    void setScanner(void *scanner);

    const std::string &getFilename() const;

    // Look up a type by FQName, "pure" names, i.e. those without package
    // or version are first looked up in the current scope chain.
    // After that lookup proceeds to imports.
    Type *lookupType(const char *name);

    void addImportedAST(AST *ast);

    status_t generateCpp(const std::string &outputPath) const;
    status_t generateJava(const std::string &outputPath) const;

    void getImportedPackages(std::set<FQName> *importSet) const;

    status_t generateVts(const std::string &outputPath) const;

private:
    Coordinator *mCoordinator;
    std::string mPath;
    Vector<Scope *> mScopePath;

    void *mScanner;
    Scope *mRootScope;

    FQName mPackage;

    // A set of all external interfaces/types that are _actually_ referenced
    // in this AST, this is a subset of those specified in import statements.
    std::set<FQName> mImportedNames;

    // A set of all ASTs we explicitly or implicitly (types.hal) import.
    std::set<AST *> mImportedASTs;

    // Types keyed by full names defined in this AST.
    KeyedVector<FQName, Type *> mDefinedTypesByFullName;

    bool addScopedTypeInternal(
            const char *localName,
            Type *type,
            std::string *errorMsg,
            bool isTypeDef);

    // Find a type matching fqName (which may be partial) and if found
    // return the associated type and fill in the full "matchingName".
    // Only types defined in this very AST are considered.
    Type *findDefinedType(const FQName &fqName, FQName *matchingName) const;

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

    status_t generateHeaderMethodSignatures(
            Formatter &out, bool abstract) const;

    status_t generateProxySource(
            Formatter &out, const std::string &baseName) const;

    status_t generateStubSource(
            Formatter &out, const std::string &baseName) const;

    status_t generateStubSourceForMethod(
            Formatter &out, const Interface *iface, const Method *method) const;

    void declareCppReaderLocals(
            Formatter &out, const std::vector<TypedVar *> &arg) const;

    void emitCppReaderWriter(
            Formatter &out,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            const TypedVar *arg,
            bool isReader,
            Type::ErrorMode mode) const;

    void emitJavaReaderWriter(
            Formatter &out,
            const std::string &parcelObj,
            const TypedVar *arg,
            bool isReader) const;

    status_t emitTypeDeclarations(Formatter &out) const;
    status_t emitJavaTypeDeclarations(Formatter &out) const;

    status_t emitVtsTypeDeclarations(
            Formatter &out, const std::vector<Type *> &types) const;

    DISALLOW_COPY_AND_ASSIGN(AST);
};

}  // namespace android

#endif  // AST_H_
