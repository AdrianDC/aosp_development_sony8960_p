#include "ArrayType.h"

#include "Formatter.h"

namespace android {

ArrayType::ArrayType(Type *elementType, const char *dimension)
    : mElementType(elementType),
      mDimension(dimension) {
}

void ArrayType::dump(Formatter &out) const {
    mElementType->dump(out);
    out << "[" << mDimension << "]";
}

}  // namespace android

