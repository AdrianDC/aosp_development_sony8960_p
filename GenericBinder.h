#ifndef GENERIC_BINDER_H_

#define GENERIC_BINDER_H_

#include "Type.h"

namespace android {

struct GenericBinder : public Type {
    GenericBinder();

    bool isBinder() const override;

    std::string getCppType(StorageMode mode, std::string *extra) const override;
    std::string getJavaType() const override;

    void emitReaderWriter(
            Formatter &out,
            const std::string &name,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode) const override;

    void emitJavaReaderWriter(
            Formatter &out,
            const std::string &parcelObj,
            const std::string &argName,
            bool isReader) const override;

    status_t emitVtsAttributeType(Formatter &out) const override;
};

}  // namespace android

#endif  // GENERIC_BINDER_H_
