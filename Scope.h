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

#ifndef SCOPE_H_

#define SCOPE_H_

#include "NamedType.h"

#include <utils/KeyedVector.h>
#include <vector>

namespace android {

struct Formatter;
struct Interface;

struct Scope : public NamedType {
    Scope(const char *localName);

    bool addType(const char *localName, Type *type, std::string *errorMsg);

    Type *lookupType(const char *name) const;

    bool isScope() const override;

    // Returns the single interface or NULL.
    Interface *getInterface() const;

    bool containsSingleInterface(std::string *ifaceName) const;

    std::string pickUniqueAnonymousName() const;

    std::string getJavaType() const override;

    status_t emitTypeDeclarations(Formatter &out) const override;

    status_t emitJavaTypeDeclarations(
            Formatter &out, bool atTopLevel) const override;

    status_t emitTypeDefinitions(
            Formatter &out, const std::string prefix) const override;

    const std::vector<Type *> &getSubTypes() const;

    status_t emitVtsTypeDeclarations(Formatter &out) const override;

    bool isJavaCompatible() const override;

    size_t countTypes() const;
    const Type *typeAt(size_t index, std::string *name) const;

private:
    std::vector<Type *> mTypes;
    KeyedVector<std::string, size_t> mTypeIndexByName;

    DISALLOW_COPY_AND_ASSIGN(Scope);
};

}  // namespace android

#endif  // SCOPE_H_

