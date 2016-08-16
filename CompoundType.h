#ifndef COMPOUND_TYPE_H_

#define COMPOUND_TYPE_H_

#include "Scope.h"

#include <vector>

namespace android {

struct CompoundField;

struct CompoundType : public Scope {
    enum Style {
        STYLE_STRUCT,
        STYLE_UNION,
    };

    CompoundType(Style style);

    bool setFields(std::vector<CompoundField *> *fields, std::string *errorMsg);

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

    status_t emitTypeDeclarations(Formatter &out) const override;

    status_t emitTypeDefinitions(
            Formatter &out, const std::string prefix) const override;

    bool needsEmbeddedReadWrite() const override;
    bool resultNeedsDeref() const override;

    status_t emitVtsTypeDeclarations(Formatter &out) const override;
    status_t emitVtsAttributeType(Formatter &out) const override;

    bool isJavaCompatible() const override;

private:
    Style mStyle;
    std::vector<CompoundField *> *mFields;

    void emitStructReaderWriter(
            Formatter &out, const std::string &prefix, bool isReader) const;

    DISALLOW_COPY_AND_ASSIGN(CompoundType);
};

struct CompoundField {
    CompoundField(const char *name, Type *type);

    std::string name() const;
    const Type &type() const;

private:
    std::string mName;
    Type *mType;

    DISALLOW_COPY_AND_ASSIGN(CompoundField);
};

}  // namespace android

#endif  // COMPOUND_TYPE_H_

