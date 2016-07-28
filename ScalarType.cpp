#include "ScalarType.h"

#include "Formatter.h"

namespace android {

ScalarType::ScalarType(Kind kind)
    : mKind(kind) {
}

void ScalarType::dump(Formatter &out) const {
    static const char *const kName[] = {
        "char", "bool", "opaque", "int8_t", "uint8_t", "int16_t", "uint16_t",
        "int32_t", "uint32_t", "int64_t", "uint64_t", "float", "double"
    };
    out << kName[mKind];
}

}  // namespace android

