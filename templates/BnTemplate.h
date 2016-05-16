// SECTION bn_h
// START file
#ifndef HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BN_header_guard_H_
#define HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BN_header_guard_H_

#include <hwbinder/IInterface.h>
#include <android/hardware/tests/Ipackage_name.h>

namespace android {

namespace hardware {

namespace tests {

class Bnpackage_name : public ::android::BnInterface<Ipackage_name> {
public:
::android::status_t onTransact(uint32_t _aidl_code, const ::android::Parcel& _aidl_data, ::android::Parcel* _aidl_reply, uint32_t _aidl_flags = 0) override;
};  // class Bnpackage_name

}  // namespace tests

}  // namespace hardware

}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BN_header_guard_H_
// END file
