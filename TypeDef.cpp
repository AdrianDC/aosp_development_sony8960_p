#include "TypeDef.h"

#include "Formatter.h"

namespace android {

TypeDef::TypeDef(const char *name, Type *type)
    : NamedType(name),
      mReferencedType(type) {
}

const ScalarType *TypeDef::resolveToScalarType() const {
    return mReferencedType->resolveToScalarType();
}

const Type *TypeDef::referencedType() const {
    return mReferencedType;
}

std::string TypeDef::getCppType(StorageMode mode, std::string *extra) const {
    return mReferencedType->getCppType(mode, extra);
}

void TypeDef::emitReaderWriter(
        Formatter &out,
        const std::string &name,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode) const {
    mReferencedType->emitReaderWriter(
            out, name, parcelObj, parcelObjIsPointer, isReader, mode);
}

void TypeDef::emitReaderWriterEmbedded(
        Formatter &out,
        const std::string &name,
        bool nameIsPointer,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode,
        const std::string &parentName,
        const std::string &offsetText) const {
    mReferencedType->emitReaderWriterEmbedded(
            out,
            name,
            nameIsPointer,
            parcelObj,
            parcelObjIsPointer,
            isReader,
            mode,
            parentName,
            offsetText);
}

}  // namespace android

