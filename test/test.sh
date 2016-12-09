#!/bin/bash

# Run hidl-gen against errors/syntax/1.0/IEx1.hal to ensure it detects as many
# syntax errors as possible.

if [ ! -d system/tools/hidl/test/errors/syntax/1.0 ] ; then
  echo "Where is system/tools/hidl/test/errors/syntax/1.0?";
  exit 1;
fi

# TODO(b/33276472)
if [ ! -d system/libhidl/transport ] ; then
  echo "Where is system/libhidl/transport?";
  exit 1;
fi

COMMAND='hidl-gen -Lc++ -rtests:system/tools/hidl/test -randroid.hidl:system/libhidl/transport -o /tmp tests.errors.syntax@1.0'
EXPECTED='system/tools/hidl/test/errors.output'

if [[ "$@" == "-h" ]]
then
    echo "$0 [-h|-u|-n]"
    echo "    (No options)  Run and diff against expected output"
    echo "    -u            Update expected output"
    echo "    -a            Run and show actual output"
    echo "    -h            Show help text"
elif [[ "$@" == "-u" ]]
then
    $COMMAND 2>$EXPECTED;
    echo Updated.
elif [[ "$@" == "-a" ]]
then
    $COMMAND
else
    $COMMAND 2>&1 | diff $EXPECTED -
    if [ $? -eq 0 ]; then
        echo Success.
    fi
fi
