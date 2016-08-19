#ifndef SCOPE_H_

#define SCOPE_H_

#include "NamedType.h"

#include <utils/KeyedVector.h>
#include <vector>

namespace android {

struct Formatter;
struct Interface;

struct Scope : public NamedType {
    Scope();

    bool addType(const char *localName, Type *type, std::string *errorMsg);

    Type *lookupType(const char *name) const;

    bool isScope() const override;

    // Returns the single interface or NULL.
    Interface *getInterface() const;

    bool containsSingleInterface(std::string *ifaceName) const;

    std::string pickUniqueAnonymousName() const;

    std::string getJavaType() const override;

    status_t emitTypeDeclarations(Formatter &out) const override;
    status_t emitJavaTypeDeclarations(Formatter &out) const override;

    status_t emitTypeDefinitions(
            Formatter &out, const std::string prefix) const override;

    const std::vector<Type *> &getSubTypes() const;

    status_t emitVtsTypeDeclarations(Formatter &out) const override;

    bool isJavaCompatible() const override;

private:
    std::vector<Type *> mTypes;
    KeyedVector<std::string, size_t> mTypeIndexByName;

    DISALLOW_COPY_AND_ASSIGN(Scope);
};

}  // namespace android

#endif  // SCOPE_H_

