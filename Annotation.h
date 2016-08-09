#ifndef ANNOTATION_H_

#define ANNOTATION_H_

#include <android-base/macros.h>

#include <string>
#include <utils/KeyedVector.h>

namespace android {

struct Formatter;

struct Annotation {
    Annotation(
            const char *name,
            KeyedVector<std::string, std::vector<std::string> *> *params);

    std::string name() const;

    void dump(Formatter &out) const;

private:
    std::string mName;
    KeyedVector<std::string, std::vector<std::string> *> *mParamsByName;

    DISALLOW_COPY_AND_ASSIGN(Annotation);
};

}  // namespace android

#endif  // ANNOTATION_H_
