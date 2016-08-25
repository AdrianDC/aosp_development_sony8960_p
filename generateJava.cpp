#include "AST.h"

#include "Coordinator.h"
#include "Formatter.h"
#include "Interface.h"
#include "Method.h"
#include "Scope.h"

#include <android-base/logging.h>

namespace android {

static std::string upcase(const std::string in) {
    std::string out{in};

    for (auto &ch : out) {
        ch = toupper(ch);
    }

    return out;
}

void AST::emitJavaReaderWriter(
        Formatter &out,
        const std::string &parcelObj,
        const TypedVar *arg,
        bool isReader) const {
    if (isReader) {
        out << arg->type().getJavaType()
            << " "
            << arg->name()
            << " = ";
    }

    arg->type().emitJavaReaderWriter(out, parcelObj, arg->name(), isReader);
}

status_t AST::generateJavaTypes(const std::string &outputPath) const {
    // Splits types.hal up into one java file per declared type.

    for (size_t i = 0; i < mRootScope->countTypes(); ++i) {
        std::string typeName;
        const Type *type = mRootScope->typeAt(i, &typeName);

        if (type->isTypeDef()) {
            continue;
        }

        std::string path = outputPath;
        path.append(mCoordinator->convertPackageRootToPath(mPackage));
        path.append(mCoordinator->getPackagePath(mPackage, true /* relative */));
        path.append(typeName);
        path.append(".java");

        CHECK(Coordinator::MakeParentHierarchy(path));
        FILE *file = fopen(path.c_str(), "w");

        if (file == NULL) {
            return -errno;
        }

        Formatter out(file);

        std::vector<std::string> packageComponents;
        getPackageAndVersionComponents(
                &packageComponents, true /* cpp_compatible */);

        out << "package " << mPackage.javaPackage() << ";\n\n";

        out << "import android.os.HwBlob;\n";
        out << "import android.os.HwParcel;\n\n";

        out << "import java.util.Vector;\n\n";

        status_t err =
            type->emitJavaTypeDeclarations(out, true /* atTopLevel */);

        if (err != OK) {
            return err;
        }
    }

    return OK;
}

status_t AST::generateJava(const std::string &outputPath) const {
    std::string ifaceName;
    if (!AST::isInterface(&ifaceName)) {
        return generateJavaTypes(outputPath);
    }

    const Interface *iface = mRootScope->getInterface();
    if (!iface->isJavaCompatible()) {
        fprintf(stderr,
                "ERROR: This interface is not Java compatible. The Java backend"
                " does NOT support union types or native handles.\n");

        return UNKNOWN_ERROR;
    }

    // cut off the leading 'I'.
    const std::string baseName = ifaceName.substr(1);

    std::string path = outputPath;
    path.append(mCoordinator->convertPackageRootToPath(mPackage));
    path.append(mCoordinator->getPackagePath(mPackage, true /* relative */));
    path.append(ifaceName);
    path.append(".java");

    CHECK(Coordinator::MakeParentHierarchy(path));
    FILE *file = fopen(path.c_str(), "w");

    if (file == NULL) {
        return -errno;
    }

    Formatter out(file);

    std::vector<std::string> packageComponents;
    getPackageAndVersionComponents(
            &packageComponents, true /* cpp_compatible */);

    out << "package " << mPackage.javaPackage() << ";\n\n";

    out << "import android.os.IHwBinder;\n";
    out << "import android.os.IHwInterface;\n";
    out << "import android.os.HwBinder;\n";
    out << "import android.os.HwBlob;\n";
    out << "import android.os.HwParcel;\n\n";
    out << "import java.util.Vector;\n\n";

    for (const auto &item : mImportedNamesForJava) {
        out << "import " << item.javaName() << ";\n";
    }

    if (!mImportedNamesForJava.empty()) {
        out << "\n";
    }

    out.setNamespace(mPackage.javaPackage() + ".");

    const Interface *superType = iface->superType();

    out << "public interface " << ifaceName << " extends ";

    if (superType != NULL) {
        out << superType->fullJavaName();
    } else {
        out << "IHwInterface";
    }

    out << " {\n";
    out.indent();

    out << "public static final String kInterfaceName = \""
        << mPackage.string()
        << "::"
        << ifaceName
        << "\";\n\n";

    out << "public static "
        << ifaceName
        << " asInterface(IHwBinder binder) {\n";

    out.indent();

    out << "if (binder == null) {\n";
    out.indent();
    out << "return null;\n";
    out.unindent();
    out << "}\n\n";

    out << "IHwInterface iface =\n";
    out.indent();
    out.indent();
    out << "binder.queryLocalInterface(kInterfaceName);\n\n";
    out.unindent();
    out.unindent();

    out << "if ((iface != null) && (iface instanceof "
        << ifaceName
        << ")) {\n";

    out.indent();
    out << "return (" << ifaceName << ")iface;\n";
    out.unindent();
    out << "}\n\n";

    out << "return new " << ifaceName << ".Proxy(binder);\n";

    out.unindent();
    out << "}\n\n";

    out << "public IHwBinder asBinder();\n\n";

    status_t err = emitJavaTypeDeclarations(out);

    if (err != OK) {
        return err;
    }

    const std::string base = (superType != NULL)
        ? (superType->fullJavaName() + ".kOpEnd")
        : "IHwBinder.FIRST_CALL_TRANSACTION";

    bool first = true;
    size_t index = 0;
    for (const auto &method : iface->methods()) {
        out << "public static final int kOp_"
            << upcase(method->name())
            << " = "
            << base;

        if (!first) {
            out << " + " << index;
        }

        out << ";\n";

        ++index;
        first = false;
    }

    out << "public static final int kOpEnd = "
        << base
        << " + "
        << index
        << ";";

    out << "\n\n";

    for (const auto &method : iface->methods()) {
        const bool returnsValue = !method->results().empty();
        const bool needsCallback = method->results().size() > 1;

        if (needsCallback) {
            out << "\npublic abstract class "
                << method->name()
                << "Callback {\n";

            out.indent();

            out << "public abstract void onValues("
                << Method::GetJavaSignature(method->results())
                << ");\n";

            out.unindent();
            out << "}\n\n";
        }

        if (returnsValue && !needsCallback) {
            out << method->results()[0]->type().getJavaType();
        } else {
            out << "void";
        }

        out << " "
            << method->name()
            << "("
            << Method::GetJavaSignature(method->args());

        if (needsCallback) {
            if (!method->args().empty()) {
                out << ", ";
            }

            out << method->name()
                << "Callback cb";
        }

        out << ");\n";
    }

    out << "\npublic static final class Proxy implements "
        << ifaceName
        << " {\n";

    out.indent();

    out << "private IHwBinder mRemote;\n\n";
    out << "public Proxy(IHwBinder remote) {\n";
    out.indent();
    out << "mRemote = remote;\n";
    out.unindent();
    out << "}\n\n";

    out << "public IHwBinder asBinder() {\n";
    out.indent();
    out << "return mRemote;\n";
    out.unindent();
    out << "}\n\n";

    std::vector<const Interface *> chain;
    while (iface != NULL) {
        chain.push_back(iface);
        iface = iface->superType();
    }

    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
        const Interface *superInterface = *it;

        out << "// Methods from "
            << superInterface->fullName()
            << " follow.\n";

        for (const auto &method : superInterface->methods()) {
            const bool returnsValue = !method->results().empty();
            const bool needsCallback = method->results().size() > 1;

            out << "public ";
            if (returnsValue && !needsCallback) {
                out << method->results()[0]->type().getJavaType();
            } else {
                out << "void";
            }

            out << " "
                << method->name()
                << "("
                << Method::GetJavaSignature(method->args());

            if (needsCallback) {
                if (!method->args().empty()) {
                    out << ", ";
                }

                out << method->name()
                    << "Callback cb";
            }

            out << ") {\n";
            out.indent();

            out << "HwParcel request = new HwParcel();\n";
            out << "request.writeInterfaceToken("
                << superInterface->fullJavaName()
                << ".kInterfaceName);\n";

            for (const auto &arg : method->args()) {
                emitJavaReaderWriter(
                        out,
                        "request",
                        arg,
                        false /* isReader */);
            }

            out << "\nHwParcel reply = new HwParcel();\n"
                << "mRemote.transact(kOp_"
                << upcase(method->name())
                << ", request, reply, ";

            if (method->isOneway()) {
                out << "IHwBinder.FLAG_ONEWAY";
            } else {
                out << "0 /* flags */";
            }

            out << ");\n";

            if (!method->isOneway()) {
                out << "reply.verifySuccess();\n";
            } else {
                CHECK(!returnsValue);
            }

            out << "request.releaseTemporaryStorage();\n";

            if (returnsValue) {
                out << "\n";

                for (const auto &arg : method->results()) {
                    emitJavaReaderWriter(
                            out,
                            "reply",
                            arg,
                            true /* isReader */);
                }

                if (needsCallback) {
                    out << "cb.onValues(";

                    bool firstField = true;
                    for (const auto &arg : method->results()) {
                        if (!firstField) {
                            out << ", ";
                        }

                        out << arg->name();
                        firstField = false;
                    }

                    out << ");\n";
                } else {
                    const std::string returnName = method->results()[0]->name();
                    out << "return " << returnName << ";\n";
                }
            }

            out.unindent();
            out << "}\n\n";
        }
    }

    out.unindent();
    out << "}\n";

    ////////////////////////////////////////////////////////////////////////////

    out << "\npublic static abstract class Stub extends HwBinder "
        << "implements "
        << ifaceName << " {\n";

    out.indent();

    out << "public IHwBinder asBinder() {\n";
    out.indent();
    out << "return this;\n";
    out.unindent();
    out << "}\n\n";

    out << "public IHwInterface queryLocalInterface(String descriptor) {\n";
    out.indent();
    // XXX what about potential superClasses?
    out << "if (kInterfaceName.equals(descriptor)) {\n";
    out.indent();
    out << "return this;\n";
    out.unindent();
    out << "}\n";
    out << "return null;\n";
    out.unindent();
    out << "}\n\n";

    out << "public void onTransact("
        << "int code, HwParcel request, final HwParcel reply, "
        << "int flags) {\n";

    out.indent();

    out << "switch (code) {\n";

    out.indent();

    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
        const Interface *superInterface = *it;

        for (const auto &method : superInterface->methods()) {
            const bool returnsValue = !method->results().empty();
            const bool needsCallback = method->results().size() > 1;

            out << "case "
                << superInterface->fullJavaName()
                << ".kOp_"
                <<
                upcase(method->name())
                << ":\n{\n";

            out.indent();

            out << "request.enforceInterface("
                << superInterface->fullJavaName()
                << ".kInterfaceName);\n\n";

            for (const auto &arg : method->args()) {
                emitJavaReaderWriter(
                        out,
                        "request",
                        arg,
                        true /* isReader */);
            }

            if (!needsCallback && returnsValue) {
                const TypedVar *returnArg = method->results()[0];

                out << returnArg->type().getJavaType()
                    << " "
                    << returnArg->name()
                    << " = ";
            }

            out << method->name()
                << "(";

            bool firstField = true;
            for (const auto &arg : method->args()) {
                if (!firstField) {
                    out << ", ";
                }

                out << arg->name();

                firstField = false;
            }

            if (needsCallback) {
                if (!firstField) {
                    out << ", ";
                }

                out << "new " << method->name() << "Callback() {\n";
                out.indent();

                out << "@Override\n"
                    << "public void onValues("
                    << Method::GetJavaSignature(method->results())
                    << ") {\n";

                out.indent();
                out << "reply.writeStatus(HwParcel.STATUS_SUCCESS);\n";

                for (const auto &arg : method->results()) {
                    emitJavaReaderWriter(
                            out,
                            "reply",
                            arg,
                            false /* isReader */);
                }

                out << "reply.send();\n"
                          << "}}";

                out.unindent();
                out.unindent();
            }

            out << ");\n";

            if (!needsCallback) {
                out << "reply.writeStatus(HwParcel.STATUS_SUCCESS);\n";

                if (returnsValue) {
                    const TypedVar *returnArg = method->results()[0];

                    emitJavaReaderWriter(
                            out,
                            "reply",
                            returnArg,
                            false /* isReader */);
                }

                out << "reply.send();\n";
            }

            out << "break;\n";
            out.unindent();
            out << "}\n\n";
        }
    }

    out.unindent();
    out << "}\n";

    out.unindent();
    out << "}\n";

    out.unindent();
    out << "}\n";

    out.unindent();
    out << "}\n";

    return OK;
}

status_t AST::emitJavaTypeDeclarations(Formatter &out) const {
    return mRootScope->emitJavaTypeDeclarations(out, false /* atTopLevel */);
}

}  // namespace android

