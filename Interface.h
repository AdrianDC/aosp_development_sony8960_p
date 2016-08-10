#ifndef INTERFACE_H_

#define INTERFACE_H_

#include "Scope.h"

#include <utils/KeyedVector.h>
#include <vector>

namespace android {

struct Annotation;
struct Method;

struct Interface : public Scope {
    Interface(
            Interface *super,
            KeyedVector<std::string, Annotation *> *annotations);

    void addMethod(Method *method);

    bool isInterface() const override;

    const Interface *superType() const;

    const std::vector<Method *> &methods() const;

    const KeyedVector<std::string, Annotation *> &annotations() const;

    std::string getCppType(StorageMode mode, std::string *extra) const override;

    void emitReaderWriter(
            Formatter &out,
            const std::string &name,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode) const override;

private:
    Interface *mSuperType;
    std::vector<Method *> mMethods;
    KeyedVector<std::string, Annotation *> *mAnnotationsByName;

    DISALLOW_COPY_AND_ASSIGN(Interface);
};

}  // namespace android

#endif  // INTERFACE_H_

