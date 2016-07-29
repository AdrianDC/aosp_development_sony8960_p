#include "Type.h"

namespace android {

Type::Type() {}
Type::~Type() {}

bool Type::isScope() const {
    return false;
}

bool Type::isInterface() const {
    return false;
}

}  // namespace android

