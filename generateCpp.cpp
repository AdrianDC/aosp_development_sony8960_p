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
#include "Formatter.h"
#include "Interface.h"
#include "Method.h"
#include "ScalarType.h"
#include "Scope.h"

#include <algorithm>
#include <android-base/logging.h>
#include <string>
#include <vector>

namespace android {

static std::string upcase(const std::string in) {
    std::string out{in};

    for (auto &ch : out) {
        ch = toupper(ch);
    }

    return out;
}

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
    std::vector<std::string> packageComponents;
    getPackageAndVersionComponents(
            &packageComponents, true /* cpp_compatible */);

    std::string guard = "HIDL_GENERATED";
    for (const auto &component : packageComponents) {
        guard += "_";
        guard += component;
    }

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

    if (isInterface) {
        out << "#include <hwbinder/Status.h>\n";
    }

    out << "#include <utils/NativeHandle.h>\n\n";

    enterLeaveNamespace(out, true /* enter */);
    out << "\n";

    // cut off the leading 'I'.
    const std::string baseName = ifaceName.substr(1);

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

        out << "virtual bool isRemote() const { return false; } \n\n";
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
                << Method::GetSignature(method->results())
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
                out << "virtual ::android::hardware::Status ";
            }

            out << method->name()
                << "("
                << Method::GetSignature(method->args());

            if (returnsValue && elidedReturn == nullptr) {
                if (!method->args().empty()) {
                    out << ", ";
                }

                out << method->name() << "_cb _hidl_cb = nullptr";
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

    // cut off the leading 'I'.
    const std::string baseName = ifaceName.substr(1);

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

        // cut off the leading I
        const std::string itemBaseName = item.name().substr(1);

        out << "Bn"
            << itemBaseName
            << ".h>\n";
    }

    out << "\n";

    out << "#include <hidl/HidlSupport.h>\n";
    out << "#include <hwbinder/IBinder.h>\n";
    out << "#include <hwbinder/IInterface.h>\n";
    out << "#include <hwbinder/Status.h>\n";

    out << "\n";

    enterLeaveNamespace(out, true /* enter */);
    out << "\n";

    out << "struct "
        << klassName
        << " : public "
        << ifaceName;

    const Interface *iface = mRootScope->getInterface();
    const Interface *superType = iface->superType();

    out << ", public ::android::hardware::IInterface";

    out << " {\n";

    out.indent();

    out << "DECLARE_HWBINDER_META_INTERFACE(" << baseName << ");\n\n";

    out << "enum Call {\n";
    out.indent();

    bool first = true;
    for (const auto &method : iface->methods()) {
        out << upcase(method->name());

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

status_t AST::generateHeaderMethodSignatures(
        Formatter &out, bool stub) const {
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
            if (stub) {
                out << "inline ";
            }
            const bool returnsValue = !method->results().empty();

            const TypedVar *elidedReturn = method->canElideCallback();

            if (elidedReturn == nullptr) {
                out << "::android::hardware::Status ";
            } else {
                std::string extra;
                out << "::android::hardware::Return<";
                out << elidedReturn->type().getCppResultType(&extra) << "> ";
            }
            out << method->name()
                << "("
                << Method::GetSignature(method->args());

            if (returnsValue && elidedReturn == nullptr) {
                if (!method->args().empty()) {
                    out << ", ";
                }

                out << method->name() << "_cb _hidl_cb";
            }

            out << ") ";
            if (stub) {
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
            } else {
                out << "override";
            }
            out << ";\n";
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

    // cut off the leading 'I'.
    const std::string baseName = ifaceName.substr(1);
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

    generateHeaderMethodSignatures(out, true); // stub
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

    // cut off the leading 'I'.
    const std::string baseName = ifaceName.substr(1);

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

    out << "virtual bool isRemote() const { return true; } \n\n";

    generateHeaderMethodSignatures(out, false); // proxy

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
        baseName = ifaceName.substr(1); // cut off the leading 'I'.
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
    } else {
        out << "#include <" << prefix << "types.h>\n";
    }

    out << "\n";

    enterLeaveNamespace(out, true /* enter */);
    out << "\n";

    status_t err = generateTypeSource(out, ifaceName);

    if (err == OK && isInterface) {
        err = generateProxySource(out, baseName);
    }

    if (err == OK && isInterface) {
        err = generateStubSource(out, baseName);
    }

    if (err == OK && isInterface) {
        out << "IMPLEMENT_REGISTER_AND_GET_SERVICE(" << baseName << ")\n";
    }

    enterLeaveNamespace(out, false /* enter */);

    return err;
}

status_t AST::generateTypeSource(
        Formatter &out, const std::string &ifaceName) const {
    return mRootScope->emitTypeDefinitions(out, ifaceName);
}

void AST::declareCppReaderLocals(
        Formatter &out, const std::vector<TypedVar *> &args) const {
    if (args.empty()) {
        return;
    }

    for (const auto &arg : args) {
        const Type &type = arg->type();

        std::string extra;
        out << type.getCppResultType(&extra)
            << " "
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
        Type::ErrorMode mode) const {
    const Type &type = arg->type();

    type.emitReaderWriter(
            out,
            arg->name(),
            parcelObj,
            parcelObjIsPointer,
            isReader,
            mode);
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
            const bool returnsValue = !method->results().empty();

            const TypedVar *elidedReturn = method->canElideCallback();
            if (elidedReturn) {
                std::string extra;
                out << "::android::hardware::Return<";
                out << elidedReturn->type().getCppResultType(&extra) << "> ";
            } else {
                out << "::android::hardware::Status ";
            }

            out << klassName
                << "::"
                << method->name()
                << "("
                << Method::GetSignature(method->args());

            if (returnsValue && elidedReturn == nullptr) {
                if (!method->args().empty()) {
                    out << ", ";
                }

                out << method->name() << "_cb _hidl_cb";
            }

            out << ") {\n";

            out.indent();

            out << "::android::hardware::Parcel _hidl_data;\n";
            out << "::android::hardware::Parcel _hidl_reply;\n";
            out << "::android::status_t _hidl_err;\n";
            out << "::android::hardware::Status _hidl_status;\n\n";
            declareCppReaderLocals(out, method->results());

            out << "_hidl_err = _hidl_data.writeInterfaceToken("
                << superInterface->fqName().cppNamespace()
                << "::IHw"
                << superInterface->getBaseName()
                << "::descriptor);\n";

            out << "if (_hidl_err != ::android::OK) { goto _hidl_error; }\n\n";

            for (const auto &arg : method->args()) {
                emitCppReaderWriter(
                        out,
                        "_hidl_data",
                        false /* parcelObjIsPointer */,
                        arg,
                        false /* reader */,
                        Type::ErrorMode_Goto);
            }

            out << "_hidl_err = remote()->transact("
                      << superInterface->fqName().cppNamespace()
                      << "::IHw"
                      << superInterface->getBaseName()
                      << "::Call::"
                      << upcase(method->name())
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

                for (const auto &arg : method->results()) {
                    emitCppReaderWriter(
                            out,
                            "_hidl_reply",
                            false /* parcelObjIsPointer */,
                            arg,
                            true /* reader */,
                            Type::ErrorMode_Goto);
                }

                if (returnsValue && elidedReturn == nullptr) {
                    out << "if (_hidl_cb != nullptr) {\n";
                    out.indent();
                    out << "_hidl_cb(";

                    bool first = true;
                    for (const auto &arg : method->results()) {
                        if (!first) {
                            out << ", ";
                        }

                        if (arg->type().resultNeedsDeref()) {
                            out << "*";
                        }
                        out << arg->name();

                        first = false;
                    }

                    out << ");\n";
                    out.unindent();
                    out << "}\n\n";
                }
            }

            if (elidedReturn != nullptr) {
                std::string extra;

                out << "_hidl_status.setFromStatusT(_hidl_err);\n";
                out << "return ::android::hardware::Return<";
                out << elidedReturn->type().getCppResultType(&extra)
                    << ">(" << elidedReturn->name() << ");\n\n";

                out.unindent();
                out << "_hidl_error:\n";
                out.indent();
                out << "_hidl_status.setFromStatusT(_hidl_err);\n";
                out << "return ::android::hardware::Return<";
                out << method->results().at(0)->type().getCppResultType(&extra)
                    << ">(_hidl_status);\n";
            } else {
                out.unindent();
                out << "_hidl_error:\n";
                out.indent();
                out << "_hidl_status.setFromStatusT(_hidl_err);\n";
                out << "return _hidl_status;\n";
            }

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
                << upcase(method->name())
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

    declareCppReaderLocals(out, method->args());

    for (const auto &arg : method->args()) {
        emitCppReaderWriter(
                out,
                "_hidl_data",
                false /* parcelObjIsPointer */,
                arg,
                true /* reader */,
                Type::ErrorMode_Break);
    }

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

        out << "_hidl_cb(*_hidl_reply);\n";
    } else {
        if (returnsValue) {
            out << "bool _hidl_callbackCalled = false;\n\n";
        }

        out << "::android::hardware::Status _hidl_status(\n";
        out.indent();
        out.indent();
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

            for (const auto &arg : method->results()) {
                emitCppReaderWriter(
                        out,
                        "_hidl_reply",
                        true /* parcelObjIsPointer */,
                        arg,
                        false /* reader */,
                        Type::ErrorMode_Ignore);
            }

            out << "_hidl_cb(*_hidl_reply);\n";

            out.unindent();
            out << "}\n";
        }

        out.unindent();
        out.unindent();
        out << "));\n\n";

        if (returnsValue) {
            out << "if (!_hidl_callbackCalled) {\n";
            out.indent();
        }

        out << "_hidl_err = _hidl_status.writeToParcel(_hidl_reply);\n";

        if (returnsValue) {
            out.unindent();
            out << "}\n\n";
        }
    }

    out << "break;\n";

    return OK;
}

}  // namespace android

