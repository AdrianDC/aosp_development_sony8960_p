#ifndef STRING_TYPE_H_

#define STRING_TYPE_H_

#include "Type.h"

namespace android {

struct StringType : public Type {
    StringType();

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

    void emitJavaFieldInitializer(
            Formatter &out, const std::string &fieldName) const override;

    void emitJavaFieldReaderWriter(
            Formatter &out,
            const std::string &blobName,
            const std::string &fieldName,
            const std::string &offset,
            bool isReader) const override;

    bool needsEmbeddedReadWrite() const override;
    bool resultNeedsDeref() const override;

    status_t emitVtsTypeDeclarations(Formatter &out) const override;

    void getAlignmentAndSize(size_t *align, size_t *size) const override;
};

}  // namespace android

#endif  // STRING_TYPE_H_

