// SECTION proxy_cpp
// START file
// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/TemplateProxy.cpp
#include <namespace_slashes/Ipackage_name.h>
#include <namespace_slashes/Bppackage_name.h>

// START namespace_open_section
namespace namespace_name {//ALL namespace_open_line
//END namespace_open_section

IMPLEMENT_META_INTERFACE(package_name, "namespace_dots.Ipackage_name");

// START namespace_close_section
}  // namespace namespace_name  //ALL namespace_close_line
//END namespace_close_section

#include <namespace_slashes/Bppackage_name.h>
#include <binder/Parcel.h>

namespace_open_section

Bppackage_name::Bppackage_name(const ::android::sp<::android::hidl::IBinder>& _aidl_impl)
    : BpInterface<Ipackage_name>(_aidl_impl){
}
// START code_snips
/*
Ipackage_name::function_name_cb _cb // ALL callback_param // Used in callback_description
const Ipackage_name::struct_name* param_name // ALL param_decl_struct_type
sp<import_name> param_name // ALL param_decl_import
struct_name *param_name; // ALL field_decl_struct_type

*/
// START code_for_function
::android::hidl::binder::Status Bppackage_name::function_name(params_and_callback) {
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

// START param_write_import
  _aidl_ret_status = _aidl_data.writeStrongBinder(IInterface::asBinder(param_name));
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_write_import

// START param_write_enum_type_scalar_uint8_t
  _aidl_ret_status = _aidl_data.writeByte((int8_t)param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_write_enum_type_scalar_uint8_t
// START param_write_enum_type_scalar_int8_t
  _aidl_ret_status = _aidl_data.writeByte((int8_t)param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_write_enum_type_scalar_int8_t
// START param_write_enum_type_scalar_uint16_t
  _aidl_ret_status = _aidl_data.writeChar((char16_t)param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_write_enum_type_scalar_uint16_t
// START param_write_enum_type_scalar_int16_t
  _aidl_ret_status = _aidl_data.writeChar((char16_t)param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_write_enum_type_scalar_int16_t

// START param_write_enum_type_scalar_uint32_t
  _aidl_ret_status = _aidl_data.writeUint32((uint32_t)param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_write_enum_type_scalar_uint32_t
// START param_write_enum_type_scalar_int32_t
  _aidl_ret_status = _aidl_data.writeInt32((int32_t)param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_write_enum_type_scalar_int32_t
// START param_write_enum_type_scalar_uint64_t
  _aidl_ret_status = _aidl_data.writeUint64((uint64_t)param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_write_enum_type_scalar_uint64_t
// START param_write_enum_type_scalar_int64_t
  _aidl_ret_status = _aidl_data.writeInt64((int64_t)param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_write_enum_type_scalar_int64_t

// START param_write_enum_type_scalar_char
  _aidl_ret_status = _aidl_data.writeByte((int8_t)param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_write_enum_type_scalar_char

// START param_write_scalar_int8_t
  _aidl_ret_status = _aidl_data.writeByte((int8_t)param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_write_scalar_int8_t
// START param_write_scalar_uint8_t
  _aidl_ret_status = _aidl_data.writeByte((int8_t)param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_write_scalar_uint8_t
// START param_write_scalar_int16_t
  _aidl_ret_status = _aidl_data.writeChar((char16_t)param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_write_scalar_int16_t
// START param_write_scalar_uint16_t
  _aidl_ret_status = _aidl_data.writeChar((char16_t)param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_write_scalar_uint16_t

// START param_write_scalar_int32_t
  _aidl_ret_status = _aidl_data.writeInt32((int32_t)param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_write_scalar_int32_t
// START param_write_scalar_uint32_t
  _aidl_ret_status = _aidl_data.writeUint32((uint32_t)param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_write_scalar_uint32_t
// START param_write_scalar_int64_t
  _aidl_ret_status = _aidl_data.writeInt64((int64_t)param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_write_scalar_int64_t
// START param_write_scalar_uint64_t
  _aidl_ret_status = _aidl_data.writeUint64((uint64_t)param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_write_scalar_uint64_t

// START param_write_scalar_char
  _aidl_ret_status = _aidl_data.writeByte((int8_t)param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_write_scalar_char

  // START param_write_struct_type
  _aidl_ret_status = _aidl_data.writeBuffer((void *)param_name, sizeof(param_name), nullptr);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  // END param_write_struct_type
    // START param_write_ref_all
  _aidl_ret_status = _aidl_data.writeFileDescriptor(param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
    // END param_write_ref_all
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
// START param_ret_read_snips

// START param_ret_read_scalar_int8_t
  _aidl_ret_status = _aidl_reply.readByte(&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_ret_read_scalar_int8_t
// START param_ret_read_scalar_uint8_t
  _aidl_ret_status = _aidl_reply.readByte((int8_t*)&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_ret_read_scalar_uint8_t
// START param_ret_read_scalar_int16_t
  _aidl_ret_status = _aidl_reply.readChar((char16_t*)&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_ret_read_scalar_int16_t
// START param_ret_read_scalar_uint16_t
  _aidl_ret_status = _aidl_reply.readChar((char16_t*)&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_ret_read_scalar_uint16_t

// START param_ret_read_scalar_int32_t
  _aidl_ret_status = _aidl_reply.readInt32(&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_ret_read_scalar_int32_t
// START param_ret_read_scalar_uint32_t
  _aidl_ret_status = _aidl_reply.readUint32(&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_ret_read_scalar_uint32_t
// START param_ret_read_scalar_int64_t
  _aidl_ret_status = _aidl_reply.readInt64(&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_ret_read_scalar_int64_t
// START param_ret_read_scalar_uint64_t
  _aidl_ret_status = _aidl_reply.readUint64(&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_ret_read_scalar_uint64_t

// START param_ret_read_scalar_char
  _aidl_ret_status = _aidl_reply.readByte((int8_t*)&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_ret_read_scalar_char

  // START param_ret_read_import
  _aidl_ret_status = _aidl_reply.readStrongBinder(&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  // END param_ret_read_import

// START param_ret_read_enum_type_scalar_uint8_t
  _aidl_ret_status = _aidl_reply.readByte((int8_t *)&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_ret_read_enum_type_scalar_uint8_t
// START param_ret_read_enum_type_scalar_int8_t
  _aidl_ret_status = _aidl_reply.readByte((int8_t *)&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_ret_read_enum_type_scalar_int8_t
// START param_ret_read_enum_type_scalar_uint16_t
  _aidl_ret_status = _aidl_reply.readChar((char16_t *)&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_ret_read_enum_type_scalar_uint16_t
// START param_ret_read_enum_type_scalar_int16_t
  _aidl_ret_status = _aidl_reply.readChar((char16_t *)&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_ret_read_enum_type_scalar_int16_t

// START param_ret_read_enum_type_scalar_uint32_t
  _aidl_ret_status = _aidl_reply.readUint32((uint32_t *)&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_ret_read_enum_type_scalar_uint32_t
// START param_ret_read_enum_type_scalar_int32_t
  _aidl_ret_status = _aidl_reply.readInt32((int32_t *)&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_ret_read_enum_type_scalar_int32_t
// START param_ret_read_enum_type_scalar_uint64_t
  _aidl_ret_status = _aidl_reply.readUint64((uint64_t *)&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_ret_read_enum_type_scalar_uint64_t
// START param_ret_read_enum_type_scalar_int64_t
  _aidl_ret_status = _aidl_reply.readInt64((int64_t *)&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_ret_read_enum_type_scalar_int64_t

// START param_ret_read_enum_type_scalar_char
  _aidl_ret_status = _aidl_reply.readByte((int8_t *)&param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_ret_read_enum_type_scalar_char

// START param_ret_read_struct_type
  param_name = (Ipackage_name::struct_name *)_aidl_data.readBuffer();
// END param_ret_read_struct_type
// END param_ret_read_snips

  // Invoke callback to client  // START callback_invocation
  if (_cb != nullptr)
  _cb(return_param_names);      // END callback_invocation

_aidl_error:
  _aidl_status.setFromStatusT(_aidl_ret_status);
  return _aidl_status;
}
// END code_for_function
param_name // ALL callback_var_default
*param_name // ALL callback_var_struct_type
// END code_snips

namespace_close_section

// END file
