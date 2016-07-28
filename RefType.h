#ifndef REF_TYPE_H_

#define REF_TYPE_H_

#include "NamedType.h"

namespace android {

struct RefType : public NamedType {
    RefType(const char *name, Type *type);

    void dump(Formatter &out) const override;

    const Type *referencedType() const;

private:
    Type *mReferencedType;

    DISALLOW_COPY_AND_ASSIGN(RefType);
};

}  // namespace android

#endif  // REF_TYPE_H_

