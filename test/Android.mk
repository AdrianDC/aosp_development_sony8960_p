LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := nuhidl_test
LOCAL_MODULE_CLASS := EXECUTABLES

LOCAL_SRC_FILES :=                                  \
    main.cpp                                        \

intermediates:= $(local-generated-sources-dir)

GEN := \
    $(intermediates)/android/hardware/foo/1.0/FooAll.cpp                \
    $(intermediates)/android/hardware/foo/1.0/FooCallbackAll.cpp        \
    $(intermediates)/android/hardware/bar/1.0/BarAll.cpp                \

$(GEN): PRIVATE_OUTPUT_DIR := $(intermediates)
$(GEN): PRIVATE_CUSTOM_TOOL = nuhidl-gen -o $(PRIVATE_OUTPUT_DIR) android.hardware.foo@1.0::IFoo android.hardware.bar@1.0::IBar
$(GEN):
	$(transform-generated-source)

LOCAL_GENERATED_SOURCES += $(GEN)

LOCAL_SHARED_LIBRARIES :=       \
    libandroid_runtime          \
    libbase                     \
    libcutils                   \
    libhwbinder                 \
    libnativehelper             \
    libutils                    \

LOCAL_MODULE_TAGS := samples

LOCAL_CFLAGS := -O0 -g

include $(BUILD_EXECUTABLE)
