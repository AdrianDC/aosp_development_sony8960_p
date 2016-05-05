mkdir -p out
../../../out/host/linux-x86/bin/hidl-cpp -On tests/ITest.hidl out/BnTest.h
../../../out/host/linux-x86/bin/hidl-cpp -Op tests/ITest.hidl out/BpTest.h
../../../out/host/linux-x86/bin/hidl-cpp -Os tests/ITest.hidl out/TestStubs.cpp
../../../out/host/linux-x86/bin/hidl-cpp -Ox tests/ITest.hidl out/TestProxy.cpp
../../../out/host/linux-x86/bin/hidl-cpp -Oi tests/ITest.hidl out/ITest.h
../../../out/host/linux-x86/bin/hidl-cpp -Oi tests/vts_gps.hidl out/Igps.h
../../../out/host/linux-x86/bin/hidl-cpp -Ov tests/vts_gps.hidl out/vts_gps_out.vts
../../../out/host/linux-x86/bin/hidl-cpp -Ov -dout/dep.txt tests/vts_lights.hidl out/vts_lights_out.vts
