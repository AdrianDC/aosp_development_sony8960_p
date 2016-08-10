#ifndef ANNOTATION_H_

#define ANNOTATION_H_

#include <android-base/macros.h>

#include <string>
#include <utils/KeyedVector.h>

namespace android {

struct Formatter;
using AnnotationParamVector =
    DefaultKeyedVector<std::string, std::vector<std::string> *>;

struct Annotation {
    Annotation(const char *name, AnnotationParamVector *params);

    std::string name() const;
    const AnnotationParamVector &params() const;

    void dump(Formatter &out) const;

private:
    std::string mName;
    AnnotationParamVector *mParamsByName;

    DISALLOW_COPY_AND_ASSIGN(Annotation);
};

}  // namespace android

#endif  // ANNOTATION_H_
