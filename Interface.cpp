#include "Interface.h"

#include "Formatter.h"
#include "Method.h"

namespace android {

Interface::Interface(const char *name, Type *super)
    : Scope(name),
      mSuperType(super) {
}

void Interface::addMethod(Method *method) {
    mMethods.push_back(method);
}

const Type *Interface::superType() const {
    return mSuperType;
}

void Interface::dump(Formatter &out) const {
    out << "interface " << name();

    if (mSuperType != NULL) {
        out << " extends ";
        mSuperType->dump(out);
    }

    out << " {\n";

    out.indent();
    Scope::dump(out);

    for (size_t i = 0; i < mMethods.size(); ++i) {
        mMethods[i]->dump(out);

        out << "\n";
    }

    out.unindent();

    out << "};\n\n";
}

}  // namespace android

