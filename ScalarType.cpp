#include "ScalarType.h"

#include "Formatter.h"

namespace android {

ScalarType::ScalarType(Kind kind)
    : mKind(kind) {
}

const ScalarType *ScalarType::resolveToScalarType() const {
    return this;
}

bool ScalarType::isValidEnumStorageType() const {
    // Only integer types.
    return mKind >= KIND_INT8 && mKind <= KIND_UINT64;
}

std::string ScalarType::getCppType(StorageMode, std::string *extra) const {
    static const char *const kName[] = {
        "bool",
        "void *",
        "int8_t",
        "uint8_t",
        "int16_t",
        "uint16_t",
        "int32_t",
        "uint32_t",
        "int64_t",
        "uint64_t",
        "float",
        "double"
    };

    extra->clear();

    return kName[mKind];
}

std::string ScalarType::getJavaType() const {
    static const char *const kName[] = {
        "boolean",
        "long",
        "byte",
        "byte",
        "short",
        "short",
        "int",
        "int",
        "long",
        "long",
        "float",
        "double"
    };

    return kName[mKind];
}

std::string ScalarType::getJavaWrapperType() const {
    static const char *const kName[] = {
        "Boolean",
        "Long",
        "Byte",
        "Byte",
        "Short",
        "Short",
        "Int",
        "Int",
        "Long",
        "Long",
        "Float",
        "Double"
    };

    return kName[mKind];
}

std::string ScalarType::getJavaSuffix() const {
    static const char *const kSuffix[] = {
        "Bool",
        "Pointer",
        "Int8",
        "Int8",
        "Int16",
        "Int16",
        "Int32",
        "Int32",
        "Int64",
        "Int64",
        "Float",
        "Double"
    };

    return kSuffix[mKind];
}

void ScalarType::emitReaderWriter(
        Formatter &out,
        const std::string &name,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode) const {
    emitReaderWriterWithCast(
            out,
            name,
            parcelObj,
            parcelObjIsPointer,
            isReader,
            mode,
            false /* needsCast */);
}

void ScalarType::emitReaderWriterWithCast(
        Formatter &out,
        const std::string &name,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode,
        bool needsCast) const {
    static const char *const kSuffix[] = {
        "Bool",
        "Pointer",
        "Int8",
        "Uint8",
        "Int16",
        "Uint16",
        "Int32",
        "Uint32",
        "Int64",
        "Uint64",
        "Float",
        "Double"
    };

    const std::string parcelObjDeref =
        parcelObj + (parcelObjIsPointer ? "->" : ".");

    out << "_hidl_err = "
        << parcelObjDeref
        << (isReader ? "read" : "write")
        << kSuffix[mKind]
        << "(";

    if (needsCast) {
        std::string extra;

        out << "("
            << Type::getCppType(&extra)
            << (isReader ? " *)" : ")");
    }

    if (isReader) {
        out << "&";
    }

    out << name
        << ");\n";

    handleError(out, mode);
}

void ScalarType::emitJavaFieldReaderWriter(
        Formatter &out,
        const std::string &blobName,
        const std::string &fieldName,
        const std::string &offset,
        bool isReader) const {
    if (isReader) {
        out << fieldName
            << " = "
            << blobName
            << ".get"
            << getJavaSuffix()
            << "("
            << offset
            << ");\n";

        return;
    }

    out << blobName
        << ".put"
        << getJavaSuffix()
        << "("
        << offset
        << ", "
        << fieldName
        << ");\n";
}

status_t ScalarType::emitVtsTypeDeclarations(Formatter &out) const {
    std::string extra;
    out << "type: TYPE_SCALAR\n"
        << "scalar_type: "
        << getCppType(StorageMode_Stack, &extra)
        << "\n";
    return OK;
}

void ScalarType::getAlignmentAndSize(size_t *align, size_t *size) const {
    static const size_t kAlign[] = {
        1,  // bool, this is NOT standardized!
        8,  // void *, 64-bit mode
        1,  // int8_t
        1,  // uint8_t
        2,  // int16_t
        2,  // uint16_t
        4,  // int32_t
        4,  // uint32_t
        8,  // int64_t
        8,  // uint64_t
        4,  // float
        8   // double
    };

    *align = *size = kAlign[mKind];
}

}  // namespace android

