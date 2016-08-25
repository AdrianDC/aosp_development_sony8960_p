#include "StringType.h"

#include "Formatter.h"

namespace android {

StringType::StringType() {}

std::string StringType::getCppType(StorageMode mode, std::string *extra) const {
    extra->clear();

    const std::string base = "::android::hardware::hidl_string";

    switch (mode) {
        case StorageMode_Stack:
            return base;

        case StorageMode_Argument:
            return "const " + base + "&";

        case StorageMode_Result:
            return "const " + base + "*";
    }
}

std::string StringType::getJavaType() const {
    return "String";
}

std::string StringType::getJavaSuffix() const {
    return "String";
}

void StringType::emitReaderWriter(
        Formatter &out,
        const std::string &name,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode) const {
    const std::string parentName = "_hidl_" + name + "_parent";
    out << "size_t " << parentName << ";\n\n";

    const std::string parcelObjDeref =
        parcelObj + (parcelObjIsPointer ? "->" : ".");

    if (isReader) {
        out << name
            << " = (const ::android::hardware::hidl_string *)"
            << parcelObjDeref
            << "readBuffer("
            << "&"
            << parentName
            << ");\n";

        out << "if ("
            << name
            << " == nullptr) {\n";

        out.indent();

        out << "_hidl_err = ::android::UNKNOWN_ERROR;\n";
        handleError2(out, mode);

        out.unindent();
        out << "}\n\n";
    } else {
        out << "_hidl_err = "
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
            "::android::hardware::hidl_string",
            "" /* childName */);
}

void StringType::emitJavaFieldInitializer(
        Formatter &out, const std::string &fieldName) const {
    out << "String "
        << fieldName
        << " = new String();\n";
}

void StringType::emitJavaFieldReaderWriter(
        Formatter &out,
        const std::string &blobName,
        const std::string &fieldName,
        const std::string &offset,
        bool isReader) const {
    if (isReader) {
        out << "\nparcel.readEmbeddedBuffer(\n";

        out.indent();
        out.indent();

        out << blobName
            << ".handle(),\n"
            << offset
            << " + 0 /* offsetof(hidl_string, mBuffer) */);\n\n";

        out.unindent();
        out.unindent();

        out << fieldName
            << " = "
            << blobName
            << ".getString("
            << offset
            << ");\n";

        return;
    }

    out << blobName
        << ".putString("
        << offset
        << ", "
        << fieldName
        << ");\n";
}

bool StringType::needsEmbeddedReadWrite() const {
    return true;
}

bool StringType::resultNeedsDeref() const {
    return true;
}

status_t StringType::emitVtsTypeDeclarations(Formatter &out) const {
    out << "type: TYPE_STRING\n";
    return OK;
}

void StringType::getAlignmentAndSize(size_t *align, size_t *size) const {
    *align = 8;  // hidl_string
    *size = 24;
}

}  // namespace android

