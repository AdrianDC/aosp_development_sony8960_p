#ifndef HIDL_GENERATED_ANDROID_HARDWARE_TESTS_I_TEST_SERVICE_H_
#define HIDL_GENERATED_ANDROID_HARDWARE_TESTS_I_TEST_SERVICE_H_

#include <hwbinder/IBinder.h>
#include <hwbinder/IInterface.h>
#include <hwbinder/Status.h>
#include <cstdint>
#include <utils/StrongPointer.h>

namespace android {

namespace hardware {

namespace tests {

class ITestService : public ::android::hidl::IInterface {
public:
typedef struct {
    uint8_t buffer[4096];
} lots_of_data;

DECLARE_META_INTERFACE(TestService);
using echoInteger_cb = std::function<void(int32_t)>;
using shareBufferWithRef_cb = std::function<void(int32_t)>;
virtual ::android::hidl::binder::Status echoInteger(int32_t echo_me, echoInteger_cb = nullptr) = 0;
virtual ::android::hidl::binder::Status shareBufferWithRef(int buffer, shareBufferWithRef_cb = nullptr) = 0;
enum Call {
  ECHOINTEGER = ::android::hidl::IBinder::FIRST_CALL_TRANSACTION + 0,
  SHAREBUFFERWITHREF = ::android::hidl::IBinder::FIRST_CALL_TRANSACTION + 1,
};
};  // class ITestService

}  // namespace tests

}  // namespace hardware

}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_TESTS_I_TEST_SERVICE_H_
