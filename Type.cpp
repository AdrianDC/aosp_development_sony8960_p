#include "Type.h"

namespace android {

Type::Type() {}
Type::~Type() {}

bool Type::isScope() const {
    return false;
}

}  // namespace android

