#ifndef INTERFACE_H_

#define INTERFACE_H_

#include "Scope.h"

#include <vector>

namespace android {

struct Method;

struct Interface : public Scope {
    Interface(const char *name, Type *super);

    void addMethod(Method *method);

    void dump(Formatter &out) const override;
    bool isInterface() const override;

    const Type *superType() const;

    const std::vector<Method *> &methods() const;

    std::string getCppType(StorageMode mode, std::string *extra) const override;

    void emitReaderWriter(
            Formatter &out,
            const std::string &name,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode) const override;

private:
    Type *mSuperType;
    std::vector<Method *> mMethods;

    DISALLOW_COPY_AND_ASSIGN(Interface);
};

}  // namespace android

#endif  // INTERFACE_H_

