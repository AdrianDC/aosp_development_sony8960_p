#ifndef INTERFACE_H_

#define INTERFACE_H_

#include "Scope.h"

#include <utils/Vector.h>

namespace android {

struct Method;

struct Interface : public Scope {
    Interface(const char *name, Type *super);

    void addMethod(Method *method);

    void dump(Formatter &out) const override;
    bool isInterface() const override;

    const Type *superType() const;

private:
    Type *mSuperType;
    Vector<Method *> mMethods;

    DISALLOW_COPY_AND_ASSIGN(Interface);
};

}  // namespace android

#endif  // INTERFACE_H_

