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

#include "AST.h"

#include "Coordinator.h"
#include "EnumType.h"
#include "Interface.h"
#include "Method.h"
#include "ScalarType.h"
#include "Scope.h"

#include <algorithm>
#include <hidl-util/StringHelper.h>
#include <hidl-util/Formatter.h>
#include <android-base/logging.h>
#include <string>
#include <vector>

namespace android {

status_t AST::generateCpp(const std::string &outputPath) const {
    status_t err = generateInterfaceHeader(outputPath);

    if (err == OK) {
        err = generateStubHeader(outputPath);
    }

    if (err == OK) {
        err = generateHwBinderHeader(outputPath);
    }

    if (err == OK) {
        err = generateProxyHeader(outputPath);
    }

    if (err == OK) {
        err = generateAllSource(outputPath);
    }

    if (err == OK) {
        generatePassthroughHeader(outputPath);
    }

    return err;
}

void AST::getPackageComponents(
        std::vector<std::string> *components) const {
    mPackage.getPackageComponents(components);
}

void AST::getPackageAndVersionComponents(
        std::vector<std::string> *components, bool cpp_compatible) const {
    mPackage.getPackageAndVersionComponents(components, cpp_compatible);
}

std::string AST::makeHeaderGuard(const std::string &baseName) const {
    std::string guard = "HIDL_GENERATED_";
    guard += mPackage.tokenName();

    guard += "_";
    guard += baseName;
    guard += "_H_";

    return guard;
}

void AST::enterLeaveNamespace(Formatter &out, bool enter) const {
    std::vector<std::string> packageComponents;
    getPackageAndVersionComponents(
            &packageComponents, true /* cpp_compatible */);

    if (enter) {
        for (const auto &component : packageComponents) {
            out << "namespace " << component << " {\n";
        }

        out.setNamespace(mPackage.cppNamespace() + "::");
    } else {
        out.setNamespace(std::string());

        for (auto it = packageComponents.rbegin();
                it != packageComponents.rend();
                ++it) {
            out << "}  // namespace " << *it << "\n";
        }
    }
}

status_t AST::generateInterfaceHeader(const std::string &outputPath) const {

    std::string path = outputPath;
    path.append(mCoordinator->convertPackageRootToPath(mPackage));
    path.append(mCoordinator->getPackagePath(mPackage, true /* relative */));

    std::string ifaceName;
    bool isInterface = true;
    if (!AST::isInterface(&ifaceName)) {
        ifaceName = "types";
        isInterface = false;
    }
    path.append(ifaceName);
    path.append(".h");

    CHECK(Coordinator::MakeParentHierarchy(path));
    FILE *file = fopen(path.c_str(), "w");

    if (file == NULL) {
        return -errno;
    }

    Formatter out(file);

    const std::string guard = makeHeaderGuard(ifaceName);

    out << "#ifndef " << guard << "\n";
    out << "#define " << guard << "\n\n";

    for (const auto &item : mImportedNames) {
        out << "#include <";

        std::vector<std::string> components;
        item.getPackageAndVersionComponents(
                &components, false /* cpp_compatible */);

        for (const auto &component : components) {
            out << component << "/";
        }

        out << item.name()
            << ".h>\n";
    }

    if (!mImportedNames.empty()) {
        out << "\n";
    }

    out << "#include <hidl/HidlSupport.h>\n";
    out << "#include <hidl/IServiceManager.h>\n";
    out << "#include <hidl/MQDescriptor.h>\n";

    if (isInterface) {
        out << "#include <hidl/Status.h>\n";
    }

    out << "#include <utils/NativeHandle.h>\n\n";

    enterLeaveNamespace(out, true /* enter */);
    out << "\n";

    if (isInterface) {
        out << "struct "
            << ifaceName;

        const Interface *iface = mRootScope->getInterface();
        const Interface *superType = iface->superType();

        if (superType == NULL) {
            out << " : virtual public RefBase";
        } else {
            out << " : public "
                << superType->fullName();
        }

        out << " {\n";

        out.indent();

    }

    status_t err = emitTypeDeclarations(out);

    if (err != OK) {
        return err;
    }

    if (isInterface) {
        const Interface *iface = mRootScope->getInterface();
        const Interface *superType = iface->superType();
        const std::string baseName = iface->getBaseName();
        out << "constexpr static hidl_version version = {"
            << mPackage.getPackageMajorVersion() << ","
            << mPackage.getPackageMinorVersion() << "};\n";
        out << "virtual const hidl_version& getInterfaceVersion() const {\n";
        out.indent();
        out << "return version;\n";
        out.unindent();
        out << "}\n\n";
        out << "virtual bool isRemote() const { return false; }\n\n";
        bool haveCallbacks = false;
        for (const auto &method : iface->methods()) {
            const bool returnsValue = !method->results().empty();

            if (!returnsValue) {
                continue;
            }

            if (method->canElideCallback() != nullptr) {
                continue;
            }

            haveCallbacks = true;

            out << "using "
                << method->name()
                << "_cb = std::function<void("
                << Method::GetArgSignature(method->results(),
                                           true /* specify namespaces */)
                << ")>;\n";
        }

        if (haveCallbacks) {
            out << "\n";
        }

        for (const auto &method : iface->methods()) {
            const bool returnsValue = !method->results().empty();

            method->dumpAnnotations(out);

            const TypedVar *elidedReturn = method->canElideCallback();
            if (elidedReturn) {
                std::string extra;
                out << "virtual ::android::hardware::Return<";
                out << elidedReturn->type().getCppResultType(&extra) << "> ";
            } else {
                out << "virtual ::android::hardware::Return<void> ";
            }

            out << method->name()
                << "("
                << Method::GetArgSignature(method->args(),
                                           true /* specify namespaces */);

            if (returnsValue && elidedReturn == nullptr) {
                if (!method->args().empty()) {
                    out << ", ";
                }

                out << method->name() << "_cb _hidl_cb";
            }

            out << ") = 0;\n";
        }

        out << "DECLARE_REGISTER_AND_GET_SERVICE(" << baseName << ")\n";
    }

    if (isInterface) {
        out.unindent();

        out << "};\n";
    }

    out << "\n";
    enterLeaveNamespace(out, false /* enter */);

    out << "\n#endif  // " << guard << "\n";

    return OK;
}

status_t AST::generateHwBinderHeader(const std::string &outputPath) const {
    std::string ifaceName;
    if(!AST::isInterface(&ifaceName)) {
        // types.hal does not get an HwBinder header.
        return OK;
    }

    const Interface *iface = mRootScope->getInterface();
    const std::string baseName = iface->getBaseName();

    const std::string klassName = "IHw" + baseName;

    std::string path = outputPath;
    path.append(mCoordinator->convertPackageRootToPath(mPackage));
    path.append(mCoordinator->getPackagePath(mPackage, true /* relative */));
    path.append(klassName + ".h");

    FILE* file = fopen(path.c_str(), "w");

    if (file == NULL) {
        return -errno;
    }

    Formatter out(file);

    const std::string guard = makeHeaderGuard(klassName);

    out << "#ifndef " << guard << "\n";
    out << "#define " << guard << "\n\n";

    std::vector<std::string> packageComponents;
    getPackageAndVersionComponents(
            &packageComponents, false /* cpp_compatible */);

    out << "#include <";
    for (const auto &component : packageComponents) {
        out << component << "/";
    }
    out << ifaceName << ".h>\n\n";

    for (const auto &item : mImportedNames) {
        if (item.name() == "types") {
            continue;
        }

        out << "#include <";

        std::vector<std::string> components;
        item.getPackageAndVersionComponents(
                &components, false /* cpp_compatible */);

        for (const auto &component : components) {
            out << component << "/";
        }

        const std::string itemBaseName = item.getInterfaceBaseName();

        out << "Bn"
            << itemBaseName
            << ".h>\n";
    }

    out << "\n";

    out << "#include <hidl/HidlSupport.h>\n";
    out << "#include <hidl/Status.h>\n";
    out << "#include <hwbinder/IBinder.h>\n";
    out << "#include <hwbinder/IInterface.h>\n";

    out << "\n";

    enterLeaveNamespace(out, true /* enter */);
    out << "\n";

    out << "struct "
        << klassName
        << " : public "
        << ifaceName;

    const Interface *superType = iface->superType();

    out << ", public ::android::hardware::IInterface";

    out << " {\n";

    out.indent();

    out << "DECLARE_HWBINDER_META_INTERFACE(" << baseName << ");\n\n";

    out << "enum Call {\n";
    out.indent();

    bool first = true;
    for (const auto &method : iface->methods()) {
        out << StringHelper::Uppercase(method->name());

        if (first) {
            out << " = ";
            if (superType != NULL) {
                out << superType->fqName().cppNamespace()
                    << "::IHw"
                    << superType->getBaseName()
                    << "::Call::CallCount";
            } else {
                out << "::android::hardware::IBinder::FIRST_CALL_TRANSACTION";
            }

            first = false;
        }

        out << ",\n";
    }

    out << "CallCount\n";

    out.unindent();
    out << "};\n\n";

    out.unindent();

    out << "};\n\n";

    enterLeaveNamespace(out, false /* enter */);

    out << "\n#endif  // " << guard << "\n";

    return OK;
}

status_t AST::emitTypeDeclarations(Formatter &out) const {
    return mRootScope->emitTypeDeclarations(out);
}

status_t AST::generateStubMethod(Formatter &out,
                                 const std::string &className,
                                 const Method *method,
                                 bool specifyNamespaces) const {
    out << "inline ";

    method->generateCppSignature(out,
                                 className,
                                 specifyNamespaces);

    const bool returnsValue = !method->results().empty();
    const TypedVar *elidedReturn = method->canElideCallback();
    out << " {\n";
    out.indent();
    out << "return mImpl->"
        << method->name()
        << "(";
    bool first = true;
    for (const auto &arg : method->args()) {
        if (!first) {
            out << ", ";
        }
        first = false;
        out << arg->name();
    }
    if (returnsValue && elidedReturn == nullptr) {
        if (!method->args().empty()) {
            out << ", ";
        }

        out << "_hidl_cb";
    }
    out << ");\n";
    out.unindent();
    out << "}";

    out << ";\n";

    return OK;
}

status_t AST::generateProxyDeclaration(Formatter &out,
                                       const std::string &className,
                                       const Method *method,
                                       bool specifyNamespaces) const {

    method->generateCppSignature(out,
                                 className,
                                 specifyNamespaces);
    out << " override;\n";

    return OK;
}


status_t AST::generatePassthroughMethod(Formatter &out,
                                        const std::string &className,
                                        const Method *method,
                                        bool specifyNamespaces) const {
    method->generateCppSignature(out, className, specifyNamespaces);

    out << " {\n";
    out.indent();

    const bool returnsValue = !method->results().empty();
    const TypedVar *elidedReturn = method->canElideCallback();

    if (returnsValue && elidedReturn == nullptr) {
        generateCheckNonNull(out, "_hidl_cb");
    }

    out << "return ";

    if (method->isOneway()) {
        out << "addOnewayTask([this";
        for (const auto &arg : method->args()) {
            out << ", " << arg->name();
        }
        out << "] {this->";
    }

    out << "mImpl->"
        << method->name()
        << "(";

    bool first = true;
    for (const auto &arg : method->args()) {
        if (!first) {
            out << ", ";
        }
        first = false;
        out << arg->name();
    }
    if (returnsValue && elidedReturn == nullptr) {
        if (!method->args().empty()) {
            out << ", ";
        }

        out << "_hidl_cb";
    }
    out << ")";

    if (method->isOneway()) {
        out << ";})";
    }
    out << ";\n";

    out.unindent();
    out << "}\n";

    return OK;
}

status_t AST::generateMethods(
        Formatter &out,
        const std::string &className,
        MethodLocation type,
        bool specifyNamespaces) const {

    const Interface *iface = mRootScope->getInterface();

    std::vector<const Interface *> chain;
    while (iface != NULL) {
        chain.push_back(iface);
        iface = iface->superType();
    }

    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
        const Interface *superInterface = *it;

        out << "// Methods from "
            << superInterface->fullName()
            << " follow.\n";

        for (const auto &method : superInterface->methods()) {
            status_t err;
            switch(type) {
                case STUB_HEADER:
                    err = generateStubMethod(out,
                                             className,
                                             method,
                                             specifyNamespaces);
                    break;
                case PROXY_HEADER:
                    err = generateProxyDeclaration(out,
                                                   className,
                                                   method,
                                                   specifyNamespaces);
                    break;
                case IMPL_HEADER:
                    err = generateStubImplDeclaration(out,
                                                      className,
                                                      method,
                                                      specifyNamespaces);
                    break;
                case IMPL_SOURCE:
                    err = generateStubImplMethod(out,
                                                 className,
                                                 method,
                                                 specifyNamespaces);
                    break;
                case PASSTHROUGH_HEADER:
                    err = generatePassthroughMethod(out,
                                                    className,
                                                    method,
                                                    specifyNamespaces);
                    break;
                default:
                    LOG(ERROR) << "Unkown method type: " << type;
                    err = UNKNOWN_ERROR;
            }

            if (err != OK) {
                return err;
            }
        }

        out << "\n";
    }

    return OK;
}

status_t AST::generateStubHeader(const std::string &outputPath) const {
    std::string ifaceName;
    if (!AST::isInterface(&ifaceName)) {
        // types.hal does not get a stub header.
        return OK;
    }

    const Interface *iface = mRootScope->getInterface();
    const std::string baseName = iface->getBaseName();
    const std::string klassName = "Bn" + baseName;

    std::string path = outputPath;
    path.append(mCoordinator->convertPackageRootToPath(mPackage));
    path.append(mCoordinator->getPackagePath(mPackage, true /* relative */));
    path.append(klassName);
    path.append(".h");

    CHECK(Coordinator::MakeParentHierarchy(path));
    FILE *file = fopen(path.c_str(), "w");

    if (file == NULL) {
        return -errno;
    }

    Formatter out(file);

    const std::string guard = makeHeaderGuard(klassName);

    out << "#ifndef " << guard << "\n";
    out << "#define " << guard << "\n\n";

    std::vector<std::string> packageComponents;
    getPackageAndVersionComponents(
            &packageComponents, false /* cpp_compatible */);

    out << "#include <";
    for (const auto &component : packageComponents) {
        out << component << "/";
    }
    out << "IHw" << baseName << ".h>\n\n";

    enterLeaveNamespace(out, true /* enter */);
    out << "\n";

    out << "struct "
        << "Bn"
        << baseName
        << " : public ::android::hardware::BnInterface<I"
        << baseName << ", IHw" << baseName
        << "> {\n";

    out.indent();
    out << "explicit Bn"
        << baseName
        << "(const ::android::sp<" << ifaceName << "> &_hidl_impl);"
        << "\n\n";
    out << "::android::status_t onTransact(\n";
    out.indent();
    out.indent();
    out << "uint32_t _hidl_code,\n";
    out << "const ::android::hardware::Parcel &_hidl_data,\n";
    out << "::android::hardware::Parcel *_hidl_reply,\n";
    out << "uint32_t _hidl_flags = 0,\n";
    out << "TransactCallback _hidl_cb = nullptr) override;\n\n";
    out.unindent();
    out.unindent();

    status_t err = generateMethods(out,
                                   "" /* class name */,
                                   MethodLocation::STUB_HEADER,
                                   true /* specify namespaces */);

    if (err != OK) {
        return err;
    }

    // Generated code for instrumentation.
    out << "// for hidl instrumentation.\n";
    out << "std::vector<InstrumentationCallback> instrumentationCallbacks;\n\n";
    out << "bool enableInstrumentation = false;\n";

    out.unindent();

    out << "};\n\n";

    enterLeaveNamespace(out, false /* enter */);

    out << "\n#endif  // " << guard << "\n";

    return OK;
}

status_t AST::generateProxyHeader(const std::string &outputPath) const {
    std::string ifaceName;
    if (!AST::isInterface(&ifaceName)) {
        // types.hal does not get a proxy header.
        return OK;
    }

    const Interface *iface = mRootScope->getInterface();
    const std::string baseName = iface->getBaseName();

    std::string path = outputPath;
    path.append(mCoordinator->convertPackageRootToPath(mPackage));
    path.append(mCoordinator->getPackagePath(mPackage, true /* relative */));
    path.append("Bp");
    path.append(baseName);
    path.append(".h");

    CHECK(Coordinator::MakeParentHierarchy(path));
    FILE *file = fopen(path.c_str(), "w");

    if (file == NULL) {
        return -errno;
    }

    Formatter out(file);

    const std::string guard = makeHeaderGuard("Bp" + baseName);

    out << "#ifndef " << guard << "\n";
    out << "#define " << guard << "\n\n";

    std::vector<std::string> packageComponents;
    getPackageAndVersionComponents(
            &packageComponents, false /* cpp_compatible */);

    out << "#include <";
    for (const auto &component : packageComponents) {
        out << component << "/";
    }
    out << "IHw" << baseName << ".h>\n\n";

    enterLeaveNamespace(out, true /* enter */);
    out << "\n";

    out << "struct "
        << "Bp"
        << baseName
        << " : public ::android::hardware::BpInterface<IHw"
        << baseName
        << "> {\n";

    out.indent();

    out << "explicit Bp"
        << baseName
        << "(const ::android::sp<::android::hardware::IBinder> &_hidl_impl);"
        << "\n\n";

    out << "virtual bool isRemote() const { return true; }\n\n";

    status_t err = generateMethods(out,
                                   "" /* class name */,
                                   MethodLocation::PROXY_HEADER,
                                   true /* generate specify namespaces */);

    if (err != OK) {
        return err;
    }

    out.unindent();

    out << "};\n\n";

    enterLeaveNamespace(out, false /* enter */);

    out << "\n#endif  // " << guard << "\n";

    return OK;
}

status_t AST::generateAllSource(const std::string &outputPath) const {

    std::string path = outputPath;
    path.append(mCoordinator->convertPackageRootToPath(mPackage));
    path.append(mCoordinator->getPackagePath(mPackage, true /* relative */));

    std::string ifaceName;
    std::string baseName;

    bool isInterface = true;
    if (!AST::isInterface(&ifaceName)) {
        baseName = "types";
        isInterface = false;
    } else {
        const Interface *iface = mRootScope->getInterface();
        baseName = iface->getBaseName();
    }

    path.append(baseName);

    if (baseName != "types") {
        path.append("All");
    }

    path.append(".cpp");

    CHECK(Coordinator::MakeParentHierarchy(path));
    FILE *file = fopen(path.c_str(), "w");

    if (file == NULL) {
        return -errno;
    }

    Formatter out(file);

    std::vector<std::string> packageComponents;
    getPackageAndVersionComponents(
            &packageComponents, false /* cpp_compatible */);

    std::string prefix;
    for (const auto &component : packageComponents) {
        prefix += component;
        prefix += "/";
    }

    if (isInterface) {
        out << "#include <" << prefix << "/Bp" << baseName << ".h>\n";
        out << "#include <" << prefix << "/Bn" << baseName << ".h>\n";
        out << "#include <" << prefix << "/Bs" << baseName << ".h>\n";
        out << "#include <cutils/properties.h>\n";
    } else {
        out << "#include <" << prefix << "types.h>\n";
    }

    out << "\n";

    enterLeaveNamespace(out, true /* enter */);
    out << "\n";

    status_t err = generateTypeSource(out, ifaceName);

    if (err == OK && isInterface) {
        out << "constexpr hidl_version " << ifaceName << "::version;\n\n";
        err = generateProxySource(out, baseName);
    }

    if (err == OK && isInterface) {
        err = generateStubSource(out, baseName);
    }

    if (err == OK && isInterface) {
        err = generatePassthroughSource(out);
    }

    if (err == OK && isInterface) {
        const Interface *iface = mRootScope->getInterface();

        out << "IMPLEMENT_REGISTER_AND_GET_SERVICE("
            << baseName << ", "
            << "\"" << iface->fqName().package()
            << iface->fqName().version() << "-impl.so\""
            << ")\n";
    }

    enterLeaveNamespace(out, false /* enter */);

    return err;
}

// static
void AST::generateCheckNonNull(Formatter &out, const std::string &nonNull) {
    out << "if (" << nonNull << " == nullptr) {\n";
    out.indent();
    out << "return ::android::hardware::Status::fromExceptionCode(\n";
    out.indent();
    out.indent();
    out << "::android::hardware::Status::EX_ILLEGAL_ARGUMENT);\n";
    out.unindent();
    out.unindent();
    out.unindent();
    out << "}\n\n";
}

status_t AST::generateTypeSource(
        Formatter &out, const std::string &ifaceName) const {
    return mRootScope->emitTypeDefinitions(out, ifaceName);
}

void AST::declareCppReaderLocals(
        Formatter &out,
        const std::vector<TypedVar *> &args,
        bool forResults) const {
    if (args.empty()) {
        return;
    }

    for (const auto &arg : args) {
        const Type &type = arg->type();

        std::string extra;
        out << type.getCppResultType(&extra)
            << " "
            << (forResults ? "_hidl_out_" : "")
            << arg->name()
            << extra
            << ";\n";
    }

    out << "\n";
}

void AST::emitCppReaderWriter(
        Formatter &out,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        const TypedVar *arg,
        bool isReader,
        Type::ErrorMode mode,
        bool addPrefixToName) const {
    const Type &type = arg->type();

    type.emitReaderWriter(
            out,
            addPrefixToName ? ("_hidl_out_" + arg->name()) : arg->name(),
            parcelObj,
            parcelObjIsPointer,
            isReader,
            mode);
}

void AST::emitCppResolveReferences(
        Formatter &out,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        const TypedVar *arg,
        bool isReader,
        Type::ErrorMode mode,
        bool addPrefixToName) const {
    const Type &type = arg->type();
    if(type.needsResolveReferences()) {
        type.emitResolveReferences(
                out,
                addPrefixToName ? ("_hidl_out_" + arg->name()) : arg->name(),
                isReader, // nameIsPointer
                parcelObj,
                parcelObjIsPointer,
                isReader,
                mode);
    }
}

status_t AST::generateProxySource(
        Formatter &out, const std::string &baseName) const {
    const std::string klassName = "Bp" + baseName;

    out << klassName
        << "::"
        << klassName
        << "(const ::android::sp<::android::hardware::IBinder> &_hidl_impl)\n";

    out.indent();
    out.indent();

    out << ": BpInterface"
        << "<IHw"
        << baseName
        << ">(_hidl_impl) {\n";

    out.unindent();
    out.unindent();
    out << "}\n\n";

    const Interface *iface = mRootScope->getInterface();

    std::vector<const Interface *> chain;
    while (iface != NULL) {
        chain.push_back(iface);
        iface = iface->superType();
    }

    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
        const Interface *superInterface = *it;

        for (const auto &method : superInterface->methods()) {
            method->generateCppSignature(out,
                                         klassName,
                                         true /* specify namespaces */);

            const bool returnsValue = !method->results().empty();
            const TypedVar *elidedReturn = method->canElideCallback();

            out << "{\n";

            out.indent();

            if (returnsValue && elidedReturn == nullptr) {
                generateCheckNonNull(out, "_hidl_cb");
            }

            out << "::android::hardware::Parcel _hidl_data;\n";
            out << "::android::hardware::Parcel _hidl_reply;\n";
            out << "::android::status_t _hidl_err;\n";
            out << "::android::hardware::Status _hidl_status;\n\n";

            declareCppReaderLocals(
                    out, method->results(), true /* forResults */);

            out << "_hidl_err = _hidl_data.writeInterfaceToken("
                << superInterface->fqName().cppNamespace()
                << "::IHw"
                << superInterface->getBaseName()
                << "::descriptor);\n";

            out << "if (_hidl_err != ::android::OK) { goto _hidl_error; }\n\n";

            // First DFS: write all buffers and resolve pointers for parent
            for (const auto &arg : method->args()) {
                emitCppReaderWriter(
                        out,
                        "_hidl_data",
                        false /* parcelObjIsPointer */,
                        arg,
                        false /* reader */,
                        Type::ErrorMode_Goto,
                        false /* addPrefixToName */);
            }

            // Second DFS: resolve references.
            for (const auto &arg : method->args()) {
                emitCppResolveReferences(
                        out,
                        "_hidl_data",
                        false /* parcelObjIsPointer */,
                        arg,
                        false /* reader */,
                        Type::ErrorMode_Goto,
                        false /* addPrefixToName */);
            }

            out << "_hidl_err = remote()->transact("
                      << superInterface->fqName().cppNamespace()
                      << "::IHw"
                      << superInterface->getBaseName()
                      << "::Call::"
                      << StringHelper::Uppercase(method->name())
                      << ", _hidl_data, &_hidl_reply";
            if (method->isOneway()) {
                out << ", ::android::hardware::IBinder::FLAG_ONEWAY";
            }
            out << ");\n";

            out << "if (_hidl_err != ::android::OK) { goto _hidl_error; }\n\n";

            if (!method->isOneway()) {
                out << "_hidl_err = _hidl_status.readFromParcel(_hidl_reply);\n";
                out << "if (_hidl_err != ::android::OK) { goto _hidl_error; }\n\n";
                out << "if (!_hidl_status.isOk()) { return _hidl_status; }\n\n";


                // First DFS: write all buffers and resolve pointers for parent
                for (const auto &arg : method->results()) {
                    emitCppReaderWriter(
                            out,
                            "_hidl_reply",
                            false /* parcelObjIsPointer */,
                            arg,
                            true /* reader */,
                            Type::ErrorMode_Goto,
                            true /* addPrefixToName */);
                }

                // Second DFS: resolve references.
                for (const auto &arg : method->results()) {
                    emitCppResolveReferences(
                            out,
                            "_hidl_reply",
                            false /* parcelObjIsPointer */,
                            arg,
                            true /* reader */,
                            Type::ErrorMode_Goto,
                            true /* addPrefixToName */);
                }

                if (returnsValue && elidedReturn == nullptr) {
                    out << "_hidl_cb(";

                    bool first = true;
                    for (const auto &arg : method->results()) {
                        if (!first) {
                            out << ", ";
                        }

                        if (arg->type().resultNeedsDeref()) {
                            out << "*";
                        }
                        out << "_hidl_out_" << arg->name();

                        first = false;
                    }

                    out << ");\n\n";
                }
            }

            if (elidedReturn != nullptr) {
                std::string extra;
                out << "_hidl_status.setFromStatusT(_hidl_err);\n";
                out << "return ::android::hardware::Return<";
                out << elidedReturn->type().getCppResultType(&extra)
                    << ">(_hidl_out_" << elidedReturn->name() << ");\n\n";
            } else {
                out << "_hidl_status.setFromStatusT(_hidl_err);\n";
                out << "return ::android::hardware::Return<void>();\n\n";
            }

            out.unindent();
            out << "_hidl_error:\n";
            out.indent();
            out << "_hidl_status.setFromStatusT(_hidl_err);\n";
            out << "return ::android::hardware::Return<";
            if (elidedReturn != nullptr) {
                std::string extra;
                out << method->results().at(0)->type().getCppResultType(&extra);
            } else {
                out << "void";
            }
            out << ">(_hidl_status);\n";

            out.unindent();
            out << "}\n\n";
        }
    }

    return OK;
}

status_t AST::generateStubSource(
        Formatter &out, const std::string &baseName) const {
    out << "IMPLEMENT_HWBINDER_META_INTERFACE("
        << baseName
        << ", \""
        << mPackage.string()
        << "::I"
        << baseName
        << "\");\n\n";

    const std::string klassName = "Bn" + baseName;

    out << klassName
        << "::"
        << klassName
        << "(const ::android::sp<I" << baseName <<"> &_hidl_impl)\n";

    out.indent();
    out.indent();

    out << ": BnInterface"
        << "<I"
        << baseName
        << ", IHw"
        << baseName
        << ">(_hidl_impl) {\n";

    out << "enableInstrumentation = "
           "property_get_bool(\"hal.instrumentation.enable\", false);\n";
    out << "registerInstrumentationCallbacks(\""
        << mPackage.string()
        << "::I"
        << baseName
        << "\", &instrumentationCallbacks);\n";
    out.unindent();
    out.unindent();
    out << "}\n\n";

    out << "::android::status_t " << klassName << "::onTransact(\n";

    out.indent();
    out.indent();

    out << "uint32_t _hidl_code,\n"
        << "const ::android::hardware::Parcel &_hidl_data,\n"
        << "::android::hardware::Parcel *_hidl_reply,\n"
        << "uint32_t _hidl_flags,\n"
        << "TransactCallback _hidl_cb) {\n";

    out.unindent();

    out << "::android::status_t _hidl_err = ::android::OK;\n\n";
    out << "switch (_hidl_code) {\n";
    out.indent();

    const Interface *iface = mRootScope->getInterface();

    std::vector<const Interface *> chain;
    while (iface != NULL) {
        chain.push_back(iface);
        iface = iface->superType();
    }

    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
        const Interface *superInterface = *it;

        for (const auto &method : superInterface->methods()) {
            out << "case "
                << superInterface->fqName().cppNamespace()
                << "::IHw"
                << superInterface->getBaseName()
                << "::Call::"
                << StringHelper::Uppercase(method->name())
                << ":\n{\n";

            out.indent();

            status_t err =
                generateStubSourceForMethod(out, superInterface, method);

            if (err != OK) {
                return err;
            }

            out.unindent();
            out << "}\n\n";
        }
    }

    out << "default:\n{\n";
    out.indent();

    out << "return ::android::hardware::BnInterface<I"
        << baseName << ", IHw" << baseName
        << ">::onTransact(\n";

    out.indent();
    out.indent();

    out << "_hidl_code, _hidl_data, _hidl_reply, "
        << "_hidl_flags, _hidl_cb);\n";

    out.unindent();
    out.unindent();

    out.unindent();
    out << "}\n";

    out.unindent();
    out << "}\n\n";

    out << "if (_hidl_err == ::android::UNEXPECTED_NULL) {\n";
    out.indent();
    out << "_hidl_err = ::android::hardware::Status::fromExceptionCode(\n";
    out.indent();
    out.indent();
    out << "::android::hardware::Status::EX_NULL_POINTER)\n";
    out.indent();
    out.indent();
    out << ".writeToParcel(_hidl_reply);\n";
    out.unindent();
    out.unindent();
    out.unindent();
    out.unindent();

    out.unindent();
    out << "}\n\n";

    out << "return _hidl_err;\n";

    out.unindent();
    out << "}\n\n";

    return OK;
}

status_t AST::generateStubSourceForMethod(
        Formatter &out, const Interface *iface, const Method *method) const {
    out << "if (!_hidl_data.enforceInterface("
        << iface->fqName().cppNamespace()
        << "::IHw"
        << iface->getBaseName()
        << "::descriptor)) {\n";

    out.indent();
    out << "_hidl_err = ::android::BAD_TYPE;\n";
    out << "break;\n";
    out.unindent();
    out << "}\n\n";

    declareCppReaderLocals(out, method->args(), false /* forResults */);

    // First DFS: write buffers
    for (const auto &arg : method->args()) {
        emitCppReaderWriter(
                out,
                "_hidl_data",
                false /* parcelObjIsPointer */,
                arg,
                true /* reader */,
                Type::ErrorMode_Break,
                false /* addPrefixToName */);
    }

    // Second DFS: resolve references
    for (const auto &arg : method->args()) {
        emitCppResolveReferences(
                out,
                "_hidl_data",
                false /* parcelObjIsPointer */,
                arg,
                true /* reader */,
                Type::ErrorMode_Break,
                false /* addPrefixToName */);
    }

    out << "if (UNLIKELY(enableInstrumentation)) {\n";
    out.indent();
    out << "std::vector<void *> args;\n";
    for (const auto &arg : method->args()) {
        out << "args.push_back((void *)"
            << (arg->type().resultNeedsDeref() ? "" : "&")
            << arg->name()
            << ");\n";
    }

    out << "for (auto callback: instrumentationCallbacks) {\n";
    out.indent();
    out << "callback(InstrumentationEvent::SERVER_API_ENTRY, \""
        << mPackage.package()
        << "\", \"" << mPackage.version().substr(1) << "\", \""
        << iface->localName() << "\", \"" << method->name() << "\", &args);\n";
    out.unindent();
    out << "}\n";
    out.unindent();
    out << "}\n\n";

    const bool returnsValue = !method->results().empty();
    const TypedVar *elidedReturn = method->canElideCallback();

    if (elidedReturn != nullptr) {
        std::string extra;

        out << elidedReturn->type().getCppResultType(&extra) << " ";
        out << elidedReturn->name() << " = ";
        out << method->name() << "(";

        bool first = true;
        for (const auto &arg : method->args()) {
            if (!first) {
                out << ", ";
            }

            if (arg->type().resultNeedsDeref()) {
                out << "*";
            }

            out << arg->name();

            first = false;
        }

        out << ");\n\n";
        out << "::android::hardware::Status::ok()"
            << ".writeToParcel(_hidl_reply);\n\n";

        elidedReturn->type().emitReaderWriter(
                out,
                elidedReturn->name(),
                "_hidl_reply",
                true, /* parcelObjIsPointer */
                false, /* isReader */
                Type::ErrorMode_Ignore);

        emitCppResolveReferences(
                out,
                "_hidl_reply",
                true /* parcelObjIsPointer */,
                elidedReturn,
                false /* reader */,
                Type::ErrorMode_Ignore,
                false /* addPrefixToName */);

        out << "if (UNLIKELY(enableInstrumentation)) {\n";
        out.indent();
        out << "std::vector<void *> results;\n";
        out << "results.push_back((void *)&" << elidedReturn->name() << ");\n";

        out << "for (auto callback: instrumentationCallbacks) {\n";
        out.indent();
        out << "callback(InstrumentationEvent::SERVER_API_EXIT, \""
            << mPackage.package()
            << "\", \"" << mPackage.version().substr(1) << "\", \""
            << iface->localName() << "\", \"" << method->name()
            << "\", &results);\n";
        out.unindent();
        out << "}\n";
        out.unindent();
        out << "}\n\n";

        out << "_hidl_cb(*_hidl_reply);\n";
    } else {
        if (returnsValue) {
            out << "bool _hidl_callbackCalled = false;\n\n";
        }

        out << method->name() << "(";

        bool first = true;
        for (const auto &arg : method->args()) {
            if (!first) {
                out << ", ";
            }

            if (arg->type().resultNeedsDeref()) {
                out << "*";
            }

            out << arg->name();

            first = false;
        }

        if (returnsValue) {
            if (!first) {
                out << ", ";
            }

            out << "[&](";

            first = true;
            for (const auto &arg : method->results()) {
                if (!first) {
                    out << ", ";
                }

                out << "const auto &" << arg->name();

                first = false;
            }

            out << ") {\n";
            out.indent();
            out << "_hidl_callbackCalled = true;\n\n";

            out << "::android::hardware::Status::ok()"
                      << ".writeToParcel(_hidl_reply);\n\n";

            // First DFS: buffers
            for (const auto &arg : method->results()) {
                emitCppReaderWriter(
                        out,
                        "_hidl_reply",
                        true /* parcelObjIsPointer */,
                        arg,
                        false /* reader */,
                        Type::ErrorMode_Ignore,
                        false /* addPrefixToName */);
            }

            // Second DFS: resolve references
            for (const auto &arg : method->results()) {
                emitCppResolveReferences(
                        out,
                        "_hidl_reply",
                        true /* parcelObjIsPointer */,
                        arg,
                        false /* reader */,
                        Type::ErrorMode_Ignore,
                        false /* addPrefixToName */);
            }

            out << "if (UNLIKELY(enableInstrumentation)) {\n";
            out.indent();
            out << "std::vector<void *> results;\n";
            for (const auto &arg : method->results()) {
                out << "results.push_back((void *)&" << arg->name() << ");\n";
            }

            out << "for (auto callback: instrumentationCallbacks) {\n";
            out.indent();
            out << "callback(InstrumentationEvent::SERVER_API_EXIT, \""
                << mPackage.package()
                << "\", \"" << mPackage.version().substr(1) << "\", \""
                << iface->localName() << "\", \"" << method->name()
                << "\", &results);\n";
            out.unindent();
            out << "}\n";
            out.unindent();
            out << "}\n\n";

            out << "_hidl_cb(*_hidl_reply);\n";

            out.unindent();
            out << "}\n";
        }

        out << ");\n\n";

        // What to do if the stub implementation has a synchronous callback
        // which does not get invoked?  This is not a transport error but a
        // service error of sorts. For now, return OK to the caller, as this is
        // not a transport error.
        //
        // TODO(b/31365311) Figure out how to deal with this later.

        if (returnsValue) {
            out << "if (!_hidl_callbackCalled) {\n";
            out.indent();
        }

        out << "::android::hardware::Status::ok()"
            << ".writeToParcel(_hidl_reply);\n";

        if (returnsValue) {
            out.unindent();
            out << "}\n\n";
        }
    }

    out << "break;\n";

    return OK;
}

status_t AST::generatePassthroughHeader(const std::string &outputPath) const {
    std::string ifaceName;
    if (!AST::isInterface(&ifaceName)) {
        // types.hal does not get a stub header.
        return OK;
    }

    const Interface *iface = mRootScope->getInterface();

    const std::string baseName = iface->getBaseName();
    const std::string klassName = "Bs" + baseName;

    bool supportOneway = iface->hasOnewayMethods();

    std::string path = outputPath;
    path.append(mCoordinator->convertPackageRootToPath(mPackage));
    path.append(mCoordinator->getPackagePath(mPackage, true /* relative */));
    path.append(klassName);
    path.append(".h");

    CHECK(Coordinator::MakeParentHierarchy(path));
    FILE *file = fopen(path.c_str(), "w");

    if (file == NULL) {
        return -errno;
    }

    Formatter out(file);

    const std::string guard = makeHeaderGuard(klassName);

    out << "#ifndef " << guard << "\n";
    out << "#define " << guard << "\n\n";

    std::vector<std::string> packageComponents;
    getPackageAndVersionComponents(
            &packageComponents, false /* cpp_compatible */);

    out << "#include <future>\n";
    out << "#include <";
    for (const auto &component : packageComponents) {
        out << component << "/";
    }
    out << ifaceName << ".h>\n\n";

    if (supportOneway) {
        out << "#include <hidl/SynchronizedQueue.h>\n";
    }

    enterLeaveNamespace(out, true /* enter */);
    out << "\n";

    out << "struct "
        << klassName
        << " : " << ifaceName
        << " {\n";

    out.indent();
    out << "explicit "
        << klassName
        << "(const sp<"
        << ifaceName
        << "> impl);\n";

    status_t err = generateMethods(out,
                                   "" /* class name */,
                                   MethodLocation::PASSTHROUGH_HEADER,
                                   true /* specify namespaces */);

    if (err != OK) {
        return err;
    }

    out.unindent();
    out << "private:\n";
    out.indent();
    out << "const sp<" << ifaceName << "> mImpl;\n";

    if (supportOneway) {
        out << "SynchronizedQueue<std::function<void(void)>> mOnewayQueue;\n";
        out << "std::thread *mOnewayThread = nullptr;\n";

        out << "\n";

        out << "::android::hardware::Return<void> addOnewayTask("
               "std::function<void(void)>);\n\n";

        out << "static const int kOnewayQueueMaxSize = 3000;\n";
    }

    out.unindent();

    out << "};\n\n";

    enterLeaveNamespace(out, false /* enter */);

    out << "\n#endif  // " << guard << "\n";

    return OK;
}


status_t AST::generatePassthroughSource(Formatter &out) const {
    const Interface *iface = mRootScope->getInterface();

    const std::string baseName = iface->getBaseName();
    const std::string klassName = "Bs" + baseName;

    out << klassName
        << "::"
        << klassName
        << "(const sp<"
        << iface->fullName()
        << "> impl) : mImpl(impl) {}\n\n";

    if (iface->hasOnewayMethods()) {
        out << "::android::hardware::Return<void> "
            << klassName
            << "::addOnewayTask(std::function<void(void)> fun) {\n";
        out.indent();
        out << "if (mOnewayThread == nullptr) {\n";
        out.indent();
        out << "mOnewayThread = new std::thread([this]() {\n";
        out.indent();
        out << "while(true) { (this->mOnewayQueue.wait_pop())(); }";
        out.unindent();
        out << "});\n";
        out.unindent();
        out << "}\n\n";

        out << "if (mOnewayQueue.size() > kOnewayQueueMaxSize) {\n";
        out.indent();
        out << "return ::android::hardware::Status::fromExceptionCode(\n";
        out.indent();
        out.indent();
        out << "::android::hardware::Status::EX_TRANSACTION_FAILED);\n";
        out.unindent();
        out.unindent();
        out.unindent();
        out << "} else {\n";
        out.indent();
        out << "mOnewayQueue.push(fun);\n";
        out.unindent();
        out << "}\n";

        out << "return Status();\n";

        out.unindent();
        out << "}\n\n";


    }

    return OK;
}

}  // namespace android

