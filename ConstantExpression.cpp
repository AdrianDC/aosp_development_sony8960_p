/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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

