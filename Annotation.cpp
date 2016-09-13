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

#include <hidl-util/Formatter.h>
#include <vector>

namespace android {


AnnotationParam::AnnotationParam(const std::string &name,
                std::vector<std::string> *values)
: mName(name), mValues(values) {}

const std::string &AnnotationParam::getName() const {
    return mName;
}

const std::vector<std::string> *AnnotationParam::getValues() const {
    return mValues;
}

Annotation::Annotation(const char *name,AnnotationParamVector *params)
        : mName(name),
          mParams(params) {
}

std::string Annotation::name() const {
    return mName;
}

const AnnotationParamVector &Annotation::params() const {
    return *mParams;
}

const AnnotationParam *Annotation::getParam(const std::string &name) {
    for (auto *i: *mParams) {
        if (i->getName() == name) {
            return i;
        }
    }

    return nullptr;
}

void Annotation::dump(Formatter &out) const {
    out << "@" << mName;

    if (mParams->size() == 0) {
        return;
    }

    out << "(";

    for (size_t i = 0; i < mParams->size(); ++i) {
        if (i > 0) {
            out << ", ";
        }

        const AnnotationParam* param = mParams->at(i);

        out << param->getName() << "=";

        const std::vector<std::string> *values = param->getValues();
        if (values->size() > 1) {
            out << "{";
        }

        bool first = true;
        for (const auto &value : *values) {
            if (!first) {
                out << ", ";
            }

            out << value;

            first = false;
        }

        if (values->size() > 1) {
            out << "}";
        }
    }

    out << ")";
}

}  // namespace android

