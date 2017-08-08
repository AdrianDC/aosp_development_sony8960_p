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
#include <memory>
#include <string>

#include "Reference.h"
#include "ScalarType.h"

namespace android {

struct LocalIdentifier;

struct LiteralConstantExpression;
struct UnaryConstantExpression;
struct BinaryConstantExpression;
struct TernaryConstantExpression;
struct ReferenceConstantExpression;

/**
 * A constant expression is represented by a tree.
 */
struct ConstantExpression {
    static std::unique_ptr<ConstantExpression> Zero(ScalarType::Kind kind);
    static std::unique_ptr<ConstantExpression> One(ScalarType::Kind kind);
    static std::unique_ptr<ConstantExpression> ValueOf(ScalarType::Kind kind, uint64_t value);

    /*
     * Runs recursive evaluation.
     * Provides sort of lazy computation,
     * mainly used for forward identifier reference.
     */
    virtual void evaluate() = 0;

    /* Returns true iff the value has already been evaluated. */
    bool isEvaluated() const;
    /* Evaluated result in a string form. */
    std::string value() const;
    /* Evaluated result in a string form. */
    std::string cppValue() const;
    /* Evaluated result in a string form. */
    std::string javaValue() const;
    /* Evaluated result in a string form, with given contextual kind. */
    std::string value(ScalarType::Kind castKind) const;
    /* Evaluated result in a string form, with given contextual kind. */
    std::string cppValue(ScalarType::Kind castKind) const;
    /* Evaluated result in a string form, with given contextual kind. */
    std::string javaValue(ScalarType::Kind castKind) const;
    /* Formatted expression with type. */
    const std::string& description() const;
    /* See mTrivialDescription */
    bool descriptionIsTrivial() const;
    /* Return a ConstantExpression that is 1 plus the original. */
    std::unique_ptr<ConstantExpression> addOne(ScalarType::Kind baseKind);

    size_t castSizeT() const;

   private:
    /* If the result value has been evaluated. */
    bool mIsEvaluated = false;

    /* The formatted expression. */
    std::string mExpr;
    /* The kind of the result value. */
    ScalarType::Kind mValueKind;
    /* The stored result value. */
    uint64_t mValue;
    /* true if description() does not offer more information than value(). */
    bool mTrivialDescription = false;

    /*
     * Helper function for all cpp/javaValue methods.
     * Returns a plain string (without any prefixes or suffixes, just the
     * digits) converted from mValue.
     */
    std::string rawValue(ScalarType::Kind castKind) const;

    /*
     * Return the value casted to the given type.
     * First cast it according to mValueKind, then cast it to T.
     * Assumes !containsIdentifiers()
     */
    template <typename T>
    T cast() const;

    friend struct LiteralConstantExpression;
    friend struct UnaryConstantExpression;
    friend struct BinaryConstantExpression;
    friend struct TernaryConstantExpression;
    friend struct ReferenceConstantExpression;
};

struct LiteralConstantExpression : public ConstantExpression {
    LiteralConstantExpression(ScalarType::Kind kind, uint64_t value);
    LiteralConstantExpression(const std::string& value);
    void evaluate() override;
};

struct UnaryConstantExpression : public ConstantExpression {
    UnaryConstantExpression(const std::string& op, ConstantExpression* value);
    void evaluate() override;

   private:
    ConstantExpression* const value;
    std::string op;
};

struct BinaryConstantExpression : public ConstantExpression {
    BinaryConstantExpression(ConstantExpression* lval, const std::string& op,
                             ConstantExpression* rval);
    void evaluate() override;

   private:
    ConstantExpression* const lval;
    ConstantExpression* const rval;
    const std::string op;
};

struct TernaryConstantExpression : public ConstantExpression {
    TernaryConstantExpression(ConstantExpression* cond, ConstantExpression* trueVal,
                              ConstantExpression* falseVal);
    void evaluate() override;

   private:
    ConstantExpression* const cond;
    ConstantExpression* const trueVal;
    ConstantExpression* const falseVal;
};

struct ReferenceConstantExpression : public ConstantExpression {
    ReferenceConstantExpression(const Reference<LocalIdentifier>& value, const std::string& expr);
    void evaluate() override;

   private:
    Reference<LocalIdentifier> value;
};

}  // namespace android

#endif  // CONSTANT_EXPRESSION_H_
