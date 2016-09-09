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

#include "CompositeDeclaration.h"
#include "FunctionDeclaration.h"
#include "Declaration.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <hidl-util/StringHelper.h>

namespace android {

CompositeDeclaration::CompositeDeclaration(
        const Type::Qualifier::Qualification qualifier,
        const std::string &name,
        std::vector<android::Declaration *> *fieldDeclarations)
    : Declaration(name), mQualifier(qualifier), mFieldDeclarations(fieldDeclarations)
    {}

CompositeDeclaration::~CompositeDeclaration() {
    if(mFieldDeclarations != NULL) {
        for(auto* decl : *mFieldDeclarations) {
            delete decl;
        }
    }
    delete mFieldDeclarations;
}


const Type::Qualifier::Qualification &CompositeDeclaration::getQualifier() const {
    return mQualifier;
}
const std::vector<android::Declaration *>*
    CompositeDeclaration::getFieldDeclarations() const {
    return mFieldDeclarations;
}

void CompositeDeclaration::generateInterface(Formatter &out) const {
    generateCommentText(out);
    out << "interface " << getInterfaceName() << " {\n\n";

    generateBody(out);

    out << "};\n";
}

void CompositeDeclaration::generateSource(Formatter &out) const {
    CHECK(mQualifier == Type::Qualifier::STRUCT ||
          mQualifier == Type::Qualifier::UNION ||
          mQualifier == Type::Qualifier::ENUM);

    out << Type::qualifierText(mQualifier) << " " << getName();

    if (mQualifier == Type::Qualifier::ENUM) {
        out << " : ";

        if (mEnumTypeName.empty()) {
            out << "int32_t /* NOTE: type is guessed */";
        } else {
            out << mEnumTypeName;
        }

    }

    out << " {\n";

    generateBody(out);

    out << "};\n";
}

void CompositeDeclaration::generateBody(Formatter &out) const {
    out.indent();

    for (auto *declaration : *mFieldDeclarations) {
        declaration->generateCommentText(out);
        declaration->generateSource(out);
        out << "\n";
    }

    out.unindent();
}

void CompositeDeclaration::processContents(AST &ast) {
    for (auto &declaration : *mFieldDeclarations) {
        declaration->processContents(ast);
    }

    if (isInterface()) {
        // move non function fields into a containing struct

        auto nonFpDecs = new std::vector<Declaration*>;

        auto it = mFieldDeclarations->begin();
        while (it != mFieldDeclarations->end()) {
            if((*it)->decType() != FunctionDeclaration::type()) {
                nonFpDecs->push_back(*it);
                it = mFieldDeclarations->erase(it);
            } else {
                it++;
            }
        }

        if (!nonFpDecs->empty()) {
            auto subStruct = new CompositeDeclaration(Type::Qualifier::STRUCT,
                                                      getName(),
                                                      nonFpDecs);

            mFieldDeclarations->insert(mFieldDeclarations->begin(), subStruct);
        }
    }
}

std::string CompositeDeclaration::getInterfaceName() const {
    std::string baseName{getName()};

    if (baseName.length() > 2 &&
        baseName.substr(baseName.length() - 2) == "_t") {

        baseName = baseName.substr(0, baseName.length() - 2);
    }

    return "I" + StringHelper::SnakeCaseToCamelCase(baseName);
}

bool CompositeDeclaration::isInterface() const {
    if (mQualifier != Type::Qualifier::STRUCT) {
        return false;
    }

    for (auto &declaration : *mFieldDeclarations) {
        if (declaration->decType() == FunctionDeclaration::type()) {
            return true;
        }
    }
    return false;
}

void CompositeDeclaration::setEnumTypeName(const std::string &name) {
    CHECK(mQualifier == Type::Qualifier::ENUM);

    mEnumTypeName = name;
}

} //namespace android