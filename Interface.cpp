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

#include "Interface.h"

#include "Annotation.h"
#include "Method.h"
#include "StringType.h"
#include "VectorType.h"

#include <android-base/logging.h>
#include <hidl-util/Formatter.h>
#include <hidl-util/StringHelper.h>
#include <iostream>
#include <sstream>

namespace android {

/* It is very important that these values NEVER change. These values
 * must remain unchanged over the lifetime of android. This is
 * because the framework on a device will be updated independently of
 * the hals on a device. If the hals are compiled with one set of
 * transaction values, and the framework with another, then the
 * interface between them will be destroyed, and the device will not
 * work.
 */
enum {
    // These values are defined in hardware::IBinder.
    /////////////////// User defined transactions
    FIRST_CALL_TRANSACTION  = 0x00000001,
    LAST_CALL_TRANSACTION   = 0x00efffff,
    /////////////////// HIDL reserved
    FIRST_HIDL_TRANSACTION  = 0x00f00000,
    HIDL_DESCRIPTOR_CHAIN_TRANSACTION = FIRST_HIDL_TRANSACTION,
    LAST_HIDL_TRANSACTION   = 0x00ffffff,
};

Interface::Interface(const char *localName, Interface *super)
    : Scope(localName),
      mSuperType(super),
      mIsJavaCompatibleInProgress(false) {
    mReservedMethods.push_back(createDescriptorChainMethod());
}

Method *Interface::createDescriptorChainMethod() const {
    VectorType *vecType = new VectorType();
    vecType->setElementType(new StringType());
    std::vector<TypedVar *> *results = new std::vector<TypedVar *>();
    results->push_back(new TypedVar("indicator", vecType));

    return new Method("interfaceChain",
        new std::vector<TypedVar *>() /* args */,
        results,
        false /* oneway */,
        new std::vector<Annotation *>(),
        HIDL_DESCRIPTOR_CHAIN_TRANSACTION,
        [this](auto &out) { /* cppImpl */
            std::vector<const Interface *> chain = typeChain();
            out << "::android::hardware::hidl_vec<::android::hardware::hidl_string> _hidl_return;\n";
            out << "_hidl_return.resize(" << chain.size() << ");\n";
            for (size_t i = 0; i < chain.size(); ++i) {
                out << "_hidl_return[" << i << "] = \"" << chain[i]->fqName().string() << "\";\n";
            }
            out << "_hidl_cb(_hidl_return);\n";
            out << "return ::android::hardware::Void();";
        },
        [this](auto &out) { /* javaImpl */
            std::vector<const Interface *> chain = typeChain();
            out << "return new ArrayList<String>(Arrays.asList(\n";
            out.indent(); out.indent();
            for (size_t i = 0; i < chain.size(); ++i) {
                if (i != 0)
                    out << ",\n";
                out << "\"" << chain[i]->fqName().string() << "\"";
            }
            out << "));";
            out.unindent(); out.unindent();
        });
}


bool Interface::addMethod(Method *method) {
    CHECK(!method->isHidlReserved());
    if (lookupMethod(method->name()) != nullptr) {
        LOG(ERROR) << "Redefinition of method " << method->name();
        return false;
    }
    size_t serial = FIRST_CALL_TRANSACTION;

    serial += userDefinedMethods().size();

    const Interface *ancestor = mSuperType;
    while (ancestor != nullptr) {
        serial += ancestor->userDefinedMethods().size();
        ancestor = ancestor->superType();
    }

    CHECK(serial <= LAST_CALL_TRANSACTION) << "More than "
            << LAST_CALL_TRANSACTION << " methods are not allowed.";
    method->setSerialId(serial);
    mUserMethods.push_back(method);

    return true;
}


const Interface *Interface::superType() const {
    return mSuperType;
}

std::vector<const Interface *> Interface::typeChain() const {
    std::vector<const Interface *> v;
    const Interface *iface = this;
    while (iface != nullptr) {
        v.push_back(iface);
        iface = iface->mSuperType;
    }
    return v;
}

std::vector<const Interface *> Interface::superTypeChain() const {
    return superType()->typeChain(); // should work even if superType is nullptr
}

bool Interface::isInterface() const {
    return true;
}

bool Interface::isBinder() const {
    return true;
}

const std::vector<Method *> &Interface::userDefinedMethods() const {
    return mUserMethods;
}

const std::vector<Method *> &Interface::hidlReservedMethods() const {
    return mReservedMethods;
}

std::vector<Method *> Interface::methods() const {
    std::vector<Method *> v(mUserMethods);
    v.insert(v.end(), mReservedMethods.begin(), mReservedMethods.end());
    return v;
}

std::vector<InterfaceAndMethod> Interface::allMethodsFromRoot() const {
    std::vector<InterfaceAndMethod> v;
    std::vector<const Interface *> chain = typeChain();
    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
        const Interface *iface = *it;
        for (Method *userMethod : iface->userDefinedMethods()) {
            v.push_back(InterfaceAndMethod(iface, userMethod));
        }
    }
    for (Method *reservedMethod : hidlReservedMethods()) {
        v.push_back(InterfaceAndMethod(this, reservedMethod));
    }
    return v;
}

Method *Interface::lookupMethod(std::string name) const {
    for (const auto &tuple : allMethodsFromRoot()) {
        Method *method = tuple.method();
        if (method->name() == name) {
            return method;
        }
    }

    return nullptr;
}

std::string Interface::getBaseName() const {
    return fqName().getInterfaceBaseName();
}

std::string Interface::getCppType(StorageMode mode,
                                  std::string *extra,
                                  bool specifyNamespaces) const {
    extra->clear();
    const std::string base =
          std::string(specifyNamespaces ? "::android::" : "")
        + "sp<"
        + (specifyNamespaces ? fullName() : partialCppName())
        + ">";

    switch (mode) {
        case StorageMode_Stack:
        case StorageMode_Result:
            return base;

        case StorageMode_Argument:
            return "const " + base + "&";
    }
}

std::string Interface::getJavaType(
        std::string *extra, bool /* forInitializer */) const {
    extra->clear();
    return fullJavaName();
}

void Interface::emitReaderWriter(
        Formatter &out,
        const std::string &name,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode) const {
    const std::string parcelObjDeref =
        parcelObj + (parcelObjIsPointer ? "->" : ".");

    if (isReader) {
        out << "{\n";
        out.indent();

        const std::string binderName = "_hidl_" + name + "_binder";

        out << "::android::sp<::android::hardware::IBinder> "
            << binderName << ";\n";

        out << "_hidl_err = ";
        out << parcelObjDeref
            << "readNullableStrongBinder(&"
            << binderName
            << ");\n";

        handleError(out, mode);

        out << name
            << " = "
            << fqName().cppNamespace()
            << "::IHw"
            << getBaseName()
            << "::asInterface("
            << binderName
            << ");\n";

        out.unindent();
        out << "}\n\n";
    } else {
        out << "if (" << name << " == nullptr) {\n";
        out.indent();
        out << "_hidl_err = ";
        out << parcelObjDeref
            << "writeStrongBinder(nullptr);\n";
        out.unindent();
        out << "} else {\n";
        out.indent();
        out << "_hidl_err = "
            << parcelObjDeref
            << "writeStrongBinder("
            << name
            << "->toBinder());\n";
        out.unindent();
        out << "}\n";

        handleError(out, mode);
    }
}

void Interface::emitJavaReaderWriter(
        Formatter &out,
        const std::string &parcelObj,
        const std::string &argName,
        bool isReader) const {
    if (isReader) {
        out << fullJavaName()
            << ".asInterface("
            << parcelObj
            << ".readStrongBinder());\n";
    } else {
        out << parcelObj
            << ".writeStrongBinder("
            << argName
            << " == null ? null : "
            << argName
            << ".asBinder());\n";
    }
}

status_t Interface::emitVtsAttributeDeclaration(Formatter &out) const {
    for (const auto &type : getSubTypes()) {
        // Skip for TypeDef as it is just an alias of a defined type.
        if (type->isTypeDef()) {
            continue;
        }
        out << "attribute: {\n";
        out.indent();
        status_t status = type->emitVtsTypeDeclarations(out);
        if (status != OK) {
            return status;
        }
        out.unindent();
        out << "}\n\n";
    }
    return OK;
}

status_t Interface::emitVtsMethodDeclaration(Formatter &out) const {
    for (const auto &method : methods()) {
        out << "api: {\n";
        out.indent();
        out << "name: \"" << method->name() << "\"\n";
        // Generate declaration for each return value.
        for (const auto &result : method->results()) {
            out << "return_type_hidl: {\n";
            out.indent();
            status_t status = result->type().emitVtsAttributeType(out);
            if (status != OK) {
                return status;
            }
            out.unindent();
            out << "}\n";
        }
        // Generate declaration for each input argument
        for (const auto &arg : method->args()) {
            out << "arg: {\n";
            out.indent();
            status_t status = arg->type().emitVtsAttributeType(out);
            if (status != OK) {
                return status;
            }
            out.unindent();
            out << "}\n";
        }
        // Generate declaration for each annotation.
        for (const auto &annotation : method->annotations()) {
            out << "callflow: {\n";
            out.indent();
            std::string name = annotation->name();
            if (name == "entry") {
                out << "entry: true\n";
            } else if (name == "exit") {
                out << "exit: true\n";
            } else if (name == "callflow") {
                const AnnotationParam *param =
                        annotation->getParam("next");
                if (param != nullptr) {
                    for (auto value : *param->getValues()) {
                        out << "next: " << value << "\n";
                    }
                }
            } else {
                std::cerr << "Invalid annotation '"
                          << name << "' for method: " << method->name()
                          << ". Should be one of: entry, exit, callflow. \n";
                return UNKNOWN_ERROR;
            }
            out.unindent();
            out << "}\n";
        }
        out.unindent();
        out << "}\n\n";
    }
    return OK;
}

status_t Interface::emitVtsAttributeType(Formatter &out) const {
    out << "type: TYPE_HIDL_CALLBACK\n"
        << "predefined_type: \""
        << localName()
        << "\"\n"
        << "is_callback: true\n";
    return OK;
}


bool Interface::hasOnewayMethods() const {
    for (auto const &method : methods()) {
        if (method->isOneway()) {
            return true;
        }
    }

    const Interface* superClass = superType();

    if (superClass != nullptr) {
        return superClass->hasOnewayMethods();
    }

    return false;
}

bool Interface::isJavaCompatible() const {
    if (mIsJavaCompatibleInProgress) {
        // We're currently trying to determine if this Interface is
        // java-compatible and something is referencing this interface through
        // one of its methods. Assume we'll ultimately succeed, if we were wrong
        // the original invocation of Interface::isJavaCompatible() will then
        // return the correct "false" result.
        return true;
    }

    if (mSuperType != nullptr && !mSuperType->isJavaCompatible()) {
        mIsJavaCompatibleInProgress = false;
        return false;
    }

    mIsJavaCompatibleInProgress = true;

    if (!Scope::isJavaCompatible()) {
        mIsJavaCompatibleInProgress = false;
        return false;
    }

    for (const auto &method : methods()) {
        if (!method->isJavaCompatible()) {
            mIsJavaCompatibleInProgress = false;
            return false;
        }
    }

    mIsJavaCompatibleInProgress = false;

    return true;
}

}  // namespace android

