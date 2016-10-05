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

#include "ArrayType.h"

#include <hidl-util/Formatter.h>
#include <android-base/logging.h>

#include "ConstantExpression.h"

namespace android {

ArrayType::ArrayType(ArrayType *srcArray, ConstantExpression *size)
    : mElementType(srcArray->mElementType),
      mSizes(srcArray->mSizes),
      mSizeComments(srcArray->mSizeComments) {
    addDimension(size);
}

ArrayType::ArrayType(Type *elementType, ConstantExpression *size)
    : mElementType(elementType) {
    addDimension(size);
}

void ArrayType::addDimension(ConstantExpression *size) {
    mSizes.insert(mSizes.begin(), size->castSizeT());
    mSizeComments.insert(mSizeComments.begin(), size->description());
}

size_t ArrayType::countDimensions() const {
    return mSizes.size();
}

void ArrayType::addNamedTypesToSet(std::set<const FQName> &set) const {
    mElementType->addNamedTypesToSet(set);
}

bool ArrayType::isArray() const {
    return true;
}

Type *ArrayType::getElementType() const {
    return mElementType;
}

std::string ArrayType::getCppType(StorageMode mode,
                                  std::string *extra,
                                  bool specifyNamespaces) const {
    const std::string base = mElementType->getCppType(extra, specifyNamespaces);
    CHECK(extra->empty());

    std::string arrayType = "hidl_array<" + base;

    for (size_t i = 0; i < mSizes.size(); ++i) {
        arrayType += ", ";
        arrayType += std::to_string(mSizes[i]);

        arrayType += " /* ";
        arrayType += mSizeComments[i];
        arrayType += " */";
    }

    arrayType += ">";

    switch (mode) {
        case StorageMode_Stack:
            return arrayType;

        case StorageMode_Argument:
            return "const " + arrayType + "&";

        case StorageMode_Result:
            return "const " + arrayType + "*";
    }

    CHECK(!"Should not be here");
}

std::string ArrayType::getJavaType(
        std::string *extra, bool forInitializer) const {
    std::string baseExtra;
    const std::string base =
        mElementType->getJavaType(&baseExtra, forInitializer);

    CHECK(baseExtra.empty());

    extra->clear();

    CHECK(mSizes.size() == mSizeComments.size());
    for (size_t i = 0; i < mSizes.size(); ++i) {
        *extra += "[";

        if (forInitializer) {
            *extra += std::to_string(mSizes[i]);
            *extra += " ";
        }

        *extra += "/* " + mSizeComments[i] + " */";

        *extra += "]";
    }

    return base;
}

std::string ArrayType::getJavaWrapperType() const {
    return mElementType->getJavaWrapperType();
}

void ArrayType::emitReaderWriter(
        Formatter &out,
        const std::string &name,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode) const {
    std::string baseExtra;
    std::string baseType = mElementType->getCppType(&baseExtra);
    CHECK(baseExtra.empty());

    const std::string parentName = "_hidl_" + name + "_parent";

    out << "size_t " << parentName << ";\n\n";

    const std::string parcelObjDeref =
        parcelObj + (parcelObjIsPointer ? "->" : ".");

    if (isReader) {
        std::string extra;

        out << name
            << " = ("
            << getCppResultType(&extra)
            << ")"
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
        size_t numArrayElements = 1;
        for (auto size : mSizes) {
            numArrayElements *= size;
        }

        out << "_hidl_err = "
            << parcelObjDeref
            << "writeBuffer("
            << name
            << ".data(), "
            << numArrayElements
            << " * sizeof("
            << baseType
            << "), &"
            << parentName
            << ");\n";

        handleError(out, mode);
    }

    emitReaderWriterEmbedded(
            out,
            0 /* depth */,
            name,
            name /* sanitizedName */,
            isReader /* nameIsPointer */,
            parcelObj,
            parcelObjIsPointer,
            isReader,
            mode,
            parentName,
            "0 /* parentOffset */");
}

void ArrayType::emitReaderWriterEmbedded(
        Formatter &out,
        size_t depth,
        const std::string &name,
        const std::string &sanitizedName,
        bool nameIsPointer,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode,
        const std::string &parentName,
        const std::string &offsetText) const {
    if (!mElementType->needsEmbeddedReadWrite()) {
        return;
    }

    const std::string nameDeref = name + (nameIsPointer ? "->" : ".");

    std::string baseExtra;
    std::string baseType = mElementType->getCppType(&baseExtra);
    CHECK(baseExtra.empty());

    std::string iteratorName = "_hidl_index_" + std::to_string(depth);

    out << "for (size_t "
        << iteratorName
        << " = 0; "
        << iteratorName
        << " < "
        << dimension()
        << "; ++"
        << iteratorName
        << ") {\n";

    out.indent();

    mElementType->emitReaderWriterEmbedded(
            out,
            depth + 1,
            nameDeref + "data()[" + iteratorName + "]",
            sanitizedName + "_indexed",
            false /* nameIsPointer */,
            parcelObj,
            parcelObjIsPointer,
            isReader,
            mode,
            parentName,
            offsetText
                + " + " + iteratorName + " * sizeof("
                + baseType
                + ")");

    out.unindent();

    out << "}\n\n";
}

void ArrayType::emitResolveReferences(
            Formatter &out,
            const std::string &name,
            bool nameIsPointer,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode) const {
    emitResolveReferencesEmbedded(
        out,
        0 /* depth */,
        name,
        name /* sanitizedName */,
        nameIsPointer,
        parcelObj,
        parcelObjIsPointer,
        isReader,
        mode,
        "_hidl_" + name + "_parent",
        "0 /* parentOffset */");
}

void ArrayType::emitResolveReferencesEmbedded(
            Formatter &out,
            size_t depth,
            const std::string &name,
            const std::string &sanitizedName,
            bool nameIsPointer,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode,
            const std::string &parentName,
            const std::string &offsetText) const {
    CHECK(needsResolveReferences() && mElementType->needsResolveReferences());

    const std::string nameDeref = name + (nameIsPointer ? "->" : ".");

    std::string baseExtra;
    std::string baseType = mElementType->getCppType(&baseExtra);

    std::string iteratorName = "_hidl_index_" + std::to_string(depth);

    out << "for (size_t "
        << iteratorName
        << " = 0; "
        << iteratorName
        << " < "
        << dimension()
        << "; ++"
        << iteratorName
        << ") {\n";

    out.indent();

    mElementType->emitResolveReferencesEmbedded(
        out,
        depth + 1,
        nameDeref + "data()[" + iteratorName + "]",
        sanitizedName + "_indexed",
        false /* nameIsPointer */,
        parcelObj,
        parcelObjIsPointer,
        isReader,
        mode,
        parentName,
        offsetText + " + " + iteratorName + " * sizeof("
        + baseType
        + ")");

    out.unindent();

    out << "}\n\n";
}


bool ArrayType::needsEmbeddedReadWrite() const {
    return mElementType->needsEmbeddedReadWrite();
}

bool ArrayType::needsResolveReferences() const {
    return mElementType->needsResolveReferences();
}

bool ArrayType::resultNeedsDeref() const {
    return true;
}

void ArrayType::emitJavaReaderWriter(
        Formatter &out,
        const std::string &parcelObj,
        const std::string &argName,
        bool isReader) const {
    if (isReader) {
        std::string extra;
        out << "new "
            << getJavaType(&extra, true /* forInitializer */)
            << extra
            << ";\n";
    }

    out << "{\n";
    out.indent();

    out << "HwBlob _hidl_blob = ";

    if (isReader) {
        out << parcelObj
            << ".readBuffer();\n";
    } else {
        size_t align, size;
        getAlignmentAndSize(&align, &size);

        out << "new HwBlob("
            << size
            << " /* size */);\n";
    }

    emitJavaFieldReaderWriter(
            out,
            0 /* depth */,
            parcelObj,
            "_hidl_blob",
            argName,
            "0 /* offset */",
            isReader);

    if (!isReader) {
        out << parcelObj << ".writeBuffer(_hidl_blob);\n";
    }

    out.unindent();
    out << "}\n";
}

void ArrayType::emitJavaFieldInitializer(
        Formatter &out, const std::string &fieldName) const {
    std::string extra;
    std::string typeName = getJavaType(&extra, false /* forInitializer */);

    std::string extraInit;
    getJavaType(&extraInit, true /* forInitializer */);

    out << "final "
        << typeName
        << extra
        << " "
        << fieldName
        << " = new "
        << typeName
        << extraInit
        << ";\n";
}

void ArrayType::emitJavaFieldReaderWriter(
        Formatter &out,
        size_t depth,
        const std::string &parcelName,
        const std::string &blobName,
        const std::string &fieldName,
        const std::string &offset,
        bool isReader) const {
    out << "{\n";
    out.indent();

    std::string offsetName = "_hidl_array_offset_" + std::to_string(depth);
    out << "long " << offsetName << " = " << offset << ";\n";

    std::string indexString;
    for (size_t dim = 0; dim < mSizes.size(); ++dim) {
        std::string iteratorName =
            "_hidl_index_" + std::to_string(depth) + "_" + std::to_string(dim);

        out << "for (int "
            << iteratorName
            << " = 0; "
            << iteratorName
            << " < "
            << mSizes[dim]
            << "; ++"
            << iteratorName
            << ") {\n";

        out.indent();

        indexString += "[" + iteratorName + "]";
    }

    if (isReader && mElementType->isCompoundType()) {
        std::string extra;
        std::string typeName =
            mElementType->getJavaType(&extra, false /* forInitializer */);

        CHECK(extra.empty());

        out << fieldName
            << indexString
            << " = new "
            << typeName
            << "();\n";
    }

    mElementType->emitJavaFieldReaderWriter(
            out,
            depth + 1,
            parcelName,
            blobName,
            fieldName + indexString,
            offsetName,
            isReader);

    size_t elementAlign, elementSize;
    mElementType->getAlignmentAndSize(&elementAlign, &elementSize);

    out << offsetName << " += " << std::to_string(elementSize) << ";\n";

    for (size_t dim = 0; dim < mSizes.size(); ++dim) {
        out.unindent();
        out << "}\n";
    }

    out.unindent();
    out << "}\n";
}

status_t ArrayType::emitVtsTypeDeclarations(Formatter &out) const {
    if (mSizes.size() > 1) {
        // Multi-dimensional arrays are yet to be supported in VTS.
        return UNKNOWN_ERROR;
    }

    out << "type: TYPE_ARRAY\n" << "vector_value: {\n";
    out.indent();
    out << "size: " << mSizes[0] << "\n";
    status_t err = mElementType->emitVtsTypeDeclarations(out);
    if (err != OK) {
        return err;
    }
    out.unindent();
    out << "}\n";
    return OK;
}

bool ArrayType::isJavaCompatible() const {
    return mElementType->isJavaCompatible();
}

void ArrayType::getAlignmentAndSize(size_t *align, size_t *size) const {
    mElementType->getAlignmentAndSize(align, size);

    for (auto sizeInDimension : mSizes) {
        (*size) *= sizeInDimension;
    }
}

size_t ArrayType::dimension() const {
    size_t numArrayElements = 1;
    for (auto size : mSizes) {
        numArrayElements *= size;
    }
    return numArrayElements;
}

}  // namespace android

