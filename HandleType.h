#ifndef HANDLE_TYPE_H_

#define HANDLE_TYPE_H_

#include "Type.h"

namespace android {

struct HandleType : public Type {
    HandleType();

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

    bool isJavaCompatible() const override;

    void getAlignmentAndSize(size_t *align, size_t *size) const override;
};

}  // namespace android

#endif  // HANDLE_TYPE_H_

