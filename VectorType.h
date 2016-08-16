#ifndef VECTOR_TYPE_H_

#define VECTOR_TYPE_H_

#include "Type.h"

namespace android {

struct VectorType : public Type {
    VectorType(Type *elementType);

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

    bool needsEmbeddedReadWrite() const override;
    bool resultNeedsDeref() const override;

    status_t emitVtsTypeDeclarations(Formatter &out) const override;
    status_t emitVtsAttributeType(Formatter &out) const override;

    bool isJavaCompatible() const override;

 private:
    Type *mElementType;

    DISALLOW_COPY_AND_ASSIGN(VectorType);
};

}  // namespace android

#endif  // VECTOR_TYPE_H_

