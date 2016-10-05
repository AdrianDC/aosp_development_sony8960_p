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

#include "RefType.h"

#include <hidl-util/Formatter.h>
#include <android-base/logging.h>

namespace android {

RefType::RefType() {
}

/* return something like "T const *".
 * The reason we don't return "const T *" is to handle cases like
 * ref<ref<ref<T>>> t_3ptr;
 * in this case the const's will get stacked on the left (const const const T *** t_3ptr)
 * but in this implementation it would be clearer (T const* const* const* t_3ptr) */
std::string RefType::getCppType(StorageMode /*mode*/, std::string *extra, bool /*specifyNamespaces*/) const {
    const std::string elementBase = mElementType->getCppType(extra);
    std::string base;
    if(extra->empty())
        base = elementBase + " const*";
    else { // for pointer to array: e.g. int32_t (*meow)[5]
        base = elementBase + " const(*";
        extra->insert(0, 1, ')');
    }

    return base;
}

void RefType::addNamedTypesToSet(std::set<const FQName> &set) const {
    mElementType->addNamedTypesToSet(set);
}

void RefType::emitReaderWriter(
        Formatter &,
        const std::string &,
        const std::string &,
        bool,
        bool,
        ErrorMode) const {
    // RefType doesn't get read / written at this stage.
    return;
}

void RefType::emitResolveReferences(
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
        "", // parentName
        ""); // offsetText
}

void RefType::emitResolveReferencesEmbedded(
            Formatter &out,
            size_t /* depth */,
            const std::string &name,
            const std::string &sanitizedName,
            bool /*nameIsPointer*/,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode,
            const std::string &parentName,
            const std::string &offsetText) const {

    std::string elementExtra;
    std::string elementBase = mElementType->getCppType(&elementExtra);
    std::string elementType = elementBase + elementExtra;

    std::string baseExtra;
    std::string baseType = Type::getCppType(&baseExtra);

    const std::string parcelObjDeref =
        parcelObjIsPointer ? ("*" + parcelObj) : parcelObj;

    const std::string parcelObjPointer =
        parcelObjIsPointer ? parcelObj : ("&" + parcelObj);

    // as if nameIsPointer is false. Pointers are always pass by values,
    // so name is always the pointer value itself. Hence nameIsPointer = false.
    const std::string namePointer = "&" + name;
    const std::string handleName = "_hidl_" + sanitizedName + "__ref_handle";
    const std::string resolveBufName = "_hidl_" + sanitizedName + "__ref_resolve_buf";

    bool isEmbedded = (!parentName.empty() && !offsetText.empty());

    out << "size_t " << handleName << ";\n"
        << "bool " << resolveBufName << ";\n\n";

    out << "_hidl_err = ";

    if (isReader) {
        out << "::android::hardware::read"
            << (isEmbedded ? "Embedded" : "")
            << "ReferenceFromParcel<"
            << elementType
            << ">(const_cast<"
            << baseType
            << " *"
            << baseExtra
            << ">("
            << namePointer
            << "),";
    } else {
        out << "::android::hardware::write"
            << (isEmbedded ? "Embedded" : "")
            << "ReferenceToParcel<"
            << elementType
            << ">("
            << name
            << ",";
    }

    out.indent();
    out.indent();

    out << (isReader ? parcelObjDeref : parcelObjPointer);
    if(isEmbedded)
        out << ",\n"
            << parentName
            << ",\n"
            << offsetText;

    out << ",\n&" + handleName;
    out << ",\n&" + resolveBufName;
    out << ");\n\n";

    out.unindent();
    out.unindent();

    handleError(out, mode);

    if(!mElementType->needsResolveReferences() && !mElementType->needsEmbeddedReadWrite())
        return; // no need to deal with element type recursively.

    out << "if(" << resolveBufName << ") {\n";
    out.indent();

    if(mElementType->needsEmbeddedReadWrite()) {
        mElementType->emitReaderWriterEmbedded(
            out,
            0 /* depth */,
            name,
            name /* sanitizedName */,
            true /* nameIsPointer */, // for element type, name is a pointer.
            parcelObj,
            parcelObjIsPointer,
            isReader,
            mode,
            handleName,
            "0 /* parentOffset */");
    }

    if(mElementType->needsResolveReferences()) {
        mElementType->emitResolveReferencesEmbedded(
            out,
            0 /* depth */,
            "(*" + name + ")",
            sanitizedName + "_deref",
            false /* nameIsPointer */,
                // must deref it and say false here, otherwise pointer to pointers don't work
            parcelObj,
            parcelObjIsPointer,
            isReader,
            mode,
            handleName,
            "0 /* parentOffset */");
    }

    out.unindent();
    out << "}\n\n";
}




bool RefType::needsResolveReferences() const {
    return true;
}

bool RefType::needsEmbeddedReadWrite() const {
    return false;
}

bool RefType::resultNeedsDeref() const {
    return false;
}

status_t RefType::emitVtsTypeDeclarations(Formatter &out) const {
    out << "type: TYPE_REF\n" << "ref_value: {\n";
    out.indent();
    status_t err = mElementType->emitVtsTypeDeclarations(out);
    if (err != OK) {
        return err;
    }
    out.unindent();
    out << "}\n";
    return OK;
}

status_t RefType::emitVtsAttributeType(Formatter &out) const {
    out << "type: TYPE_REF\n" << "ref_value: {\n";
    out.indent();
    status_t status = mElementType->emitVtsAttributeType(out);
    if (status != OK) {
        return status;
    }
    out.unindent();
    out << "}\n";
    return OK;
}

bool RefType::isJavaCompatible() const {
    return false;
}

}  // namespace android

