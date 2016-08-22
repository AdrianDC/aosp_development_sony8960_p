#include "NamedType.h"

namespace android {

NamedType::NamedType(const char *localName)
    : mLocalName(localName) {
}

bool NamedType::isNamedType() const {
    return true;
}

void NamedType::setFullName(const FQName &fullName) {
    mFullName = fullName;
}

const FQName &NamedType::fqName() const {
    return mFullName;
}

std::string NamedType::localName() const {
    return mLocalName;
}

std::string NamedType::fullName() const {
    return mFullName.cppName();
}

std::string NamedType::fullJavaName() const {
    return mFullName.javaName();
}

}  // namespace android

