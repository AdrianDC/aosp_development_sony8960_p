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

#ifndef ANNOTATION_H_

#define ANNOTATION_H_

#include <android-base/macros.h>

#include <string>
#include <utils/KeyedVector.h>

namespace android {

struct Formatter;
using AnnotationParamVector =
    DefaultKeyedVector<std::string, std::vector<std::string> *>;

struct Annotation {
    Annotation(const char *name, AnnotationParamVector *params);

    std::string name() const;
    const AnnotationParamVector &params() const;

    void dump(Formatter &out) const;

private:
    std::string mName;
    AnnotationParamVector *mParamsByName;

    DISALLOW_COPY_AND_ASSIGN(Annotation);
};

}  // namespace android

#endif  // ANNOTATION_H_
