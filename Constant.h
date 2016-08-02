#ifndef CONSTANT_H_

#define CONSTANT_H_

#include <android-base/macros.h>
#include <string>

namespace android {

struct Formatter;
struct Type;

struct Constant {
    Constant(const char *name, Type *type, const char *value);

    std::string name() const;
    const Type *type() const;
    std::string value() const;

private:
    std::string mName;
    Type *mType;
    std::string mValue;

    DISALLOW_COPY_AND_ASSIGN(Constant);
};

}  // namespace android

#endif  // CONSTANT_H_
