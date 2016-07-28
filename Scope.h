#ifndef SCOPE_H_

#define SCOPE_H_

#include "NamedType.h"

#include <utils/KeyedVector.h>
#include <utils/Vector.h>

namespace android {

struct Constant;

struct Scope : public NamedType {
    Scope(const char *name);

    bool addType(NamedType *type);
    Type *lookupType(const char *name) const;

    bool addConstant(Constant *constant);

    void dump(Formatter &out) const override;

private:
    Vector<Type *> mTypes;
    KeyedVector<std::string, size_t> mTypeIndexByName;

    KeyedVector<std::string, Constant *> mConstants;

    DISALLOW_COPY_AND_ASSIGN(Scope);
};

}  // namespace android

#endif  // SCOPE_H_

