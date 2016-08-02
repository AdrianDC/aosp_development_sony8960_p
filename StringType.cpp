#include "StringType.h"

#include "Formatter.h"

namespace android {

StringType::StringType() {}

std::string StringType::getCppType(StorageMode mode, std::string *extra) const {
    extra->clear();

    const std::string base = "::android::hidl::hidl_string";

    switch (mode) {
        case StorageMode_Stack:
            return base;

        case StorageMode_Argument:
            return "const " + base + "&";

        case StorageMode_Result:
            return "const " + base + "*";
    }
}

void StringType::emitReaderWriter(
        Formatter &out,
        const std::string &name,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode) const {
    const std::string parentName = "_aidl_" + name + "_parent";
    out << "size_t " << parentName << ";\n\n";

    const std::string parcelObjDeref =
        parcelObj + (parcelObjIsPointer ? "->" : ".");

    if (isReader) {
        out << name
            << " = (const ::android::hidl::hidl_string *)"
            << parcelObjDeref
            << "readBuffer("
            << "&"
            << parentName
            << ");\n";

        out << "if ("
            << name
            << " == nullptr) {\n";

        out.indent();

        out << "_aidl_err = ::android::UNKNOWN_ERROR;\n";
        handleError2(out, mode);

        out.unindent();
        out << "}\n\n";
    } else {
        out << "_aidl_err = "
            << parcelObjDeref
            << "writeBuffer(&"
            << name
            << ", sizeof("
            << name
            << "), &"
            << parentName
            << ");\n";

        handleError(out, mode);
    }

    emitReaderWriterEmbedded(
            out,
            name,
            isReader /* nameIsPointer */,
            parcelObj,
            parcelObjIsPointer,
            isReader,
            mode,
            parentName,
            "0 /* parentOffset */");
}

void StringType::emitReaderWriterEmbedded(
        Formatter &out,
        const std::string &name,
        bool nameIsPointer,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode,
        const std::string &parentName,
        const std::string &offsetText) const {
    emitReaderWriterEmbeddedForTypeName(
            out,
            name,
            nameIsPointer,
            parcelObj,
            parcelObjIsPointer,
            isReader,
            mode,
            parentName,
            offsetText,
            "::android::hidl::hidl_string",
            "" /* childName */);
}

bool StringType::needsEmbeddedReadWrite() const {
    return true;
}

bool StringType::resultNeedsDeref() const {
    return true;
}

}  // namespace android

