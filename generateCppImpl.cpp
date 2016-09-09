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
#include <android-base/logging.h>
#include <string>
#include <vector>
#include <set>

namespace android {

status_t AST::generateCppImpl(const std::string &outputPath) const {
    status_t err = generateStubImplHeader(outputPath);

    if (err == OK) {
        err = generateStubImplSource(outputPath);
    }

    return err;
}

void AST::generateFetchSymbol(Formatter &out, const std::string& ifaceName) const {
    out << "HIDL_FETCH_" << ifaceName;
}

status_t AST::generateStubImplMethod(Formatter &out,
                                     const std::string &className,
                                     const Method *method,
                                     bool specifyNamespaces) const {

    method->generateCppSignature(out, className, specifyNamespaces);

    out << " {\n";

    out.indent();
    out << "// TODO implement\n";

    const TypedVar *elidedReturn = method->canElideCallback();

    if (elidedReturn == nullptr) {
        out << "return Void();\n";
    } else {
        std::string extra;
        out << "return "
            << elidedReturn->type().getCppResultType(&extra)
            << " {};\n";
    }

    out.unindent();

    out << "}\n\n";

    return OK;
}

status_t AST::generateStubImplDeclaration(Formatter &out,
                                          const std::string &className,
                                          const Method *method,
                                          bool specifyNamespaces) const {

    method->generateCppSignature(out,
                                 className,
                                 specifyNamespaces);
    out << " override;\n";

    return OK;
}

status_t AST::generateStubImplHeader(const std::string &outputPath) const {
    std::string ifaceName;
    if (!AST::isInterface(&ifaceName)) {
        // types.hal does not get a stub header.
        return OK;
    }

    const Interface *iface = mRootScope->getInterface();

    // cut off the leading 'I'.
    const std::string baseName = ifaceName.substr(1);

    std::string path = outputPath;
    path.append(baseName);
    path.append(".h");

    CHECK(Coordinator::MakeParentHierarchy(path));
    FILE *file = fopen(path.c_str(), "w");

    if (file == NULL) {
        return -errno;
    }

    Formatter out(file);

    const std::string guard = makeHeaderGuard(baseName);

    out << "#ifndef " << guard << "\n";
    out << "#define " << guard << "\n\n";

    std::vector<std::string> packageComponents;
    getPackageAndVersionComponents(
            &packageComponents, false /* cpp_compatible */);

    out << "#include <";
    for (const auto &component : packageComponents) {
        out << component << "/";
    }
    out << "I" << baseName << ".h>\n";

    out << "#include <hidl/Status.h>\n\n";

    enterLeaveNamespace(out, true /* enter */);
    out << "namespace implementation {\n\n";

    // this is namespace aware code and doesn't require post-processing
    out.setNamespace("");

    const Interface *ancestor = iface;
    std::vector<const Interface *> chain;
    while (ancestor != NULL) {
        chain.push_back(ancestor);
        ancestor = ancestor->superType();
    }

    std::set<const FQName> usedTypes{};

    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
        const Interface *superInterface = *it;

        superInterface->addNamedTypesToSet(usedTypes);

        for (const auto &method : superInterface->methods()) {
            for(const auto & arg : method->args()) {
                arg->type().addNamedTypesToSet(usedTypes);
            }
            for(const auto & results : method->results()) {
                results->type().addNamedTypesToSet(usedTypes);
            }
        }
    }

    std::set<const FQName> topLevelTypes{};

    for (const auto &name : usedTypes) {
        topLevelTypes.insert(name.getTopLevelType());
    }

    for (const FQName &name : topLevelTypes) {
        out << "using " << name.cppName() << ";\n";
    }

    out << "using ::android::hardware::Return;\n";
    out << "using ::android::hardware::Void;\n";
    out << "using ::android::hardware::hidl_vec;\n";
    out << "using ::android::hardware::hidl_string;\n";
    out << "using ::android::sp;\n";

    out << "\n";

    out << "struct "
        << baseName
        << " : public "
        << ifaceName
        << " {\n";

    out.indent();

    status_t err = generateMethods(out,
                                   "", /* class name */
                                   MethodLocation::IMPL_HEADER,
                                   false /* specify namespaces */);

    if (err != OK) {
        return err;
    }

    out.unindent();

    out << "};\n\n";

    out << "extern \"C\" "
        << ifaceName
        << "* ";
    generateFetchSymbol(out, ifaceName);
    out << "(const char* name);\n\n";

    out << "}  // namespace implementation\n";
    enterLeaveNamespace(out, false /* leave */);

    out << "\n#endif  // " << guard << "\n";

    return OK;
}

status_t AST::generateStubImplSource(const std::string &outputPath) const {
    std::string ifaceName;
    if (!AST::isInterface(&ifaceName)) {
        // types.hal does not get a stub header.
        return OK;
    }

    // cut off the leading 'I'.
    const std::string baseName = ifaceName.substr(1);

    std::string path = outputPath;
    path.append(baseName);
    path.append(".cpp");

    CHECK(Coordinator::MakeParentHierarchy(path));
    FILE *file = fopen(path.c_str(), "w");

    if (file == NULL) {
        return -errno;
    }

    Formatter out(file);

    out << "#include \"" << baseName << ".h\"\n\n";

    enterLeaveNamespace(out, true /* enter */);
    out << "namespace implementation {\n\n";

    // this is namespace aware code and doesn't require post-processing
    out.setNamespace("");

    generateMethods(out,
                    baseName,
                    MethodLocation::IMPL_SOURCE,
                    false /* specify namespaces */);

    const Interface *iface = mRootScope->getInterface();

    out << ifaceName
        << "* ";
    generateFetchSymbol(out, ifaceName);
    out << "(const char* /* name */) {\n";
    out.indent();
    out << "return new " << baseName << "();\n";
    out.unindent();
    out << "}\n\n";

    out << "} // namespace implementation\n";
    enterLeaveNamespace(out, false /* leave */);

    return OK;
}

}  // namespace android
