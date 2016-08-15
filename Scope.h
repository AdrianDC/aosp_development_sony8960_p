#ifndef SCOPE_H_

#define SCOPE_H_

#include "NamedType.h"

#include <utils/KeyedVector.h>
#include <utils/Vector.h>

namespace android {

struct Constant;
struct Formatter;
struct Interface;

struct Scope : public NamedType {
    Scope();

    bool addType(const char *localName, NamedType *type);

    Type *lookupType(const char *name) const;

    bool addConstant(Constant *constant);

    bool isScope() const override;

    // Returns the single interface or NULL.
    Interface *getInterface() const;

    bool containsSingleInterface(std::string *ifaceName) const;

    std::string getJavaType() const override;

    status_t emitTypeDeclarations(Formatter &out) const override;
    status_t emitJavaTypeDeclarations(Formatter &out) const override;

    status_t emitTypeDefinitions(
            Formatter &out, const std::string prefix) const override;

private:
    Vector<Type *> mTypes;
    KeyedVector<std::string, size_t> mTypeIndexByName;

    KeyedVector<std::string, Constant *> mConstants;

    DISALLOW_COPY_AND_ASSIGN(Scope);
};

}  // namespace android

#endif  // SCOPE_H_

