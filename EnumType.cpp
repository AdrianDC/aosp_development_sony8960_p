#include "EnumType.h"

#include "Formatter.h"
#include "ScalarType.h"

#include <android-base/logging.h>

namespace android {

EnumType::EnumType(
        std::vector<EnumValue *> *values, Type *storageType)
    : mValues(values),
      mStorageType(
              storageType != NULL
                ? storageType
                : new ScalarType(ScalarType::KIND_INT32)) {
}

const ScalarType *EnumType::resolveToScalarType() const {
    return mStorageType->resolveToScalarType();
}

std::string EnumType::getCppType(StorageMode, std::string *extra) const {
    extra->clear();

    return fullName();
}

void EnumType::emitReaderWriter(
        Formatter &out,
        const std::string &name,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode) const {
    const ScalarType *scalarType = mStorageType->resolveToScalarType();
    CHECK(scalarType != NULL);

    scalarType->emitReaderWriterWithCast(
            out,
            name,
            parcelObj,
            parcelObjIsPointer,
            isReader,
            mode,
            true /* needsCast */);
}

status_t EnumType::emitTypeDeclarations(Formatter &out) const {
    const ScalarType *scalarType = mStorageType->resolveToScalarType();
    CHECK(scalarType != NULL);

    std::string extra;

    out << "enum class "
        << localName()
        << " : "
        << ((Type *)scalarType)->getCppType(&extra)
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

////////////////////////////////////////////////////////////////////////////////

EnumValue::EnumValue(const char *name, const char *value)
    : mName(name),
      mValue(value) {
}

std::string EnumValue::name() const {
    return mName;
}

const char *EnumValue::value() const {
    return mValue;
}

}  // namespace android

