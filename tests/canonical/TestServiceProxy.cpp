#include <android/hardware/tests/ITestService.h>
#include <android/hardware/tests/BpTestService.h>

namespace android {

namespace hardware {

namespace tests {

IMPLEMENT_META_INTERFACE(TestService, "android.hardware.tests.ITestService");

}  // namespace tests

}  // namespace hardware

}  // namespace android
#include <android/hardware/tests/BpTestService.h>
#include <binder/Parcel.h>

namespace android {

namespace hardware {

namespace tests {

BpTestService::BpTestService(const ::android::sp<::android::IBinder>& _aidl_impl)
    : BpInterface<ITestService>(_aidl_impl){
}
::android::binder::Status BpTestService::echoInteger(int32_t echo_me, int32_t* _aidl_return) {
::android::Parcel _aidl_data;
::android::Parcel _aidl_reply;
::android::status_t _aidl_ret_status = ::android::OK;
::android::binder::Status _aidl_status;
_aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
if (((_aidl_ret_status) != (::android::OK))) {
goto _aidl_error;
}
_aidl_ret_status = _aidl_data.writeInt32(echo_me);
if (((_aidl_ret_status) != (::android::OK))) {
goto _aidl_error;
}

_aidl_ret_status = remote()->transact(ITestService::ECHOINTEGER, _aidl_data, &_aidl_reply);
if (((_aidl_ret_status) != (::android::OK))) {
goto _aidl_error;
}
_aidl_ret_status = _aidl_status.readFromParcel(_aidl_reply);
if (((_aidl_ret_status) != (::android::OK))) {
goto _aidl_error;
}
if (!_aidl_status.isOk()) {
return _aidl_status;
}
_aidl_ret_status = _aidl_reply.readInt32(_aidl_return);
if (((_aidl_ret_status) != (::android::OK))) {
goto _aidl_error;
}

_aidl_error:
_aidl_status.setFromStatusT(_aidl_ret_status);
return _aidl_status;
}

}  // namespace tests

}  // namespace hardware

}  // namespace android
