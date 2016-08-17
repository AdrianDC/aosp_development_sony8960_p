#include "ConstantExpression.h"

#include <utils/String8.h>

namespace android {

ConstantExpression::ConstantExpression(const char *value)
    : mValue(value) {
}

ConstantExpression::ConstantExpression(const ConstantExpression *lval, const char *op, const ConstantExpression* rval)
    : mValue(android::String8::format("(%s %s %s)", lval->c_str(), op, rval->c_str()).string()) {
}
ConstantExpression::ConstantExpression(const char *op, const ConstantExpression *value)
    : mValue(android::String8::format("(%s%s)", op, value->c_str()).string()) {
}
ConstantExpression::ConstantExpression(const ConstantExpression *cond,
                                       const ConstantExpression *trueVal,
                                       const ConstantExpression *falseVal)
    : mValue(android::String8::format("(%s?%s:%s)",
             cond->c_str(), trueVal->c_str(), falseVal->c_str()).string()) {
}

const char *ConstantExpression::c_str() const {
    return mValue.c_str();
}
const char *ConstantExpression::value() const {
    return mValue.c_str();
}

}  // namespace android

