// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/BpTemplate.h
#ifndef HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BP_Math_H_
#define HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BP_Math_H_

#include <hwbinder/IInterface.h>
#include <utils/Errors.h>
#include <IMath.h>

namespace android {

namespace hardware {

class PpMath : public IMath {
public:
  PpMath() = default;
  virtual ~PpMath() = default;
  virtual hidl::IBinder* onAsBinder() {return nullptr;}
  virtual status_t sayHi( );
  virtual status_t doCalc(int32_t i , std::function<void(int32_t sum, int32_t product)> callback = nullptr);

};  // class PpMath

}  // namespace hardware

}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BP_Math_H_
