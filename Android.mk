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

include $(call all-makefiles-under,$(LOCAL_PATH))
