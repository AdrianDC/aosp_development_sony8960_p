#include "Annotation.h"

#include "Formatter.h"

#include <vector>

namespace android {

Annotation::Annotation(const char *name,AnnotationParamVector *params)
        : mName(name),
          mParamsByName(params) {
}

std::string Annotation::name() const {
    return mName;
}

const AnnotationParamVector &Annotation::params() const {
    return *mParamsByName;
}

void Annotation::dump(Formatter &out) const {
    out << "@" << mName;

    if (mParamsByName->size() == 0) {
        return;
    }

    out << "(";

    for (size_t i = 0; i < mParamsByName->size(); ++i) {
        if (i > 0) {
            out << ", ";
        }

        out << mParamsByName->keyAt(i) << "=";

        const std::vector<std::string> *param = mParamsByName->valueAt(i);
        if (param->size() > 1) {
            out << "{";
        }

        bool first = true;
        for (const auto &value : *param) {
            if (!first) {
                out << ", ";
            }

            out << value;

            first = false;
        }

        if (param->size() > 1) {
            out << "}";
        }
    }

    out << ")";
}

}  // namespace android

