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

#include "Method.h"

#include "Annotation.h"
#include "ScalarType.h"
#include "Type.h"

#include <hidl-util/Formatter.h>

namespace android {

Method::Method(const char *name,
       std::vector<TypedVar *> *args,
       std::vector<TypedVar *> *results,
       bool oneway,
       AnnotationVector *annotations)
    : mName(name),
      mArgs(args),
      mResults(results),
      mOneway(oneway),
      mAnnotationsByName(annotations) {
}

std::string Method::name() const {
    return mName;
}

const std::vector<TypedVar *> &Method::args() const {
    return *mArgs;
}

const std::vector<TypedVar *> &Method::results() const {
    return *mResults;
}

const AnnotationVector &Method::annotations() const {
    return *mAnnotationsByName;
}

void Method::generateCppSignature(Formatter &out,
                                  const std::string &className,
                                  bool specifyNamespaces) const {
    const bool returnsValue = !results().empty();

    const TypedVar *elidedReturn = canElideCallback();

    std::string space = (specifyNamespaces ? "::android::hardware::" : "");

    if (elidedReturn == nullptr) {
        out << space << "Return<void> ";
    } else {
        std::string extra;
        out << space
            << "Return<"
            << elidedReturn->type().getCppResultType(&extra)
            << "> ";
    }

    if (!className.empty()) {
        out << className << "::";
    }

    out << name()
        << "("
        << GetArgSignature(args(), specifyNamespaces);

    if (returnsValue && elidedReturn == nullptr) {
        if (!args().empty()) {
            out << ", ";
        }

        out << name() << "_cb _hidl_cb";
    }

    out << ") ";
}

// static
std::string Method::GetArgSignature(const std::vector<TypedVar *> &args,
                                    bool specifyNamespaces) {
    bool first = true;
    std::string out;
    for (const auto &arg : args) {
        if (!first) {
            out += ", ";
        }

        std::string extra;
        out += arg->type().getCppArgumentType(&extra,
                                              specifyNamespaces);
        out += " ";
        out += arg->name();
        out += extra;

        first = false;
    }

    return out;
}

// static
std::string Method::GetJavaArgSignature(const std::vector<TypedVar *> &args) {
    bool first = true;
    std::string out;
    for (const auto &arg : args) {
        if (!first) {
            out += ", ";
        }

        std::string extra;
        out += arg->type().getJavaType();
        out += " ";
        out += arg->name();
        out += extra;

        first = false;
    }

    return out;
}

void Method::dumpAnnotations(Formatter &out) const {
    if (mAnnotationsByName->size() == 0) {
        return;
    }

    out << "// ";
    for (size_t i = 0; i < mAnnotationsByName->size(); ++i) {
        if (i > 0) {
            out << " ";
        }
        mAnnotationsByName->valueAt(i)->dump(out);
    }
    out << "\n";
}

bool Method::isJavaCompatible() const {
    for (const auto &arg : *mArgs) {
        if (!arg->isJavaCompatible()) {
            return false;
        }
    }

    for (const auto &result : *mResults) {
        if (!result->isJavaCompatible()) {
            return false;
        }
    }

    return true;
}

const TypedVar* Method::canElideCallback() const {
    auto &res = results();

    // Can't elide callback for void or tuple-returning methods
    if (res.size() != 1) {
        return nullptr;
    }

    const TypedVar *typedVar = res.at(0);

    // We only elide callbacks for methods returning a single scalar.
    if (typedVar->type().resolveToScalarType() != nullptr) {
        return typedVar;
    }

    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////

TypedVar::TypedVar(const char *name, Type *type)
    : mName(name),
      mType(type) {
}

std::string TypedVar::name() const {
    return mName;
}

const Type &TypedVar::type() const {
    return *mType;
}

bool TypedVar::isJavaCompatible() const {
    return mType->isJavaCompatible();
}

}  // namespace android

