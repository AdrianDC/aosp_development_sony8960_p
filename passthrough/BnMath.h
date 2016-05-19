// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/BpTemplate.h
#ifndef HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BN_Math_H_
#define HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BN_Math_H_

#include <hwbinder/IInterface.h>
#include <IMath.h>

namespace android {

namespace hardware {

class BnMath : public hidl::BnInterface<IMath> {
public:
  ::android::status_t onTransact(uint32_t _aidl_code, const hidl::Parcel& _aidl_data,
                                 hidl::Parcel* _aidl_reply, uint32_t _aidl_flags = 0) override;
};  // class BnMath

}  // namespace hardware

}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BN_Math_H_
