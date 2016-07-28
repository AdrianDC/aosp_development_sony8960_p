#ifndef ENUM_TYPE_H_

#define ENUM_TYPE_H_

#include "NamedType.h"

#include <utils/Vector.h>

namespace android {

struct EnumValue {
    EnumValue(const char *name, const char *value = NULL);

    void dump(Formatter &out) const;

private:
    std::string mName;
    const char *mValue;

    DISALLOW_COPY_AND_ASSIGN(EnumValue);
};

struct EnumType : public NamedType {
    EnumType(const char *name,
             Vector<EnumValue *> *values,
             Type *storageType = NULL);

    void dump(Formatter &out) const override;

private:
    Vector<EnumValue *> *mValues;
    Type *mStorageType;

    DISALLOW_COPY_AND_ASSIGN(EnumType);
};

}  // namespace android

#endif  // ENUM_TYPE_H_

