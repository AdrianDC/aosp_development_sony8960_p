#include "Interface.h"

#include "Annotation.h"
#include "Formatter.h"
#include "Method.h"

namespace android {

Interface::Interface(
        Interface *super,
        AnnotationVector *annotations)
        : mSuperType(super),
          mAnnotationsByName(annotations) {
}

void Interface::addMethod(Method *method) {
    mMethods.push_back(method);
}

const Interface *Interface::superType() const {
    return mSuperType;
}

bool Interface::isInterface() const {
    return true;
}

const std::vector<Method *> &Interface::methods() const {
    return mMethods;
}

const AnnotationVector &Interface::annotations() const {
    return *mAnnotationsByName;
}

std::string Interface::getCppType(StorageMode mode, std::string *extra) const {
    extra->clear();
    const std::string base = "::android::sp<" + fullName() + ">";

    switch (mode) {
        case StorageMode_Stack:
        case StorageMode_Result:
            return base;

        case StorageMode_Argument:
            return "const " + base + "&";
    }
}

std::string Interface::getJavaType() const {
    return fullJavaName();
}

void Interface::emitReaderWriter(
        Formatter &out,
        const std::string &name,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode) const {
    const std::string parcelObjDeref =
        parcelObj + (parcelObjIsPointer ? "->" : ".");

    if (isReader) {
        const std::string binderName = "_hidl_" + name + "_binder";

        out << "::android::sp<::android::hardware::IBinder> "
            << binderName << ";\n";

        out << "_hidl_err = ";
        out << parcelObjDeref
            << "readNullableStrongBinder(&"
            << binderName
            << ");\n";

        handleError(out, mode);

        out << name
            << " = "
            << fullName()
            << "::asInterface("
            << binderName
            << ");\n";
    } else {
        out << "_hidl_err = ";
        out << parcelObjDeref
            << "writeStrongBinder("
            << fullName()
            << "::asBinder("
            << name
            << "));\n";

        handleError(out, mode);
    }
}

void Interface::emitJavaReaderWriter(
        Formatter &out,
        const std::string &parcelObj,
        const std::string &argName,
        bool isReader) const {
    if (isReader) {
        out << fullJavaName()
            << ".asInterface("
            << parcelObj
            << ".readStrongBinder());\n";
    } else {
        out << parcelObj
            << ".writeStrongBinder("
            << argName
            << " == null ? null : "
            << argName
            << ".asBinder());\n";
    }
}

status_t Interface::emitVtsArgumentType(Formatter &out) const {
    out << "type: TYPE_HIDL_CALLBACK\n"
        << "predefined_type: \""
        << localName()
        << "\"\n";
    return OK;
}

}  // namespace android

