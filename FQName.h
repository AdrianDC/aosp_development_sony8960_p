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
           const std::string &name);

    bool isValid() const;
    bool setTo(const std::string &s);

    void applyDefaults(
            const std::string &defaultPackage,
            const std::string &defaultVersion);

    std::string package() const;
    std::string version() const;
    std::string name() const;

    bool isFullyQualified() const;

    void print() const;
    std::string string() const;

    bool operator<(const FQName &other) const;

    // Returns an absolute C++ namespace prefix, i.e.
    // ::android::hardware::Foo.
    std::string cppNamespace() const;

    // Returns a fully qualified absolute C++ type name, i.e.
    // ::android::hardware::Foo::bar::baz.
    std::string cppName() const;

    void getPackageComponents(std::vector<std::string> *components) const;

    void getPackageAndVersionComponents(
            std::vector<std::string> *components,
            bool cpp_compatible) const;

private:
    bool mValid;
    std::string mPackage;
    std::string mVersion;
    std::string mName;
};

}  // namespace android

#endif  // FQNAME_H_
