// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/TemplateStubs.cpp
#include <android/hardware/tests/ITestService.h>
#include <android/hardware/tests/BpTestService.h>

namespace android {
namespace hardware {
namespace tests {


IMPLEMENT_META_INTERFACE(TestService, "android.hardware.tests.ITestService");

}  // namespace tests
}  // namespace hardware
}  // namespace android

#include <iostream>
#include <android/hardware/tests/BnTestService.h>
#include <binder/Parcel.h>

namespace android {
namespace hardware {
namespace tests {

::android::status_t BnTestService::onTransact(uint32_t _aidl_code, const ::android::hidl::Parcel& _aidl_data, ::android::hidl::Parcel* _aidl_reply, uint32_t _aidl_flags, TransactCallback _cb) {
    ::android::status_t _aidl_ret_status = ::android::OK;
    switch (_aidl_code) {
        case Call::ECHOINTEGER:
        {
            int32_t in_echo_me;
            int32_t _aidl_return;
            bool callback_called;
            simple_t *my_struct;
            if (!(_aidl_data.checkInterface(this))) {
                _aidl_ret_status = ::android::BAD_TYPE;
                break;
            }
            _aidl_ret_status = _aidl_data.readInt32(&in_echo_me);
                if (((_aidl_ret_status) != (::android::OK))) {
                break;
            }
            const uint8_t *parcel_data = _aidl_data.data();
            std::cout << "parcel data at addr " << (void *)parcel_data << std::endl;
            my_struct = (simple_t *)_aidl_data.readBuffer();
            // Make the call into the server
            ::android::hidl::binder::Status _aidl_status(echoInteger(in_echo_me, my_struct,
                        [&](auto ret) {
                            callback_called = true;
                            // Write "OK" to parcel
                            ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                            // Serialize
                            _aidl_reply->writeInt32(ret);
                            // Callback
                            _cb(*_aidl_reply);
                        }
            ));
            if (!callback_called) {
                // Callback not called, the call must have returned an error
                // TODO set something like ERR_NO_CALLBACK if the call retuned OK
                _aidl_ret_status = _aidl_status.writeToParcel(_aidl_reply);
            }
            break;
        }
        case Call::SHAREBUFFERWITHREF:
        {
            int32_t in_buffer;
            int32_t _aidl_return;
            bool callback_called;
            if (!(_aidl_data.checkInterface(this))) {
                _aidl_ret_status = ::android::BAD_TYPE;
                break;
            }
            in_buffer = _aidl_data.readFileDescriptor();
            // Make the call into the server
            ::android::hidl::binder::Status _aidl_status(shareBufferWithRef(in_buffer,
                        [&](auto ret) {
                            callback_called = true;
                            // Write "OK" to parcel
                            ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                            // Serialize
                            _aidl_reply->writeInt32(ret);
                            // Callback
                            _cb(*_aidl_reply);
                        }
            ));
            if (!callback_called) {
                // Callback not called, the call must have returned an error
                // TODO set something like ERR_NO_CALLBACK if the call retuned OK
                _aidl_ret_status = _aidl_status.writeToParcel(_aidl_reply);
            }
            break;
        }
        default:
        {
            _aidl_ret_status = ::android::hidl::BBinder::onTransact(_aidl_code, _aidl_data, _aidl_reply, _aidl_flags);
        }
        break;
    }
    if (_aidl_ret_status == ::android::UNEXPECTED_NULL) {
        _aidl_ret_status = ::android::hidl::binder::Status::fromExceptionCode(::android::hidl::binder::Status::EX_NULL_POINTER).writeToParcel(_aidl_reply);
    }
    return _aidl_ret_status;
}

}  // namespace tests
}  // namespace hardware
}  // namespace android

