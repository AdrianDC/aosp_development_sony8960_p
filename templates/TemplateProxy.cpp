// SECTION proxy_cpp
// START file
// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/TemplateProxy.cpp
#include <android/hardware/tests/Ipackage_name.h>
#include <android/hardware/tests/Bppackage_name.h>

// START namespace_open_section
namespace namespace_name {//ALL namespace_open_line
//END namespace_open_section

IMPLEMENT_META_INTERFACE(package_name, "android.hardware.tests.Ipackage_name");

// START namespace_close_section
}  // namespace namespace_name  //ALL namespace_close_line
//END namespace_close_section

#include <android/hardware/tests/Bppackage_name.h>
#include <binder/Parcel.h>

namespace_open_section

Bppackage_name::Bppackage_name(const ::android::sp<::android::hidl::IBinder>& _aidl_impl)
    : BpInterface<Ipackage_name>(_aidl_impl){
}
// START code_snips
/*
, ITestService::function_name_cb _cb // ALL callback_param // Used in callback_description
const Ipackage_name::struct_name* param_name // ALL param_decl_named_type_struct_decl

*/
// START code_for_function
::android::hidl::binder::Status Bppackage_name::function_name(call_param_list callback_description) {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
  generates_variables
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
  // START param_write_named_type_struct_decl
  _aidl_ret_status = _aidl_data.writeBuffer((void *)param_name, sizeof(param_name));
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  // END param_write_named_type_struct_decl
    // START param_write_ref
  _aidl_ret_status = _aidl_data.writeFileDescriptor(param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
    // END param_write_ref
// END param_write_snips
  _aidl_ret_status = remote()->transact(Ipackage_name::func_name_as_enum, _aidl_data, &_aidl_reply);
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
  _aidl_ret_status = _aidl_reply.readInt32(&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_read_scalar_int32_t
// START param_read_named_type_struct_decl
  _aidl_ret_status = _aidl_reply.CODE_HERE
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_read_named_type_struct_decl
// END param_read_ret_snips

  // Invoke callback to client
  _cb(return_param_names);

_aidl_error:
  _aidl_status.setFromStatusT(_aidl_ret_status);
  return _aidl_status;
}
// END code_for_function
// END code_snips

namespace_close_section

// END file
