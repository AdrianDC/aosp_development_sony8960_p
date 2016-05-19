// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/ITemplate.h
#ifndef HIDL_GENERATED_ANDROID_HARDWARE_TESTS_I_TestService_H_
#define HIDL_GENERATED_ANDROID_HARDWARE_TESTS_I_TestService_H_

#include <hwbinder/IBinder.h>
#include <hwbinder/IInterface.h>
#include <hwbinder/Status.h>
#include <cstdint>
#include <utils/StrongPointer.h>

namespace android {
namespace hardware {
namespace tests {


class ITestService : public hidl::IInterface {
public:
DECLARE_META_INTERFACE(TestService);
  virtual ::android::hidl::binder::Status echoInteger(int32_t echo_me , std::function<void(int32_t _aidl_return)> callback = nullptr) = 0;

enum Call {
  ECHOINTEGER = hidl::IBinder::FIRST_CALL_TRANSACTION + 0,


};
};  // class ITestService

}  // namespace tests
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_ANDROID_HARDWARE_TESTS_I_TestService_H_
