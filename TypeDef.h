#ifndef TYPE_DEF_H_

#define TYPE_DEF_H_

#include "NamedType.h"

namespace android {

struct TypeDef : public NamedType {
    TypeDef(const char *name, Type *type);

    void dump(Formatter &out) const override;

    const Type *referencedType() const;

private:
    Type *mReferencedType;

    DISALLOW_COPY_AND_ASSIGN(TypeDef);
};

}  // namespace android

#endif  // TYPE_DEF_H_

