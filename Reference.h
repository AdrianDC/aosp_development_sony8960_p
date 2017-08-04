/*
 * Copyright (C) 2017 The Android Open Source Project
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

#ifndef REFERENCE_H_

#define REFERENCE_H_

#include <android-base/logging.h>
#include <hidl-util/FQName.h>

#include "Location.h"

namespace android {

/**
 * Reference placeholder
 */
template <class T>
struct Reference {
    Reference() = default;

    Reference(const FQName& fqName, const Location& location)
        : mResolved(nullptr), mFqName(fqName), mLocation(location) {}

    Reference(T* type, const Location& location) : mResolved(type), mLocation(location) {
        CHECK(type != nullptr);
    }

    Reference(const Reference& ref)
        : mResolved(ref.mResolved), mFqName(ref.mFqName), mLocation(ref.mLocation) {}

    /* Storing type cast, valid only before resolving */
    template <class OtherT>
    Reference(const Reference<OtherT>& ref)
        : mResolved(nullptr), mFqName(ref.mFqName), mLocation(ref.mLocation) {
        CHECK(!ref.isResolved());
    }

    /* Returns true iff referred type is resolved
       Referred type's field might be not resolved */
    bool isResolved() const { return mResolved != nullptr; }

    operator T*() const { return get(); }

    T* operator->() const { return get(); }

    T* get() const {
        CHECK(mResolved != nullptr);
        return mResolved;
    }

    void set(T* resolved) {
        CHECK(!isResolved());
        CHECK(resolved != nullptr);
        mResolved = resolved;
    }

    /* Returns true iff this is reference to null:
       not resolved and has not name for lookup */
    bool isEmptyReference() const { return !isResolved() && !hasLookupFqName(); }

    const FQName& getLookupFqName() const {
        CHECK(hasLookupFqName());
        return mFqName;
    }

    bool hasLocation() const { return mLocation.isValid(); }

    const Location& getLocation() const {
        CHECK(hasLocation());
        return mLocation;
    }

   private:
    /* Referred type */
    T* mResolved = nullptr;
    /* Reference name for lookup */
    FQName mFqName;
    /* Reference location is mainly used for printing errors */
    Location mLocation;

    bool hasLookupFqName() const {
        // Valid only while not resolved to prevent confusion when
        // ref.hasLookupFqName() is false while ref,get()->fqName is valid.
        CHECK(!isResolved());
        return mFqName.isValid();
    }

    template <class OtherT>
    friend struct Reference;
};

}  // namespace android

#endif  // REFERENCE_H_
