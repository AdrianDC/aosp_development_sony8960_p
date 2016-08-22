#include "TypeDef.h"

#include "Formatter.h"

#include <android-base/logging.h>

namespace android {

TypeDef::TypeDef(Type *type)
    : Type(),
      mReferencedType(type) {
}

const ScalarType *TypeDef::resolveToScalarType() const {
    CHECK(!"Should not be here");
    return NULL;
}

Type *TypeDef::referencedType() const {
    return mReferencedType;
}

bool TypeDef::isInterface() const {
    return false;
}

bool TypeDef::isEnum() const {
    CHECK(!"Should not be here");
    return false;
}

bool TypeDef::isTypeDef() const {
    return true;
}

std::string TypeDef::getJavaType() const {
    CHECK(!"Should not be here");
    return std::string();
}

bool TypeDef::needsEmbeddedReadWrite() const {
    CHECK(!"Should not be here");
    return false;
}

bool TypeDef::resultNeedsDeref() const {
    CHECK(!"Should not be here");
    return false;
}

}  // namespace android

