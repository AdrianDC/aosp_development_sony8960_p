#ifndef METHOD_H_

#define METHOD_H_

#include <android-base/macros.h>
#include <string>
#include <utils/Vector.h>

namespace android {

struct Formatter;
struct Type;

struct TypedVar {
    TypedVar(const char *name, Type *type);

    void dump(Formatter &out) const;

private:
    std::string mName;
    Type *mType;

    DISALLOW_COPY_AND_ASSIGN(TypedVar);
};

struct Method {
    Method(const char *name,
           Vector<TypedVar *> *args,
           Vector<TypedVar *> *results = NULL);

    void dump(Formatter &out) const;

private:
    std::string mName;
    Vector<TypedVar *> *mArgs;
    Vector<TypedVar *> *mResults;

    DISALLOW_COPY_AND_ASSIGN(Method);
};

}  // namespace android

#endif  // METHOD_H_

