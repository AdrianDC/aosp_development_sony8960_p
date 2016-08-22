#ifndef TYPE_DEF_H_

#define TYPE_DEF_H_

#include "Type.h"

namespace android {

struct TypeDef : public Type {
    TypeDef(Type *type);

    const ScalarType *resolveToScalarType() const override;

    Type *referencedType() const;

    bool isInterface() const override;
    bool isEnum() const override;
    bool isTypeDef() const override;

    std::string getJavaType() const override;

    bool needsEmbeddedReadWrite() const override;
    bool resultNeedsDeref() const override;

private:
    Type *mReferencedType;

    DISALLOW_COPY_AND_ASSIGN(TypeDef);
};

}  // namespace android

#endif  // TYPE_DEF_H_

