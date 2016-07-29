#include "Scope.h"

#include "Constant.h"
#include "Formatter.h"

namespace android {

Scope::Scope(const char *name)
    : NamedType(name) {
}

bool Scope::addType(NamedType *type) {
    std::string name = type->name();

    if (mTypeIndexByName.indexOfKey(name) >= 0) {
        return false;
    }

    size_t index = mTypes.size();
    mTypes.push_back(type);
    mTypeIndexByName.add(name, index);

    return true;
}

bool Scope::addConstant(Constant *constant) {
    ssize_t index = mConstants.indexOfKey(constant->name());

    if (index >= 0) {
        return false;
    }

    mConstants.add(constant->name(), constant);

    return true;
}

Type *Scope::lookupType(const char *name) const {
    ssize_t index = mTypeIndexByName.indexOfKey(name);

    if (index >= 0) {
        return mTypes[mTypeIndexByName.valueAt(index)];
    }

    return NULL;
}

void Scope::dump(Formatter &out) const {
    for (size_t i = 0; i < mConstants.size(); ++i) {
        mConstants.valueAt(i)->dump(out);
    }

    if (mConstants.size() > 0) {
        out << "\n";
    }

    for (size_t i = 0; i < mTypes.size(); ++i) {
        mTypes[i]->dump(out);
    }
}

bool Scope::isScope() const {
    return true;
}

}  // namespace android

