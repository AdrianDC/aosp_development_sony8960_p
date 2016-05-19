// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/ITemplate.h
#ifndef HIDL_GENERATED_ANDROID_HARDWARE_TESTS_I_Math_H_
#define HIDL_GENERATED_ANDROID_HARDWARE_TESTS_I_Math_H_

#include <hwbinder/IBinder.h>
#include <hwbinder/IInterface.h>
#include <hwbinder/Status.h>
#include <cstdint>
#include <utils/StrongPointer.h>

namespace android {

namespace hardware {

class IMath : public hidl::IInterface {
public:
  DECLARE_META_INTERFACE(Math);

  virtual status_t sayHi( ) = 0;
  virtual status_t doCalc(int32_t i , std::function<void(int32_t sum, int32_t product)> callback = nullptr) = 0;

enum Call {
  SAYHI = hidl::IBinder::FIRST_CALL_TRANSACTION + 0,

DOCALC, 
};
};  // class IMath

}  // namespace hardware

}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_TESTS_I_Math_H_
