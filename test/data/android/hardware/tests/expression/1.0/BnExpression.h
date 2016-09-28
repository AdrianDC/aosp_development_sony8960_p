#ifndef HIDL_GENERATED_android_hardware_tests_expression_V1_0_BnExpression_H_
#define HIDL_GENERATED_android_hardware_tests_expression_V1_0_BnExpression_H_

#include <android/hardware/tests/expression/1.0/IHwExpression.h>

namespace android {
namespace hardware {
namespace tests {
namespace expression {
namespace V1_0 {

struct BnExpression : public ::android::hardware::BnInterface<IExpression, IHwExpression> {
    explicit BnExpression(const ::android::sp<IExpression> &_hidl_impl);

    ::android::status_t onTransact(
            uint32_t _hidl_code,
            const ::android::hardware::Parcel &_hidl_data,
            ::android::hardware::Parcel *_hidl_reply,
            uint32_t _hidl_flags = 0,
            TransactCallback _hidl_cb = nullptr) override;

    // Methods from IExpression follow.
    inline ::android::hardware::Return<void> foo1(const hidl_array<int32_t, 1 /* (Constants:CONST_FOO + 1) */>& array)  {
        return mImpl->foo1(array);
    };
    inline ::android::hardware::Return<void> foo2(const hidl_array<int32_t, 13 /* (5 + 8) */>& array)  {
        return mImpl->foo2(array);
    };
    inline ::android::hardware::Return<void> foo3(const hidl_array<int32_t, 20 /* Constants:MAX_ARRAY_SIZE */>& array)  {
        return mImpl->foo3(array);
    };

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

#endif  // HIDL_GENERATED_android_hardware_tests_expression_V1_0_BnExpression_H_
