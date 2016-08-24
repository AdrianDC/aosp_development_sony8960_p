#include "CompoundType.h"

#include "Formatter.h"

#include <android-base/logging.h>

namespace android {

CompoundType::CompoundType(Style style, const char *localName)
    : Scope(localName),
      mStyle(style),
      mFields(NULL) {
}

bool CompoundType::setFields(
        std::vector<CompoundField *> *fields, std::string *errorMsg) {
    mFields = fields;

    for (const auto &field : *fields) {
        const Type &type = field->type();

        if (type.isBinder()) {
            *errorMsg =
                "Structs/Unions must not contain references to interfaces.";

            return false;
        }

        if (mStyle == STYLE_UNION) {
            if (type.needsEmbeddedReadWrite()) {
                // Can't have those in a union.

                *errorMsg =
                    "Unions must not contain any types that need fixup.";

                return false;
            }
        }
    }

    return true;
}

std::string CompoundType::getCppType(
        StorageMode mode, std::string *extra) const {
    extra->clear();
    const std::string base = fullName();

    switch (mode) {
        case StorageMode_Stack:
            return base;

        case StorageMode_Argument:
            return "const " + base + "&";

        case StorageMode_Result:
            return "const " + base + "*";
    }
}

void CompoundType::emitReaderWriter(
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
            << " = (const "
            << fullName()
            << " *)"
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

    if (mStyle != STYLE_STRUCT || !needsEmbeddedReadWrite()) {
        return;
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

void CompoundType::emitReaderWriterEmbedded(
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
            fullName(),
            "" /* childName */);
}

status_t CompoundType::emitTypeDeclarations(Formatter &out) const {
    out << ((mStyle == STYLE_STRUCT) ? "struct" : "union")
        << " "
        << localName()
        << " {\n";

    out.indent();

    Scope::emitTypeDeclarations(out);

    for (const auto &field : *mFields) {
        std::string extra;
        out << field->type().getCppType(&extra)
            << " "
            << field->name()
            << extra
            << ";\n";
    }

    if (needsEmbeddedReadWrite()) {
        out << "\n::android::status_t readEmbeddedFromParcel(\n";

        out.indent();
        out.indent();

        out << "const ::android::hardware::Parcel &parcel,\n"
                  << "size_t parentHandle,\n"
                  << "size_t parentOffset);\n\n";

        out.unindent();
        out.unindent();

        out << "::android::status_t writeEmbeddedToParcel(\n";

        out.indent();
        out.indent();

        out << "::android::hardware::Parcel *parcel,\n"
                  << "size_t parentHandle,\n"
                  << "size_t parentOffset) const;\n";

        out.unindent();
        out.unindent();
    }

    out.unindent();
    out << "};\n\n";

    return OK;
}

status_t CompoundType::emitTypeDefinitions(
        Formatter &out, const std::string prefix) const {
    status_t err = Scope::emitTypeDefinitions(out, prefix + "::" + localName());

    if (err != OK) {
        return err;
    }

    if (!needsEmbeddedReadWrite()) {
        return OK;
    }

    emitStructReaderWriter(out, prefix, true /* isReader */);
    emitStructReaderWriter(out, prefix, false /* isReader */);

    return OK;
}

void CompoundType::emitStructReaderWriter(
        Formatter &out, const std::string &prefix, bool isReader) const {
    out << "::android::status_t "
              << (prefix.empty() ? "" : (prefix + "::"))
              << localName()
              << (isReader ? "::readEmbeddedFromParcel"
                           : "::writeEmbeddedToParcel")
              << "(\n";

    out.indent();
    out.indent();

    if (isReader) {
        out << "const ::android::hardware::Parcel &parcel,\n";
    } else {
        out << "::android::hardware::Parcel *parcel,\n";
    }

    out << "size_t parentHandle,\n"
        << "size_t parentOffset)";

    if (!isReader) {
        out << " const";
    }

    out << " {\n";

    out.unindent();
    out.unindent();
    out.indent();

    out << "::android::status_t _hidl_err = ::android::OK;\n\n";

    for (const auto &field : *mFields) {
        if (!field->type().needsEmbeddedReadWrite()) {
            continue;
        }

        field->type().emitReaderWriterEmbedded(
                out,
                field->name(),
                false /* nameIsPointer */,
                "parcel",
                !isReader /* parcelObjIsPointer */,
                isReader,
                ErrorMode_Return,
                "parentHandle",
                "parentOffset + offsetof("
                    + fullName()
                    + ", "
                    + field->name()
                    + ")");
    }

    out.unindent();
    out << "_hidl_error:\n";
    out.indent();
    out << "return _hidl_err;\n";

    out.unindent();
    out << "}\n\n";
}

bool CompoundType::needsEmbeddedReadWrite() const {
    if (mStyle != STYLE_STRUCT) {
        return false;
    }

    for (const auto &field : *mFields) {
        if (field->type().needsEmbeddedReadWrite()) {
            return true;
        }
    }

    return false;
}

bool CompoundType::resultNeedsDeref() const {
    return true;
}

status_t CompoundType::emitVtsTypeDeclarations(Formatter &out) const {
    out << "name: \"" << localName() << "\"\n";
    switch (mStyle) {
        case STYLE_STRUCT:
        {
            out << "type: TYPE_STRUCT\n";
            break;
        }
        case STYLE_UNION:
        {
            out << "type: TYPE_UNION\n";
            break;
        }
        default:
            break;
    }

    // Emit declaration for each subtype.
    for (const auto &type : getSubTypes()) {
        switch (mStyle) {
            case STYLE_STRUCT:
            {
                out << "struct_value: {\n";
                break;
            }
            case STYLE_UNION:
            {
                out << "union_value: {\n";
                break;
            }
            default:
                break;
        }
        out.indent();
        status_t status(type->emitVtsTypeDeclarations(out));
        if (status != OK) {
            return status;
        }
        out.unindent();
        out << "}\n";
    }

    // Emit declaration for each field.
    for (const auto &field : *mFields) {
        switch (mStyle) {
            case STYLE_STRUCT:
            {
                out << "struct_value: {\n";
                break;
            }
            case STYLE_UNION:
            {
                out << "union_value: {\n";
                break;
            }
            default:
                break;
        }
        out.indent();
        out << "name: \"" << field->name() << "\"\n";
        status_t status = field->type().emitVtsAttributeType(out);
        if (status != OK) {
            return status;
        }
        out.unindent();
        out << "}\n";
    }

    return OK;
}

status_t CompoundType::emitVtsAttributeType(Formatter &out) const {
    switch (mStyle) {
        case STYLE_STRUCT:
        {
            out << "type: TYPE_STRUCT\n";
            break;
        }
        case STYLE_UNION:
        {
            out << "type: TYPE_UNION\n";
            break;
        }
        default:
            break;
    }
    out << "predefined_type: \"" << localName() << "\"\n";
    return OK;
}

bool CompoundType::isJavaCompatible() const {
    return false;
}

////////////////////////////////////////////////////////////////////////////////

CompoundField::CompoundField(const char *name, Type *type)
    : mName(name),
      mType(type) {
}

std::string CompoundField::name() const {
    return mName;
}

const Type &CompoundField::type() const {
    return *mType;
}

}  // namespace android

