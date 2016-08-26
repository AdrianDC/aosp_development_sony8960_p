/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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

std::string CompoundType::getJavaType() const {
    return fullJavaName();
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

void CompoundType::emitJavaReaderWriter(
        Formatter &out,
        const std::string &parcelObj,
        const std::string &argName,
        bool isReader) const {
    if (isReader) {
        out << "new " << fullJavaName() << "();\n";
    }

    out << argName
        << "."
        << (isReader ? "readFromParcel" : "writeToParcel")
        << "("
        << parcelObj
        << ");\n";
}

void CompoundType::emitJavaFieldInitializer(
        Formatter &out, const std::string &fieldName) const {
    out << "final "
        << fullJavaName()
        << " "
        << fieldName
        << " = new "
        << fullJavaName()
        << "();\n";
}

void CompoundType::emitJavaFieldReaderWriter(
        Formatter &out,
        const std::string &blobName,
        const std::string &fieldName,
        const std::string &offset,
        bool isReader) const {
    if (isReader) {
        out << fieldName
            << ".readEmbeddedFromParcel(parcel, "
            << blobName
            << ", "
            << offset
            << ");\n";

        return;
    }

    out << fieldName
        << ".writeEmbeddedToBlob("
        << blobName
        << ", "
        << offset
        << ");\n";
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

status_t CompoundType::emitJavaTypeDeclarations(
        Formatter &out, bool atTopLevel) const {
    out << "public final ";

    if (!atTopLevel) {
        out << "static ";
    }

    out << "class "
        << localName()
        << " {\n";

    out.indent();

    Scope::emitJavaTypeDeclarations(out, false /* atTopLevel */);

    for (const auto &field : *mFields) {
        const bool isScope = field->type().isScope();  // really isStruct...

        out << "public ";

        field->type().emitJavaFieldInitializer(out, field->name());
    }

    if (!mFields->empty()) {
        out << "\n";
    }

    out << "public final void readFromParcel(HwParcel parcel) {\n";
    out.indent();
    out << "HwBlob blob = parcel.readBuffer();\n";
    out << "readEmbeddedFromParcel(parcel, blob, 0 /* parentOffset */);\n";
    out.unindent();
    out << "}\n\n";

    out << "public final void readEmbeddedFromParcel(\n";
    out.indent();
    out.indent();
    out << "HwParcel parcel, HwBlob _hidl_blob, long _hidl_offset) {\n";
    out.unindent();

    size_t offset = 0;
    for (const auto &field : *mFields) {
        size_t fieldAlign, fieldSize;
        field->type().getAlignmentAndSize(&fieldAlign, &fieldSize);

        size_t pad = offset % fieldAlign;
        if (pad > 0) {
            offset += fieldAlign - pad;
        }

        field->type().emitJavaFieldReaderWriter(
                out,
                "_hidl_blob",
                field->name(),
                "_hidl_offset + " + std::to_string(offset),
                true /* isReader */);

        offset += fieldSize;
    }

    out.unindent();
    out << "}\n\n";

    out << "public final void writeToParcel(HwParcel parcel) {\n";
    out.indent();

    size_t structAlign, structSize;
    getAlignmentAndSize(&structAlign, &structSize);

    out << "HwBlob _hidl_blob = new HwBlob("
        << structSize
        << " /* size */);\n";

    out << "writeEmbeddedToBlob(_hidl_blob, 0 /* parentOffset */);\n"
        << "parcel.writeBuffer(_hidl_blob);\n";

    out.unindent();
    out << "}\n\n";

    out << "public final void writeEmbeddedToBlob(\n";
    out.indent();
    out.indent();
    out << "HwBlob _hidl_blob, long _hidl_offset) {\n";
    out.unindent();

    offset = 0;
    for (const auto &field : *mFields) {
        size_t fieldAlign, fieldSize;
        field->type().getAlignmentAndSize(&fieldAlign, &fieldSize);

        size_t pad = offset % fieldAlign;
        if (pad > 0) {
            offset += fieldAlign - pad;
        }

        field->type().emitJavaFieldReaderWriter(
                out,
                "_hidl_blob",
                field->name(),
                "_hidl_offset + " + std::to_string(offset),
                false /* isReader */);

        offset += fieldSize;
    }

    out.unindent();
    out << "}\n";

    out.unindent();
    out << "};\n\n";

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
    return mStyle == STYLE_STRUCT && Scope::isJavaCompatible();
}

void CompoundType::getAlignmentAndSize(size_t *align, size_t *size) const {
    *align = 1;

    size_t offset = 0;
    for (const auto &field : *mFields) {
        // Each field is aligned according to its alignment requirement.
        // The surrounding structure's alignment is the maximum of its
        // fields' aligments.

        size_t fieldAlign, fieldSize;
        field->type().getAlignmentAndSize(&fieldAlign, &fieldSize);

        size_t pad = offset % fieldAlign;
        if (pad > 0) {
            offset += fieldAlign - pad;
        }

        offset += fieldSize;

        if (fieldAlign > (*align)) {
            *align = fieldAlign;
        }
    }

    // Final padding to account for the structure's alignment.
    size_t pad = offset % (*align);
    if (pad > 0) {
        offset += (*align) - pad;
    }

    *size = offset;
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

