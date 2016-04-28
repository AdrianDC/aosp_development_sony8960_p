
ruby snippets.rb
if [ $? -eq 0 ]
then
mm -j 44
if [ $? -eq 0 ]
then
~/android/master/out/host/linux-x86/bin/hidl-cpp -On ITest.hidl BnTest.h
~/android/master/out/host/linux-x86/bin/hidl-cpp -Op ITest.hidl BpTest.h
~/android/master/out/host/linux-x86/bin/hidl-cpp -Os ITest.hidl TestStubs.cpp
~/android/master/out/host/linux-x86/bin/hidl-cpp -Ox ITest.hidl TestProxy.cpp
~/android/master/out/host/linux-x86/bin/hidl-cpp -Oi ITest.hidl ITest.h
fi
fi
