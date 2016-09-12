LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := c2hal
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_IS_HOST_MODULE := true

LOCAL_SRC_FILES :=              \
    AST.cpp                     \
    c2hal_l.ll                  \
    c2hal_y.yy                  \
    CompositeDeclaration.cpp    \
    Declaration.cpp             \
    Define.cpp                  \
    EnumVarDeclaration.cpp      \
    Expression.cpp              \
    FunctionDeclaration.cpp     \
    Include.cpp                 \
    main.cpp                    \
    Note.cpp                    \
    Type.cpp                    \
    TypeDef.cpp                 \
    VarDeclaration.cpp          \

LOCAL_SHARED_LIBRARIES :=       \
    libbase                     \
    liblog                      \
    libhidl-gen-utils           \

LOCAL_STATIC_LIBRARIES :=       \
    libutils                    \

LOCAL_CFLAGS := -O0 -g

include $(BUILD_HOST_EXECUTABLE)
