// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/BpTemplate.h
#ifndef HIDL_GENERATED_android_hardware_nfc_BP_Nfc_H_
#define HIDL_GENERATED_android_hardware_nfc_BP_Nfc_H_

#include <hwbinder/IBinder.h>
#include <hwbinder/IInterface.h>
#include <utils/Errors.h>
//#include <android/hardware/nfc/INfc.h>
#include <INfc.h>

namespace android {
namespace hardware {
namespace nfc {


class BpNfc : public ::android::hardware::BpInterface<INfc> {
public:
explicit BpNfc(const ::android::sp<::android::hardware::IBinder>& _aidl_impl);
virtual ~BpNfc() = default;
::android::hardware::Status open(sp<INfcClientCallback> clientCallback , INfc::open_cb _cb ) override;
::android::hardware::Status write(const INfc::nfc_data_t &data , INfc::write_cb _cb ) override;
::android::hardware::Status core_initialized(const hidl_vec<uint8_t> &data , INfc::core_initialized_cb _cb ) override;
::android::hardware::Status pre_discover(INfc::pre_discover_cb _cb ) override;
::android::hardware::Status close(INfc::close_cb _cb ) override;
::android::hardware::Status control_granted(INfc::control_granted_cb _cb ) override;
::android::hardware::Status power_cycle(INfc::power_cycle_cb _cb ) override;

};  // class BpNfc

}  // namespace nfc
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_android_hardware_nfc_BP_Nfc_H_
