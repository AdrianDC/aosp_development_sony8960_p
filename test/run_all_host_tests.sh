#!/bin/bash

function run() {
    local FAILED_TESTS=()

    local COMPILE_TIME_TESTS=(\
        hidl_error_test \
        hidl_hash_test \
        hidl_impl_test)

    local RUN_TIME_TESTS=(\
        libhidl-gen-utils_test \
        hidl-gen-host_test \
    )

    $ANDROID_BUILD_TOP/build/soong/soong_ui.bash --make-mode -j \
        ${COMPILE_TIME_TESTS[*]} ${RUN_TIME_TESTS[*]} || return

    local BITNESS=("nativetest" "nativetest64")

    for bits in ${BITNESS[@]}; do
        for test in ${RUN_TIME_TESTS[@]}; do
            echo $bits $test
            $ANDROID_BUILD_TOP/out/host/linux-x86/$bits/$test/$test ||
                FAILED_TESTS+=("$bits:$test")
        done
    done

    echo
    echo ===== SUMMARY =====
    echo
    if [ ${#FAILED_TESTS[@]} -gt 0 ]; then
        for failed in ${FAILED_TESTS[@]}; do
            echo "FAILED TEST: $failed"
        done
    else
        echo "SUCCESS"
    fi
}

run