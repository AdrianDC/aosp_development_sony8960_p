/*
 * Copyright (C) 2017 The Android Open Source Project
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

#include "Hash.h"

#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>

#include <openssl/sha.h>

namespace android {

const Hash &Hash::getHash(const std::string &path) {
    static std::map<std::string, Hash> hashes;

    auto it = hashes.find(path);

    if (hashes.find(path) == hashes.end()) {
        it = hashes.insert(it, {path, Hash(path)});
    }

    return it->second;
}

static std::vector<uint8_t> sha256File(const std::string &path) {
    std::ifstream stream(path);
    std::stringstream fileStream;
    fileStream << stream.rdbuf();
    std::string fileContent = fileStream.str();

    std::vector<uint8_t> ret = std::vector<uint8_t>(SHA256_DIGEST_LENGTH);

    SHA256(reinterpret_cast<const uint8_t *>(fileContent.c_str()),
            fileContent.size(), ret.data());

    return ret;
}

Hash::Hash(const std::string &path)
  : mPath(path),
    mHash(sha256File(path)) {}

std::string Hash::hexString() const {
    std::ostringstream s;
    s << std::hex << std::setfill('0');
    for (uint8_t i : mHash) {
        s << std::setw(2) << static_cast<int>(i);
    }
    return s.str();
}

const std::vector<uint8_t> &Hash::raw() const {
    return mHash;
}

const std::string &Hash::getPath() const {
    return mPath;
}

}  // android