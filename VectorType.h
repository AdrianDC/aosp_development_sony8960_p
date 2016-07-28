#ifndef VECTOR_TYPE_H_

#define VECTOR_TYPE_H_

#include "Type.h"

namespace android {

struct VectorType : public Type {
    VectorType(Type *elementType);

    void dump(Formatter &out) const override;

private:
    Type *mElementType;

    DISALLOW_COPY_AND_ASSIGN(VectorType);
};

}  // namespace android

#endif  // VECTOR_TYPE_H_

