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
#include "ScalarType.h"

namespace android {

/**
 * A constant expression is represented by a tree.
 */
struct ConstantExpression {

    enum ConstExprType {
        kConstExprLiteral,
        kConstExprUnknown,
        kConstExprUnary,
        kConstExprBinary,
        kConstExprTernary
    };

    /* Literals, identifiers */
    ConstantExpression(const char *value, ConstExprType type);
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
    const char *expr() const;
    /* Evaluated result in a string form. */
    const char *value() const;
    /* Evaluated result in a string form, with given contextual kind. */
    const char *cppValue(ScalarType::Kind castKind) const;
    /* Original expression with type. */
    const char *description() const;

private:
    /* The formatted expression. */
    std::string mFormatted;
    /* The type of the expression. Hints on its original form. */
    ConstExprType mType;
    /* The kind of the result value.
     * Is valid only when mType != kConstExprUnknown. */
    ScalarType::Kind mValueKind;
    /* The stored result value.
     * Is valid only when mType != kConstExprUnknown. */
    uint64_t mValue;
    /* Return a signed int64_t value.
     * First cast it according to mValueKind, then cast it to int64_t. */
    int64_t int64Value() const;
    /* Helper function for value(ScalarType::Kind) */
    const char *value0(ScalarType::Kind castKind) const;

    DISALLOW_COPY_AND_ASSIGN(ConstantExpression);
};

}  // namespace android

#endif  // CONSTANT_EXPRESSION_H_
