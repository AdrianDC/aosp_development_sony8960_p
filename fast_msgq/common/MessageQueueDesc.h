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

#ifndef _FMSGQ_DESCRIPTOR_H
#define _FMSGQ_DESCRIPTOR_H

#include <cutils/native_handle.h>
#include <sys/mman.h>
#include <utils/NativeHandle.h>
#include <vector>

namespace android {
namespace hidl {

struct GrantorDescriptor {
  uint32_t flags;
  uint32_t fdIndex;
  uint32_t offset;
  uint32_t extent;
};

enum GrantorType : int { READPTRPOS = 0, WRITEPTRPOS, DATAPTRPOS };

struct MQDescriptor {
  MQDescriptor(const std::vector<GrantorDescriptor>& grantors,
               native_handle_t* nhandle, int32_t flags, size_t size);

  size_t getSize() const;

  size_t getQuantum() const;

  int32_t getFlags() const;
  const sp<NativeHandle> getHandle() const;
  const std::vector<GrantorDescriptor> getGrantors() const;
  void* mapGrantorDescr(uint32_t grantor_idx);
  void unmapGrantorDescr(void* address, uint32_t grantor_idx);

 private:
  MQDescriptor();

  sp<NativeHandle> mHandle = nullptr;
  std::vector<GrantorDescriptor> mGrantors;
  size_t mQuantum = 0;
  int32_t mFlags = 0;
};

inline size_t MQDescriptor::getSize() const {
  return mGrantors[DATAPTRPOS].extent;
}

inline size_t MQDescriptor::getQuantum() const { return mQuantum; }

inline int32_t MQDescriptor::getFlags() const { return mFlags; }

inline const sp<NativeHandle> MQDescriptor::getHandle() const {
  return mHandle;
}

inline const std::vector<GrantorDescriptor> MQDescriptor::getGrantors() const {
  return mGrantors;
}
}  // namespace hidl
}  // namespace android
#endif  // FMSGQ_DESCRIPTOR_H
