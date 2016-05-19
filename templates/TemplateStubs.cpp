// SECTION stubs_cpp
// START file
// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/TemplateStubs.cpp
#include <android/hardware/tests/Ipackage_name.h>
#include <android/hardware/tests/Bppackage_name.h>

// START namespace_open_section
namespace namespace_name {//ALL namespace_open_line
//END namespace_open_section

IMPLEMENT_META_INTERFACE(package_name, "android.hardware.tests.Ipackage_name");

// START namespace_close_section
}  // namespace namespace_name  //ALL namespace_close_line
//END namespace_close_section

#include <android/hardware/tests/Bnpackage_name.h>
#include <binder/Parcel.h>

namespace_open_section

::android::status_t Bnpackage_name::onTransact(uint32_t _aidl_code, const ::android::Parcel& _aidl_data, ::android::Parcel* _aidl_reply, uint32_t _aidl_flags) {
::android::status_t _aidl_ret_status = ::android::OK;
switch (_aidl_code) {
// START code_snips
// START code_for_function
case Call::func_name_as_enum:
{
// START param_decls
int32_t in_echo_me;
int32_t _aidl_return;
// END param_decls
if (!(_aidl_data.checkInterface(this))) {
_aidl_ret_status = ::android::BAD_TYPE;
break;
}
// START param_read_snips
// START param_read_scalar_int32_t
_aidl_ret_status = _aidl_data.readInt32(&param_name);
if (((_aidl_ret_status) != (::android::OK))) {
break;
}
// END param_read_scalar_int32_t
// END param_read_snips
::android::binder::Status _aidl_status(function_name(function_params_stubs));
_aidl_ret_status = _aidl_status.writeToParcel(_aidl_reply);
if (((_aidl_ret_status) != (::android::OK))) {
break;
}
if (!_aidl_status.isOk()) {
break;
}
// START param_write_ret_snips
// START param_write_scalar_int32_t
_aidl_ret_status = _aidl_reply->writeInt32(param_name);
if (((_aidl_ret_status) != (::android::OK))) {
break;
}
// END param_write_scalar_int32_t
// END param_write_ret_snips
}
break;
// END code_for_function
// END code_snips
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

namespace_close_section
// END file
