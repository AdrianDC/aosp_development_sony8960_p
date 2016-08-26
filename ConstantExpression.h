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

#ifndef CONSTANT_EXPRESSION_H_

#define CONSTANT_EXPRESSION_H_

#include <android-base/macros.h>
#include <string>

namespace android {


/**
 * A constant expression is represented by a tree.
 */
struct ConstantExpression {
    /* Literals, identifiers */
    ConstantExpression(const char *value);
    /* binary operations */
    ConstantExpression(const ConstantExpression *value1,
        const char *op, const ConstantExpression* value2);
    /* unary operations */
    ConstantExpression(const char *op, const ConstantExpression *value);
    /* ternary ?: */
    ConstantExpression(const ConstantExpression *cond,
                       const ConstantExpression *trueVal,
                       const ConstantExpression *falseVal);

    /* Original expression. */
    const char *c_str() const;
    /* Evaluated result. */
    const char *value() const;

private:
    std::string mValue;

    DISALLOW_COPY_AND_ASSIGN(ConstantExpression);
};

}  // namespace android

#endif  // CONSTANT_EXPRESSION_H_
