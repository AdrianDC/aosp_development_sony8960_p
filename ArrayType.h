#ifndef ARRAY_TYPE_H_

#define ARRAY_TYPE_H_

#include "Type.h"

#include <string>

namespace android {

struct ArrayType : public Type {
    ArrayType(Type *elementType, const char *dimension);

    void dump(Formatter &out) const override;

private:
    Type *mElementType;
    std::string mDimension;

    DISALLOW_COPY_AND_ASSIGN(ArrayType);
};

}  // namespace android

#endif  // ARRAY_TYPE_H_

