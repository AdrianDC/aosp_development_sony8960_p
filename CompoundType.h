#ifndef COMPOUND_TYPE_H_

#define COMPOUND_TYPE_H_

#include "Scope.h"

#include <utils/Vector.h>

namespace android {

struct CompoundField {
    CompoundField(const char *name, Type *type);

    void dump(Formatter &out) const;

private:
    std::string mName;
    Type *mType;

    DISALLOW_COPY_AND_ASSIGN(CompoundField);
};

struct CompoundType : public Scope {
    enum Style {
        STYLE_STRUCT,
        STYLE_UNION,
    };

    CompoundType(Style style, const char *name);

    void setFields(Vector<CompoundField *> *fields);

    void dump(Formatter &out) const override;

private:
    Style mStyle;
    Vector<CompoundField *> *mFields;

    DISALLOW_COPY_AND_ASSIGN(CompoundType);
};

}  // namespace android

#endif  // COMPOUND_TYPE_H_

