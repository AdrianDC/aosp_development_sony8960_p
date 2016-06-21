#
# Copyright (C) 2015 The Android Open Source Project
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

hidl_cflags := -Wall -Wextra -Werror
hidl_static_libraries := libbase libcutils

hidl_module_host_os := darwin linux windows
ifdef BRILLO
  hidl_module_host_os := darwin linux
endif

# This tool is prebuilt if we're doing an app-only build.
ifeq ($(TARGET_BUILD_APPS)$(filter true,$(TARGET_BUILD_PDK)),)

# Logic shared between hidl and its unittests
include $(CLEAR_VARS)
LOCAL_MODULE := libhidl-common
LOCAL_MODULE_HOST_OS := $(hidl_module_host_os)

LOCAL_C_INCLUDES := external/gtest/include
LOCAL_CLANG_CFLAGS := $(hidl_cflags)
# Tragically, the code is riddled with unused parameters.
LOCAL_CLANG_CFLAGS += -Wno-unused-parameter
# yacc dumps a lot of code *just in case*.
LOCAL_CLANG_CFLAGS += -Wno-unused-function
LOCAL_CLANG_CFLAGS += -Wno-unneeded-internal-declaration
# yacc is a tool from a more civilized age.
LOCAL_CLANG_CFLAGS += -Wno-deprecated-register
# yacc also has a habit of using char* over const char*.
LOCAL_CLANG_CFLAGS += -Wno-writable-strings
LOCAL_STATIC_LIBRARIES := $(hidl_static_libraries)

LOCAL_SRC_FILES := \
    code_writer.cpp \
    generate.cpp \
    ast.cpp \
    hidl_language_l.ll \
    hidl_language_y.yy \
    io_delegate.cpp \
    line_reader.cpp \
    options.cpp \

include $(BUILD_HOST_STATIC_LIBRARY)


# hidl-gen executable
include $(CLEAR_VARS)
LOCAL_MODULE := hidl-gen
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_IS_HOST_MODULE := true

intermediates:= $(local-generated-sources-dir)
private_input_files := \
	$(LOCAL_PATH)/templates/BnTemplate.h \
	$(LOCAL_PATH)/templates/BpTemplate.h \
	$(LOCAL_PATH)/templates/ITemplate.h \
	$(LOCAL_PATH)/templates/Template.vts \
	$(LOCAL_PATH)/templates/Template.json \
	$(LOCAL_PATH)/templates/TemplateAll.cpp \
	$(LOCAL_PATH)/templates/TemplateBinder.h \
	$(LOCAL_PATH)/templates/TemplateProxy.cpp \
	$(LOCAL_PATH)/templates/TemplateStubs.cpp \

the_py_script := $(LOCAL_PATH)/make_snippets.py
GEN := $(intermediates)/snippets.cpp
$(GEN): PRIVATE_INPUT_FILES := $(private_input_files)
$(GEN): PRIVATE_PY_SCRIPT := $(the_py_script)
$(GEN): PRIVATE_CUSTOM_TOOL = python $(PRIVATE_PY_SCRIPT) $@ $(PRIVATE_INPUT_FILES)
$(GEN): $(private_input_files) $(the_py_script)
	$(transform-generated-source)
LOCAL_GENERATED_SOURCES += $(GEN)


LOCAL_MODULE_HOST_OS := $(hidl_module_host_os)
LOCAL_CFLAGS := $(hidl_cflags)
LOCAL_C_INCLUDES := external/gtest/include 
LOCAL_SRC_FILES := main_hidl.cpp

LOCAL_STATIC_LIBRARIES := libhidl-common $(hidl_static_libraries)
include $(BUILD_HOST_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := hidl_test
LOCAL_MODULE_CLASS := FAKE
LOCAL_IS_HOST_MODULE := true

include $(BUILD_SYSTEM)/base_rules.mk

the_py_script := $(LOCAL_PATH)/tests/testAll.py
$(LOCAL_BUILT_MODULE): PRIVATE_PY_SCRIPT := $(the_py_script)
$(LOCAL_BUILT_MODULE): PRIVATE_OUT_DIR := $(intermediates)/test_out
$(LOCAL_BUILT_MODULE): PRIVATE_TEST_DIR := $(LOCAL_PATH)/tests
$(LOCAL_BUILT_MODULE): PRIVATE_HIDL_EXEC := $(HOST_OUT_EXECUTABLES)/hidl-gen
$(LOCAL_BUILT_MODULE): $(the_py_script) $(HOST_OUT_EXECUTABLES)/hidl-gen
	@echo "host Test: $(PRIVATE_MODULE)"
	@mkdir -p $(dir $@)
	@mkdir -p $(PRIVATE_OUT_DIR)
	$(hide) python $(PRIVATE_PY_SCRIPT) $(PRIVATE_HIDL_EXEC) $(PRIVATE_TEST_DIR) $(PRIVATE_OUT_DIR)
	$(hide) touch $@


# Unit tests
include $(CLEAR_VARS)
LOCAL_MODULE := hidl_unittests
LOCAL_MODULE_HOST_OS := darwin linux

LOCAL_CFLAGS := $(hidl_cflags) -g -DUNIT_TEST
# Tragically, the code is riddled with unused parameters.
LOCAL_CLANG_CFLAGS := -Wno-unused-parameter
LOCAL_SRC_FILES := \
    hidl_unittest.cpp \
    ast_cpp_unittest.cpp \
    ast_java_unittest.cpp \
    generate_cpp_unittest.cpp \
    options_unittest.cpp \
    tests/end_to_end_tests.cpp \
    tests/fake_io_delegate.cpp \
    tests/main.cpp \
    tests/test_data_example_interface.cpp \
    tests/test_data_ping_responder.cpp \
    tests/test_util.cpp \
    type_cpp_unittest.cpp \
    type_java_unittest.cpp \

LOCAL_STATIC_LIBRARIES := \
    libhidl-common \
    libhidl-parse \
    $(hidl_static_libraries) \
    libgmock_host \
    libgtest_host \

LOCAL_LDLIBS_linux := -lrt
#include $(BUILD_HOST_NATIVE_TEST)

endif # No TARGET_BUILD_APPS or TARGET_BUILD_PDK

#
# Everything below here is used for integration testing of generated HIDL code.
#
hidl_integration_test_cflags := $(hidl_cflags) -Wunused-parameter
hidl_integration_test_shared_libs := \
    libbase \
    libbinder \
    liblog \
    libutils

include $(CLEAR_VARS)
LOCAL_MODULE := libhidl-integration-test
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_CFLAGS := $(hidl_integration_test_cflags)
LOCAL_SHARED_LIBRARIES := $(hidl_integration_test_shared_libs)
LOCAL_HIDL_INCLUDES := \
    system/tools/hidl/tests/ \
    frameworks/native/hidl/binder
LOCAL_SRC_FILES := \
    tests/android/hidl/tests/ITestService.hidl \
    tests/android/hidl/tests/INamedCallback.hidl \
    tests/simple_parcelable.cpp
#include $(BUILD_SHARED_LIBRARY)
# TODO re-enable tests

include $(CLEAR_VARS)
LOCAL_MODULE := hidl_test_service
LOCAL_CFLAGS := $(hidl_integration_test_cflags)
LOCAL_SHARED_LIBRARIES := \
    libhidl-integration-test \
    $(hidl_integration_test_shared_libs)
LOCAL_SRC_FILES := \
    tests/hidl_test_service.cpp
#include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := hidl_test_client
LOCAL_CFLAGS := $(hidl_integration_test_cflags)
LOCAL_SHARED_LIBRARIES := \
    libhidl-integration-test \
    $(hidl_integration_test_shared_libs)
LOCAL_SRC_FILES := \
    tests/hidl_test_client.cpp \
    tests/hidl_test_client_file_descriptors.cpp \
    tests/hidl_test_client_parcelables.cpp \
    tests/hidl_test_client_nullables.cpp \
    tests/hidl_test_client_primitives.cpp \
    tests/hidl_test_client_utf8_strings.cpp \
    tests/hidl_test_client_service_exceptions.cpp
#include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := hidl_test_sentinel_searcher
LOCAL_SRC_FILES := tests/hidl_test_sentinel_searcher.cpp
LOCAL_CFLAGS := $(hidl_integration_test_cflags)
#include $(BUILD_EXECUTABLE)


# hidl on its own doesn't need the framework, but testing native/java
# compatibility introduces java dependencies.
ifndef BRILLO

include $(CLEAR_VARS)
LOCAL_PACKAGE_NAME := hidl_test_services
# Turn off Java optimization tools to speed up our test iterations.
LOCAL_PROGUARD_ENABLED := disabled
LOCAL_DEX_PREOPT := false
LOCAL_CERTIFICATE := platform
LOCAL_MANIFEST_FILE := tests/java_app/AndroidManifest.xml
LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/tests/java_app/resources
LOCAL_SRC_FILES := \
    tests/android/hidl/tests/ITestService.hidl \
    tests/android/hidl/tests/INamedCallback.hidl \
    tests/java_app/src/android/hidl/tests/SimpleParcelable.java \
    tests/java_app/src/android/hidl/tests/TestServiceClient.java
LOCAL_HIDL_INCLUDES := \
    system/tools/hidl/tests/ \
    frameworks/native/hidl/binder
#include $(BUILD_PACKAGE)

endif  # not defined BRILLO

include $(call all-makefiles-under,$(LOCAL_PATH))
