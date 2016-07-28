#include "StringType.h"

#include "Formatter.h"

namespace android {

StringType::StringType() {}

void StringType::dump(Formatter &out) const {
    out << "string";
}

}  // namespace android

