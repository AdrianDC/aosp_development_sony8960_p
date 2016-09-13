# Copyright (C) 2016 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
LOCAL_PATH:= $(call my-dir)

common_libhidl-gen-utils_src_files :=   \
    Formatter.cpp                       \
    StringHelper.cpp                    \

#
# libhidl-gen-utils for device
#

include $(CLEAR_VARS)
LOCAL_MODULE := libhidl-gen-utils
LOCAL_CFLAGS := -O0 -g
LOCAL_SRC_FILES := $(common_libhidl-gen-utils_src_files)
LOCAL_SHARED_LIBRARIES :=               \
    libbase                             \
    liblog                              \

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include/hidl-util
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include
include $(BUILD_SHARED_LIBRARY)

#
# libhidl-gen-utils for host
#

include $(CLEAR_VARS)
LOCAL_MODULE := libhidl-gen-utils
LOCAL_CFLAGS := -O0 -g
LOCAL_SRC_FILES := $(common_libhidl-gen-utils_src_files)
LOCAL_SHARED_LIBRARIES :=               \
    libbase                             \
    liblog                              \

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include/hidl-util
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include
include $(BUILD_HOST_SHARED_LIBRARY)