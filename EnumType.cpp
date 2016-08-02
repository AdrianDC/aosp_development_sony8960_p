#include "EnumType.h"

#include "Formatter.h"
#include "ScalarType.h"

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

std::string EnumValue::name() const {
    return mName;
}

const char *EnumValue::value() const {
    return mValue;
}

EnumType::EnumType(
        const char *name, std::vector<EnumValue *> *values, Type *storageType)
    : NamedType(name),
      mValues(values),
      mStorageType(
              storageType != NULL
                ? storageType
                : new ScalarType(ScalarType::KIND_INT32)) {
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
        (*mValues)[i]->dump(out);
        out << "\n";
    }

    out.unindent();

    out << "};\n\n";
}

std::string EnumType::getCppType(StorageMode, std::string *extra) const {
    extra->clear();

    return name();
}

void EnumType::emitReaderWriter(
        Formatter &out,
        const std::string &name,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode) const {
    mStorageType->emitReaderWriter(
            out, name, parcelObj, parcelObjIsPointer, isReader, mode);
}

status_t EnumType::emitTypeDeclarations(Formatter &out) const {
    std::string extra;

    out << "enum class "
        << name()
        << " : "
        << mStorageType->getCppType(&extra)
        << " {\n";

    out.indent();

    for (const auto &entry : *mValues) {
        out << entry->name();

        const char *value = entry->value();
        if (value != NULL) {
            out << " = " << value;
        }

        out << ",\n";
    }

    out.unindent();
    out << "};\n\n";

    return OK;
}

}  // namespace android

