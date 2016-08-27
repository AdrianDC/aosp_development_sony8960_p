# hidl-gen user guide

## 1. Build

```
croot
make hidl-gen
```

## 2. Run

```
hidl-gen -o output-path -L language (-r interface-root) fqname

output-path: directory to store the output files.
language: output file for given language. e.g.c++, vts..

fqname: fully qualified name of the input files.
For singe file input, follow the format: package@version::fileName
For directory input, follow the format: package@version

interface-root(optional): prefix and root path for fqname.
If not set, use the default prefix: android.hardware and default root path
defined in $TOP.

examples:

hidl-gen -o output -L c++ android.hardware.nfc@1.0::INfc.hal
hidl-gen -o output -L vts android.hardware.nfc@1.0
hidl-gen -o test -L c++ -r android.hardware:/home/android/master/hardware/interfaces android.hardware.nfc@1.0
```
