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

namespace android {

ArrayType::ArrayType(Type *elementType, const char *dimension)
    : mElementType(elementType),
      mDimension(dimension) {
}

void ArrayType::addNamedTypesToSet(std::set<const FQName> &set) const {
    mElementType->addNamedTypesToSet(set);
}

std::string ArrayType::getCppType(StorageMode mode,
                                  std::string *extra,
                                  bool specifyNamespaces) const {
    const std::string base = mElementType->getCppType(extra, specifyNamespaces);

    *extra = "[" + mDimension + "]" + (*extra);

    switch (mode) {
        case StorageMode_Stack:
            return base;

        case StorageMode_Argument:
        case StorageMode_Result:
        {
            *extra = " /* " + base + (*extra) + " */";

            return "const " + base + "*";
        }
    }
}

std::string ArrayType::getJavaType() const {
    return mElementType->getJavaType() + "[]";
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

    const std::string parentName = "_hidl_" + name + "_parent";

    out << "size_t " << parentName << ";\n\n";

    const std::string parcelObjDeref =
        parcelObj + (parcelObjIsPointer ? "->" : ".");

    if (isReader) {
        out << name
            << " = (const "
            << baseType
            << " *)"
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
            << "writeBuffer("
            << name
            << ", "
            << mDimension
            << " * sizeof("
            << baseType
            << baseExtra
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

void ArrayType::emitReaderWriterEmbedded(
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
    if (!mElementType->needsEmbeddedReadWrite()) {
        return;
    }

    const std::string nameDeref = name + (nameIsPointer ? "->" : ".");

    std::string baseExtra;
    std::string baseType = mElementType->getCppType(&baseExtra);

    std::string iteratorName = "_hidl_index_" + std::to_string(depth);

    out << "for (size_t "
        << iteratorName
        << " = 0; "
        << iteratorName
        << " < "
        << mDimension
        << "; ++"
        << iteratorName
        << ") {\n";

    out.indent();

    mElementType->emitReaderWriterEmbedded(
            out,
            depth + 1,
            name + "[" + iteratorName + "]",
            false /* nameIsPointer */,
            parcelObj,
            parcelObjIsPointer,
            isReader,
            mode,
            parentName,
            offsetText
                + " + " + iteratorName + " * sizeof("
                + baseType
                + baseExtra
                + ")");

    out.unindent();

    out << "}\n\n";
}

bool ArrayType::needsEmbeddedReadWrite() const {
    return mElementType->needsEmbeddedReadWrite();
}

void ArrayType::emitJavaReaderWriter(
        Formatter &out,
        const std::string &parcelObj,
        const std::string &argName,
        bool isReader) const {
    if (mElementType->isCompoundType()) {
        if (isReader) {
            out << mElementType->getJavaType()
                << ".readArrayFromParcel("
                << parcelObj
                << ", "
                << mDimension
                << ");\n";
        } else {
            out << mElementType->getJavaType()
                << ".writeArrayToParcel("
                << parcelObj
                << ", "
                << mDimension
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
            mElementType->getJavaSuffix() + "Array",
            mDimension);
}

void ArrayType::emitJavaFieldInitializer(
        Formatter &out, const std::string &fieldName) const {
    out << "final "
        << mElementType->getJavaType()
        << "[] "
        << fieldName
        << " = new "
        << mElementType->getJavaType()
        << "["
        << mDimension
        << "];\n";
}

void ArrayType::emitJavaFieldReaderWriter(
        Formatter &out,
        const std::string &blobName,
        const std::string &fieldName,
        const std::string &offset,
        bool isReader) const {
    out << "for (int _hidl_index = 0; _hidl_index < "
        << mDimension
        << "; ++_hidl_index) {\n";

    out.indent();

    size_t elementAlign, elementSize;
    mElementType->getAlignmentAndSize(&elementAlign, &elementSize);

    mElementType->emitJavaFieldReaderWriter(
            out,
            blobName,
            fieldName + "[_hidl_index]",
            offset + " + _hidl_index * " + std::to_string(elementSize),
            isReader);

    out.unindent();
    out << "}\n";
}

status_t ArrayType::emitVtsTypeDeclarations(Formatter &out) const {
    out << "type: TYPE_ARRAY\n" << "vector_value: {\n";
    out.indent();
    out << "size: " << mDimension << "\n";
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

    char *end;
    unsigned long dim = strtoul(mDimension.c_str(), &end, 10);
    CHECK(end > mDimension.c_str() && *end == '\0');

    (*size) *= dim;
}

}  // namespace android

