#include "Interface.h"

#include "Annotation.h"
#include "Formatter.h"
#include "Method.h"

#include <iostream>

namespace android {

Interface::Interface(Interface *super, AnnotationVector *annotations)
    : mSuperType(super),
      mAnnotationsByName(annotations),
      mIsJavaCompatibleInProgress(false) {
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

bool Interface::isBinder() const {
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
        out << "{\n";
        out.indent();

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

        out.unindent();
        out << "}\n\n";
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

status_t Interface::emitVtsAttributeDeclaration(Formatter &out) const {
    for (const auto &type : getSubTypes()) {
        out << "attribute: {\n";
        out.indent();
        status_t status = type->emitVtsTypeDeclarations(out);
        if (status != OK) {
            return status;
        }
        out.unindent();
        out << "}\n\n";
    }
    return OK;
}

status_t Interface::emitVtsMethodDeclaration(Formatter &out) const {
    for (const auto &method : mMethods) {
        out << "api: {\n";
        out.indent();
        out << "name: \"" << method->name() << "\"\n";
        // Generate declaration for each return value.
        for (const auto &result : method->results()) {
            out << "return_type_hidl: {\n";
            out.indent();
            status_t status = result->type().emitVtsAttributeType(out);
            if (status != OK) {
                return status;
            }
            out.unindent();
            out << "}\n";
        }
        // Generate declaration for each input argument
        for (const auto &arg : method->args()) {
            out << "arg: {\n";
            out.indent();
            status_t status = arg->type().emitVtsAttributeType(out);
            if (status != OK) {
                return status;
            }
            out.unindent();
            out << "}\n";
        }
        // Generate declaration for each annotation.
        const AnnotationVector & annotations = method->annotations();
        for (size_t i = 0; i < annotations.size(); i++) {
            out << "callflow: {\n";
            out.indent();
            std::string name = annotations.keyAt(i);
            if (name == "entry") {
                out << "entry: true\n";
            } else if (name == "exit") {
                out << "exit: true\n";
            } else if (name == "callflow") {
                Annotation* annotation = annotations.valueAt(i);
                std::vector<std::string> * values = annotation->params()
                        .valueFor("next");
                for (auto value : *values) {
                    out << "next: " << value << "\n";
                }
            } else {
                std::cerr << "Invalid annotation '"
                          << name << "' for method: " << method->name()
                          << ". Should be one of: entry, exit, callflow. \n";
                return UNKNOWN_ERROR;
            }
            out.unindent();
            out << "}\n";
        }
        out.unindent();
        out << "}\n\n";
    }
    return OK;
}

status_t Interface::emitVtsAttributeType(Formatter &out) const {
    out << "type: TYPE_HIDL_CALLBACK\n"
        << "predefined_type: \""
        << localName()
        << "\"\n";
    return OK;
}

bool Interface::isJavaCompatible() const {
    if (mIsJavaCompatibleInProgress) {
        // We're currently trying to determine if this Interface is
        // java-compatible and something is referencing this interface through
        // one of its methods. Assume we'll ultimately succeed, if we were wrong
        // the original invocation of Interface::isJavaCompatible() will then
        // return the correct "false" result.
        return true;
    }

    mIsJavaCompatibleInProgress = true;

    if (!Scope::isJavaCompatible()) {
        mIsJavaCompatibleInProgress = false;
        return false;
    }

    for (const auto &method : mMethods) {
        if (!method->isJavaCompatible()) {
            mIsJavaCompatibleInProgress = false;
            return false;
        }
    }

    mIsJavaCompatibleInProgress = false;

    return true;
}

}  // namespace android

