../../../../out/host/linux-x86/bin/hidl-gen i_h ITypes.hidl android/hardware/tests/ITypes.h
if [ $? -ne 0 ]; 
then exit 1
else
../../../../out/host/linux-x86/bin/hidl-gen all_cpp ITypes.hidl TypesAll.cpp
../../../../out/host/linux-x86/bin/hidl-gen proxy_cpp ITypes.hidl TypesProxy.cpp
../../../../out/host/linux-x86/bin/hidl-gen stubs_cpp ITypes.hidl TypesStubs.cpp
../../../../out/host/linux-x86/bin/hidl-gen bp_h ITypes.hidl android/hardware/tests/BpTypes.h
../../../../out/host/linux-x86/bin/hidl-gen bn_h ITypes.hidl android/hardware/tests/BnTypes.h

my=$PWD
top=$(cd ../../../..;pwd)
cd $top
. build/envsetup.sh
lunch angler-userdebug
make -j44 hidl-types-service hidl-types-client
adb sync system
adb shell hidl-types-service &
sleep 0.2
adb shell hidl-types-client
sleep 0.2
exit 0
fi
