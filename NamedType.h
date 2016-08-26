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

#ifndef NAMED_TYPE_H_

#define NAMED_TYPE_H_

#include "Type.h"

#include "FQName.h"

#include <string>

namespace android {

struct NamedType : public Type {
    NamedType(const char *localName);

    bool isNamedType() const override;

    void setFullName(const FQName &fullName);

    const FQName &fqName() const;
    std::string localName() const;
    std::string fullName() const;
    std::string fullJavaName() const;

private:
    std::string mLocalName;
    FQName mFullName;

    DISALLOW_COPY_AND_ASSIGN(NamedType);
};

}  // namespace android

#endif  // NAMED_TYPE_H_

