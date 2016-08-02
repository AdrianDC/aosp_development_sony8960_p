#include "Type.h"

#include "Formatter.h"

#include <android-base/logging.h>

namespace android {

Type::Type() {}
Type::~Type() {}

bool Type::isScope() const {
    return false;
}

bool Type::isInterface() const {
    return false;
}

const ScalarType *Type::resolveToScalarType() const {
    return NULL;
}

std::string Type::getCppType(StorageMode, std::string *) const {
    CHECK(!"Should not be here");
    return std::string();
}

void Type::emitReaderWriter(
        Formatter &,
        const std::string &,
        const std::string &,
        bool,
        bool,
        ErrorMode) const {
    CHECK(!"Should not be here");
}

void Type::emitReaderWriterEmbedded(
        Formatter &,
        const std::string &,
        bool,
        const std::string &,
        bool,
        bool,
        ErrorMode,
        const std::string &,
        const std::string &) const {
    CHECK(!"Should not be here");
}

void Type::handleError(Formatter &out, ErrorMode mode) const {
    switch (mode) {
        case ErrorMode_Ignore:
        {
            out << "/* _aidl_err ignored! */\n\n";
            break;
        }

        case ErrorMode_Goto:
        {
            out << "if (_aidl_err != ::android::OK) { goto _aidl_error; }\n\n";
            break;
        }

        case ErrorMode_Break:
        {
            out << "if (_aidl_err != ::android::OK) { break; }\n\n";
            break;
        }

        case ErrorMode_Return:
        {
            out << "if (_aidl_err != ::android::OK) { return _aidl_err; }\n\n";
            break;
        }
    }
}

void Type::handleError2(Formatter &out, ErrorMode mode) const {
    switch (mode) {
        case ErrorMode_Goto:
        {
            out << "goto _aidl_error;\n";
            break;
        }

        case ErrorMode_Break:
        {
            out << "break;\n";
            break;
        }

        case ErrorMode_Ignore:
        {
            out << "/* ignoring _aidl_error! */";
            break;
        }

        case ErrorMode_Return:
        {
            out << "return _aidl_err;\n";
            break;
        }
    }
}

void Type::emitReaderWriterEmbeddedForTypeName(
        Formatter &out,
        const std::string &name,
        bool nameIsPointer,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode,
        const std::string &parentName,
        const std::string &offsetText,
        const std::string &typeName,
        const std::string &childName) const {
    const std::string parcelObjDeref =
        parcelObjIsPointer ? ("*" + parcelObj) : parcelObj;

    const std::string parcelObjPointer =
        parcelObjIsPointer ? parcelObj : ("&" + parcelObj);

    const std::string nameDeref = name + (nameIsPointer ? "->" : ".");
    const std::string namePointer = nameIsPointer ? name : ("&" + name);

    out << "_aidl_err = ";

    if (isReader) {
        out << "const_cast<"
            << typeName
            << " *>("
            << namePointer
            << ")->readEmbeddedFromParcel(\n";
    } else {
        out << nameDeref
            << "writeEmbeddedToParcel(\n";
    }

    out.indent();
    out.indent();

    out << (isReader ? parcelObjDeref : parcelObjPointer)
        << ",\n"
        << parentName
        << ",\n"
        << offsetText;

    if (!childName.empty()) {
        out << ", &"
            << childName;
    }

    out << ");\n\n";

    out.unindent();
    out.unindent();

    handleError(out, mode);
}

status_t Type::emitTypeDeclarations(Formatter &) const {
    return OK;
}

status_t Type::emitTypeDefinitions(
        Formatter &, const std::string) const {
    return OK;
}

bool Type::needsEmbeddedReadWrite() const {
    return false;
}

bool Type::resultNeedsDeref() const {
    return false;
}

std::string Type::getCppType(std::string *extra) const {
    return getCppType(StorageMode_Stack, extra);
}

std::string Type::getCppResultType(std::string *extra) const {
    return getCppType(StorageMode_Result, extra);
}

std::string Type::getCppArgumentType(std::string *extra) const {
    return getCppType(StorageMode_Argument, extra);
}

}  // namespace android

