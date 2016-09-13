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

#include "VectorType.h"

#include <hidl-util/Formatter.h>
#include <android-base/logging.h>

namespace android {

VectorType::VectorType(Type *elementType)
    : mElementType(elementType) {
}

void VectorType::addNamedTypesToSet(std::set<const FQName> &set) const {
    mElementType->addNamedTypesToSet(set);
}

std::string VectorType::getCppType(StorageMode mode,
                                   std::string *extra,
                                   bool specifyNamespaces) const {
    const std::string base =
          std::string(specifyNamespaces ? "::android::hardware::" : "")
        + "hidl_vec<"
        + mElementType->getCppType(extra, specifyNamespaces)
        + (*extra)
        + ">";

    extra->clear();

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
            << baseExtra
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
            0 /* depth */,
            name,
            isReader /* nameIsPointer */,
            parcelObj,
            parcelObjIsPointer,
            isReader,
            mode,
            parentName,
            "0 /* parentOffset */");
}

// Remove any trailing array indices from the given name, i.e. foo[2][3] => foo
static std::string StripIndex(const std::string &name) {
    size_t pos = name.find("[");
    if (pos == std::string::npos) {
        return name;
    }

    return name.substr(0, pos);
}

void VectorType::emitReaderWriterEmbedded(
        Formatter &out,
        size_t depth,
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

    const std::string childName = "_hidl_" + StripIndex(name) + "_child";
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

    std::string iteratorName = "_hidl_index_" + std::to_string(depth);

    out << "for (size_t "
        << iteratorName
        << " = 0; "
        << iteratorName
        << " < "
        << nameDeref
        << "size(); ++"
        << iteratorName
        << ") {\n";

    out.indent();

    mElementType->emitReaderWriterEmbedded(
            out,
            depth + 1,
            (nameIsPointer ? "(*" + name + ")" : name)
                + "[" + iteratorName + "]",
            false /* nameIsPointer */,
            parcelObj,
            parcelObjIsPointer,
            isReader,
            mode,
            childName,
            iteratorName + " * sizeof(" + baseType + baseExtra + ")");

    out.unindent();

    out << "}\n\n";
}

void VectorType::emitJavaReaderWriter(
        Formatter &out,
        const std::string &parcelObj,
        const std::string &argName,
        bool isReader) const {
    if (mElementType->isCompoundType()) {
        if (isReader) {
            out << mElementType->getJavaType()
                << ".readVectorFromParcel("
                << parcelObj
                << ");\n";
        } else {
            out << mElementType->getJavaType()
                << ".writeVectorToParcel("
                << parcelObj
                << ", "
                << argName
                << ");\n";
        }

        return;
    }

    emitJavaReaderWriterWithSuffix(
            out,
            parcelObj,
            argName,
            isReader,
            mElementType->getJavaSuffix() + "Vector",
            "" /* extra */);
}

void VectorType::emitJavaFieldInitializer(
        Formatter &out, const std::string &fieldName) const {
    out << "final Vector<"
        << mElementType->getJavaWrapperType()
        << "> "
        << fieldName
        << " = new Vector();\n";
}

void VectorType::emitJavaFieldReaderWriter(
        Formatter &out,
        const std::string &blobName,
        const std::string &fieldName,
        const std::string &offset,
        bool isReader) const {
    VectorType::EmitJavaFieldReaderWriterForElementType(
            out, mElementType, blobName, fieldName, offset, isReader);
}

// static
void VectorType::EmitJavaFieldReaderWriterForElementType(
        Formatter &out,
        const Type *elementType,
        const std::string &blobName,
        const std::string &fieldName,
        const std::string &offset,
        bool isReader) {
    if (isReader) {
        out << "{\n";
        out.indent();

        out << "HwBlob childBlob = parcel.readEmbeddedBuffer(\n";
        out.indent();
        out.indent();

        out << blobName
            << ".handle(),\n"
            << offset
            << " + 0 /* offsetof(hidl_vec<T>, mBuffer) */);\n\n";

        out.unindent();
        out.unindent();

        out << fieldName << ".clear();\n";
        out << "long _hidl_vec_size = "
            << blobName
            << ".getInt64("
            << offset
            << " + 8 /* offsetof(hidl_vec<T>, mSize) */);\n";

        out << "for (int _hidl_index = 0; _hidl_index < _hidl_vec_size; "
            << "++_hidl_index) {\n";

        out.indent();

        elementType->emitJavaFieldInitializer(out, "_hidl_vec_element");

        size_t elementAlign, elementSize;
        elementType->getAlignmentAndSize(&elementAlign, &elementSize);

        elementType->emitJavaFieldReaderWriter(
                out,
                "childBlob",
                "_hidl_vec_element",
                "_hidl_index * " + std::to_string(elementSize),
                true /* isReader */);

        out << fieldName
            << ".add(_hidl_vec_element);\n";

        out.unindent();

        out << "}\n";

        out.unindent();
        out << "}\n";

        return;
    }

    out << "{\n";
    out.indent();

    out << "long _hidl_vec_size = "
        << fieldName
        << ".size();\n";

    out << blobName
        << ".putInt64("
        << offset
        << " + 8 /* offsetof(hidl_vec<T>, mSize) */, _hidl_vec_size);\n";

    out << blobName
        << ".putBool("
        << offset
        << " + 16 /* offsetof(hidl_vec<T>, mOwnsBuffer) */, false);\n";

    size_t elementAlign, elementSize;
    elementType->getAlignmentAndSize(&elementAlign, &elementSize);

    // XXX make HwBlob constructor take a long instead of an int?
    out << "HwBlob childBlob = new HwBlob((int)(_hidl_vec_size * "
        << elementSize
        << "));\n";

    out << "for (int _hidl_index = 0; _hidl_index < _hidl_vec_size; "
        << "++_hidl_index) {\n";

    out.indent();

    elementType->emitJavaFieldReaderWriter(
            out,
            "childBlob",
            fieldName + ".elementAt(_hidl_index)",
            "_hidl_index * " + std::to_string(elementSize),
            false /* isReader */);

    out.unindent();

    out << "}\n";

    out << blobName
        << ".putBlob("
        << offset
        << " + 0 /* offsetof(hidl_vec<T>, mBuffer) */, childBlob);\n";

    out.unindent();
    out << "}\n";
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

void VectorType::getAlignmentAndSize(size_t *align, size_t *size) const {
    *align = 8;  // hidl_vec<T>
    *size = 24;
}

}  // namespace android

