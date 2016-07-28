#ifndef NAMED_TYPE_H_

#define NAMED_TYPE_H_

#include "Type.h"

#include <string>

namespace android {

struct NamedType : public Type {
    NamedType(const char *name);

    std::string name() const;

private:
    std::string mName;

    DISALLOW_COPY_AND_ASSIGN(NamedType);
};

}  // namespace android

#endif  // NAMED_TYPE_H_

