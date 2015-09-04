# Copyright (C) 2014 The Android Open Source Project
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

LOCAL_PATH := $(call my-dir)

# for Android JUnit runner and Espresso
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_PREBUILT_JAVA_LIBRARIES := \
    android-support-test:android-support-test.jar \
    espresso-core:espresso/espresso-core-2.2-release.jar \
    espresso-contrib:espresso/espresso-contrib-2.2-release.jar \
    espresso-contrib-nodep:espresso/espresso-contrib-2.2-release-no-dep.jar \
    espresso-idling-resource-nodep:espresso/espresso-idling-resource-2.2-release-no-dep.jar \
    espresso-intents:espresso/espresso-intents-2.2-release.jar \
    espresso-intents-nodep:espresso/espresso-intents-2.2-release-no-dep.jar \

include $(BUILD_MULTI_PREBUILT)
