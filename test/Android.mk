LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := hidl_test
LOCAL_MODULE_CLASS := EXECUTABLES

LOCAL_SRC_FILES :=                                  \
    main.cpp                                        \

LOCAL_SHARED_LIBRARIES :=       \
    libandroid_runtime          \
    libbase                     \
    libcutils                   \
    libhwbinder                 \
    libnativehelper             \
    libutils                    \
    android.hardware.foo@1.0    \
    android.hardware.bar@1.0    \
    android.hardware.nfc@1.0    \

LOCAL_MODULE_TAGS := samples

LOCAL_CFLAGS := -O0 -g

# include $(BUILD_EXECUTABLE)
