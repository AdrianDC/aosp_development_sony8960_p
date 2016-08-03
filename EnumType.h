#ifndef ENUM_TYPE_H_

#define ENUM_TYPE_H_

#include "NamedType.h"

#include <vector>

namespace android {

struct EnumValue;

struct EnumType : public NamedType {
    EnumType(std::vector<EnumValue *> *values,
             Type *storageType = NULL);

    const ScalarType *resolveToScalarType() const override;

    std::string getCppType(StorageMode mode, std::string *extra) const override;

    void emitReaderWriter(
            Formatter &out,
            const std::string &name,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode) const override;

    status_t emitTypeDeclarations(Formatter &out) const override;

private:
    std::vector<EnumValue *> *mValues;
    Type *mStorageType;

    DISALLOW_COPY_AND_ASSIGN(EnumType);
};

struct EnumValue {
    EnumValue(const char *name, const char *value = NULL);

    std::string name() const;
    const char *value() const;

private:
    std::string mName;
    const char *mValue;

    DISALLOW_COPY_AND_ASSIGN(EnumValue);
};

}  // namespace android

#endif  // ENUM_TYPE_H_

