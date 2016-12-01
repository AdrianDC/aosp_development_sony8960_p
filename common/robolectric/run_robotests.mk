# Rules for running robolectric tests.
#
# Uses the following variables:
#
#   LOCAL_JAVA_LIBRARIES
#   LOCAL_STATIC_JAVA_LIBRARIES
#   LOCAL_ROBOTEST_FAILURE_FATAL
#   LOCAL_ROBOTEST_TIMEOUT
#   LOCAL_TEST_PACKAGE
#   ROBOTEST_FAILURE_FATAL
#   ROBOTEST_FILTER
#   ROBOTEST_RUN_INDIVIDUALLY
#
#
# If ROBOTEST_FAILURE_FATAL is set to false then failing tests will not cause a
# build failure. Otherwise failures will be fatal.
#
# If ROBOTEST_FILTER is set to a regex then only tests matching that pattern
# will be run. This currently only works at the class level.
#
# TODO: Switch to a JUnit runner which can support method-level test
# filtering and use that rather than grep to implement ROBOTEST_FILTER.
#
# If ROBOTEST_RUN_INDIVIDUALLY is set to true, each test class will be run by a
# different JVM, preventing any interaction between different tests. This is
# significantly slower than running all tests within the same JVM, but prevents
# unwanted interactions.
#
# Tests classes are found by looking for *Test.java files in
# LOCAL_PATH recursively.

################################################
# General settings, independent of the module. #
################################################

### Used for running tests.

# Where to find Robolectric.
my_robolectric_script_path := $(call my-dir)
my_robolectric_path := $(my_robolectric_script_path)/lib

# Whether or not to ignore the result of running the robotests.
# LOCAL_ROBOTEST_FAILURE_FATAL will take precedence over ROBOTEST_FAILURE_FATAL,
# if present.
ifneq ($(strip $(LOCAL_ROBOTEST_FAILURE_FATAL)),)
    ifeq ($(strip $(LOCAL_ROBOTEST_FAILURE_FATAL)),true)
        my_failure_fatal := true
    else
        my_failure_fatal := false
    endif
else
    ifeq ($(strip $(ROBOTEST_FAILURE_FATAL)),false)
        my_failure_fatal := false
    else
        my_failure_fatal := true
    endif
endif
# The timeout for the command. A value of '0' means no timeout. The default is
# 10 minutes.
ifneq ($(strip $(LOCAL_ROBOTEST_TIMEOUT)),)
    my_timeout := $(LOCAL_ROBOTEST_TIMEOUT)
else
    my_timeout := 600
endif
# Command to filter the list of test classes.
ifeq ($(strip $(ROBOTEST_FILTER)),)
    # If not specified, defaults to including all the tests.
    my_test_filter_command := cat
else
    my_test_filter_command := grep -E "$(ROBOTEST_FILTER)"
endif

##########################
# Used by base_rules.mk. #
##########################

LOCAL_MODULE_CLASS := FAKE
# This is actually a phony target that is never built.
LOCAL_BUILT_MODULE_STEM := test.fake
# Since it is not built, it cannot be installed. But we will define our own
# dist files, depending on which of the specific targets is invoked.
LOCAL_UNINSTALLABLE_MODULE := true

include $(BUILD_SYSTEM)/base_rules.mk


#############################
# Module specific settings. #
#############################

### Used for running tests.

# The list of test classes. Robolectric requires an explicit list of tests to
# run, which is compiled from the Java files ending in "Test" within the
# directory from which this module is invoked.
my_tests := \
    $(sort $(shell find $(LOCAL_PATH) -type f -name '*Test.java' \
        | sed -e 's!.*\(com/google.*Test\)\.java!\1!' \
              -e 's!.*\(com/android.*Test\)\.java!\1!' \
        | sed 's!/!\.!g' \
        | $(my_test_filter_command)))
# The source jars containing the tests.
my_srcs_jars := \
    $(foreach lib,$(LOCAL_JAVA_LIBRARIES), \
        $(call intermediates-dir-for,JAVA_LIBRARIES,$(lib),,COMMON)/javalib.jar) \
    $(foreach lib, \
        $(LOCAL_STATIC_JAVA_LIBRARIES), \
        $(call intermediates-dir-for,JAVA_LIBRARIES,$(lib),,COMMON)/classes.jar) \
    $(foreach lib, \
        $(LOCAL_TEST_PACKAGE), \
        $(call intermediates-dir-for,APPS,$(lib),,COMMON)/classes.jar)
# The jars needed to run the tests.
my_jars := \
    prebuilts/sdk/$(LOCAL_SDK_VERSION)/android.jar \
    $(my_srcs_jars)

# Run tests.
my_target := $(LOCAL_BUILT_MODULE)
include $(my_robolectric_script_path)/robotest-internal.mk

# Clear temporary variables.
my_failure_fatal :=
my_jars :=
my_java_args :=
my_robolectric_path :=
my_robolectric_script_path :=
my_srcs_jars :=
my_target :=
my_target_message :=
my_test_filter_command :=
my_tests :=

# Clear local variables specific to this build.
LOCAL_ROBOTEST_FAILURE_FATAL :=
LOCAL_ROBOTEST_TIMEOUT :=
