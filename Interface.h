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

#ifndef INTERFACE_H_

#define INTERFACE_H_

#include "Method.h"
#include "Scope.h"

#include <utils/KeyedVector.h>
#include <vector>

namespace android {

struct Annotation;
struct Method;

struct Interface : public Scope {
    Interface(
            const char *localName,
            Interface *super,
            AnnotationVector *annotations);

    void addMethod(Method *method);

    bool isInterface() const override;
    bool isBinder() const override;

    const Interface *superType() const;

    const std::vector<Method *> &methods() const;

    const AnnotationVector &annotations() const;

    std::string getBaseName() const;

    std::string getCppType(StorageMode mode, std::string *extra) const override;

    std::string getJavaType() const override;

    void emitReaderWriter(
            Formatter &out,
            const std::string &name,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode) const override;

    void emitJavaReaderWriter(
            Formatter &out,
            const std::string &parcelObj,
            const std::string &argName,
            bool isReader) const override;

    status_t emitVtsAttributeType(Formatter &out) const override;

    status_t emitVtsAttributeDeclaration(Formatter &out) const;
    status_t emitVtsMethodDeclaration(Formatter &out) const;

    bool isJavaCompatible() const override;

private:
    Interface *mSuperType;
    std::vector<Method *> mMethods;
    AnnotationVector *mAnnotationsByName;
    mutable bool mIsJavaCompatibleInProgress;

    DISALLOW_COPY_AND_ASSIGN(Interface);
};

}  // namespace android

#endif  // INTERFACE_H_

