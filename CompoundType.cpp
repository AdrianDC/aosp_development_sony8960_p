#include "CompoundType.h"

#include "Formatter.h"

namespace android {

CompoundField::CompoundField(const char *name, Type *type)
    : mName(name),
      mType(type) {
}

void CompoundField::dump(Formatter &out) const {
    mType->dump(out);

    out << " " << mName << ";";
}

CompoundType::CompoundType(Style style, const char *name)
    : Scope(name),
      mStyle(style),
      mFields(NULL) {
}

void CompoundType::setFields(Vector<CompoundField *> *fields) {
    mFields = fields;
}

void CompoundType::dump(Formatter &out) const {
    out << ((mStyle == STYLE_STRUCT) ? "struct" : "union")
        << " "
        << name()
        << " {\n";

    out.indent();

    Scope::dump(out);

    for (size_t i = 0; i < mFields->size(); ++i) {
        mFields->itemAt(i)->dump(out);
        out << "\n";
    }

    out.unindent();

    out << "};\n\n";
}

}  // namespace android

