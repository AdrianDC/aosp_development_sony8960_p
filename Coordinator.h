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
    Coordinator();
    ~Coordinator();

    AST *parse(const FQName &fqName);

    Type *lookupType(const FQName &fqName) const;

    static std::string GetPackagePath(const FQName &fqName);

private:
    KeyedVector<FQName, AST *> mCache;

    DISALLOW_COPY_AND_ASSIGN(Coordinator);
};

}  // namespace android

#endif  // COORDINATOR_H_
