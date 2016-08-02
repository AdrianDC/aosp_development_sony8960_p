#include "Method.h"

#include "Formatter.h"
#include "Type.h"

namespace android {

TypedVar::TypedVar(const char *name, Type *type)
    : mName(name),
      mType(type) {
}

void TypedVar::dump(Formatter &out) const {
    mType->dump(out);
    out << " ";
    out << mName;
}

std::string TypedVar::name() const {
    return mName;
}

const Type &TypedVar::type() const {
    return *mType;
}


Method::Method(
        const char *name,
        std::vector<TypedVar *> *args,
        std::vector<TypedVar *> *results)
    : mName(name),
      mArgs(args),
      mResults(results) {
}

void Method::dump(Formatter &out) const {
    out << mName << "(";

    for (size_t i = 0; i < mArgs->size(); ++i) {
        if (i > 0) {
            out << ", ";
        }

        (*mArgs)[i]->dump(out);
    }

    out << ")";

    if (mResults != NULL) {
        out << " generates (";

        for (size_t i = 0; i < mResults->size(); ++i) {
            if (i > 0) {
                out << ", ";
            }

            (*mResults)[i]->dump(out);
        }

        out << ")";
    }

    out << ";";
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

}  // namespace android

