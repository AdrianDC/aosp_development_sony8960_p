#include "Method.h"

#include "Formatter.h"
#include "Type.h"

namespace android {

Method::Method(
        const char *name,
        std::vector<TypedVar *> *args,
        std::vector<TypedVar *> *results)
    : mName(name),
      mArgs(args),
      mResults(results) {
}

std::string Method::name() const {
    return mName;
}

const std::vector<TypedVar *> &Method::args() const {
    return *mArgs;
}

const std::vector<TypedVar *> &Method::results() const {
    return *mResults;
}

// static
std::string Method::GetSignature(const std::vector<TypedVar *> &args) {
    bool first = true;
    std::string out;
    for (const auto &arg : args) {
        if (!first) {
            out += ", ";
        }

        std::string extra;
        out += arg->type().getCppArgumentType(&extra);
        out += " ";
        out += arg->name();
        out += extra;

        first = false;
    }

    return out;
}

////////////////////////////////////////////////////////////////////////////////

TypedVar::TypedVar(const char *name, Type *type)
    : mName(name),
      mType(type) {
}

std::string TypedVar::name() const {
    return mName;
}

const Type &TypedVar::type() const {
    return *mType;
}

}  // namespace android

