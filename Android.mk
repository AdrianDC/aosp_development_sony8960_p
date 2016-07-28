LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := nuhidl-gen
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_IS_HOST_MODULE := true

LOCAL_SRC_FILES :=              \
    ArrayType.cpp               \
    CompoundType.cpp            \
    Constant.cpp                \
    EnumType.cpp                \
    Formatter.cpp               \
    HandleType.cpp              \
    hidl-gen_y.yy               \
    hidl-gen_l.ll               \
    Interface.cpp               \
    Method.cpp                  \
    NamedType.cpp               \
    RefType.cpp                 \
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

include $(BUILD_HOST_EXECUTABLE)
