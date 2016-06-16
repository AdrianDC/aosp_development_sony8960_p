// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/BpTemplate.h
#ifndef HIDL_GENERATED_android_hardware_nfc_BN_NfcClientCallback_H_
#define HIDL_GENERATED_android_hardware_nfc_BN_NfcClientCallback_H_

#include <hwbinder/IInterface.h>
#include <android/hardware/nfc/INfcClientCallback.h>

namespace android {
namespace hardware {
namespace nfc {


class BnNfcClientCallback : public ::android::hidl::BnInterface<INfcClientCallback> {
public:
  ::android::status_t onTransact(uint32_t _aidl_code, const ::android::hidl::Parcel& _aidl_data, ::android::hidl::Parcel* _aidl_reply, uint32_t _aidl_flags = 0, TransactCallback _callback = nullptr) override;
};  // class BnNfcClientCallback

}  // namespace nfc
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_android_hardware_nfc_BN_NfcClientCallback_H_
