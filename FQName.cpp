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

#include "FQName.h"

#include <hidl-util/StringHelper.h>
#include <android-base/logging.h>
#include <iostream>
#include <regex>
#include <sstream>

#define RE_COMPONENT    "[a-zA-Z_][a-zA-Z_0-9]*"
#define RE_PATH         RE_COMPONENT "(?:[.]" RE_COMPONENT ")*"
#define RE_VERSION      "@[0-9]+[.][0-9]+"

static const std::regex kRE1("(" RE_PATH ")(" RE_VERSION ")?::(" RE_PATH ")");
static const std::regex kRE2("(" RE_VERSION ")::(" RE_PATH ")");
static const std::regex kRE3("(" RE_PATH ")(" RE_VERSION ")");
static const std::regex kRE4("(" RE_COMPONENT ")([.]" RE_COMPONENT ")+");
static const std::regex kRE5("(" RE_COMPONENT ")");

static const std::regex kRE6("(" RE_PATH ")(" RE_VERSION ")?::(" RE_PATH "):(" RE_COMPONENT ")");
static const std::regex kRE7("(" RE_VERSION ")::(" RE_PATH "):(" RE_COMPONENT ")");
static const std::regex kRE8("(" RE_PATH "):(" RE_COMPONENT ")");

namespace android {

FQName::FQName()
    : mValid(false),
      mIsIdentifier(false) {
}

FQName::FQName(const std::string &s)
    : mValid(false),
      mIsIdentifier(false) {
    setTo(s);
}

FQName::FQName(
        const std::string &package,
        const std::string &version,
        const std::string &name,
        const std::string &valueName)
    : mValid(true),
      mIsIdentifier(false),
      mPackage(package),
      mVersion(version),
      mName(name),
      mValueName(valueName) {
}

FQName::FQName(const FQName& other)
    : mValid(other.mValid),
      mIsIdentifier(other.mIsIdentifier),
      mPackage(other.mPackage),
      mVersion(other.mVersion),
      mName(other.mName),
      mValueName(other.mValueName) {
}

FQName::FQName(const std::vector<std::string> &names)
    : mValid(false),
      mIsIdentifier(false) {
    setTo(StringHelper::JoinStrings(names, "."));
}

bool FQName::isValid() const {
    return mValid;
}

bool FQName::isIdentifier() const {
    return mIsIdentifier;
}

bool FQName::isFullyQualified() const {
    return !mPackage.empty() && !mVersion.empty() && !mName.empty();
}

bool FQName::isValidValueName() const {
    return mIsIdentifier
        || (!mName.empty() && !mValueName.empty());
}

bool FQName::setTo(const std::string &s) {
    mPackage.clear();
    mVersion.clear();
    mName.clear();

    mValid = true;

    std::smatch match;
    if (std::regex_match(s, match, kRE1)) {
        CHECK_EQ(match.size(), 4u);

        mPackage = match.str(1);
        mVersion = match.str(2);
        mName = match.str(3);
    } else if (std::regex_match(s, match, kRE2)) {
        CHECK_EQ(match.size(), 3u);

        mVersion = match.str(1);
        mName = match.str(2);
    } else if (std::regex_match(s, match, kRE3)) {
        CHECK_EQ(match.size(), 3u);

        mPackage = match.str(1);
        mVersion = match.str(2);
    } else if (std::regex_match(s, match, kRE4)) {
        mName = match.str(0);
    } else if (std::regex_match(s, match, kRE5)) {
        mIsIdentifier = true;
        mName = match.str(0);
    } else if (std::regex_match(s, match, kRE6)) {
        CHECK_EQ(match.size(), 5u);

        mPackage = match.str(1);
        mVersion = match.str(2);
        mName = match.str(3);
        mValueName = match.str(4);
    } else if (std::regex_match(s, match, kRE7)) {
        CHECK_EQ(match.size(), 4u);

        mVersion = match.str(1);
        mName = match.str(2);
        mValueName = match.str(3);
    } else if (std::regex_match(s, match, kRE8)) {
        CHECK_EQ(match.size(), 3u);

        mName = match.str(1);
        mValueName = match.str(2);
    } else {
        mValid = false;
    }

    // mValueName must go with mName.
    if (!mValueName.empty()) {
        CHECK(!mName.empty());
    }

    return isValid();
}

std::string FQName::package() const {
    return mPackage;
}

std::string FQName::version() const {
    return mVersion;
}

std::string FQName::name() const {
    return mName;
}

std::vector<std::string> FQName::names() const {
    std::vector<std::string> res {};
    std::istringstream ss(name());
    std::string s;
    while (std::getline(ss, s, '.')) {
        res.push_back(s);
    }
    return res;
}

std::string FQName::valueName() const {
    return mValueName;
}

FQName FQName::typeName() const {
    return FQName(mPackage, mVersion, mName);
}

void FQName::applyDefaults(
        const std::string &defaultPackage,
        const std::string &defaultVersion) {
    if (mPackage.empty()) {
        mPackage = defaultPackage;
    }

    if (mVersion.empty()) {
        mVersion = defaultVersion;
    }
}

std::string FQName::string() const {
    CHECK(mValid);

    std::string out;
    out.append(mPackage);
    out.append(mVersion);
    if (!mName.empty()) {
        if (!mPackage.empty() || !mVersion.empty()) {
            out.append("::");
        }
        out.append(mName);

        if (!mValueName.empty()) {
            out.append(":");
            out.append(mValueName);
        }
    }

    return out;
}

void FQName::print() const {
    if (!mValid) {
        LOG(INFO) << "INVALID";
        return;
    }

    LOG(INFO) << string();
}

bool FQName::operator<(const FQName &other) const {
    return string() < other.string();
}

bool FQName::operator==(const FQName &other) const {
    return string() == other.string();
}

std::string FQName::getInterfaceBaseName() const {
    CHECK(names().size() == 1) << "Must be a top level type";
    CHECK(!mName.empty() && mName[0] == 'I') << mName;

    // cut off the leading 'I'.
    return mName.substr(1);
}

FQName FQName::getTypesForPackage() const {
    return FQName(package(), version(), "types");
}

const FQName FQName::getTopLevelType() const {
    auto idx = mName.find('.');

    if (idx == std::string::npos) {
        return *this;
    }

    return FQName(mPackage, mVersion, mName.substr(0, idx));
}

std::string FQName::tokenName() const {
    std::vector<std::string> components;
    getPackageAndVersionComponents(&components, true /* cpp_compatible */);

    std::vector<std::string> nameComponents;
    StringHelper::SplitString(mName, '.', &nameComponents);

    components.insert(components.end(), nameComponents.begin(), nameComponents.end());

    return StringHelper::JoinStrings(components, "_");
}

std::string FQName::cppNamespace() const {
    std::vector<std::string> components;
    getPackageAndVersionComponents(&components, true /* cpp_compatible */);

    std::string out = "::";
    out += StringHelper::JoinStrings(components, "::");

    return out;
}

std::string FQName::cppLocalName() const {
    std::vector<std::string> components;
    StringHelper::SplitString(mName, '.', &components);

    return StringHelper::JoinStrings(components, "::")
            + (mValueName.empty() ? "" : ("::" + mValueName));
}

std::string FQName::cppName() const {
    std::string out = cppNamespace();

    std::vector<std::string> components;
    StringHelper::SplitString(name(), '.', &components);
    out += "::";
    out += StringHelper::JoinStrings(components, "::");
    if (!mValueName.empty()) {
        out  += "::" + mValueName;
    }

    return out;
}

std::string FQName::javaPackage() const {
    std::vector<std::string> components;
    getPackageAndVersionComponents(&components, true /* cpp_compatible */);

    return StringHelper::JoinStrings(components, ".");
}

std::string FQName::javaName() const {
    return javaPackage() + "." + name()
            + (mValueName.empty() ? "" : ("." + mValueName));
}

void FQName::getPackageComponents(std::vector<std::string> *components) const {
    StringHelper::SplitString(package(), '.', components);
}

void FQName::getPackageAndVersionComponents(
        std::vector<std::string> *components,
        bool cpp_compatible) const {
    getPackageComponents(components);

    if (!cpp_compatible) {
        components->push_back(getPackageMajorVersion() +
                "." + getPackageMinorVersion());
        return;
    }

    // Form "Vmajor_minor".
    std::string versionString = "V";
    versionString.append(getPackageMajorVersion());
    versionString.append("_");
    versionString.append(getPackageMinorVersion());

    components->push_back(versionString);
}

std::string FQName::getPackageMajorVersion() const {
    const std::string packageVersion = version();
    CHECK(packageVersion[0] == '@');
    const size_t dotPos = packageVersion.find('.');
    CHECK(dotPos != std::string::npos);
    return packageVersion.substr(1, dotPos - 1);
}

std::string FQName::getPackageMinorVersion() const {
    const std::string packageVersion = version();
    CHECK(packageVersion[0] == '@');
    const size_t dotPos = packageVersion.find('.');
    CHECK(dotPos != std::string::npos);
    return packageVersion.substr(dotPos + 1);
}

std::string FQName::getPackageFullVersion() const {
    const std::string packageVersion = version();
    CHECK_GT(packageVersion.length(), 1u);
    return packageVersion.substr(1);
}

bool FQName::endsWith(const FQName &other) const {
    std::string s1 = string();
    std::string s2 = other.string();

    size_t pos = s1.rfind(s2);
    if (pos == std::string::npos || pos + s2.size() != s1.size()) {
        return false;
    }

    // A match is only a match if it is preceded by a "boundary", i.e.
    // we perform a component-wise match from the end.
    // "az" is not a match for "android.hardware.foo@1.0::IFoo.bar.baz",
    // "baz", "bar.baz", "IFoo.bar.baz", "@1.0::IFoo.bar.baz" are.
    if (pos == 0) {
        // matches "android.hardware.foo@1.0::IFoo.bar.baz"
        return true;
    }

    if (s1[pos - 1] == '.') {
        // matches "baz" and "bar.baz"
        return true;
    }

    if (s1[pos - 1] == ':') {
        // matches "IFoo.bar.baz"
        return true;
    }

    if (s1[pos] == '@') {
        // matches "@1.0::IFoo.bar.baz"
        return true;
    }

    return false;
}

}  // namespace android

