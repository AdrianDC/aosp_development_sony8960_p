#ifndef HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BP_TEST_SERVICE_H_
#define HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BP_TEST_SERVICE_H_

#include <hwbinder/IBinder.h>
#include <hwbinder/IInterface.h>
#include <utils/Errors.h>
#include <android/hardware/tests/ITestService.h>

namespace android {

namespace hardware {

namespace tests {

class BpTestService : public ::android::hidl::BpInterface<ITestService> {
public:
explicit BpTestService(const ::android::sp<::android::hidl::IBinder>& _aidl_impl);
virtual ~BpTestService() = default;
::android::hidl::binder::Status echoInteger(int32_t echo_me, ITestService::echoInteger_cb _cb) override;
};  // class BpTestService

}  // namespace tests

}  // namespace hardware

}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BP_TEST_SERVICE_H_
