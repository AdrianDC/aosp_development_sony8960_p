// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/ITemplate.h
#ifndef HIDL_GENERATED_android_hardware_tests_I_TestService_H_
#define HIDL_GENERATED_android_hardware_tests_I_TestService_H_

#include <hwbinder/IBinder.h>
#include <hwbinder/IInterface.h>
#include <hwbinder/Status.h>
#include <cstdint>
#include <utils/StrongPointer.h>

namespace android {
namespace hardware {
namespace tests {


template<typename T>
using hidl_ref = int;

class ITestService : public ::android::hidl::IInterface {
public:
template<typename T>
struct hidl_vec {
  T *buffer;
  size_t count;
};
struct hidl_string {
  char *buffer;
  ptrdiff_t length;
};

DECLARE_META_INTERFACE(TestService);
typedef struct {
uint32_t int1;
uint32_t int2;

} simple_t;

typedef struct {
uint8_t buffer[4096] ;

} lots_of_data;


  using echoInteger_cb = std::function<void(int32_t ret)>;
  using shareBufferWithRef_cb = std::function<void(int32_t ret)>;


  virtual ::android::hidl::binder::Status echoInteger(int32_t echo_me, const ITestService::simple_t *my_struct , echoInteger_cb _cb = nullptr ) = 0;
  virtual ::android::hidl::binder::Status shareBufferWithRef(hidl_ref<lots_of_data> buffer, shareBufferWithRef_cb _cb = nullptr ) = 0;

enum Call {
  ECHOINTEGER = ::android::hidl::IBinder::FIRST_CALL_TRANSACTION + 0,

  SHAREBUFFERWITHREF, 
};
};  // class ITestService

}  // namespace tests
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_android_hardware_tests_I_TestService_H_
