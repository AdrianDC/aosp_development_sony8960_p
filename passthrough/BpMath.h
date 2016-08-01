// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/BpTemplate.h
#ifndef HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BP_Math_H_
#define HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BP_Math_H_

#include <hwbinder/IBinder.h>
#include <hwbinder/IInterface.h>
#include <utils/Errors.h>
#include <IMath.h>

namespace android {

namespace hardware {

class BpMath : public hidl::BpInterface<IMath> {
public:
  explicit BpMath(const ::android::sp<::android::hardware::IBinder>& _aidl_impl) : BpInterface(_aidl_impl) {}
  virtual ~BpMath() = default;
  virtual status_t sayHi( );
  virtual status_t doCalc(int32_t i , std::function<void(int32_t sum, int32_t product)> callback = nullptr);

};  // class BpMath

}  // namespace hardware

}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BP_Math_H_
