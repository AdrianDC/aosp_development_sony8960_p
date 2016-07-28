#ifndef HANDLE_TYPE_H_

#define HANDLE_TYPE_H_

#include "Type.h"

namespace android {

struct HandleType : public Type {
    HandleType();

    void dump(Formatter &out) const override;
};

}  // namespace android

#endif  // HANDLE_TYPE_H_

