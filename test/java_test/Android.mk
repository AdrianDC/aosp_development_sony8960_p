LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(call all-subdir-java-files)
LOCAL_MODULE := hidl_test_java_lib
LOCAL_MODULE_STEM := hidl_test_java
LOCAL_MODULE_CLASS := JAVA_LIBRARIES

intermediates := $(local-generated-sources-dir)

HIDL := $(HOST_OUT_EXECUTABLES)/hidl-gen$(HOST_EXECUTABLE_SUFFIX)

GEN := $(intermediates)/android/hardware/tests/baz/1.0/IBaz.java              \
       $(intermediates)/android/hardware/tests/baz/1.0/IBazCallback.java      \
       $(intermediates)/android/hardware/tests/baz/1.0/IBase.java             \

$(GEN): $(HIDL)
$(GEN): PRIVATE_HIDL := $(HIDL)
$(GEN): PRIVATE_OUTPUT_DIR := $(intermediates)
$(GEN): PRIVATE_CUSTOM_TOOL = $(PRIVATE_HIDL) -Ljava -randroid.hardware:hardware/interfaces -o $(PRIVATE_OUTPUT_DIR) android.hardware.tests.baz@1.0
$(GEN):
	$(transform-generated-source)

LOCAL_GENERATED_SOURCES += $(GEN)

include $(BUILD_JAVA_LIBRARY)

################################################################################

include $(CLEAR_VARS)
LOCAL_MODULE := hidl_test_java
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := hidl_test_java
LOCAL_REQUIRED_MODULES := hidl_test_java_lib
include $(BUILD_PREBUILT)

################################################################################

include $(CLEAR_VARS)

LOCAL_MODULE := hidl_test_java_native

LOCAL_SRC_FILES := hidl_test_java_native.cpp

LOCAL_SHARED_LIBRARIES :=           \
    libbase                         \
    libhidl                         \
    libhwbinder                     \
    libutils                        \
    android.hardware.tests.baz@1.0  \

LOCAL_STATIC_LIBRARIES := \
    libgtest

include $(BUILD_EXECUTABLE)
