#ifndef METHOD_H_

#define METHOD_H_

#include <android-base/macros.h>
#include <string>
#include <utils/KeyedVector.h>
#include <vector>

namespace android {

struct Annotation;
struct Formatter;
struct Type;
struct TypedVar;

struct Method {
    Method(const char *name,
           std::vector<TypedVar *> *args,
           std::vector<TypedVar *> *results,
           KeyedVector<std::string, Annotation *> *annotations);

    std::string name() const;
    const std::vector<TypedVar *> &args() const;
    const std::vector<TypedVar *> &results() const;
    const KeyedVector<std::string, Annotation *> &annotations() const;

    static std::string GetSignature(const std::vector<TypedVar *> &args);
    static std::string GetJavaSignature(const std::vector<TypedVar *> &args);

    void dumpAnnotations(Formatter &out) const;

private:
    std::string mName;
    std::vector<TypedVar *> *mArgs;
    std::vector<TypedVar *> *mResults;
    KeyedVector<std::string, Annotation *> *mAnnotationsByName;

    DISALLOW_COPY_AND_ASSIGN(Method);
};

struct TypedVar {
    TypedVar(const char *name, Type *type);

    std::string name() const;
    const Type &type() const;

private:
    std::string mName;
    Type *mType;

    DISALLOW_COPY_AND_ASSIGN(TypedVar);
};

}  // namespace android

#endif  // METHOD_H_

