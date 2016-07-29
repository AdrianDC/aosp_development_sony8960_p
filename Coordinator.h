#ifndef COORDINATOR_H_

#define COORDINATOR_H_

#include <android-base/macros.h>
#include <string>
#include <utils/KeyedVector.h>

namespace android {

struct AST;
struct FQName;
struct Type;

struct Coordinator {
    Coordinator(const std::string &interfacesPath);
    ~Coordinator();

    AST *parse(const FQName &fqName);

    Type *lookupType(const FQName &fqName) const;

    std::string getPackagePath(const FQName &fqName) const;

private:
    std::string mInterfacesPath;
    KeyedVector<FQName, AST *> mCache;

    DISALLOW_COPY_AND_ASSIGN(Coordinator);
};

}  // namespace android

#endif  // COORDINATOR_H_
