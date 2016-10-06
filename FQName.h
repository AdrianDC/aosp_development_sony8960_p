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

#ifndef FQNAME_H_

#define FQNAME_H_

#include <android-base/macros.h>
#include <string>
#include <vector>

namespace android {

struct FQName {
    explicit FQName();
    explicit FQName(const std::string &s);

    FQName(const std::string &package,
           const std::string &version,
           const std::string &name,
           const std::string &valueName = "");

    // a synonym to FQName(names.join("."))
    FQName(const std::vector<std::string> &names);

    FQName(const FQName& other);

    bool isValid() const;
    bool isIdentifier() const;
    bool setTo(const std::string &s);

    void applyDefaults(
            const std::string &defaultPackage,
            const std::string &defaultVersion);

    std::string package() const;
    std::string version() const;

    // The next two methods return the name part of the FQName, that is, the
    // part after the version field.  For example:
    //
    // package android.hardware.tests.foo@1.0;
    // interface IFoo {
    //    struct bar {
    //        struct baz {
    //            ...
    //        };
    //    };
    // };
    //
    // package android.hardware.tests.bar@1.0;
    // import android.hardware.tests.foo@1.0;
    // interface {
    //    struct boo {
    //        IFoo.bar.baz base;
    //    };
    // }
    //
    // The FQName for base is android.hardware.tests.foo@1.0::IFoo.bar.baz; so
    // FQName::name() will return "IFoo.bar.baz". FQName::names() will return
    // std::vector<std::string>{"IFoo","bar","baz"}

    std::string name() const;
    std::vector<std::string> names() const;

    // The next two methods returns two parts of the FQName, that is,
    // the first part package + version + name, the second part valueName.
    FQName typeName() const;
    std::string valueName() const;

    bool isFullyQualified() const;

    // true if:
    // 1. (package)?(version)?(name):(valueName)
    // 2. (valueName), aka a single identifier
    bool isValidValueName() const;

    void print() const;
    std::string string() const;

    bool operator<(const FQName &other) const;
    bool operator==(const FQName &other) const;

    // Must be called on an interface
    // ::android::hardware::Foo::V1_0::IBar
    // -> Bar
    std::string getInterfaceBaseName() const;

    // the following comments all assume that the FQName
    // is ::android::hardware::Foo::V1_0::IBar::Baz

    // returns highest type in the hidl namespace, i.e.
    // ::android::hardware::Foo::V1_0::IBar
    const FQName getTopLevelType() const;

    // returns an unambiguous fully qualified name which can be
    // baked into a token, i.e.
    // android_hardware_Foo_V1_0_IBar_Baz
    std::string tokenName() const;

    // Returns an absolute C++ namespace prefix, i.e.
    // ::android::hardware::Foo::V1_0.
    std::string cppNamespace() const;

    // Returns a name qualified assuming we are in cppNamespace, i.e.
    // IBar::Baz.
    std::string cppLocalName() const;

    // Returns a fully qualified absolute C++ type name, i.e.
    // ::android::hardware::Foo::V1_0::IBar::Baz.
    std::string cppName() const;

    // Returns the java package name, i.e. "android.hardware.Foo.V1_0".
    std::string javaPackage() const;

    // Returns the fully qualified java type name,
    // i.e. "android.hardware.Foo.V1_0.IBar.Baz"
    std::string javaName() const;

    bool endsWith(const FQName &other) const;

    void getPackageComponents(std::vector<std::string> *components) const;

    void getPackageAndVersionComponents(
            std::vector<std::string> *components,
            bool cpp_compatible) const;

    std::string getPackageMajorVersion() const;

    std::string getPackageMinorVersion() const;

    // Returns the version of the package by cutting off the leading '@' prefix.
    std::string getPackageFullVersion() const;

private:
    bool mValid;
    bool mIsIdentifier;
    std::string mPackage;
    std::string mVersion;
    std::string mName;
    std::string mValueName;
};

}  // namespace android

#endif  // FQNAME_H_
