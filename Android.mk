LOCAL_PATH := $(call my-dir)

common_libhidl-gen_src_files := \
    Annotation.cpp              \
    ArrayType.cpp               \
    CompoundType.cpp            \
    ConstantExpression.cpp      \
    EnumType.cpp                \
    FQName.cpp                  \
    GenericBinder.cpp           \
    HandleType.cpp              \
    Interface.cpp               \
    Method.cpp                  \
    NamedType.cpp               \
    PredefinedType.cpp          \
    ScalarType.cpp              \
    Scope.cpp                   \
    StringType.cpp              \
    Type.cpp                    \
    TypeDef.cpp                 \
    VectorType.cpp              \

#
# libhidl-gen for device
#

include $(CLEAR_VARS)
LOCAL_MODULE := libhidl-gen
LOCAL_CFLAGS := -O0 -g
LOCAL_SRC_FILES := $(common_libhidl-gen_src_files)
LOCAL_SHARED_LIBRARIES :=       \
    libbase                     \
    liblog                      \
    libhidl-gen-utils           \

include $(BUILD_SHARED_LIBRARY)

#
# libhidl-gen for host
#

include $(CLEAR_VARS)
LOCAL_MODULE := libhidl-gen
LOCAL_CFLAGS := -O0 -g
LOCAL_SRC_FILES := $(common_libhidl-gen_src_files)
LOCAL_SHARED_LIBRARIES :=       \
    libbase                     \
    liblog                      \
    libhidl-gen-utils           \

include $(BUILD_HOST_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := hidl-gen
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_IS_HOST_MODULE := true

LOCAL_SRC_FILES :=              \
    Coordinator.cpp             \
    generateCpp.cpp             \
    generateCppImpl.cpp         \
    generateJava.cpp            \
    generateVts.cpp             \
    hidl-gen_y.yy               \
    hidl-gen_l.ll               \
    AST.cpp                     \
    main.cpp                    \

LOCAL_SHARED_LIBRARIES :=       \
    libbase                     \
    liblog                      \
    libhidl-gen                 \
    libhidl-gen-utils           \

LOCAL_CFLAGS := -O0 -g -Wno-deprecated-register

include $(BUILD_HOST_EXECUTABLE)

include $(call all-makefiles-under,$(LOCAL_PATH))
