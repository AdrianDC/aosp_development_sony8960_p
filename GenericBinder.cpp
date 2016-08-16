#include "GenericBinder.h"

#include "Formatter.h"

namespace android {

GenericBinder::GenericBinder() {}

bool GenericBinder::isBinder() const {
    return true;
}

std::string GenericBinder::getCppType(
        StorageMode mode, std::string *extra) const {
    extra->clear();
    const std::string base = "::android::sp<::android::hardware::IBinder>";

    switch (mode) {
        case StorageMode_Stack:
        case StorageMode_Result:
            return base;

        case StorageMode_Argument:
            return "const " + base + "&";
    }
}

std::string GenericBinder::getJavaType() const {
    return "IHwBinder";
}

void GenericBinder::emitReaderWriter(
        Formatter &out,
        const std::string &name,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode) const {
    const std::string parcelObjDeref =
        parcelObj + (parcelObjIsPointer ? "->" : ".");

    if (isReader) {
        out << "_hidl_err = ";
        out << parcelObjDeref
            << "readNullableStrongBinder(&"
            << name
            << ");\n";

        handleError(out, mode);
    } else {
        out << "_hidl_err = ";
        out << parcelObjDeref
            << "writeStrongBinder("
            << name
            << ");\n";

        handleError(out, mode);
    }
}

void GenericBinder::emitJavaReaderWriter(
        Formatter &out,
        const std::string &parcelObj,
        const std::string &argName,
        bool isReader) const {
    if (isReader) {
        out << parcelObj
            << ".readStrongBinder());\n";
    } else {
        out << parcelObj
            << ".writeStrongBinder("
            << argName
            << " == null ? null : "
            << argName
            << ");\n";
    }
}

status_t GenericBinder::emitVtsAttributeType(Formatter &) const {
    return UNKNOWN_ERROR;
}

}  // namespace android
