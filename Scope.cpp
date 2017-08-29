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

#include "Annotation.h"
#include "ConstantExpression.h"
#include "Interface.h"

#include <android-base/logging.h>
#include <hidl-util/Formatter.h>
#include <vector>

namespace android {

Scope::Scope(const char* localName, const Location& location, Scope* parent)
    : NamedType(localName, location, parent) {}
Scope::~Scope(){}

bool Scope::addType(NamedType *type, std::string *errorMsg) {
    const std::string &localName = type->localName();

    auto it = mTypeIndexByName.find(localName);

    if (it != mTypeIndexByName.end()) {
        *errorMsg = "A type named '";
        (*errorMsg) += localName;
        (*errorMsg) += "' is already declared in the  current scope.";

        return false;
    }

    size_t index = mTypes.size();
    mTypes.push_back(type);
    mTypeIndexByName[localName] = index;

    return true;
}

NamedType *Scope::lookupType(const FQName &fqName) const {
    CHECK(fqName.package().empty() && fqName.version().empty());
    if (!fqName.valueName().empty()) {
        LOG(WARNING) << fqName.string() << " does not refer to a type.";
        return nullptr;
    }
    std::vector<std::string> names = fqName.names();
    CHECK_GT(names.size(), 0u);
    auto it = mTypeIndexByName.find(names[0]);

    if (it == mTypeIndexByName.end()) {
        return nullptr;
    }

    NamedType *outerType = mTypes[it->second];
    if (names.size() == 1) {
        return outerType;
    }
    if (!outerType->isScope()) {
        // more than one names, but the first name is not a scope
        return nullptr;
    }
    Scope *outerScope = static_cast<Scope *>(outerType);
    // *slowly* pop first element
    names.erase(names.begin());
    FQName innerName(names);
    return outerScope->lookupType(innerName);
}

LocalIdentifier *Scope::lookupIdentifier(const std::string & /*name*/) const {
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

bool Scope::containsInterfaces() const {
    for (const NamedType *type : mTypes) {
        if (type->isInterface()) {
            return true;
        }
    }

    return false;
}

const std::vector<Annotation*>& Scope::annotations() const {
    return mAnnotations;
}

void Scope::setAnnotations(std::vector<Annotation*>* annotations) {
    CHECK(mAnnotations.empty());
    CHECK(annotations != nullptr);
    mAnnotations = *annotations;
}

std::vector<Type*> Scope::getDefinedTypes() const {
    std::vector<Type*> ret;
    for (auto* type : mTypes) {
        ret.push_back(type);
    }
    return ret;
}

std::vector<ConstantExpression*> Scope::getConstantExpressions() const {
    std::vector<ConstantExpression*> ret;
    for (const auto* annotation : mAnnotations) {
        const auto& retAnnotation = annotation->getConstantExpressions();
        ret.insert(ret.end(), retAnnotation.begin(), retAnnotation.end());
    }
    return ret;
}

status_t Scope::forEachType(const std::function<status_t(Type *)> &func) const {
    for (size_t i = 0; i < mTypes.size(); ++i) {
        status_t err = func(mTypes[i]);

        if (err != OK) {
            return err;
        }
    }

    return OK;
}

status_t Scope::emitTypeDeclarations(Formatter &out) const {
    return forEachType([&](Type *type) {
        return type->emitTypeDeclarations(out);
    });
}

status_t Scope::emitGlobalTypeDeclarations(Formatter &out) const {
    return forEachType([&](Type *type) {
        return type->emitGlobalTypeDeclarations(out);
    });
}

status_t Scope::emitGlobalHwDeclarations(Formatter &out) const {
    return forEachType([&](Type *type) {
        return type->emitGlobalHwDeclarations(out);
    });
}

status_t Scope::emitJavaTypeDeclarations(
        Formatter &out, bool atTopLevel) const {
    return forEachType([&](Type *type) {
        return type->emitJavaTypeDeclarations(out, atTopLevel);
    });
}

status_t Scope::emitTypeDefinitions(Formatter& out, const std::string& prefix) const {
    return forEachType([&](Type *type) {
        return type->emitTypeDefinitions(out, prefix);
    });
}

const std::vector<NamedType *> &Scope::getSubTypes() const {
    return mTypes;
}

status_t Scope::emitVtsTypeDeclarations(Formatter &out) const {
    return forEachType([&](Type *type) {
        return type->emitVtsTypeDeclarations(out);
    });
}

bool Scope::isJavaCompatible() const {
    for (const auto &type : mTypes) {
        if (!type->isJavaCompatible()) {
            return false;
        }
    }

    return true;
}

bool Scope::containsPointer() const {
    for (const auto &type : mTypes) {
        if (type->containsPointer()) {
            return true;
        }
    }

    return false;
}

void Scope::appendToExportedTypesVector(
        std::vector<const Type *> *exportedTypes) const {
    forEachType([&](Type *type) {
        type->appendToExportedTypesVector(exportedTypes);
        return OK;
    });
}

////////////////////////////////////////

RootScope::RootScope(const char* localName, const Location& location, Scope* parent)
    : Scope(localName, location, parent) {}
RootScope::~RootScope() {}

std::string RootScope::typeName() const {
    return "(root scope)";
}

status_t RootScope::validate() const {
    CHECK(annotations().empty());
    return Scope::validate();
}

////////////////////////////////////////

LocalIdentifier::LocalIdentifier(){}
LocalIdentifier::~LocalIdentifier(){}

bool LocalIdentifier::isEnumValue() const {
    return false;
}

ConstantExpression* LocalIdentifier::constExpr() const {
    return nullptr;
}

}  // namespace android

