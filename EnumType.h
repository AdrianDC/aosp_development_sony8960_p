#ifndef ENUM_TYPE_H_

#define ENUM_TYPE_H_

#include "NamedType.h"

#include <vector>

namespace android {

struct EnumValue;

struct EnumType : public NamedType {
    EnumType(const char *localName,
             std::vector<EnumValue *> *values,
             Type *storageType = NULL);

    const Type *storageType() const;
    const std::vector<EnumValue *> &values() const;

    const ScalarType *resolveToScalarType() const override;

    bool isEnum() const override;

    std::string getCppType(StorageMode mode, std::string *extra) const override;

    std::string getJavaType() const override;
    std::string getJavaSuffix() const override;

    void emitReaderWriter(
            Formatter &out,
            const std::string &name,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode) const override;

    status_t emitTypeDeclarations(Formatter &out) const override;
    status_t emitJavaTypeDeclarations(Formatter &out) const override;

    status_t emitVtsTypeDeclarations(Formatter &out) const override;
    status_t emitVtsAttributeType(Formatter &out) const override;

private:
    void getTypeChain(std::vector<const EnumType *> *out) const;
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

