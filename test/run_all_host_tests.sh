#!/bin/bash

$ANDROID_BUILD_TOP/build/soong/soong_ui.bash --make-mode -j \
    hidl_error_test \
    hidl_hash_test \
    hidl_impl_test \
    libhidl-gen-utils_test &&

$ANDROID_BUILD_TOP/out/host/linux-x86/nativetest/libhidl-gen-utils_test/libhidl-gen-utils_test &&
$ANDROID_BUILD_TOP/out/host/linux-x86/nativetest64/libhidl-gen-utils_test/libhidl-gen-utils_test &&
echo SUCCESS
