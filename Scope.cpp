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

#include "Scope.h"

#include "Formatter.h"
#include "Interface.h"

#include <android-base/logging.h>

namespace android {

Scope::Scope(const char *localName)
    : NamedType(localName) {
}

bool Scope::addType(const char *localName, Type *type, std::string *errorMsg) {
    if (mTypeIndexByName.indexOfKey(localName) >= 0) {
        *errorMsg = "A type named '";
        (*errorMsg) += localName;
        (*errorMsg) += "' is already declared in the  current scope.";

        return false;
    }

    size_t index = mTypes.size();
    mTypes.push_back(type);
    mTypeIndexByName.add(localName, index);

    return true;
}

Type *Scope::lookupType(const char *name) const {
    ssize_t index = mTypeIndexByName.indexOfKey(name);

    if (index >= 0) {
        return mTypes[mTypeIndexByName.valueAt(index)];
    }

    return NULL;
}

bool Scope::isScope() const {
    return true;
}

Interface *Scope::getInterface() const {
    if (mTypes.size() == 1 && mTypes[0]->isInterface()) {
        return static_cast<Interface *>(mTypes[0]);
    }

    return NULL;
}

bool Scope::containsSingleInterface(std::string *ifaceName) const {
    Interface *iface = getInterface();

    if (iface != NULL) {
        *ifaceName = iface->localName();
        return true;
    }

    return false;
}

std::string Scope::pickUniqueAnonymousName() const {
    static size_t sNextID = 0;

    for (;;) {
        std::string anonName = "_hidl_Anon_" + std::to_string(sNextID++);

        if (mTypeIndexByName.indexOfKey(anonName) < 0) {
            return anonName;
        }
    }
}

std::string Scope::getJavaType() const {
    CHECK(!"Should not be here");
    return std::string();
}

status_t Scope::emitTypeDeclarations(Formatter &out) const {
    for (size_t i = 0; i < mTypes.size(); ++i) {
        status_t err = mTypes[i]->emitTypeDeclarations(out);

        if (err != OK) {
            return err;
        }
    }

    return OK;
}

status_t Scope::emitJavaTypeDeclarations(
        Formatter &out, bool atTopLevel) const {
    for (size_t i = 0; i < mTypes.size(); ++i) {
        status_t err = mTypes[i]->emitJavaTypeDeclarations(out, atTopLevel);

        if (err != OK) {
            return err;
        }
    }

    return OK;
}

status_t Scope::emitTypeDefinitions(
        Formatter &out, const std::string prefix) const {
    for (size_t i = 0; i < mTypes.size(); ++i) {
        status_t err = mTypes[i]->emitTypeDefinitions(out, prefix);

        if (err != OK) {
            return err;
        }
    }

    return OK;
}

const std::vector<Type *> &Scope::getSubTypes() const {
    return mTypes;
}

status_t Scope::emitVtsTypeDeclarations(Formatter &out) const {
    for (size_t i = 0; i < mTypes.size(); ++i) {
        status_t status = mTypes[i]->emitVtsTypeDeclarations(out);
        if (status != OK) {
            return status;
        }
    }
    return OK;
}

bool Scope::isJavaCompatible() const {
    for (const auto &type : mTypes) {
        if (!type->isJavaCompatible()) {
            return false;
        }
    }

    return true;
}

size_t Scope::countTypes() const {
    return mTypeIndexByName.size();
}

const Type *Scope::typeAt(size_t index, std::string *name) const {
    *name = mTypeIndexByName.keyAt(index);
    return mTypes[mTypeIndexByName.valueAt(index)];
}

}  // namespace android

