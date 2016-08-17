LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := hidl-gen
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_IS_HOST_MODULE := true

LOCAL_SRC_FILES :=              \
    Annotation.cpp              \
    ArrayType.cpp               \
    CompoundType.cpp            \
    ConstantExpression.cpp      \
    Coordinator.cpp             \
    EnumType.cpp                \
    Formatter.cpp               \
    FQName.cpp                  \
    generateCpp.cpp             \
    generateJava.cpp            \
    generateVts.cpp             \
    GenericBinder.cpp           \
    HandleType.cpp              \
    hidl-gen_y.yy               \
    hidl-gen_l.ll               \
    Interface.cpp               \
    Method.cpp                  \
    NamedType.cpp               \
    ScalarType.cpp              \
    Scope.cpp                   \
    StringType.cpp              \
    Type.cpp                    \
    TypeDef.cpp                 \
    VectorType.cpp              \
    AST.cpp                     \
    main.cpp                    \

LOCAL_SHARED_LIBRARIES :=       \
    libbase                     \
    liblog                      \

LOCAL_STATIC_LIBRARIES :=       \
    libutils                    \

LOCAL_CFLAGS := -O0 -g -Wno-deprecated-register

include $(BUILD_HOST_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := hidl_gen_test
LOCAL_MODULE_CLASS := FAKE
LOCAL_IS_HOST_MODULE := true

include $(BUILD_SYSTEM)/base_rules.mk

the_py_script := $(LOCAL_PATH)/test/test_output.py
$(LOCAL_BUILT_MODULE): PRIVATE_PY_SCRIPT := $(the_py_script)
$(LOCAL_BUILT_MODULE): PRIVATE_OUT_DIR := $(LOCAL_PATH)/test_out
$(LOCAL_BUILT_MODULE): PRIVATE_CANONICAL_DIR := $(LOCAL_PATH)/test/data
$(LOCAL_BUILT_MODULE): PRIVATE_HIDL_EXEC := $(HOST_OUT_EXECUTABLES)/hidl-gen
$(LOCAL_BUILT_MODULE): PRIVATE_PACKAGE_ROOT := android.hardware:$(TOP)/hardware/interfaces
$(LOCAL_BUILT_MODULE): $(the_py_script) $(HOST_OUT_EXECUTABLES)/hidl-gen
	@echo "host Test: $(PRIVATE_MODULE)"
	$(hide) python $(PRIVATE_PY_SCRIPT) $(PRIVATE_HIDL_EXEC) $(PRIVATE_CANONICAL_DIR) $(PRIVATE_OUT_DIR) $(PRIVATE_PACKAGE_ROOT)

include $(call all-makefiles-under,$(LOCAL_PATH))
