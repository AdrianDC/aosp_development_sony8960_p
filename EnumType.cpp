#include "EnumType.h"

#include "Formatter.h"

namespace android {

EnumValue::EnumValue(const char *name, const char *value)
    : mName(name),
      mValue(value) {
}

void EnumValue::dump(Formatter &out) const {
    out << mName;

    if (mValue) {
        out << " = " << mValue;
    }

    out << ",";
}

EnumType::EnumType(
        const char *name, Vector<EnumValue *> *values, Type *storageType)
    : NamedType(name),
      mValues(values),
      mStorageType(storageType) {
}

void EnumType::dump(Formatter &out) const {
    out << "enum " << name() << " ";

    if (mStorageType) {
        out << ": ";
        mStorageType->dump(out);
        out << " ";
    }

    out << "{\n";
    out.indent();

    for (size_t i = 0; i < mValues->size(); ++i) {
        mValues->itemAt(i)->dump(out);
        out << "\n";
    }

    out.unindent();

    out << "};\n\n";
}

}  // namespace android

