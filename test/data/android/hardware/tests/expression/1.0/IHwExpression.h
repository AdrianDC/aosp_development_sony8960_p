#ifndef HIDL_GENERATED_android_hardware_tests_expression_V1_0_IHwExpression_H_
#define HIDL_GENERATED_android_hardware_tests_expression_V1_0_IHwExpression_H_

#include <android/hardware/tests/expression/1.0/IExpression.h>


#include <hidl/HidlSupport.h>
#include <hwbinder/IBinder.h>
#include <hwbinder/IInterface.h>
#include <hwbinder/Status.h>

namespace android {
namespace hardware {
namespace tests {
namespace expression {
namespace V1_0 {

struct IHwExpression : public IExpression, public ::android::hardware::IInterface {
  DECLARE_HWBINDER_META_INTERFACE(Expression);

  enum Call {
    CallCount
  };

};

}  // namespace V1_0
}  // namespace expression
}  // namespace tests
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_android_hardware_tests_expression_V1_0_IHwExpression_H_
