#ifndef HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BN_TestService_H_
#define HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BN_TestService_H_

#include <hwbinder/IInterface.h>
#include <android/hardware/tests/ITestService.h>

namespace android {

namespace hardware {

namespace tests {

class BnTestService : public ::android::BnInterface<ITestService> {
public:
::android::status_t onTransact(uint32_t _aidl_code, const ::android::Parcel& _aidl_data, ::android::Parcel* _aidl_reply, uint32_t _aidl_flags = 0) override;
};  // class BnTestService

}  // namespace tests

}  // namespace hardware

}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BN_TestService_H_
