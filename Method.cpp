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

Method::Method(
        const char *name,
        Vector<TypedVar *> *args,
        Vector<TypedVar *> *results)
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

        mArgs->itemAt(i)->dump(out);
    }

    out << ")";

    if (mResults != NULL) {
        out << " generates (";

        for (size_t i = 0; i < mResults->size(); ++i) {
            if (i > 0) {
                out << ", ";
            }

            mResults->itemAt(i)->dump(out);
        }

        out << ")";
    }

    out << ";";
}

}  // namespace android

