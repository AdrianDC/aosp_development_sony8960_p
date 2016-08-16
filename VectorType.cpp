#include "VectorType.h"

#include "Formatter.h"

#include <android-base/logging.h>

namespace android {

VectorType::VectorType(Type *elementType)
    : mElementType(elementType) {
}

std::string VectorType::getCppType(StorageMode mode, std::string *extra) const {
    const std::string base =
        "::android::hardware::hidl_vec<"
        + mElementType->getCppType(extra)
        + ">";

    CHECK(extra->empty());

    switch (mode) {
        case StorageMode_Stack:
            return base;

        case StorageMode_Argument:
            return "const " + base + "&";

        case StorageMode_Result:
            return "const " + base + "*";
    }
}

std::string VectorType::getJavaType() const {
    return mElementType->getJavaType() + "[]";
}

std::string VectorType::getJavaSuffix() const {
    return mElementType->getJavaSuffix() + "Vector";
}

void VectorType::emitReaderWriter(
        Formatter &out,
        const std::string &name,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode) const {
    std::string baseExtra;
    std::string baseType = mElementType->getCppType(&baseExtra);

    const std::string parentName = "_hidl_" + name + "_parent";

    out << "size_t " << parentName << ";\n\n";

    const std::string parcelObjDeref =
        parcelObj + (parcelObjIsPointer ? "->" : ".");

    if (isReader) {
        out << name
            << " = (const ::android::hardware::hidl_vec<"
            << baseType
            << "> *)"
            << parcelObjDeref
            << "readBuffer(&"
            << parentName
            << ");\n\n";

        out << "if (" << name << " == nullptr) {\n";

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

void VectorType::emitReaderWriterEmbedded(
        Formatter &out,
        const std::string &name,
        bool nameIsPointer,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode,
        const std::string &parentName,
        const std::string &offsetText) const {
    std::string baseExtra;
    std::string baseType = Type::getCppType(&baseExtra);

    const std::string childName = "_hidl_" + name + "_child";
    out << "size_t " << childName << ";\n\n";

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
            baseType,
            childName);

    if (!mElementType->needsEmbeddedReadWrite()) {
        return;
    }

    const std::string nameDeref = name + (nameIsPointer ? "->" : ".");

    baseType = mElementType->getCppType(&baseExtra);

    out << "for (size_t _hidl_index = 0; _hidl_index < "
        << nameDeref
        << "size(); ++_hidl_index) {\n";

    out.indent();

    mElementType->emitReaderWriterEmbedded(
            out,
            (nameIsPointer ? "(*" + name + ")" : name) + "[_hidl_index]",
            false /* nameIsPointer */,
            parcelObj,
            parcelObjIsPointer,
            isReader,
            mode,
            childName,
            "_hidl_index * sizeof(" + baseType + ")");

    out.unindent();

    out << "}\n\n";
}

bool VectorType::needsEmbeddedReadWrite() const {
    return true;
}

bool VectorType::resultNeedsDeref() const {
    return true;
}

status_t VectorType::emitVtsTypeDeclarations(Formatter &out) const {
    out << "type: TYPE_VECTOR\n" << "vector_value: {\n";
    out.indent();
    status_t err = mElementType->emitVtsTypeDeclarations(out);
    if (err != OK) {
        return err;
    }
    out.unindent();
    out << "}\n";
    return OK;
}

status_t VectorType::emitVtsAttributeType(Formatter &out) const {
    out << "type: TYPE_VECTOR\n" << "vector_value: {\n";
    out.indent();
    status_t status = mElementType->emitVtsAttributeType(out);
    if (status != OK) {
        return status;
    }
    out.unindent();
    out << "}\n";
    return OK;
}

bool VectorType::isJavaCompatible() const {
    return mElementType->isJavaCompatible();
}

}  // namespace android

