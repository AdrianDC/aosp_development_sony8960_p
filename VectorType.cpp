#include "VectorType.h"

#include "Formatter.h"

namespace android {

VectorType::VectorType(Type *elementType)
    : mElementType(elementType) {
}

void VectorType::dump(Formatter &out) const {
    out<< "vec<";
    mElementType->dump(out);
    out << ">";
}

}  // namespace android

