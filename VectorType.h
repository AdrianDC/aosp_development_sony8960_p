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

struct VectorType : public Type {
    VectorType(Type *elementType);

    std::string getCppType(StorageMode mode, std::string *extra) const override;

    std::string getJavaType() const override;
    std::string getJavaSuffix() const override;

    void emitReaderWriter(
            Formatter &out,
            const std::string &name,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode) const override;

    void emitReaderWriterEmbedded(
            Formatter &out,
            const std::string &name,
            bool nameIsPointer,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode,
            const std::string &parentName,
            const std::string &offsetText) const override;

    void emitJavaFieldInitializer(
            Formatter &out, const std::string &fieldName) const override;

    void emitJavaFieldReaderWriter(
            Formatter &out,
            const std::string &blobName,
            const std::string &fieldName,
            const std::string &offset,
            bool isReader) const override;

    bool needsEmbeddedReadWrite() const override;
    bool resultNeedsDeref() const override;

    status_t emitVtsTypeDeclarations(Formatter &out) const override;
    status_t emitVtsAttributeType(Formatter &out) const override;

    bool isJavaCompatible() const override;

    void getAlignmentAndSize(size_t *align, size_t *size) const override;

 private:
    Type *mElementType;

    DISALLOW_COPY_AND_ASSIGN(VectorType);
};

}  // namespace android

#endif  // VECTOR_TYPE_H_

