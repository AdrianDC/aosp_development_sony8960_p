#ifndef METHOD_H_

#define METHOD_H_

#include <android-base/macros.h>
#include <string>
#include <vector>

namespace android {

struct Formatter;
struct Type;

struct TypedVar {
    TypedVar(const char *name, Type *type);

    std::string name() const;
    const Type &type() const;

private:
    std::string mName;
    Type *mType;

    DISALLOW_COPY_AND_ASSIGN(TypedVar);
};

struct Method {
    Method(const char *name,
           std::vector<TypedVar *> *args,
           std::vector<TypedVar *> *results = NULL);

    std::string name() const;
    const std::vector<TypedVar *> &args() const;
    const std::vector<TypedVar *> &results() const;

    static std::string GetSignature(const std::vector<TypedVar *> &args);

private:
    std::string mName;
    std::vector<TypedVar *> *mArgs;
    std::vector<TypedVar *> *mResults;

    DISALLOW_COPY_AND_ASSIGN(Method);
};

}  // namespace android

#endif  // METHOD_H_

