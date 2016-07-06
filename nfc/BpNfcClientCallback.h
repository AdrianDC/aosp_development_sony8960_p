// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/BpTemplate.h
#ifndef HIDL_GENERATED_android_hardware_nfc_BP_NfcClientCallback_H_
#define HIDL_GENERATED_android_hardware_nfc_BP_NfcClientCallback_H_

#include <hwbinder/IBinder.h>
#include <hwbinder/IInterface.h>
#include <utils/Errors.h>
#include <android/hardware/nfc/INfcClientCallback.h>

namespace android {
namespace hardware {
namespace nfc {


class BpNfcClientCallback : public ::android::hidl::BpInterface<INfcClientCallback> {
public:
explicit BpNfcClientCallback(const ::android::sp<::android::hidl::IBinder>& _aidl_impl);
virtual ~BpNfcClientCallback() = default;
::android::hidl::binder::Status sendEvent(nfc_event_t event, nfc_status_t event_status) override;
::android::hidl::binder::Status sendData(const INfcClientCallback::nfc_data_t &data ) override;

};  // class BpNfcClientCallback

}  // namespace nfc
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_android_hardware_nfc_BP_NfcClientCallback_H_
