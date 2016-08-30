LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := hidl_test
LOCAL_MODULE_CLASS := EXECUTABLES

LOCAL_SRC_FILES :=                                  \
    main.cpp                                        \

LOCAL_C_INCLUDES := \
    external/gtest/include

LOCAL_SHARED_LIBRARIES :=       	\
    libandroid_runtime          	\
    libbase                     	\
    libcutils                   	\
    libhidl                      	\
    libhwbinder                     	\
    libnativehelper             	\
    libutils                    	\
    android.hardware.tests.foo@1.0	\
    android.hardware.tests.bar@1.0	\

LOCAL_STATIC_LIBRARIES := \
    libgtest

LOCAL_MODULE_TAGS := samples

LOCAL_CFLAGS := -O0 -g

include $(BUILD_EXECUTABLE)

# build for hidl_gen_test which verify hidl_gen generates expected output codes.
include $(CLEAR_VARS)
LOCAL_MODULE := hidl_gen_test
LOCAL_MODULE_CLASS := FAKE
LOCAL_IS_HOST_MODULE := true

include $(BUILD_SYSTEM)/base_rules.mk

the_py_script := $(LOCAL_PATH)/test_output.py
$(LOCAL_BUILT_MODULE): PRIVATE_PY_SCRIPT := $(the_py_script)
$(LOCAL_BUILT_MODULE): PRIVATE_OUT_DIR := $(LOCAL_PATH)/test_out
$(LOCAL_BUILT_MODULE): PRIVATE_CANONICAL_DIR := $(LOCAL_PATH)/data
$(LOCAL_BUILT_MODULE): PRIVATE_HIDL_EXEC := $(HOST_OUT_EXECUTABLES)/hidl-gen
$(LOCAL_BUILT_MODULE): PRIVATE_PACKAGE_ROOT := android.hardware:$(TOP)/hardware/interfaces
$(LOCAL_BUILT_MODULE): $(the_py_script) $(HOST_OUT_EXECUTABLES)/hidl-gen
	@echo "host Test: $(PRIVATE_MODULE)"
	$(hide) python $(PRIVATE_PY_SCRIPT) $(PRIVATE_HIDL_EXEC) $(PRIVATE_CANONICAL_DIR) $(PRIVATE_OUT_DIR) $(PRIVATE_PACKAGE_ROOT)
	$(hide) touch $@

include $(call all-makefiles-under,$(LOCAL_PATH))
