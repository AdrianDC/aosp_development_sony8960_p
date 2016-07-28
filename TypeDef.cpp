#include "TypeDef.h"

#include "Formatter.h"

namespace android {

TypeDef::TypeDef(const char *name, Type *type)
    : NamedType(name),
      mReferencedType(type) {
}

const Type *TypeDef::referencedType() const {
    return mReferencedType;
}

void TypeDef::dump(Formatter &out) const {
    out << "typedef ";
    mReferencedType->dump(out);
    out << " " << name() << ";\n\n";
}

}  // namespace android

