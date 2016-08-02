#include "RefType.h"

#include "Formatter.h"

namespace android {

RefType::RefType(const char *name, Type *type)
    : NamedType(name),
      mReferencedType(type) {
}

const Type *RefType::referencedType() const {
    return mReferencedType;
}

void RefType::dump(Formatter &out) const {
    out << name();
}

std::string RefType::getCppType(StorageMode mode, std::string *extra) const {
    return mReferencedType->getCppType(mode, extra);
}

void RefType::emitReaderWriter(
        Formatter &out,
        const std::string &name,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode) const {
    mReferencedType->emitReaderWriter(
            out, name, parcelObj, parcelObjIsPointer, isReader, mode);
}

void RefType::emitReaderWriterEmbedded(
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

