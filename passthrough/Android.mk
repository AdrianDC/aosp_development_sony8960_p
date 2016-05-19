LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := passthrough-hal

#LOCAL_LDFLAGS := -Wl
LOCAL_SRC_FILES:= MathStub.cpp MathImplementation.cpp MathPassthroughEntry.cpp
#LOCAL_CFLAGS := -Wall -Wextra -Wunused -Werror
#LOCAL_CXX_STL := none
LOCAL_C_INCLUDES := system/libhwbinder/include
LOCAL_SHARED_LIBRARIES := libhwbinder libbase libcutils libutils

#LOCAL_SANITIZE := never
#include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE := passthrough-client

LOCAL_SHARED_LIBRARIES += libdl
#LOCAL_LDFLAGS := -Wl
LOCAL_SRC_FILES:= MathClient.cpp MathProxy.cpp
#LOCAL_CFLAGS := -Wall -Wextra -Wunused -Werror
LOCAL_C_INCLUDES := system/libhwbinder/include
LOCAL_SHARED_LIBRARIES := libhwbinder libbase libcutils libutils

#LOCAL_CXX_STL := none
#LOCAL_SANITIZE := never
#include $(BUILD_EXECUTABLE)

