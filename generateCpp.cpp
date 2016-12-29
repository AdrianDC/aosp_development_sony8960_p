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
#include <hidl-util/Formatter.h>
#include <hidl-util/StringHelper.h>
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
        err = generatePassthroughHeader(outputPath);
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

std::string AST::makeHeaderGuard(const std::string &baseName,
                                 bool indicateGenerated) const {
    std::string guard;

    if (indicateGenerated) {
        guard += "HIDL_GENERATED_";
    }

    guard += StringHelper::Uppercase(mPackage.tokenName());
    guard += "_";
    guard += StringHelper::Uppercase(baseName);
    guard += "_H";

    return guard;
}

// static
void AST::generateCppPackageInclude(
        Formatter &out,
        const FQName &package,
        const std::string &klass) {

    out << "#include <";

    std::vector<std::string> components;
    package.getPackageAndVersionComponents(&components, false /* cpp_compatible */);

    for (const auto &component : components) {
        out << component << "/";
    }

    out << klass
        << ".h>\n";
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

static void declareServiceManagerInteractions(Formatter &out, const std::string &baseName) {
    out << "static ::android::sp<I" << baseName << "> getService("
        << "const std::string &serviceName, bool getStub=false);\n";
    out << "::android::status_t registerAsService(const std::string &serviceName);\n";
    out << "static bool registerForNotifications(\n";
    out.indent(2, [&] {
        out << "const std::string &serviceName,\n"
            << "const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> "
            << "&notification);\n";
    });

}

static void implementServiceManagerInteractions(Formatter &out,
        const std::string &baseName, const std::string &package) {

    out << "// static\n"
        << "::android::sp<I" << baseName << "> I" << baseName << "::getService("
        << "const std::string &serviceName, bool getStub) ";
    out.block([&] {
        out << "::android::sp<I" << baseName << "> iface;\n"
            << "const ::android::sp<::android::hidl::manager::V1_0::IServiceManager> sm\n";
        out.indent(2, [&] {
            out << "= ::android::hardware::defaultServiceManager();\n";
        });
        out.sIf("sm != nullptr && !getStub", [&] {
            out << "::android::sp<::android::hidl::base::V1_0::IBase> base;\n"
                << "::android::hardware::Return<void> ret =\n";
            out.indent(2, [&] {
                out << "sm->get(\"" << package << "::I" << baseName << "\", serviceName.c_str(),\n";
                out.indent(2, [&] {
                    out << "[&base](::android::sp<::android::hidl::base::V1_0::IBase> found) ";
                    out.block([&] {
                        out << "base = found;\n";
                    });
                    out << ");\n";
                });
            });
            out.sIf("ret.isOk()", [&] {
                out << "iface = I" << baseName << "::castFrom(base);\n";
                out.sIf("iface != nullptr", [&] {
                    out << "return iface;\n";
                }).endl();
            }).endl();
        }).endl();
        out << "const int dlMode = RTLD_LAZY;\n";
        out << "void *handle = nullptr;\n";
        for (const auto &path : std::vector<std::string>({
            "HAL_LIBRARY_PATH_ODM", "HAL_LIBRARY_PATH_VENDOR", "HAL_LIBRARY_PATH_SYSTEM"
        })) {
            out.sIf("handle == nullptr", [&] {
                out << "handle = dlopen("
                    << path << " \"" << package << "-impl.so\", dlMode);\n";
            }).endl();
        }
        out.sIf("handle == nullptr", [&] {
            out << "return iface;\n";
        }).endl();
        out << "I" << baseName << "* (*generator)(const char* name);\n"
            << "*(void **)(&generator) = dlsym(handle, \"HIDL_FETCH_I" << baseName << "\");\n";
        out.sIf("generator", [&] {
            out << "iface = (*generator)(serviceName.c_str());\n";
            out.sIf("iface != nullptr", [&] {
                out << "iface = new Bs" << baseName << "(iface);\n";
            }).endl();
        }).endl();
        out << "return iface;\n";
    }).endl().endl();

    out << "::android::status_t I" << baseName << "::registerAsService("
        << "const std::string &serviceName) ";
    out.block([&] {
        out << "const ::android::sp<::android::hidl::manager::V1_0::IServiceManager> sm\n";
        out.indent(2, [&] {
            out << "= ::android::hardware::defaultServiceManager();\n";
        });
        out.sIf("sm == nullptr", [&] {
            out << "return ::android::INVALID_OPERATION;\n";
        }).endl();
        out << "bool success = false;\n"
            << "::android::hardware::Return<void> ret =\n";
        out.indent(2, [&] {
            out << "this->interfaceChain("
                << "[&success, &sm, &serviceName, this](const auto &chain) ";
            out.block([&] {
                out << "::android::hardware::Return<bool> addRet = "
                    << "sm->add(chain, serviceName.c_str(), this);\n";
                out << "success = addRet.isOk() && addRet;\n";
            });
            out << ");\n";
            out << "success = success && ret.isOk();\n";
        });
        out << "return success ? ::android::OK : ::android::UNKNOWN_ERROR;\n";
    }).endl().endl();

    out << "bool I" << baseName << "::registerForNotifications(\n";
    out.indent(2, [&] {
        out << "const std::string &serviceName,\n"
            << "const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> "
            << "&notification) ";
    });
    out.block([&] {
        out << "const ::android::sp<::android::hidl::manager::V1_0::IServiceManager> sm\n";
        out.indent(2, [&] {
            out << "= ::android::hardware::defaultServiceManager();\n";
        });
        out.sIf("sm == nullptr", [&] {
            out << "return false;\n";
        }).endl();
        out << "::android::hardware::Return<bool> success =\n";
        out.indent(2, [&] {
            out << "sm->registerForNotifications(\"" << package << "::I" << baseName << "\",\n";
            out.indent(2, [&] {
                out << "serviceName, notification);\n";
            });
        });
        out << "return success.isOk() && success;\n";
    }).endl().endl();
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
        generateCppPackageInclude(out, item, item.name());
    }

    if (!mImportedNames.empty()) {
        out << "\n";
    }

    if (isInterface) {
        if (isIBase()) {
            out << "// skipped #include IServiceNotification.h\n\n";
        } else {
            out << "#include <android/hidl/manager/1.0/IServiceNotification.h>\n\n";
        }
    }

    out << "#include <hidl/HidlSupport.h>\n";
    out << "#include <hidl/MQDescriptor.h>\n";

    if (isInterface) {
        out << "#include <hidl/Status.h>\n";
    }

    out << "#include <utils/NativeHandle.h>\n";
    out << "#include <utils/misc.h>\n\n"; /* for report_sysprop_change() */

    enterLeaveNamespace(out, true /* enter */);
    out << "\n";

    if (isInterface) {
        out << "struct "
            << ifaceName;

        const Interface *iface = mRootScope->getInterface();
        const Interface *superType = iface->superType();

        if (superType == NULL) {
            out << " : virtual public ::android::RefBase";
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
        out << "constexpr static ::android::hardware::hidl_version version = {"
            << mPackage.getPackageMajorVersion() << ", "
            << mPackage.getPackageMinorVersion() << "};\n";
        out << "virtual const ::android::hardware::hidl_version &"
            << "getInterfaceVersion() const {\n";
        out.indent();
        out << "return version;\n";
        out.unindent();
        out << "}\n\n";
        out << "virtual bool isRemote() const ";
        if (!isIBase()) {
            out << "override ";
        }
        out << "{ return false; }\n\n";

        for (const auto &method : iface->methods()) {
            out << "\n";

            const bool returnsValue = !method->results().empty();
            const TypedVar *elidedReturn = method->canElideCallback();

            if (elidedReturn == nullptr && returnsValue) {
                out << "using "
                    << method->name()
                    << "_cb = std::function<void("
                    << Method::GetArgSignature(method->results(),
                                               true /* specify namespaces */)
                    << ")>;\n";
            }

            method->dumpAnnotations(out);

            if (elidedReturn) {
                out << "virtual ::android::hardware::Return<";
                out << elidedReturn->type().getCppResultType() << "> ";
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

            out << ")";
            if (method->isHidlReserved()) {
                if (!isIBase()) {
                    out << " override";
                }
                out << " {\n";
                out.indent();
                method->cppImpl(IMPL_HEADER, out);
                out.unindent();
                out << "\n}\n";
            } else {
                out << " = 0;\n";
            }
        }

        out << "// cast static functions\n";
        std::string childTypeResult = iface->getCppResultType();

        for (const Interface *superType : iface->typeChain()) {
            out << "static "
                << childTypeResult
                << " castFrom("
                << superType->getCppArgumentType()
                << " parent"
                << ");\n";
        }

        out << "\nstatic const char* descriptor;\n\n";

        if (isIBase()) {
            out << "// skipped getService, registerAsService, registerForNotifications\n\n";
        } else {
            declareServiceManagerInteractions(out, baseName);
        }

        out << "private: static int hidlStaticBlock;\n";
    }

    if (isInterface) {
        out.unindent();

        out << "};\n\n";
    }

    err = mRootScope->emitGlobalTypeDeclarations(out);

    if (err != OK) {
        return err;
    }

    out << "\n";
    enterLeaveNamespace(out, false /* enter */);

    out << "\n#endif  // " << guard << "\n";

    return OK;
}

status_t AST::generateHwBinderHeader(const std::string &outputPath) const {
    std::string ifaceName;
    bool isInterface = AST::isInterface(&ifaceName);
    const Interface *iface = nullptr;
    std::string baseName{};
    std::string klassName{};

    if(isInterface) {
        iface = mRootScope->getInterface();
        baseName = iface->getBaseName();
        klassName = "IHw" + baseName;
    } else {
        klassName = "hwtypes";
    }

    std::string path = outputPath;
    path.append(mCoordinator->convertPackageRootToPath(mPackage));
    path.append(mCoordinator->getPackagePath(mPackage, true /* relative */));
    path.append(klassName + ".h");

    FILE *file = fopen(path.c_str(), "w");

    if (file == NULL) {
        return -errno;
    }

    Formatter out(file);

    const std::string guard = makeHeaderGuard(klassName);

    out << "#ifndef " << guard << "\n";
    out << "#define " << guard << "\n\n";

    if (isInterface) {
        generateCppPackageInclude(out, mPackage, ifaceName);
    } else {
        generateCppPackageInclude(out, mPackage, "types");
    }

    out << "\n";

    for (const auto &item : mImportedNames) {
        if (item.name() == "types") {
            generateCppPackageInclude(out, item, "hwtypes");
        } else {
            generateCppPackageInclude(out, item, "Bn" + item.getInterfaceBaseName());
            generateCppPackageInclude(out, item, "Bp" + item.getInterfaceBaseName());
        }
    }

    out << "\n";

    out << "#include <hidl/Status.h>\n";
    out << "#include <hwbinder/IBinder.h>\n";
    out << "#include <hwbinder/Parcel.h>\n";

    out << "\n";

    enterLeaveNamespace(out, true /* enter */);

    status_t err = mRootScope->emitGlobalHwDeclarations(out);
    if (err != OK) {
        return err;
    }

    enterLeaveNamespace(out, false /* enter */);

    out << "\n#endif  // " << guard << "\n";

    return OK;
}

status_t AST::emitTypeDeclarations(Formatter &out) const {
    return mRootScope->emitTypeDeclarations(out);
}

static void wrapPassthroughArg(Formatter &out,
        const TypedVar *arg, bool addPrefixToName,
        std::function<void(void)> handleError) {
    if (!arg->type().isInterface()) {
        return;
    }
    std::string name = (addPrefixToName ? "_hidl_out_" : "") + arg->name();
    std::string wrappedName = (addPrefixToName ? "_hidl_out_wrapped_" : "_hidl_wrapped_")
            + arg->name();
    const Interface &iface = static_cast<const Interface &>(arg->type());
    out << iface.getCppStackType() << " " << wrappedName << ";\n";
    // TODO(elsk): b/33754152 Should not wrap this if object is Bs*
    out.sIf(name + " != nullptr && !" + name + "->isRemote()", [&] {
        out << wrappedName
            << " = "
            << iface.fqName().cppName()
            << "::castFrom(::android::hardware::wrapPassthrough("
            << name << "));\n";
        out.sIf(wrappedName + " == nullptr", [&] {
            // Fatal error. Happens when the BsFoo class is not found in the binary
            // or any dynamic libraries.
            handleError();
        }).endl();
    }).sElse([&] {
        out << wrappedName << " = " << name << ";\n";
    }).endl().endl();
}

status_t AST::generatePassthroughMethod(Formatter &out,
                                        const Method *method) const {
    method->generateCppSignature(out);

    out << " {\n";
    out.indent();

    const bool returnsValue = !method->results().empty();
    const TypedVar *elidedReturn = method->canElideCallback();

    if (returnsValue && elidedReturn == nullptr) {
        generateCheckNonNull(out, "_hidl_cb");
    }

    generateCppInstrumentationCall(
            out,
            InstrumentationEvent::PASSTHROUGH_ENTRY,
            method);


    for (const auto &arg : method->args()) {
        wrapPassthroughArg(out, arg, false /* addPrefixToName */, [&] {
            out << "return ::android::hardware::Status::fromExceptionCode(\n";
            out.indent(2, [&] {
                out << "::android::hardware::Status::EX_TRANSACTION_FAILED,\n"
                    << "\"Cannot wrap passthrough interface.\");\n";
            });
        });
    }

    out << "auto _hidl_error = ::android::hardware::Void();\n";
    out << "auto _hidl_return = ";

    if (method->isOneway()) {
        out << "addOnewayTask([this, &_hidl_error";
        for (const auto &arg : method->args()) {
            out << ", "
                << (arg->type().isInterface() ? "_hidl_wrapped_" : "")
                << arg->name();
        }
        out << "] {\n";
        out.indent();
        out << "this->";
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
        out << (arg->type().isInterface() ? "_hidl_wrapped_" : "") << arg->name();
    }
    if (returnsValue && elidedReturn == nullptr) {
        if (!method->args().empty()) {
            out << ", ";
        }
        out << "[&](";
        first = true;
        for (const auto &arg : method->results()) {
            if (!first) {
                out << ", ";
            }

            out << "const auto &_hidl_out_"
                << arg->name();

            first = false;
        }

        out << ") {\n";
        out.indent();
        status_t status = generateCppInstrumentationCall(
                out,
                InstrumentationEvent::PASSTHROUGH_EXIT,
                method);
        if (status != OK) {
            return status;
        }

        for (const auto &arg : method->results()) {
            wrapPassthroughArg(out, arg, true /* addPrefixToName */, [&] {
                out << "_hidl_error = ::android::hardware::Status::fromExceptionCode(\n";
                out.indent(2, [&] {
                    out << "::android::hardware::Status::EX_TRANSACTION_FAILED,\n"
                        << "\"Cannot wrap passthrough interface.\");\n";
                });
                out << "return;\n";
            });
        }

        out << "_hidl_cb(";
        first = true;
        for (const auto &arg : method->results()) {
            if (!first) {
                out << ", ";
            }
            first = false;
            out << (arg->type().isInterface() ? "_hidl_out_wrapped_" : "_hidl_out_")
                << arg->name();
        }
        out << ");\n";
        out.unindent();
        out << "});\n\n";
    } else {
        out << ");\n\n";
        if (elidedReturn != nullptr) {
            out << elidedReturn->type().getCppResultType()
                << " _hidl_out_"
                << elidedReturn->name()
                << " = _hidl_return;\n";
        }
        status_t status = generateCppInstrumentationCall(
                out,
                InstrumentationEvent::PASSTHROUGH_EXIT,
                method);
        if (status != OK) {
            return status;
        }
    }

    if (method->isOneway()) {
        out.unindent();
        out << "});\n";
    }

    out << "return _hidl_return;\n";

    out.unindent();
    out << "}\n";

    return OK;
}

status_t AST::generateMethods(Formatter &out, MethodGenerator gen) const {

    const Interface *iface = mRootScope->getInterface();

    const Interface *prevIterface = nullptr;
    for (const auto &tuple : iface->allMethodsFromRoot()) {
        const Method *method = tuple.method();
        const Interface *superInterface = tuple.interface();

        if(prevIterface != superInterface) {
            if (prevIterface != nullptr) {
                out << "\n";
            }
            out << "// Methods from "
                << superInterface->fullName()
                << " follow.\n";
            prevIterface = superInterface;
        }
        status_t err = gen(method, superInterface);

        if (err != OK) {
            return err;
        }
    }

    out << "\n";

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

    generateCppPackageInclude(out, mPackage, "IHw" + baseName);
    out << "\n";

    enterLeaveNamespace(out, true /* enter */);
    out << "\n";

    out << "struct "
        << "Bn"
        << baseName;
    if (iface->isIBase()) {
        out << " : public ::android::hardware::BBinder";
        out << ", public ::android::hardware::HidlInstrumentor {\n";
    } else {
        out << " : public ::android::hidl::base::V1_0::BnBase {\n";
    }

    out.indent();
    out << "explicit Bn"
        << baseName
        << "(const ::android::sp<" << ifaceName << "> &_hidl_impl);"
        << "\n";
    out << "explicit Bn"
        << baseName
        << "(const ::android::sp<" << ifaceName << "> &_hidl_impl,"
        << " const std::string& prefix);"
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

    out << "::android::sp<" << ifaceName << "> getImpl() { return _hidl_mImpl; };\n";
    out.unindent();
    out << "private:\n";
    out.indent();
    out << "::android::sp<" << ifaceName << "> _hidl_mImpl;\n";
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

    out << "#include <hidl/HidlTransportSupport.h>\n\n";

    std::vector<std::string> packageComponents;
    getPackageAndVersionComponents(
            &packageComponents, false /* cpp_compatible */);

    generateCppPackageInclude(out, mPackage, "IHw" + baseName);
    out << "\n";

    enterLeaveNamespace(out, true /* enter */);
    out << "\n";

    out << "struct "
        << "Bp"
        << baseName
        << " : public ::android::hardware::BpInterface<I"
        << baseName
        << ">, public ::android::hardware::HidlInstrumentor {\n";

    out.indent();

    out << "explicit Bp"
        << baseName
        << "(const ::android::sp<::android::hardware::IBinder> &_hidl_impl);"
        << "\n\n";

    out << "virtual bool isRemote() const override { return true; }\n\n";

    status_t err = generateMethods(out, [&](const Method *method, const Interface *) {
        method->generateCppSignature(out);
        out << " override;\n";
        return OK;
    });

    if (err != OK) {
        return err;
    }

    out.unindent();
    out << "private:\n";
    out.indent();
    out << "std::mutex _hidl_mMutex;\n"
        << "std::vector<::android::sp<::android::hardware::hidl_binder_death_recipient>>"
        << " _hidl_mDeathRecipients;\n";
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

    const Interface *iface = nullptr;
    bool isInterface;
    if (!AST::isInterface(&ifaceName)) {
        baseName = "types";
        isInterface = false;
    } else {
        iface = mRootScope->getInterface();
        baseName = iface->getBaseName();
        isInterface = true;
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

    out << "#include <android/log.h>\n";
    out << "#include <cutils/trace.h>\n";
    out << "#include <hidl/HidlTransportSupport.h>\n\n";
    if (isInterface) {
        // This is a no-op for IServiceManager itself.
        out << "#include <android/hidl/manager/1.0/IServiceManager.h>\n";

        generateCppPackageInclude(out, mPackage, "Bp" + baseName);
        generateCppPackageInclude(out, mPackage, "Bn" + baseName);
        generateCppPackageInclude(out, mPackage, "Bs" + baseName);

        for (const Interface *superType : iface->superTypeChain()) {
            generateCppPackageInclude(out,
                                      superType->fqName(),
                                      "Bp" + superType->getBaseName());
        }

        out << "#include <hidl/ServiceManagement.h>\n";
    } else {
        generateCppPackageInclude(out, mPackage, "types");
        generateCppPackageInclude(out, mPackage, "hwtypes");
    }

    out << "\n";

    enterLeaveNamespace(out, true /* enter */);
    out << "\n";

    status_t err = generateTypeSource(out, ifaceName);

    if (err == OK && isInterface) {
        const Interface *iface = mRootScope->getInterface();
        out << "constexpr ::android::hardware::hidl_version "
            << ifaceName
            << "::version;\n\n";

        // need to be put here, generateStubSource is using this.
        out << "const char* I"
            << iface->getBaseName()
            << "::descriptor(\""
            << iface->fqName().string()
            << "\");\n\n";

        out << "int I"
            << iface->getBaseName()
            << "::hidlStaticBlock = []() -> int {\n";
        out.indent([&] {
            out << "::android::hardware::gBnConstructorMap[I"
                << iface->getBaseName()
                << "::descriptor]\n";
            out.indent(2, [&] {
                out << "= [](void *iIntf) -> ::android::sp<::android::hardware::IBinder> {\n";
                out.indent([&] {
                    out << "return new Bn"
                        << iface->getBaseName()
                        << "(reinterpret_cast<I"
                        << iface->getBaseName()
                        << " *>(iIntf));\n";
                });
                out << "};\n";
            });
            out << "::android::hardware::gBsConstructorMap[I"
                << iface->getBaseName()
                << "::descriptor]\n";
            out.indent(2, [&] {
                out << "= [](void *iIntf) -> ::android::sp<"
                    << gIBaseFqName.cppName()
                    << "> {\n";
                out.indent([&] {
                    out << "return new Bs"
                        << iface->getBaseName()
                        << "(reinterpret_cast<I"
                        << iface->getBaseName()
                        << " *>(iIntf));\n";
                });
                out << "};\n";
            });
            out << "return 1;\n";
        });
        out << "}();\n\n";

        err = generateInterfaceSource(out);
    }

    if (err == OK && isInterface) {
        err = generateProxySource(out, baseName);
    }

    if (err == OK && isInterface) {
        err = generateStubSource(out, iface, baseName);
    }

    if (err == OK && isInterface) {
        err = generatePassthroughSource(out);
    }

    if (err == OK && isInterface) {
        const Interface *iface = mRootScope->getInterface();

        if (isIBase()) {
            out << "// skipped getService, registerAsService, registerForNotifications\n";
        } else {
            std::string package = iface->fqName().package()
                    + iface->fqName().atVersion();

            implementServiceManagerInteractions(out, baseName, package);
        }
    }

    enterLeaveNamespace(out, false /* enter */);

    return err;
}

// static
void AST::generateCheckNonNull(Formatter &out, const std::string &nonNull) {
    out.sIf(nonNull + " == nullptr", [&] {
        out << "return ::android::hardware::Status::fromExceptionCode(\n";
        out.indent(2, [&] {
            out << "::android::hardware::Status::EX_ILLEGAL_ARGUMENT);\n";
        });
    }).endl().endl();
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

        out << type.getCppResultType()
            << " "
            << (forResults ? "_hidl_out_" : "") + arg->name()
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

status_t AST::generateProxyMethodSource(Formatter &out,
                                        const std::string &klassName,
                                        const Method *method,
                                        const Interface *superInterface) const {

    method->generateCppSignature(out,
                                 klassName,
                                 true /* specify namespaces */);

    const bool returnsValue = !method->results().empty();
    const TypedVar *elidedReturn = method->canElideCallback();

    out << " {\n";

    out.indent();

    if (method->isHidlReserved() && method->overridesCppImpl(IMPL_PROXY)) {
        method->cppImpl(IMPL_PROXY, out);
        out.unindent();
        out << "}\n\n";
        return OK;
    }

    if (returnsValue && elidedReturn == nullptr) {
        generateCheckNonNull(out, "_hidl_cb");
    }

    status_t status = generateCppInstrumentationCall(
            out,
            InstrumentationEvent::CLIENT_API_ENTRY,
            method);
    if (status != OK) {
        return status;
    }

    out << "::android::hardware::Parcel _hidl_data;\n";
    out << "::android::hardware::Parcel _hidl_reply;\n";
    out << "::android::status_t _hidl_err;\n";
    out << "::android::hardware::Status _hidl_status;\n\n";

    declareCppReaderLocals(
            out, method->results(), true /* forResults */);

    out << "_hidl_err = _hidl_data.writeInterfaceToken(";
    out << superInterface->fqName().cppNamespace()
        << "::I"
        << superInterface->getBaseName();
    out << "::descriptor);\n";
    out << "if (_hidl_err != ::android::OK) { goto _hidl_error; }\n\n";

    bool hasInterfaceArgument;
    // First DFS: write all buffers and resolve pointers for parent
    for (const auto &arg : method->args()) {
        if (arg->type().isInterface()) {
            hasInterfaceArgument = true;
        }
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

    if (hasInterfaceArgument) {
        // Start binder threadpool to handle incoming transactions
        out << "::android::hardware::ProcessState::self()->startThreadPool();\n";
    }
    out << "_hidl_err = remote()->transact("
        << method->getSerialId()
        << " /* "
        << method->name()
        << " */, _hidl_data, &_hidl_reply";

    if (method->isOneway()) {
        out << ", ::android::hardware::IBinder::FLAG_ONEWAY";
    }
    out << ");\n";

    out << "if (_hidl_err != ::android::OK) { goto _hidl_error; }\n\n";

    if (!method->isOneway()) {
        out << "_hidl_err = ::android::hardware::readFromParcel(&_hidl_status, _hidl_reply);\n";
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
    status = generateCppInstrumentationCall(
            out,
            InstrumentationEvent::CLIENT_API_EXIT,
            method);
    if (status != OK) {
        return status;
    }

    if (elidedReturn != nullptr) {
        out << "_hidl_status.setFromStatusT(_hidl_err);\n";
        out << "return ::android::hardware::Return<";
        out << elidedReturn->type().getCppResultType()
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
        out << method->results().at(0)->type().getCppResultType();
    } else {
        out << "void";
    }
    out << ">(_hidl_status);\n";

    out.unindent();
    out << "}\n\n";
    return OK;
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
        << "<I"
        << baseName
        << ">(_hidl_impl),\n"
        << "  ::android::hardware::HidlInstrumentor(\""
        << mPackage.string()
        << "::I"
        << baseName
        << "\") {\n";

    out.unindent();
    out.unindent();
    out << "}\n\n";

    status_t err = generateMethods(out, [&](const Method *method, const Interface *superInterface) {
        return generateProxyMethodSource(out, klassName, method, superInterface);
    });

    return err;
}

status_t AST::generateStubSource(
        Formatter &out,
        const Interface *iface,
        const std::string &baseName) const {
    const std::string klassName = "Bn" + baseName;

    out << klassName
        << "::"
        << klassName
        << "(const ::android::sp<I" << baseName <<"> &_hidl_impl)\n";

    out.indent();
    out.indent();

    if (iface->isIBase()) {
        out << ": ::android::hardware::HidlInstrumentor(\"";
    } else {
        out << ": ::android::hidl::base::V1_0::BnBase(_hidl_impl, \"";
    }

    out << mPackage.string()
        << "::I"
        << baseName
        << "\") { \n";
    out.indent();
    out << "_hidl_mImpl = _hidl_impl;\n";
    out.unindent();

    out.unindent();
    out.unindent();
    out << "}\n\n";

    if (iface->isIBase()) {
        // BnBase has a constructor to initialize the HidlInstrumentor
        // class properly.
        out << klassName
            << "::"
            << klassName
            << "(const ::android::sp<I" << baseName <<"> &_hidl_impl,"
            << " const std::string &prefix)\n";

        out.indent();
        out.indent();

        out << ": ::android::hardware::HidlInstrumentor(prefix) { \n";
        out.indent();
        out << "_hidl_mImpl = _hidl_impl;\n";
        out.unindent();

        out.unindent();
        out.unindent();
        out << "}\n\n";
    }


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

    for (const auto &tuple : iface->allMethodsFromRoot()) {
        const Method *method = tuple.method();
        const Interface *superInterface = tuple.interface();
        out << "case "
            << method->getSerialId()
            << " /* "
            << method->name()
            << " */:\n{\n";

        out.indent();

        status_t err =
            generateStubSourceForMethod(out, superInterface, method);

        if (err != OK) {
            return err;
        }

        out.unindent();
        out << "}\n\n";
    }

    out << "default:\n{\n";
    out.indent();

    out << "return onTransact(\n";

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

    out.sIf("_hidl_err == ::android::UNEXPECTED_NULL", [&] {
        out << "_hidl_err = ::android::hardware::writeToParcel(\n";
        out.indent(2, [&] {
            out << "::android::hardware::Status::fromExceptionCode(::android::hardware::Status::EX_NULL_POINTER),\n";
            out << "_hidl_reply);\n";
        });
    });

    out << "return _hidl_err;\n";

    out.unindent();
    out << "}\n\n";

    return OK;
}

status_t AST::generateStubSourceForMethod(
        Formatter &out, const Interface *iface, const Method *method) const {
    if (method->isHidlReserved() && method->overridesCppImpl(IMPL_STUB)) {
        method->cppImpl(IMPL_STUB, out);
        out << "break;\n";
        return OK;
    }

    out << "if (!_hidl_data.enforceInterface(";

    out << iface->fqName().cppNamespace()
        << "::I"
        << iface->getBaseName();

    out << "::descriptor)) {\n";

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

    status_t status = generateCppInstrumentationCall(
            out,
            InstrumentationEvent::SERVER_API_ENTRY,
            method);
    if (status != OK) {
        return status;
    }

    const bool returnsValue = !method->results().empty();
    const TypedVar *elidedReturn = method->canElideCallback();

    if (elidedReturn != nullptr) {
        out << elidedReturn->type().getCppResultType()
            << " _hidl_out_"
            << elidedReturn->name()
            << " = "
            << "_hidl_mImpl->" << method->name()
            << "(";

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
        out << "::android::hardware::writeToParcel(::android::hardware::Status::ok(), "
            << "_hidl_reply);\n\n";

        elidedReturn->type().emitReaderWriter(
                out,
                "_hidl_out_" + elidedReturn->name(),
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
                true /* addPrefixToName */);

        status_t status = generateCppInstrumentationCall(
                out,
                InstrumentationEvent::SERVER_API_EXIT,
                method);
        if (status != OK) {
            return status;
        }

        out << "_hidl_cb(*_hidl_reply);\n";
    } else {
        if (returnsValue) {
            out << "bool _hidl_callbackCalled = false;\n\n";
        }

        out << "_hidl_mImpl->" << method->name() << "(";

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

                out << "const auto &_hidl_out_" << arg->name();

                first = false;
            }

            out << ") {\n";
            out.indent();
            out << "if (_hidl_callbackCalled) {\n";
            out.indent();
            out << "LOG_ALWAYS_FATAL(\""
                << method->name()
                << ": _hidl_cb called a second time, but must be called once.\");\n";
            out.unindent();
            out << "}\n";
            out << "_hidl_callbackCalled = true;\n\n";

            out << "::android::hardware::writeToParcel(::android::hardware::Status::ok(), "
                << "_hidl_reply);\n\n";

            // First DFS: buffers
            for (const auto &arg : method->results()) {
                emitCppReaderWriter(
                        out,
                        "_hidl_reply",
                        true /* parcelObjIsPointer */,
                        arg,
                        false /* reader */,
                        Type::ErrorMode_Ignore,
                        true /* addPrefixToName */);
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
                        true /* addPrefixToName */);
            }

            status_t status = generateCppInstrumentationCall(
                    out,
                    InstrumentationEvent::SERVER_API_EXIT,
                    method);
            if (status != OK) {
                return status;
            }

            out << "_hidl_cb(*_hidl_reply);\n";

            out.unindent();
            out << "}\n";
        }
        out << ");\n\n";

        if (returnsValue) {
            out << "if (!_hidl_callbackCalled) {\n";
            out.indent();
            out << "LOG_ALWAYS_FATAL(\""
                << method->name()
                << ": _hidl_cb not called, but must be called once.\");\n";
            out.unindent();
            out << "}\n\n";
        } else {
            out << "::android::hardware::writeToParcel("
                << "::android::hardware::Status::ok(), "
                << "_hidl_reply);\n\n";
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

    out << "#include <cutils/trace.h>\n";
    out << "#include <future>\n";

    generateCppPackageInclude(out, mPackage, ifaceName);
    out << "\n";

    out << "#include <hidl/HidlPassthroughSupport.h>\n";
    if (supportOneway) {
        out << "#include <hidl/TaskRunner.h>\n";
    }

    enterLeaveNamespace(out, true /* enter */);
    out << "\n";

    out << "struct "
        << klassName
        << " : " << ifaceName
        << ", ::android::hardware::HidlInstrumentor {\n";

    out.indent();
    out << "explicit "
        << klassName
        << "(const ::android::sp<"
        << ifaceName
        << "> impl);\n";

    status_t err = generateMethods(out, [&](const Method *method, const Interface *) {
        return generatePassthroughMethod(out, method);
    });

    if (err != OK) {
        return err;
    }

    out.unindent();
    out << "private:\n";
    out.indent();
    out << "const ::android::sp<" << ifaceName << "> mImpl;\n";

    if (supportOneway) {
        out << "::android::hardware::TaskRunner mOnewayQueue;\n";

        out << "\n";

        out << "::android::hardware::Return<void> addOnewayTask("
               "std::function<void(void)>);\n\n";
    }

    out.unindent();

    out << "};\n\n";

    enterLeaveNamespace(out, false /* enter */);

    out << "\n#endif  // " << guard << "\n";

    return OK;
}

status_t AST::generateInterfaceSource(Formatter &out) const {
    const Interface *iface = mRootScope->getInterface();

    // generate castFrom functions
    std::string childTypeResult = iface->getCppResultType();

    for (const Interface *superType : iface->typeChain()) {
        out << "// static \n"
            << childTypeResult
            << " I"
            << iface->getBaseName()
            << "::castFrom("
            << superType->getCppArgumentType()
            << " parent) {\n";
        out.indent();
        if (iface == superType) {
            out << "return parent;\n";
        } else {
            out << "return ::android::hardware::castInterface<";
            out << "I" << iface->getBaseName() << ", "
                << superType->fqName().cppName() << ", "
                << iface->getProxyName().cppLocalName() << ", "
                << superType->getProxyName().cppName()
                << ">(\n";
            out.indent();
            out.indent();
            out << "parent, \""
                << iface->fqName().string()
                << "\");\n";
            out.unindent();
            out.unindent();
        }
        out.unindent();
        out << "}\n\n";
    }

    return OK;
}

status_t AST::generatePassthroughSource(Formatter &out) const {
    const Interface *iface = mRootScope->getInterface();

    const std::string baseName = iface->getBaseName();
    const std::string klassName = "Bs" + baseName;

    out << klassName
        << "::"
        << klassName
        << "(const ::android::sp<"
        << iface->fullName()
        << "> impl) : ::android::hardware::HidlInstrumentor(\""
        << iface->fqName().string()
        << "\"), mImpl(impl) {";
    if (iface->hasOnewayMethods()) {
        out << "\n";
        out.indent([&] {
            out << "mOnewayQueue.setLimit(3000 /* similar limit to binderized */);\n";
        });
    }
    out << "}\n\n";

    if (iface->hasOnewayMethods()) {
        out << "::android::hardware::Return<void> "
            << klassName
            << "::addOnewayTask(std::function<void(void)> fun) {\n";
        out.indent();
        out << "if (!mOnewayQueue.push(fun)) {\n";
        out.indent();
        out << "return ::android::hardware::Status::fromExceptionCode(\n";
        out.indent();
        out.indent();
        out << "::android::hardware::Status::EX_TRANSACTION_FAILED);\n";
        out.unindent();
        out.unindent();
        out.unindent();
        out << "}\n";

        out << "return ::android::hardware::Status();\n";

        out.unindent();
        out << "}\n\n";


    }

    return OK;
}

status_t AST::generateCppAtraceCall(Formatter &out,
                                    InstrumentationEvent event,
                                    const Method *method) const {
    const Interface *iface = mRootScope->getInterface();
    std::string baseString = "HIDL::I" + iface->getBaseName() + "::" + method->name();
    switch (event) {
        case SERVER_API_ENTRY:
        {
            out << "atrace_begin(ATRACE_TAG_HAL, \""
                << baseString + "::server\");\n";
            break;
        }
        case CLIENT_API_ENTRY:
        {
            out << "atrace_begin(ATRACE_TAG_HAL, \""
                << baseString + "::client\");\n";
            break;
        }
        case PASSTHROUGH_ENTRY:
        {
            out << "atrace_begin(ATRACE_TAG_HAL, \""
                << baseString + "::passthrough\");\n";
            break;
        }
        case SERVER_API_EXIT:
        case CLIENT_API_EXIT:
        case PASSTHROUGH_EXIT:
        {
            out << "atrace_end(ATRACE_TAG_HAL);\n";
            break;
        }
        default:
        {
            LOG(ERROR) << "Unsupported instrumentation event: " << event;
            return UNKNOWN_ERROR;
        }
    }

    return OK;
}

status_t AST::generateCppInstrumentationCall(
        Formatter &out,
        InstrumentationEvent event,
        const Method *method) const {
    status_t err = generateCppAtraceCall(out, event, method);
    if (err != OK) {
        return err;
    }

    out << "if (UNLIKELY(mEnableInstrumentation)) {\n";
    out.indent();
    out << "std::vector<void *> _hidl_args;\n";
    std::string event_str = "";
    switch (event) {
        case SERVER_API_ENTRY:
        {
            event_str = "InstrumentationEvent::SERVER_API_ENTRY";
            for (const auto &arg : method->args()) {
                out << "_hidl_args.push_back((void *)"
                    << (arg->type().resultNeedsDeref() ? "" : "&")
                    << arg->name()
                    << ");\n";
            }
            break;
        }
        case SERVER_API_EXIT:
        {
            event_str = "InstrumentationEvent::SERVER_API_EXIT";
            for (const auto &arg : method->results()) {
                out << "_hidl_args.push_back((void *)&_hidl_out_"
                    << arg->name()
                    << ");\n";
            }
            break;
        }
        case CLIENT_API_ENTRY:
        {
            event_str = "InstrumentationEvent::CLIENT_API_ENTRY";
            for (const auto &arg : method->args()) {
                out << "_hidl_args.push_back((void *)&"
                    << arg->name()
                    << ");\n";
            }
            break;
        }
        case CLIENT_API_EXIT:
        {
            event_str = "InstrumentationEvent::CLIENT_API_EXIT";
            for (const auto &arg : method->results()) {
                out << "_hidl_args.push_back((void *)"
                    << (arg->type().resultNeedsDeref() ? "" : "&")
                    << "_hidl_out_"
                    << arg->name()
                    << ");\n";
            }
            break;
        }
        case PASSTHROUGH_ENTRY:
        {
            event_str = "InstrumentationEvent::PASSTHROUGH_ENTRY";
            for (const auto &arg : method->args()) {
                out << "_hidl_args.push_back((void *)&"
                    << arg->name()
                    << ");\n";
            }
            break;
        }
        case PASSTHROUGH_EXIT:
        {
            event_str = "InstrumentationEvent::PASSTHROUGH_EXIT";
            for (const auto &arg : method->results()) {
                out << "_hidl_args.push_back((void *)&_hidl_out_"
                    << arg->name()
                    << ");\n";
            }
            break;
        }
        default:
        {
            LOG(ERROR) << "Unsupported instrumentation event: " << event;
            return UNKNOWN_ERROR;
        }
    }

    const Interface *iface = mRootScope->getInterface();

    out << "for (const auto &callback: mInstrumentationCallbacks) {\n";
    out.indent();
    out << "callback("
        << event_str
        << ", \""
        << mPackage.package()
        << "\", \""
        << mPackage.version()
        << "\", \""
        << iface->localName()
        << "\", \""
        << method->name()
        << "\", &_hidl_args);\n";
    out.unindent();
    out << "}\n";
    out.unindent();
    out << "}\n\n";

    return OK;
}

}  // namespace android

