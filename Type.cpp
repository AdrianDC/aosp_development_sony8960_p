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

#include "Type.h"

#include "Formatter.h"
#include "ScalarType.h"

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

bool Type::isEnum() const {
    return false;
}

bool Type::isTypeDef() const {
    return false;
}

bool Type::isBinder() const {
    return false;
}

bool Type::isNamedType() const {
    return false;
}

const ScalarType *Type::resolveToScalarType() const {
    return NULL;
}

bool Type::isValidEnumStorageType() const {
    const ScalarType *scalarType = resolveToScalarType();

    if (scalarType == NULL) {
        return false;
    }

    return scalarType->isValidEnumStorageType();
}

std::string Type::getCppType(StorageMode, std::string *) const {
    CHECK(!"Should not be here");
    return std::string();
}

std::string Type::getJavaWrapperType() const {
    return getJavaType();
}

std::string Type::getJavaSuffix() const {
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

void Type::emitJavaReaderWriter(
        Formatter &out,
        const std::string &parcelObj,
        const std::string &argName,
        bool isReader) const {
    emitJavaReaderWriterWithSuffix(
            out,
            parcelObj,
            argName,
            isReader,
            getJavaSuffix(),
            "" /* extra */);
}

void Type::emitJavaFieldInitializer(
        Formatter &out,
        const std::string &fieldName) const {
    out << getJavaType()
        << " "
        << fieldName
        << ";\n";
}

void Type::emitJavaFieldReaderWriter(
        Formatter &,
        const std::string &,
        const std::string &,
        const std::string &,
        bool) const {
    CHECK(!"Should not be here");
}

void Type::handleError(Formatter &out, ErrorMode mode) const {
    switch (mode) {
        case ErrorMode_Ignore:
        {
            out << "/* _hidl_err ignored! */\n\n";
            break;
        }

        case ErrorMode_Goto:
        {
            out << "if (_hidl_err != ::android::OK) { goto _hidl_error; }\n\n";
            break;
        }

        case ErrorMode_Break:
        {
            out << "if (_hidl_err != ::android::OK) { break; }\n\n";
            break;
        }

        case ErrorMode_Return:
        {
            out << "if (_hidl_err != ::android::OK) { return _hidl_err; }\n\n";
            break;
        }
    }
}

void Type::handleError2(Formatter &out, ErrorMode mode) const {
    switch (mode) {
        case ErrorMode_Goto:
        {
            out << "goto _hidl_error;\n";
            break;
        }

        case ErrorMode_Break:
        {
            out << "break;\n";
            break;
        }

        case ErrorMode_Ignore:
        {
            out << "/* ignoring _hidl_error! */";
            break;
        }

        case ErrorMode_Return:
        {
            out << "return _hidl_err;\n";
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

    out << "_hidl_err = ";

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

status_t Type::emitJavaTypeDeclarations(Formatter &, bool) const {
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

void Type::emitJavaReaderWriterWithSuffix(
        Formatter &out,
        const std::string &parcelObj,
        const std::string &argName,
        bool isReader,
        const std::string &suffix,
        const std::string &extra) const {
    out << parcelObj
        << "."
        << (isReader ? "read" : "write")
        << suffix
        << "(";

    if (isReader) {
        out << extra;
    } else {
        out << (extra.empty() ? "" : (extra + ", "));
        out << argName;
    }

    out << ");\n";
}

status_t Type::emitVtsTypeDeclarations(Formatter &) const {
    return OK;
}

status_t Type::emitVtsAttributeType(Formatter &out) const {
    return emitVtsTypeDeclarations(out);
}

bool Type::isJavaCompatible() const {
    return true;
}

void Type::getAlignmentAndSize(size_t *, size_t *) const {
    CHECK(!"Should not be here");
}

}  // namespace android

