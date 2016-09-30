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

#ifndef VECTOR_TYPE_H_

#define VECTOR_TYPE_H_

#include "Type.h"

namespace android {

struct VectorType : public TemplatedType {
    VectorType();

    void addNamedTypesToSet(std::set<const FQName> &set) const override;

    std::string getCppType(
            StorageMode mode,
            std::string *extra,
            bool specifyNamespaces) const override;

    std::string getJavaType(
            std::string *extra, bool forInitializer) const override;

    void emitReaderWriter(
            Formatter &out,
            const std::string &name,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode) const override;

    void emitReaderWriterEmbedded(
            Formatter &out,
            size_t depth,
            const std::string &name,
            bool nameIsPointer,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode,
            const std::string &parentName,
            const std::string &offsetText) const override;

    void emitResolveReferences(
            Formatter &out,
            const std::string &name,
            bool nameIsPointer,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode) const override;

    void emitResolveReferencesEmbedded(
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
            const std::string &offsetText) const override;

    bool useParentInEmitResolveReferencesEmbedded() const override;

    void emitJavaReaderWriter(
            Formatter &out,
            const std::string &parcelObj,
            const std::string &argName,
            bool isReader) const override;

    void emitJavaFieldInitializer(
            Formatter &out, const std::string &fieldName) const override;

    void emitJavaFieldReaderWriter(
            Formatter &out,
            size_t depth,
            const std::string &parcelName,
            const std::string &blobName,
            const std::string &fieldName,
            const std::string &offset,
            bool isReader) const override;

    static void EmitJavaFieldReaderWriterForElementType(
            Formatter &out,
            size_t depth,
            const Type *elementType,
            const std::string &parcelName,
            const std::string &blobName,
            const std::string &fieldName,
            const std::string &offset,
            bool isReader);

    bool needsEmbeddedReadWrite() const override;
    bool needsResolveReferences() const override;
    bool resultNeedsDeref() const override;

    status_t emitVtsTypeDeclarations(Formatter &out) const override;
    status_t emitVtsAttributeType(Formatter &out) const override;

    bool isJavaCompatible() const override;

    void getAlignmentAndSize(size_t *align, size_t *size) const override;

 private:
    // Helper method for emitResolveReferences[Embedded].
    // Pass empty childName and childOffsetText if the original
    // childHandle is unknown.
    // For example, given a vec<ref<T>> (T is a simple struct that
    // contains primitive values only), then the following methods are
    // invoked:
    // 1. VectorType::emitResolveReferences
    //    ... which calls the helper with empty childName and childOffsetText
    // 2. RefType::emitResolveReferencesEmbedded
    void emitResolveReferencesEmbeddedHelper(
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
            const std::string &childOffsetText) const;

    DISALLOW_COPY_AND_ASSIGN(VectorType);
};

}  // namespace android

#endif  // VECTOR_TYPE_H_

