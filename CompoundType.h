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

#ifndef COMPOUND_TYPE_H_

#define COMPOUND_TYPE_H_

#include "Scope.h"

#include <vector>

namespace android {

struct CompoundField;

struct CompoundType : public Scope {
    enum Style {
        STYLE_STRUCT,
        STYLE_UNION,
    };

    CompoundType(Style style, const char *localName);

    bool setFields(std::vector<CompoundField *> *fields, std::string *errorMsg);

    std::string getCppType(StorageMode mode, std::string *extra) const override;

    std::string getJavaType() const override;

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

    void emitJavaReaderWriter(
            Formatter &out,
            const std::string &parcelObj,
            const std::string &argName,
            bool isReader) const override;

    void emitJavaFieldInitializer(
            Formatter &out, const std::string &fieldName) const override;

    void emitJavaFieldReaderWriter(
            Formatter &out,
            const std::string &blobName,
            const std::string &fieldName,
            const std::string &offset,
            bool isReader) const override;

    status_t emitTypeDeclarations(Formatter &out) const override;

    status_t emitTypeDefinitions(
            Formatter &out, const std::string prefix) const override;

    status_t emitJavaTypeDeclarations(
            Formatter &out, bool atTopLevel) const override;

    bool needsEmbeddedReadWrite() const override;
    bool resultNeedsDeref() const override;

    status_t emitVtsTypeDeclarations(Formatter &out) const override;
    status_t emitVtsAttributeType(Formatter &out) const override;

    bool isJavaCompatible() const override;

    void getAlignmentAndSize(size_t *align, size_t *size) const;

private:
    Style mStyle;
    std::vector<CompoundField *> *mFields;

    void emitStructReaderWriter(
            Formatter &out, const std::string &prefix, bool isReader) const;

    DISALLOW_COPY_AND_ASSIGN(CompoundType);
};

struct CompoundField {
    CompoundField(const char *name, Type *type);

    std::string name() const;
    const Type &type() const;

private:
    std::string mName;
    Type *mType;

    DISALLOW_COPY_AND_ASSIGN(CompoundField);
};

}  // namespace android

#endif  // COMPOUND_TYPE_H_

