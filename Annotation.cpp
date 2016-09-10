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

#include "Annotation.h"

#include "Formatter.h"

#include <vector>

namespace android {

Annotation::Annotation(const char *name,AnnotationParamVector *params)
        : mName(name),
          mParamsByName(params) {
}

std::string Annotation::name() const {
    return mName;
}

const AnnotationParamVector &Annotation::params() const {
    return *mParamsByName;
}

void Annotation::dump(Formatter &out) const {
    out << "@" << mName;

    if (mParamsByName->size() == 0) {
        return;
    }

    out << "(";

    for (size_t i = 0; i < mParamsByName->size(); ++i) {
        if (i > 0) {
            out << ", ";
        }

        out << mParamsByName->keyAt(i) << "=";

        const std::vector<std::string> *param = mParamsByName->valueAt(i);
        if (param->size() > 1) {
            out << "{";
        }

        bool first = true;
        for (const auto &value : *param) {
            if (!first) {
                out << ", ";
            }

            out << value;

            first = false;
        }

        if (param->size() > 1) {
            out << "}";
        }
    }

    out << ")";
}

}  // namespace android

