// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/BpTemplate.h
#ifndef HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BP_Nfc_H_
#define HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BP_Nfc_H_

#include <hwbinder/IBinder.h>
#include <hwbinder/IInterface.h>
#include <utils/Errors.h>
#include <android/hardware/tests/INfc.h>

namespace android {
namespace hardware {
namespace nfc {


class BpNfc : public ::android::hidl::BpInterface<INfc> {
public:
explicit BpNfc(const ::android::sp<::android::hidl::IBinder>& _aidl_impl);
virtual ~BpNfc() = default;
::android::hidl::binder::Status open(INfcClientCallback clientCallback, INfc::open_cb _cb ) override;
::android::hidl::binder::Status write(const INfc::nfc_data_t *data , INfc::write_cb _cb ) override;
::android::hidl::binder::Status core_initialized(INfc::core_initialized_cb _cb ) override;
::android::hidl::binder::Status pre_discover(INfc::pre_discover_cb _cb ) override;
::android::hidl::binder::Status close(INfc::close_cb _cb ) override;
::android::hidl::binder::Status control_granted(INfc::control_granted_cb _cb ) override;
::android::hidl::binder::Status power_cycle(INfc::power_cycle_cb _cb ) override;

};  // class BpNfc

}  // namespace nfc
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BP_Nfc_H_
