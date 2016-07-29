#ifndef TYPE_H_

#define TYPE_H_

#include <android-base/macros.h>

namespace android {

struct Formatter;

struct Type {
    Type();
    virtual ~Type();

    virtual void dump(Formatter &out) const = 0;
    virtual bool isScope() const;
    virtual bool isInterface() const;

private:
    DISALLOW_COPY_AND_ASSIGN(Type);
};

}  // namespace android

#endif  // TYPE_H_

