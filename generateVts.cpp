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

#include "Annotation.h"
#include "Coordinator.h"
#include "Formatter.h"
#include "Interface.h"
#include "Method.h"
#include "Scope.h"

#include <android-base/logging.h>
#include <string>
#include <vector>

namespace android {

// Remove the double quotas in a string.
static std::string removeQuotes(const std::string in) {
    std::string out{in};
    return out.substr(1, out.size() - 2);
}

status_t AST::emitVtsTypeDeclarations(Formatter &out) const {
    std::set<AST *> allImportedASTs;
    return emitVtsTypeDeclarationsHelper(out, &allImportedASTs);
}

status_t AST::emitVtsTypeDeclarationsHelper(
        Formatter &out,
        std::set<AST *> *allImportSet) const {
    // First, generate vts type declaration for all imported AST.
    for (const auto &ast : mImportedASTs) {
        // Already processed, skip.
        if (allImportSet->find(ast) != allImportSet->end()) {
            continue;
        }
        allImportSet->insert(ast);
        std::string ifaceName;
        // We only care about types.hal.
        if (!ast->isInterface(&ifaceName)) {
            status_t status = ast->emitVtsTypeDeclarationsHelper(
                    out, allImportSet);
            if (status != OK) {
                return status;
            }
        }
    }
    // Next, generate vts type declaration for the current AST.
    for (const auto &type : mRootScope->getSubTypes()) {
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

status_t AST::generateVts(const std::string &outputPath) const {
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
        baseName = ifaceName.substr(1);  // cut off the leading 'I'.
    }

    path.append(baseName);
    path.append(".vts");

    CHECK(Coordinator::MakeParentHierarchy(path));
    FILE *file = fopen(path.c_str(), "w");

    if (file == NULL) {
        return -errno;
    }

    Formatter out(file);

    out << "component_class: HAL_HIDL\n";

    // Get the component_type for interface from annotation.
    if (isInterface) {
        const Interface *iface = mRootScope->getInterface();
        Annotation *annotation = iface->annotations().valueFor("hal_type");
        if (annotation != NULL) {
            std::vector<std::string> * values = annotation->params().valueFor(
                    "type");
            if (values != NULL) {
                out << "component_type: "
                    << removeQuotes(values->at(0))
                    << "\n";
            }
        }
    }

    out << "component_type_version: " << mPackage.version().substr(1) << "\n";
    out << "component_name: \""
        << (isInterface ? ifaceName : "types")
        << "\"\n\n";

    out << "package: \"" << mPackage.package() << "\"\n\n";

    for (const auto &item : mImportedNames) {
        out << "import: \"" << item.string() << "\"\n";
    }

    out << "\n";

    if (isInterface) {
        const Interface *iface = mRootScope->getInterface();
        out << "interface: {\n";
        out.indent();

        std::vector<const Interface *> chain;
        while (iface != NULL) {
            chain.push_back(iface);
            iface = iface->superType();
        }

        // Generate all the attribute declarations first.
        for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
            const Interface *superInterface = *it;
            status_t status = superInterface->emitVtsAttributeDeclaration(out);
            if (status != OK) {
                return status;
            }
        }

        // Generate all the method declarations.
        for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
            const Interface *superInterface = *it;
            status_t status = superInterface->emitVtsMethodDeclaration(out);
            if (status != OK) {
                return status;
            }
        }

        out.unindent();
        out << "}\n";
    } else {
        status_t status = emitVtsTypeDeclarations(out);
        if (status != OK) {
            return status;
        }
    }
    return OK;
}

}  // namespace android




