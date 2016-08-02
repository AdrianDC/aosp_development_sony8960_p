#include "Constant.h"

#include "Formatter.h"
#include "Type.h"

namespace android {

Constant::Constant(const char *name, Type *type, const char *value)
    : mName(name),
      mType(type),
      mValue(value) {
}

std::string Constant::name() const {
    return mName;
}

const Type *Constant::type() const {
    return mType;
}

std::string Constant::value() const {
    return mValue;
}

}  // namespace android

