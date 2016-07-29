#include "Coordinator.h"

#include "AST.h"
#include "RefType.h"

#include <android-base/logging.h>

extern void parseFile(android::AST *ast, const char *path);

namespace android {

Coordinator::Coordinator() {}
Coordinator::~Coordinator() {}

AST *Coordinator::parse(const char *path) {
    ssize_t index = mCache.indexOfKey(path);
    if (index >= 0) {
        AST *ast = mCache.valueAt(index);

        return ast;
    }

    mCache.add(path, NULL);

    AST *ast = new AST(this);
    parseFile(ast, path);

    mCache.add(path, ast);

    return ast;
}

// static
std::string Coordinator::GetPackagePath(const FQName &fqName) {
    CHECK(!fqName.package().empty());
    CHECK(!fqName.version().empty());
    const char *const kPrefix = "android.hardware.";
    CHECK_EQ(fqName.package().find(kPrefix), 0u);

    const std::string packageSuffix = fqName.package().substr(strlen(kPrefix));

    std::string packagePath =
        "/Volumes/Source/nyc-mr1-dev-lego/hardware/interfaces/";

    size_t startPos = 0;
    size_t dotPos;
    while ((dotPos = packageSuffix.find('.', startPos)) != std::string::npos) {
        packagePath.append(packageSuffix.substr(startPos, dotPos - startPos));
        packagePath.append("/");

        startPos = dotPos + 1;
    }
    CHECK_LT(startPos + 1, packageSuffix.length());
    packagePath.append(packageSuffix.substr(startPos));
    packagePath.append("/");

    CHECK_EQ(fqName.version().find('@'), 0u);
    packagePath.append(fqName.version().substr(1));
    packagePath.append("/");

    return packagePath;
}

Type *Coordinator::lookupType(const FQName &fqName) const {
    // Fully qualified.
    CHECK(!fqName.package().empty());
    CHECK(!fqName.version().empty());
    CHECK(!fqName.name().empty());

    const std::string packagePath = GetPackagePath(fqName);

    std::string topType;
    size_t dotPos = fqName.name().find('.');
    if (dotPos == std::string::npos) {
        topType = fqName.name();
    } else {
        topType = fqName.name().substr(0, dotPos);
    }

    std::string path = packagePath;
    path.append(topType);
    path.append(".hal");

    ssize_t index = mCache.indexOfKey(path);
    if (index >= 0) {
        AST *ast = mCache.valueAt(index);
        Type *type = ast->lookupTypeInternal(fqName.name());

        if (type != NULL) {
            return new RefType(fqName.debugString().c_str(), type);
        }
    }

    path = packagePath;
    path.append("Types.hal");

    index = mCache.indexOfKey(path);
    if (index >= 0) {
        AST *ast = mCache.valueAt(index);
        Type *type = ast->lookupTypeInternal(fqName.name());

        if (type != NULL) {
            return new RefType(fqName.debugString().c_str(), type);
        }
    }

    return NULL;
}

}  // namespace android

