// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/BpTemplate.h
#ifndef HIDL_GENERATED_android_hardware_nfc_BN_Nfc_H_
#define HIDL_GENERATED_android_hardware_nfc_BN_Nfc_H_

#include <hwbinder/IInterface.h>
#include <android/hardware/nfc/INfc.h>

namespace android {
namespace hardware {
namespace nfc {


class BnNfc : public ::android::hidl::BnInterface<INfc> {
public:
  ::android::status_t onTransact(uint32_t _aidl_code, const ::android::hidl::Parcel& _aidl_data, ::android::hidl::Parcel* _aidl_reply, uint32_t _aidl_flags = 0, TransactCallback _callback = nullptr) override;
};  // class BnNfc

}  // namespace nfc
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_android_hardware_nfc_BN_Nfc_H_
