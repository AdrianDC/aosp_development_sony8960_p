my=$PWD
top=$(cd ../../../..;pwd)
cd $top
. build/envsetup.sh
lunch angler-userdebug
mmm -j44 $my
$OUT/../../../host/linux-x86/bin/adb push $OUT/system/lib64/passthrough-hal.so /data/local/tmp
$OUT/../../../host/linux-x86/bin/adb push $OUT/system/bin/passthrough-client /data/local/tmp
adb shell "(cd /data/local/tmp && ./passthrough-client)"
