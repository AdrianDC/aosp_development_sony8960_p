#ifndef FQNAME_H_

#define FQNAME_H_

#include <android-base/macros.h>
#include <string>

namespace android {

struct FQName {
    explicit FQName(const std::string &s);

    bool isValid() const;
    bool setTo(const std::string &s);

    void applyDefaults(
            const std::string &defaultPackage,
            const std::string &defaultVersion);

    std::string package() const;
    std::string version() const;
    std::string name() const;

    std::string debugString() const;

private:
    bool mValid;
    std::string mPackage;
    std::string mVersion;
    std::string mName;

    DISALLOW_COPY_AND_ASSIGN(FQName);
};

}  // namespace android

#endif  // FQNAME_H_
