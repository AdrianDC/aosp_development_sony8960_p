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

#include "ArrayType.h"

#include <hidl-util/Formatter.h>
#include <android-base/logging.h>

namespace android {

VectorType::VectorType() {
}

void VectorType::addNamedTypesToSet(std::set<const FQName> &set) const {
    mElementType->addNamedTypesToSet(set);
}

std::string VectorType::getCppType(StorageMode mode,
                                   std::string *extra,
                                   bool specifyNamespaces) const {
    std::string elementExtra;
    const std::string elementBase = mElementType->getCppType(&elementExtra, specifyNamespaces);

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

std::string VectorType::getJavaType(
        std::string *extra, bool /* forInitializer */) const {
    *extra = "[]";

    std::string elementExtra;
    return mElementType->getJavaType(&elementExtra) + elementExtra;
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
            name /* sanitizedName */ ,
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
    std::string baseExtra;
    std::string baseType = Type::getCppType(&baseExtra);

    const std::string childName = "_hidl_" + sanitizedName + "_child";
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
            sanitizedName + (nameIsPointer ? "_deref" : "") + "_indexed",
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

void VectorType::emitResolveReferences(
            Formatter &out,
            const std::string &name,
            bool nameIsPointer,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode) const {
    emitResolveReferencesEmbeddedHelper(
        out,
        0, /* depth */
        name,
        name /* sanitizedName */,
        nameIsPointer,
        parcelObj,
        parcelObjIsPointer,
        isReader,
        mode,
        "_hidl_" + name + "_child",
        "0 /* parentOffset */");
}

void VectorType::emitResolveReferencesEmbedded(
            Formatter &out,
            size_t depth,
            const std::string &name,
            const std::string &sanitizedName,
            bool nameIsPointer,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode,
            const std::string & /* parentName */,
            const std::string & /* offsetText */) const {
    emitResolveReferencesEmbeddedHelper(
        out, depth, name, sanitizedName, nameIsPointer, parcelObj,
        parcelObjIsPointer, isReader, mode, "", "");
}

bool VectorType::useParentInEmitResolveReferencesEmbedded() const {
    // parentName and offsetText is not used in emitResolveReferencesEmbedded
    return false;
}

void VectorType::emitResolveReferencesEmbeddedHelper(
            Formatter &out,
            size_t depth,
            const std::string &name,
            const std::string &sanitizedName,
            bool nameIsPointer,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode,
            const std::string &childName,
            const std::string &childOffsetText) const {
    CHECK(needsResolveReferences() && mElementType->needsResolveReferences());

    const std::string nameDeref = name + (nameIsPointer ? "->" : ".");
    std::string elementExtra;
    std::string elementType = mElementType->getCppType(&elementExtra);

    std::string myChildName = childName, myChildOffset = childOffsetText;

    if(myChildName.empty() && myChildOffset.empty()) {
        myChildName = "_hidl_" + sanitizedName + "_child";
        myChildOffset = "0";

        out << "size_t " << myChildName << ";\n";
        out << "_hidl_err = " << nameDeref << "findInParcel("
            << (parcelObjIsPointer ? "*" : "") << parcelObj << ", "
            << "&" << myChildName
            << ");\n";

        handleError(out, mode);
    }

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

    mElementType->emitResolveReferencesEmbedded(
        out,
        depth + 1,
        (nameIsPointer ? "(*" + name + ")" : name) + "[" + iteratorName + "]",
        sanitizedName + (nameIsPointer ? "_deref" : "") + "_indexed",
        false /* nameIsPointer */,
        parcelObj,
        parcelObjIsPointer,
        isReader,
        mode,
        myChildName,
        myChildOffset + " + " +
                iteratorName + " * sizeof(" + elementType + elementExtra + ")");

    out.unindent();

    out << "}\n\n";
}

void VectorType::emitJavaReaderWriter(
        Formatter &out,
        const std::string &parcelObj,
        const std::string &argName,
        bool isReader) const {
    if (mElementType->isCompoundType()) {
        std::string extra;  // unused, because CompoundType leaves this empty.

        if (isReader) {
            out << mElementType->getJavaType(&extra)
                << ".readVectorFromParcel("
                << parcelObj
                << ");\n";
        } else {
            out << mElementType->getJavaType(&extra)
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
    std::string extra;
    mElementType->getJavaType(&extra);

    const std::string wrapperType = mElementType->getJavaWrapperType();

    out << "final ArrayList<"
        << wrapperType
        << extra
        << "> "
        << fieldName
        << " = new ArrayList<"
        << wrapperType
        << extra
        << ">();\n";
}

void VectorType::emitJavaFieldReaderWriter(
        Formatter &out,
        size_t depth,
        const std::string &parcelName,
        const std::string &blobName,
        const std::string &fieldName,
        const std::string &offset,
        bool isReader) const {
    VectorType::EmitJavaFieldReaderWriterForElementType(
            out,
            depth,
            mElementType,
            parcelName,
            blobName,
            fieldName,
            offset,
            isReader);
}

// static
void VectorType::EmitJavaFieldReaderWriterForElementType(
        Formatter &out,
        size_t depth,
        const Type *elementType,
        const std::string &parcelName,
        const std::string &blobName,
        const std::string &fieldName,
        const std::string &offset,
        bool isReader) {
    if (isReader) {
        out << "{\n";
        out.indent();

        out << "HwBlob childBlob = "
            << parcelName
            << ".readEmbeddedBuffer(\n";

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

        std::string iteratorName = "_hidl_index_" + std::to_string(depth);

        out << "for (int "
            << iteratorName
            << " = 0; "
            << iteratorName
            << " < _hidl_vec_size; "
            << "++"
            << iteratorName
            << ") {\n";

        out.indent();

        elementType->emitJavaFieldInitializer(out, "_hidl_vec_element");

        size_t elementAlign, elementSize;
        elementType->getAlignmentAndSize(&elementAlign, &elementSize);

        elementType->emitJavaFieldReaderWriter(
                out,
                depth + 1,
                parcelName,
                "childBlob",
                "_hidl_vec_element",
                iteratorName + " * " + std::to_string(elementSize),
                true /* isReader */);

        out << fieldName;

        if (elementType->isArray()
                && static_cast<const ArrayType *>(
                    elementType)->getElementType()->resolveToScalarType()
                        != nullptr) {
            out << ".add(HwBlob.wrapArray(_hidl_vec_element));\n";
        } else {
            out << ".add(_hidl_vec_element);\n";
        }

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

    std::string iteratorName = "_hidl_index_" + std::to_string(depth);

    out << "for (int "
        << iteratorName
        << " = 0; "
        << iteratorName
        << " < _hidl_vec_size; "
        << "++"
        << iteratorName
        << ") {\n";

    out.indent();

    elementType->emitJavaFieldReaderWriter(
            out,
            depth + 1,
            parcelName,
            "childBlob",
            fieldName + ".get(" + iteratorName + ")",
            iteratorName + " * " + std::to_string(elementSize),
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

bool VectorType::needsResolveReferences() const {
    return mElementType->needsResolveReferences();
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
    if (!mElementType->isJavaCompatible()) {
        return false;
    }

    if (mElementType->isArray()) {
        return static_cast<ArrayType *>(mElementType)->countDimensions() == 1;
    }

    return true;
}

void VectorType::getAlignmentAndSize(size_t *align, size_t *size) const {
    *align = 8;  // hidl_vec<T>
    *size = 24;
}

}  // namespace android

