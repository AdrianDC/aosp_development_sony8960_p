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
        mEnableInstrumentation = property_get_bool("hal.instrumentation.enable", false);
        registerInstrumentationCallbacks("android.hardware.tests.expression@1.0::IExpression", &mInstrumentationCallbacks);
}

::android::hardware::Return<void> BpExpression::foo1(const hidl_array<int32_t, 1 /* (Constants:CONST_FOO + 1) */>& array) {
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> args;
        args.push_back((void *)&array);
        for (auto callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_ENTRY, "android.hardware.tests.expression", "1.0", "IExpression", "foo1", &args);
        }
    }

    ::android::hardware::Parcel _hidl_data;
    ::android::hardware::Parcel _hidl_reply;
    ::android::status_t _hidl_err;
    ::android::hardware::Status _hidl_status;

    _hidl_err = _hidl_data.writeInterfaceToken(::android::hardware::tests::expression::V1_0::IHwExpression::descriptor);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    size_t _hidl_array_parent;

    _hidl_err = _hidl_data.writeBuffer(array.data(), 1 * sizeof(int32_t), &_hidl_array_parent);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = remote()->transact(::android::hardware::tests::expression::V1_0::IHwExpression::Call::FOO1, _hidl_data, &_hidl_reply);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = _hidl_status.readFromParcel(_hidl_reply);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    if (!_hidl_status.isOk()) { return _hidl_status; }

    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> args;
        for (auto callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_EXIT, "android.hardware.tests.expression", "1.0", "IExpression", "foo1", &args);
        }
    }

    _hidl_status.setFromStatusT(_hidl_err);
    return ::android::hardware::Return<void>();

_hidl_error:
    _hidl_status.setFromStatusT(_hidl_err);
    return ::android::hardware::Return<void>(_hidl_status);
}

::android::hardware::Return<void> BpExpression::foo2(const hidl_array<int32_t, 13 /* (5 + 8) */>& array) {
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> args;
        args.push_back((void *)&array);
        for (auto callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_ENTRY, "android.hardware.tests.expression", "1.0", "IExpression", "foo2", &args);
        }
    }

    ::android::hardware::Parcel _hidl_data;
    ::android::hardware::Parcel _hidl_reply;
    ::android::status_t _hidl_err;
    ::android::hardware::Status _hidl_status;

    _hidl_err = _hidl_data.writeInterfaceToken(::android::hardware::tests::expression::V1_0::IHwExpression::descriptor);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    size_t _hidl_array_parent;

    _hidl_err = _hidl_data.writeBuffer(array.data(), 13 * sizeof(int32_t), &_hidl_array_parent);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = remote()->transact(::android::hardware::tests::expression::V1_0::IHwExpression::Call::FOO2, _hidl_data, &_hidl_reply);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = _hidl_status.readFromParcel(_hidl_reply);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    if (!_hidl_status.isOk()) { return _hidl_status; }

    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> args;
        for (auto callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_EXIT, "android.hardware.tests.expression", "1.0", "IExpression", "foo2", &args);
        }
    }

    _hidl_status.setFromStatusT(_hidl_err);
    return ::android::hardware::Return<void>();

_hidl_error:
    _hidl_status.setFromStatusT(_hidl_err);
    return ::android::hardware::Return<void>(_hidl_status);
}

::android::hardware::Return<void> BpExpression::foo3(const hidl_array<int32_t, 20 /* Constants:MAX_ARRAY_SIZE */>& array) {
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> args;
        args.push_back((void *)&array);
        for (auto callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_ENTRY, "android.hardware.tests.expression", "1.0", "IExpression", "foo3", &args);
        }
    }

    ::android::hardware::Parcel _hidl_data;
    ::android::hardware::Parcel _hidl_reply;
    ::android::status_t _hidl_err;
    ::android::hardware::Status _hidl_status;

    _hidl_err = _hidl_data.writeInterfaceToken(::android::hardware::tests::expression::V1_0::IHwExpression::descriptor);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    size_t _hidl_array_parent;

    _hidl_err = _hidl_data.writeBuffer(array.data(), 20 * sizeof(int32_t), &_hidl_array_parent);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = remote()->transact(::android::hardware::tests::expression::V1_0::IHwExpression::Call::FOO3, _hidl_data, &_hidl_reply);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = _hidl_status.readFromParcel(_hidl_reply);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    if (!_hidl_status.isOk()) { return _hidl_status; }

    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> args;
        for (auto callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_EXIT, "android.hardware.tests.expression", "1.0", "IExpression", "foo3", &args);
        }
    }

    _hidl_status.setFromStatusT(_hidl_err);
    return ::android::hardware::Return<void>();

_hidl_error:
    _hidl_status.setFromStatusT(_hidl_err);
    return ::android::hardware::Return<void>(_hidl_status);
}

IMPLEMENT_HWBINDER_META_INTERFACE(Expression, "android.hardware.tests.expression@1.0::IExpression");

BnExpression::BnExpression(const ::android::sp<IExpression> &_hidl_impl)
        : BnInterface<IExpression, IHwExpression>(_hidl_impl) {
        mEnableInstrumentation = property_get_bool("hal.instrumentation.enable", false);
        registerInstrumentationCallbacks("android.hardware.tests.expression@1.0::IExpression", &mInstrumentationCallbacks);
}

::android::status_t BnExpression::onTransact(
        uint32_t _hidl_code,
        const ::android::hardware::Parcel &_hidl_data,
        ::android::hardware::Parcel *_hidl_reply,
        uint32_t _hidl_flags,
        TransactCallback _hidl_cb) {
    ::android::status_t _hidl_err = ::android::OK;

    switch (_hidl_code) {
        case ::android::hardware::tests::expression::V1_0::IHwExpression::Call::FOO1:
        {
            if (!_hidl_data.enforceInterface(::android::hardware::tests::expression::V1_0::IHwExpression::descriptor)) {
                _hidl_err = ::android::BAD_TYPE;
                break;
            }

            const hidl_array<int32_t, 1 /* (Constants:CONST_FOO + 1) */>* array;

            size_t _hidl_array_parent;

            array = (const hidl_array<int32_t, 1 /* (Constants:CONST_FOO + 1) */>*)_hidl_data.readBuffer(&_hidl_array_parent);

            if (array == nullptr) {
                _hidl_err = ::android::UNKNOWN_ERROR;
                break;
            }

            if (UNLIKELY(mEnableInstrumentation)) {
                std::vector<void *> args;
                args.push_back((void *)array);
                for (auto callback: mInstrumentationCallbacks) {
                    callback(InstrumentationEvent::SERVER_API_ENTRY, "android.hardware.tests.expression", "1.0", "IExpression", "foo1", &args);
                }
            }

            foo1(*array);

            ::android::hardware::Status::ok().writeToParcel(_hidl_reply);
            break;
        }

        case ::android::hardware::tests::expression::V1_0::IHwExpression::Call::FOO2:
        {
            if (!_hidl_data.enforceInterface(::android::hardware::tests::expression::V1_0::IHwExpression::descriptor)) {
                _hidl_err = ::android::BAD_TYPE;
                break;
            }

            const hidl_array<int32_t, 13 /* (5 + 8) */>* array;

            size_t _hidl_array_parent;

            array = (const hidl_array<int32_t, 13 /* (5 + 8) */>*)_hidl_data.readBuffer(&_hidl_array_parent);

            if (array == nullptr) {
                _hidl_err = ::android::UNKNOWN_ERROR;
                break;
            }

            if (UNLIKELY(mEnableInstrumentation)) {
                std::vector<void *> args;
                args.push_back((void *)array);
                for (auto callback: mInstrumentationCallbacks) {
                    callback(InstrumentationEvent::SERVER_API_ENTRY, "android.hardware.tests.expression", "1.0", "IExpression", "foo2", &args);
                }
            }

            foo2(*array);

            ::android::hardware::Status::ok().writeToParcel(_hidl_reply);
            break;
        }

        case ::android::hardware::tests::expression::V1_0::IHwExpression::Call::FOO3:
        {
            if (!_hidl_data.enforceInterface(::android::hardware::tests::expression::V1_0::IHwExpression::descriptor)) {
                _hidl_err = ::android::BAD_TYPE;
                break;
            }

            const hidl_array<int32_t, 20 /* Constants:MAX_ARRAY_SIZE */>* array;

            size_t _hidl_array_parent;

            array = (const hidl_array<int32_t, 20 /* Constants:MAX_ARRAY_SIZE */>*)_hidl_data.readBuffer(&_hidl_array_parent);

            if (array == nullptr) {
                _hidl_err = ::android::UNKNOWN_ERROR;
                break;
            }

            if (UNLIKELY(mEnableInstrumentation)) {
                std::vector<void *> args;
                args.push_back((void *)array);
                for (auto callback: mInstrumentationCallbacks) {
                    callback(InstrumentationEvent::SERVER_API_ENTRY, "android.hardware.tests.expression", "1.0", "IExpression", "foo3", &args);
                }
            }

            foo3(*array);

            ::android::hardware::Status::ok().writeToParcel(_hidl_reply);
            break;
        }

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
