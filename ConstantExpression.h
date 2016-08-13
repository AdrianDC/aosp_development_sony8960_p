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
