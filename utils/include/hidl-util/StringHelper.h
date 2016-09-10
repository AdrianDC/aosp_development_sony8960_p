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

#ifndef STRING_HELPER_H_

#define STRING_HELPER_H_

#include <string>
#include <android-base/macros.h>
#include <vector>

namespace android {

struct StringHelper {

    static std::string Upcase(const std::string &in);

    static void SplitString(
        const std::string &s,
        char c,
        std::vector<std::string> *components);

    static std::string JoinStrings(
        const std::vector<std::string> &components,
        const std::string &separator);

private:
    StringHelper() {}

    DISALLOW_COPY_AND_ASSIGN(StringHelper);
};

}  // namespace android

#endif  // STRING_HELPER_H_

