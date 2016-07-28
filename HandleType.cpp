#include "HandleType.h"

#include "Formatter.h"

namespace android {

HandleType::HandleType() {}

void HandleType::dump(Formatter &out) const {
    out << "handle";
}

}  // namespace android

