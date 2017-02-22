LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

my_jacoco_version := 0.7.2.201409121644

LOCAL_SRC_FILES := \
    $(call all-java-files-under, src/main/java) \

LOCAL_STATIC_JAVA_LIBRARIES := \
        jvm-jacoco-core \
        jvm-jacoco-report \
        jvm-jacoco-asm \
        commons-cli-1.2

LOCAL_MODULE := jvm-jacoco-reporter

include $(BUILD_HOST_JAVA_LIBRARY)

include $(CLEAR_VARS)

LOCAL_IS_HOST_MODULE := true
LOCAL_PREBUILT_STATIC_JAVA_LIBRARIES := \
        jvm-jacoco-core:lib/org.jacoco.core-$(my_jacoco_version).jar \
        jvm-jacoco-report:lib/org.jacoco.report-$(my_jacoco_version).jar \
        jvm-jacoco-asm:lib/asm-debug-all-5.0.1.jar

include $(BUILD_MULTI_PREBUILT)

my_jacoco_version :=
