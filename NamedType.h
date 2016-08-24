#ifndef NAMED_TYPE_H_

#define NAMED_TYPE_H_

#include "Type.h"

#include "FQName.h"

#include <string>

namespace android {

struct NamedType : public Type {
    NamedType(const char *localName);

    bool isNamedType() const override;

    void setFullName(const FQName &fullName);

    const FQName &fqName() const;
    std::string localName() const;
    std::string fullName() const;
    std::string fullJavaName() const;

private:
    std::string mLocalName;
    FQName mFullName;

    DISALLOW_COPY_AND_ASSIGN(NamedType);
};

}  // namespace android

#endif  // NAMED_TYPE_H_

