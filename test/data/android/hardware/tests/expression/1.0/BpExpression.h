#ifndef HIDL_GENERATED_android_hardware_tests_expression_V1_0_BpExpression_H_
#define HIDL_GENERATED_android_hardware_tests_expression_V1_0_BpExpression_H_

#include <android/hardware/tests/expression/1.0/IHwExpression.h>

namespace android {
namespace hardware {
namespace tests {
namespace expression {
namespace V1_0 {

struct BpExpression : public ::android::hardware::BpInterface<IHwExpression> {
    explicit BpExpression(const ::android::sp<::android::hardware::IBinder> &_hidl_impl);

    virtual bool isRemote() const { return true; }

    // Methods from IExpression follow.
    ::android::hardware::Return<void> foo1(const hidl_array<int32_t, 1 /* (Constants:CONST_FOO + 1) */>& array)  override;
    ::android::hardware::Return<void> foo2(const hidl_array<int32_t, 13 /* (5 + 8) */>& array)  override;
    ::android::hardware::Return<void> foo3(const hidl_array<int32_t, 20 /* Constants:MAX_ARRAY_SIZE */>& array)  override;

private:
    // for hidl instrumentation.
    std::vector<InstrumentationCallback> mInstrumentationCallbacks;
    bool mEnableInstrumentation;
};

}  // namespace V1_0
}  // namespace expression
}  // namespace tests
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_android_hardware_tests_expression_V1_0_BpExpression_H_
