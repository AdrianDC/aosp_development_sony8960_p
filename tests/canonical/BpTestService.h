// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/BpTemplate.h
#ifndef HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BP_TestService_H_
#define HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BP_TestService_H_

#include <hwbinder/IBinder.h>
#include <hwbinder/IInterface.h>
#include <utils/Errors.h>
#include <android/hardware/tests/ITestService.h>

namespace android {
namespace hardware {
namespace tests {


class BpTestService : public ::android::BpInterface<ITestService> {
public:
explicit BpTestService(const ::android::sp<::android::IBinder>& _aidl_impl);
virtual ~BpTestService() = default;
::android::binder::Status echoInteger(int32_t echo_me , std::function<void(int32_t _aidl_return)> callback = nullptr) override;

};  // class BpTestService

}  // namespace tests
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BP_TestService_H_
