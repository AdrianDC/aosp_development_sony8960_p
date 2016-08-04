my=$PWD
top=$(cd ../../../..;pwd)
cd $top
. build/envsetup.sh
lunch angler-userdebug
make -j44 hidl-types-service hidl-types-client
if [ $? -ne 0 ];
then exit 1
else
adb sync system
adb shell hidl-types-service &
sleep 0.2
adb shell hidl-types-client
sleep 0.2
exit 0
fi

