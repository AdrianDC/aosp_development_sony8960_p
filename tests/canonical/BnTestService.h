// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/BpTemplate.h
#ifndef HIDL_GENERATED_android_hardware_tests_BN_TestService_H_
#define HIDL_GENERATED_android_hardware_tests_BN_TestService_H_

#include <hwbinder/IInterface.h>
//#include <android/hardware/tests/ITestService.h>
#include <ITestService.h>

namespace android {
namespace hardware {
namespace tests {


class BnTestService : public ::android::hidl::BnInterface<ITestService> {
public:
  ::android::status_t onTransact(uint32_t _aidl_code, const ::android::hidl::Parcel& _aidl_data, ::android::hidl::Parcel* _aidl_reply, uint32_t _aidl_flags = 0, TransactCallback _callback = nullptr) override;
};  // class BnTestService

}  // namespace tests
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_android_hardware_tests_BN_TestService_H_
