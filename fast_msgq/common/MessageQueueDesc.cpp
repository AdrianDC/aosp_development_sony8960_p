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

#include "MessageQueueDesc.h"

namespace android {
namespace hidl {

MQDescriptor::MQDescriptor(const std::vector<GrantorDescriptor>& grantors,
                           native_handle_t* nhandle, int32_t flags, size_t size)
    : mGrantors(grantors), mQuantum(size), mFlags(flags) {
  mHandle = NativeHandle::create(nhandle, true);
}

/*
 * TODO: Modify design to send access flags over WireMQDescriptor
 * as well and use the same for the mmap call.
 */
void* MQDescriptor::mapGrantorDescr(uint32_t grantor_idx) {
  const native_handle_t* handle = mHandle->handle();
  int fdIndex = mGrantors[grantor_idx].fdIndex;
  /*
   * Offset for mmap must be a multiple of PAGE_SIZE.
   */
  int mapOffset = (mGrantors[grantor_idx].offset / PAGE_SIZE) * PAGE_SIZE;
  int mapLength =
      mGrantors[grantor_idx].offset - mapOffset + mGrantors[grantor_idx].extent;
  void* address = mmap(0, mapLength, PROT_READ | PROT_WRITE, MAP_SHARED,
                       handle->data[fdIndex], mapOffset);
  return (address == MAP_FAILED)
             ? nullptr
             : (uint8_t*)address + (mGrantors[grantor_idx].offset - mapOffset);
}

void MQDescriptor::unmapGrantorDescr(void* address, uint32_t grantor_idx) {
  const native_handle_t* handle = mHandle->handle();
  int mapOffset = (mGrantors[grantor_idx].offset / PAGE_SIZE) * PAGE_SIZE;
  int mapLength =
      mGrantors[grantor_idx].offset - mapOffset + mGrantors[grantor_idx].extent;
  void* baseAddress =
      (uint8_t*)address - (mGrantors[grantor_idx].offset - mapOffset);
  if (baseAddress) munmap(baseAddress, mapLength);
}
}  // namespace hidl
}  // namespace android
