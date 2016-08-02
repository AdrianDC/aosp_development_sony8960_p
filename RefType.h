#ifndef REF_TYPE_H_

#define REF_TYPE_H_

#include "NamedType.h"

namespace android {

struct RefType : public NamedType {
    RefType(const char *name, Type *type);

    void dump(Formatter &out) const override;

    const Type *referencedType() const;

    std::string getCppType(StorageMode mode, std::string *extra) const override;

    void emitReaderWriter(
            Formatter &out,
            const std::string &name,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode) const override;

    void emitReaderWriterEmbedded(
            Formatter &out,
            const std::string &name,
            bool nameIsPointer,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode,
            const std::string &parentName,
            const std::string &offsetText) const override;

private:
    Type *mReferencedType;

    DISALLOW_COPY_AND_ASSIGN(RefType);
};

}  // namespace android

#endif  // REF_TYPE_H_

