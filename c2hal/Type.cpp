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

#include "Type.h"
#include <sstream>

namespace android {

Type::Type(std::vector<Qualifier*> *qualifiers)
    : mQualifiers(qualifiers)
    {}

Type::~Type() {
    delete mArray;

    if(mQualifiers != NULL) {
        for(auto* qual : *mQualifiers) {
            delete qual;
        }
    }
    delete mQualifiers;}


void Type::setArray(Expression *array) {
    mArray = array;
}

const std::string Type::decorateName(const std::string &name) const {
    std::stringstream ss;

    std::string special = getSpecialTypeName();

    if(special.empty()) {
        ss << getHidlType();
    } else {
        ss << special;
    }

    ss << " " << name;

    return ss.str();
}

// static
std::map<std::string, std::string> Type::kCToHidlMap = {
    { "int", "int32_t" },
    { "native_handle_t", "handle" },

    // { "hidl_string", "string" },
    // { "hidl_vec", "vec"},
};

// static
const std::string Type::cToHidlType(const std::string &cType) {
    auto it = kCToHidlMap.find(cType);

    if (it == kCToHidlMap.end()) {
        return cType;
    }

    return (*it).second;
}

const std::string Type::getHidlType() const {
    if (mQualifiers == NULL) {
        return "";
    }

    std::stringstream ss;

    for(auto it = mQualifiers->begin(); it != mQualifiers->end(); ++it) {
        if (it != mQualifiers->begin()) {
            ss << " ";
        }

        switch((*it)->qualification) {
            case Type::Qualifier::STRUCT:
            case Type::Qualifier::UNION:
            case Type::Qualifier::ENUM:
            case Type::Qualifier::POINTER:
            case Type::Qualifier::CONST:
                ss << "/* "
                   << Type::qualifierText((*it)->qualification)
                   << " */";
                break;
            case Type::Qualifier::ID:
                ss << cToHidlType((*it)->id);
                break;
            case Type::Qualifier::GENERICS:
                ss << "<"
                   << (*it)->generics->decorateName("")
                   << ">";
                break;
            default: {
                ss << Type::qualifierText((*it)->qualification);
            }
        }
    }

    if (mArray != NULL) {
        ss << "[" << mArray->toString() << "]";
    }

    return ss.str();
}

const std::string Type::getRawQualifierList() const {
    if (mQualifiers == NULL) {
        return "";
    }

    std::stringstream ss;

    for(auto* qualifier : *mQualifiers) {
        ss << Type::qualifierText(qualifier->qualification) << " ";
    }

    return ss.str();
}

const std::string Type::getSpecialTypeName() const {
    // this makes for a relatively expensive comparison, but it is
    // readable until the converstion get nailed down.
    std::string qualifiers = getRawQualifierList();

    if (qualifiers == "const ID * " ||
        qualifiers == "ID * ") {

        std::string id = mQualifiers->at(mQualifiers->size() - 2)->id;

        if (id == "char") {
            return "string";
        } else {
            // can't tell if it's a hidl_vec or a pointer
            // return "vec<" + id + ">";
            return "";
        }
    }

    return "";
}

bool Type::isVoid() const {
    if (mQualifiers->size() == 0) {
        return true;
    }

    return mQualifiers->size() == 1 &&
           (*mQualifiers)[0]->qualification == Type::Qualifier::VOID;
}


std::string Type::removeLastId() {
    if(mQualifiers == NULL || mQualifiers->size() == 0) {
        return "";
    }

    Qualifier *last = (*mQualifiers)[mQualifiers->size() - 1];

    if(last == NULL || last->qualification != Qualifier::ID) {
        return "";
    }

    std::string ret{last->id};

    mQualifiers->erase(mQualifiers->end() - 1);

    return ret;
}

} //namespace android