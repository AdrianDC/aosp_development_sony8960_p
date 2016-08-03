#ifndef COORDINATOR_H_

#define COORDINATOR_H_

#include <android-base/macros.h>
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

private:
    std::string mInterfacesPath;
    KeyedVector<FQName, AST *> mCache;

    DISALLOW_COPY_AND_ASSIGN(Coordinator);
};

}  // namespace android

#endif  // COORDINATOR_H_
