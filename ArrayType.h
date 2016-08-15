#ifndef ARRAY_TYPE_H_

#define ARRAY_TYPE_H_

#include "Type.h"

#include <string>

namespace android {

struct ArrayType : public Type {
    ArrayType(Type *elementType, const char *dimension);

    std::string getCppType(StorageMode mode, std::string *extra) const override;

    std::string getJavaType() const override;

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

    void emitJavaReaderWriter(
            Formatter &out,
            const std::string &parcelObj,
            const std::string &argName,
            bool isReader) const override;

private:
    Type *mElementType;
    std::string mDimension;

    DISALLOW_COPY_AND_ASSIGN(ArrayType);
};

}  // namespace android

#endif  // ARRAY_TYPE_H_

