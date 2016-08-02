#include "HandleType.h"

#include "Formatter.h"

namespace android {

HandleType::HandleType() {}

std::string HandleType::getCppType(StorageMode mode, std::string *extra) const {
    extra->clear();

    const std::string base = "::android::sp<::android::NativeHandle>";

    switch (mode) {
        case StorageMode_Stack:
        case StorageMode_Result:
            return base;

        case StorageMode_Argument:
            return "const " + base + "&";
    }
}

void HandleType::emitReaderWriter(
        Formatter &out,
        const std::string &name,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode) const {
    const std::string parcelObjDeref =
        parcelObj + (parcelObjIsPointer ? "->" : ".");

    if (isReader) {
        out << name
            << " = "
            << "::android::NativeHandle::create(\n";

        out.indent();
        out.indent();

        out << parcelObjDeref
            << "readNativeHandle(), true /* ownsHandle */);\n\n";

        out.unindent();
        out.unindent();

        out << "if ("
            << name
            << " == nullptr) {\n";

        out.indent();

        out << "_aidl_err = ::android::UNKNOWN_ERROR;\n";
        handleError2(out, mode);

        out.unindent();
        out << "}\n\n";
    } else {
        out << "_aidl_err = ";
        out << parcelObjDeref
            << "writeNativeHandle("
            << name
            << "->handle());\n";

        handleError(out, mode);
    }
}

void HandleType::emitReaderWriterEmbedded(
        Formatter &out,
        const std::string &name,
        bool nameIsPointer,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode,
        const std::string &parentName,
        const std::string &offsetText) const {
    if (isReader) {
        const std::string ptrName = "_aidl_" + name  + "_ptr";

        out << "const native_handle_t *"
            << ptrName
            << " = "
            << parcelObj
            << (parcelObjIsPointer ? "->" : ".")
            << "readEmbeddedNativeHandle(\n";

        out.indent();
        out.indent();

        out << parentName
            << ",\n"
            << offsetText
            << "/* + offsetof(::android::NativeHandle, mHandle) */"
            << ");\n\n";

        out.unindent();
        out.unindent();

        out << "if ("
            << ptrName
            << " == nullptr) {\n";

        out.indent();
        out << "_aidl_err = ::android::UNKNOWN_ERROR;\n";
        handleError2(out, mode);
        out.unindent();
        out << "}\n\n";
    } else {
        out << "_aidl_err = "
            << parcelObj
            << (parcelObjIsPointer ? "->" : ".")
            << "writeEmbeddedNativeHandle(\n";

        out.indent();
        out.indent();

        const std::string nameDeref =
            nameIsPointer ? ("(*" + name + ")") : name;

        out << nameDeref
            << "->"
            << "handle(),\n"
            << parentName
            << ",\n"
            << offsetText
            << "/* + offsetof(::android::NativeHandle, mHandle) */"
            << ");\n\n";

        out.unindent();
        out.unindent();

        handleError(out, mode);
    }
}

bool HandleType::needsEmbeddedReadWrite() const {
    return true;
}

}  // namespace android

