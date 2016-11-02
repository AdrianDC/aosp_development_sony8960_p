#!/bin/bash

hidl-gen -Lmakefile -r tests:system/tools/hidl/test/ tests.vendor@1.0;
hidl-gen -Landroidbp -r tests:system/tools/hidl/test/ tests.vendor@1.0;