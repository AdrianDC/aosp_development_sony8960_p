#include "RefType.h"

#include "Formatter.h"

namespace android {

RefType::RefType(const char *name, Type *type)
    : NamedType(name),
      mReferencedType(type) {
}

const Type *RefType::referencedType() const {
    return mReferencedType;
}

void RefType::dump(Formatter &out) const {
    out << name();
}

}  // namespace android

