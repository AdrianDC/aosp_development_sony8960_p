#include <android/hardware/tests/ITestService.h>
#include <android/hardware/tests/BpTestService.h>

namespace android {

namespace hardware {

namespace tests {

IMPLEMENT_META_INTERFACE(TestService, "android.hardware.tests.ITestService");

}  // namespace tests

}  // namespace hardware

}  // namespace android

#include <android/hardware/tests/BnTestService.h>
#include <binder/Parcel.h>

namespace android {

namespace hardware {

namespace tests {

::android::status_t BnTestService::onTransact(uint32_t _aidl_code, const ::android::Parcel& _aidl_data, ::android::Parcel* _aidl_reply, uint32_t _aidl_flags) {
::android::status_t _aidl_ret_status = ::android::OK;
switch (_aidl_code) {
case Call::ECHOINTEGER:
{
int32_t echo_me;
int32_t _aidl_return;

if (!(_aidl_data.checkInterface(this))) {
_aidl_ret_status = ::android::BAD_TYPE;
break;
}
_aidl_ret_status = _aidl_data.readInt32(&echo_me);
if (((_aidl_ret_status) != (::android::OK))) {
break;
}

::android::binder::Status _aidl_status(echoInteger(echo_me, &_aidl_return));
_aidl_ret_status = _aidl_status.writeToParcel(_aidl_reply);
if (((_aidl_ret_status) != (::android::OK))) {
break;
}
if (!_aidl_status.isOk()) {
break;
}
_aidl_ret_status = _aidl_reply->writeInt32(_aidl_return);
if (((_aidl_ret_status) != (::android::OK))) {
break;
}

}
break;

default:
{
_aidl_ret_status = ::android::BBinder::onTransact(_aidl_code, _aidl_data, _aidl_reply, _aidl_flags);
}
break;
}
if (_aidl_ret_status == ::android::UNEXPECTED_NULL) {
_aidl_ret_status = ::android::binder::Status::fromExceptionCode(::android::binder::Status::EX_NULL_POINTER).writeToParcel(_aidl_reply);
}
return _aidl_ret_status;
}

}  // namespace tests

}  // namespace hardware

}  // namespace android
