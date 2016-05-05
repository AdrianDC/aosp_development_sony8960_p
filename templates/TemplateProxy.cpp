// SECTION proxy_cpp
// START file
#include <android/hardware/tests/Ipackage_name.h>
#include <android/hardware/tests/Bppackage_name.h>

namespace android {

namespace hardware {

namespace tests {

IMPLEMENT_META_INTERFACE(package_name, "android.hardware.tests.Ipackage_name");

}  // namespace tests

}  // namespace hardware

}  // namespace android
#include <android/hardware/tests/Bppackage_name.h>
#include <binder/Parcel.h>

namespace android {

namespace hardware {

namespace tests {

Bppackage_name::Bppackage_name(const ::android::sp<::android::IBinder>& _aidl_impl)
    : BpInterface<Ipackage_name>(_aidl_impl){
}
// START code_snips
// START code_for_function
::android::binder::Status Bppackage_name::function_name(function_params_proxy) {
::android::Parcel _aidl_data;
::android::Parcel _aidl_reply;
::android::status_t _aidl_ret_status = ::android::OK;
::android::binder::Status _aidl_status;
_aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
if (((_aidl_ret_status) != (::android::OK))) {
goto _aidl_error;
}
// START param_write_snips
// START param_write_scalar_int32_t
_aidl_ret_status = _aidl_data.writeInt32(param_name);
if (((_aidl_ret_status) != (::android::OK))) {
goto _aidl_error;
}
// END param_write_scalar_int32_t
// END param_write_snips
_aidl_ret_status = remote()->transact(Ipackage_name::ECHOINTEGER, _aidl_data, &_aidl_reply);
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
// START param_read_ret_snips
// START param_read_scalar_int32_t
_aidl_ret_status = _aidl_reply.readInt32(param_name);
if (((_aidl_ret_status) != (::android::OK))) {
goto _aidl_error;
}
// END param_read_scalar_int32_t
// END param_read_ret_snips
_aidl_error:
_aidl_status.setFromStatusT(_aidl_ret_status);
return _aidl_status;
}
// END code_for_function
// END code_snips
}  // namespace tests

}  // namespace hardware

}  // namespace android
// END file
