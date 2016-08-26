/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "GenericBinder.h"

#include "Formatter.h"

namespace android {

GenericBinder::GenericBinder() {}

bool GenericBinder::isBinder() const {
    return true;
}

std::string GenericBinder::getCppType(
        StorageMode mode, std::string *extra) const {
    extra->clear();
    const std::string base = "::android::sp<::android::hardware::IBinder>";

    switch (mode) {
        case StorageMode_Stack:
        case StorageMode_Result:
            return base;

        case StorageMode_Argument:
            return "const " + base + "&";
    }
}

std::string GenericBinder::getJavaType() const {
    return "IHwBinder";
}

void GenericBinder::emitReaderWriter(
        Formatter &out,
        const std::string &name,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode) const {
    const std::string parcelObjDeref =
        parcelObj + (parcelObjIsPointer ? "->" : ".");

    if (isReader) {
        out << "_hidl_err = ";
        out << parcelObjDeref
            << "readNullableStrongBinder(&"
            << name
            << ");\n";

        handleError(out, mode);
    } else {
        out << "_hidl_err = ";
        out << parcelObjDeref
            << "writeStrongBinder("
            << name
            << ");\n";

        handleError(out, mode);
    }
}

void GenericBinder::emitJavaReaderWriter(
        Formatter &out,
        const std::string &parcelObj,
        const std::string &argName,
        bool isReader) const {
    if (isReader) {
        out << parcelObj
            << ".readStrongBinder());\n";
    } else {
        out << parcelObj
            << ".writeStrongBinder("
            << argName
            << " == null ? null : "
            << argName
            << ");\n";
    }
}

status_t GenericBinder::emitVtsAttributeType(Formatter &) const {
    return UNKNOWN_ERROR;
}

}  // namespace android
