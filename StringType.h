#ifndef STRING_TYPE_H_

#define STRING_TYPE_H_

#include "Type.h"

namespace android {

struct StringType : public Type {
    StringType();

    void dump(Formatter &out) const override;
};

}  // namespace android

#endif  // STRING_TYPE_H_

