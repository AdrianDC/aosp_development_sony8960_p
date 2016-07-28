#include "NamedType.h"

namespace android {

NamedType::NamedType(const char *name)
    : mName(name) {
}

std::string NamedType::name() const {
    return mName;
}

}  // namespace android

