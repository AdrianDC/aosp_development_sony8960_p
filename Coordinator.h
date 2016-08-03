#ifndef COORDINATOR_H_

#define COORDINATOR_H_

#include <android-base/macros.h>
#include <functional>
#include <string>
#include <utils/KeyedVector.h>

namespace android {

struct AST;
struct FQName;
struct RefType;

struct Coordinator {
    Coordinator(const std::string &interfacesPath);
    ~Coordinator();

    AST *parse(const FQName &fqName);

    RefType *lookupType(const FQName &fqName) const;

    std::string getPackagePath(
            const FQName &fqName, bool relative = false) const;

    // Iterate over all cached ASTs and abort if any callback returns an error.
    using for_each_cb = std::function<status_t(const AST *ast)>;
    status_t forEachAST(for_each_cb cb) const;

private:
    std::string mInterfacesPath;
    KeyedVector<FQName, AST *> mCache;

    DISALLOW_COPY_AND_ASSIGN(Coordinator);
};

}  // namespace android

#endif  // COORDINATOR_H_
