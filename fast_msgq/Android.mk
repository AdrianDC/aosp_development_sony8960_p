#
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
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := mq_test_service
LOCAL_SRC_FILES := \
    common/MessageQueueDesc.cpp \
    TestMsgQService.cpp \
    ITestMsgQ.hidl

LOCAL_SHARED_LIBRARIES := libhwbinder libbase libcutils libutils
LOCAL_C_INCLUDES := system/libhwbinder/include
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_REQUIRED_MODULES := libgtest
LOCAL_SRC_FILES := common/MessageQueueDesc.cpp \
                   TestMsgQClient.cpp \
                   test_main.cpp \
                   ITestMsgQ.hidl

LOCAL_SHARED_LIBRARIES := libhwbinder libbase libcutils libutils
LOCAL_STATIC_LIBRARIES := libgtest
LOCAL_MODULE := mq_test_client
LOCAL_C_INCLUDES := system/libhwbinder/include
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_REQUIRED_MODULES := libgtest
LOCAL_SRC_FILES := common/MessageQueueDesc.cpp \
                   MQTest.cpp \
                   test_main.cpp
LOCAL_STATIC_LIBRARIES := libutils libcutils liblog libgtest
LOCAL_SHARED_LIBRARIES := libhwbinder
LOCAL_C_INCLUDES := system/libhwbinder/include
LOCAL_MODULE := mq_tests
include $(BUILD_EXECUTABLE)

