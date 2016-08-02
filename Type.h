#ifndef TYPE_H_

#define TYPE_H_

#include <android-base/macros.h>
#include <string>
#include <utils/Errors.h>

namespace android {

struct Formatter;
struct ScalarType;

struct Type {
    Type();
    virtual ~Type();

    virtual void dump(Formatter &out) const = 0;
    virtual bool isScope() const;
    virtual bool isInterface() const;
    virtual const ScalarType *resolveToScalarType() const;

    enum StorageMode {
        StorageMode_Stack,
        StorageMode_Argument,
        StorageMode_Result
    };
    virtual std::string getCppType(
            StorageMode mode, std::string *extra) const;

    // Convenience, gets StorageMode_Stack type.
    std::string getCppType(std::string *extra) const;

    std::string getCppResultType(std::string *extra) const;
    std::string getCppArgumentType(std::string *extra) const;

    enum ErrorMode {
        ErrorMode_Ignore,
        ErrorMode_Goto,
        ErrorMode_Break,
        ErrorMode_Return,
    };
    virtual void emitReaderWriter(
            Formatter &out,
            const std::string &name,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode) const;

    virtual void emitReaderWriterEmbedded(
            Formatter &out,
            const std::string &name,
            bool nameIsPointer,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode,
            const std::string &parentName,
            const std::string &offsetText) const;

    virtual status_t emitTypeDeclarations(Formatter &out) const;

    virtual status_t emitTypeDefinitions(
            Formatter &out, const std::string prefix) const;

    virtual bool needsEmbeddedReadWrite() const;
    virtual bool resultNeedsDeref() const;

protected:
    void handleError(Formatter &out, ErrorMode mode) const;
    void handleError2(Formatter &out, ErrorMode mode) const;

    void emitReaderWriterEmbeddedForTypeName(
            Formatter &out,
            const std::string &name,
            bool nameIsPointer,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode,
            const std::string &parentName,
            const std::string &offsetText,
            const std::string &typeName,
            const std::string &childName) const;

private:
    DISALLOW_COPY_AND_ASSIGN(Type);
};

}  // namespace android

#endif  // TYPE_H_

