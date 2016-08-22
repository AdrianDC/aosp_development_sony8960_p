#include "NamedType.h"

namespace android {

NamedType::NamedType() {}

bool NamedType::isNamedType() const {
    return true;
}

void NamedType::setLocalName(const std::string &localName) {
    mLocalName = localName;
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

