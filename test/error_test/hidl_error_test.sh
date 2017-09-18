#!/bin/bash

if [ $# -ne 1 ]; then
    echo "hidl_errr_test.sh hidl-gen_path"
    exit 1
fi

readonly HIDL_GEN_PATH=$1
readonly HIDL_ERROR_TEST_DIR='system/tools/hidl/test/error_test'

for dir in $(ls -d $HIDL_ERROR_TEST_DIR/*/); do
  package=$(basename $dir)
  $HIDL_GEN_PATH -L check -r test:$HIDL_ERROR_TEST_DIR test.$package@1.0 >/dev/null 2>&1
  if [ $? -ne 1 ]; then
    printf "error: %s test did not fail\n" $package
    exit 1
  fi
done
