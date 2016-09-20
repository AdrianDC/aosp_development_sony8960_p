#include <android/hardware/tests/expression/1.0//BpExpression.h>
#include <android/hardware/tests/expression/1.0//BnExpression.h>
#include <android/hardware/tests/expression/1.0//BsExpression.h>
#include <cutils/properties.h>

namespace android {
namespace hardware {
namespace tests {
namespace expression {
namespace V1_0 {

constexpr hidl_version IExpression::version;

BpExpression::BpExpression(const ::android::sp<::android::hardware::IBinder> &_hidl_impl)
    : BpInterface<IHwExpression>(_hidl_impl) {
}

IMPLEMENT_HWBINDER_META_INTERFACE(Expression, "android.hardware.tests.expression@1.0::IExpression");

BnExpression::BnExpression(const ::android::sp<IExpression> &_hidl_impl)
    : BnInterface<IExpression, IHwExpression>(_hidl_impl) {
    enableInstrumentation = property_get_bool("hal.instrumentation.enable", false);
    registerInstrumentationCallbacks("android.hardware.tests.expression@1.0::IExpression", &instrumentationCallbacks);
}

::android::status_t BnExpression::onTransact(
    uint32_t _hidl_code,
    const ::android::hardware::Parcel &_hidl_data,
    ::android::hardware::Parcel *_hidl_reply,
    uint32_t _hidl_flags,
    TransactCallback _hidl_cb) {
  ::android::status_t _hidl_err = ::android::OK;

  switch (_hidl_code) {
    default:
    {
      return ::android::hardware::BnInterface<IExpression, IHwExpression>::onTransact(
          _hidl_code, _hidl_data, _hidl_reply, _hidl_flags, _hidl_cb);
    }
  }

  if (_hidl_err == ::android::UNEXPECTED_NULL) {
    _hidl_err = ::android::hardware::Status::fromExceptionCode(
        ::android::hardware::Status::EX_NULL_POINTER)
            .writeToParcel(_hidl_reply);
  }

  return _hidl_err;
}

BsExpression::BsExpression(const sp<IExpression> impl) : mImpl(impl) {}

IMPLEMENT_REGISTER_AND_GET_SERVICE(Expression, "android.hardware.tests.expression@1.0-impl.so")
}  // namespace V1_0
}  // namespace expression
}  // namespace tests
}  // namespace hardware
}  // namespace android
