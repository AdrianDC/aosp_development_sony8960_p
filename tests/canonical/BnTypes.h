// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/BpTemplate.h
#ifndef HIDL_GENERATED_android_hardware_tests_BN_Types_H_
#define HIDL_GENERATED_android_hardware_tests_BN_Types_H_

#include <hwbinder/IInterface.h>
#include <android/hardware/tests/ITypes.h>

namespace android {
namespace hardware {
namespace tests {


class BnTypes : public ::android::hidl::BnInterface<ITypes> {
public:
  ::android::status_t onTransact(uint32_t _aidl_code, const ::android::hidl::Parcel& _aidl_data, ::android::hidl::Parcel* _aidl_reply, uint32_t _aidl_flags = 0, TransactCallback _callback = nullptr) override;
};  // class BnTypes

}  // namespace tests
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_android_hardware_tests_BN_Types_H_
