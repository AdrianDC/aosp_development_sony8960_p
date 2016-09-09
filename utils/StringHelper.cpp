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

#include "StringHelper.h"

#include <sstream>

namespace android {

// static
std::string StringHelper::Upcase(const std::string &in) {
    std::string out{in};

    for (auto &ch : out) {
        ch = toupper(ch);
    }

    return out;
}

// static
std::string StringHelper::Capitalize(const std::string &in) {
    std::string out{in};

    if(!out.empty()) {
        out[0] = toupper(out[0]);
    }

    return out;
}

// static
std::string StringHelper::SnakeCaseToCamelCase(const std::string &in) {
    std::istringstream ss(in);
    std::string word;
    std::string out;

    while (std::getline(ss, word, '_')) {
        out += Capitalize(word);
    }

    return out;
}

// static
void StringHelper::SplitString(
        const std::string &s, char c, std::vector<std::string> *components) {
    components->clear();

    size_t startPos = 0;
    size_t matchPos;
    while ((matchPos = s.find(c, startPos)) != std::string::npos) {
        components->push_back(s.substr(startPos, matchPos - startPos));
        startPos = matchPos + 1;
    }

    if (startPos + 1 < s.length()) {
        components->push_back(s.substr(startPos));
    }
}

// static
std::string StringHelper::JoinStrings(
        const std::vector<std::string> &components,
        const std::string &separator) {
    std::string out;
    bool first = true;
    for (const auto &component : components) {
        if (!first) {
            out += separator;
        }
        out += component;

        first = false;
    }

    return out;
}

}  // namespace android

