// SECTION all_cpp
// START file
// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/TemplateAll.cpp
#include <namespace_slashes/Ipackage_name.h>
#include <namespace_slashes/Bppackage_name.h>
#include <iostream>
#include <stdio.h>
using std::cout;
using std::endl;

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
const Ipackage_name::struct_name& param_name // ALL param_decl_struct_type
sp<Ipackage_name::import_name> param_name // ALL param_decl_import

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
  _aidl_ret_status = _aidl_data.writeStrongBinder(param_name);
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
  {
    uint64_t parent_handle;
    _aidl_ret_status = _aidl_data.writeBuffer((void *)&param_name, sizeof(param_name), &parent_handle);
    if (((_aidl_ret_status) != (::android::OK))) {
      goto _aidl_error;
    }
    fixup_write_struct;
  }
  // END param_write_struct_type
  // START param_write_string
  _aidl_ret_status = _aidl_data.writeBuffer((void *)param_name.buffer,
                       (param_name.length < 0 ? strlen(param_name.buffer)+1 : param_name.length),
                       nullptr);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  // END param_write_string
  // START fixup_write_string
  _aidl_ret_status = _aidl_data.writeBuffer((void *)param_name.buffer,
                       (param_name.length < 0 ? strlen(param_name.buffer)+1 : param_name.length),
                       nullptr, parent_handle,
                       (size_t)((char *)&(param_name.buffer)-(char *)(base_pointer)));
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  // END fixup_write_string
  // START fixup_write_vec
  {
    uint64_t child_handle;
    cout << "Client: calling writeBuffer in echoStruct. ptr " << &s << " size " << sizeof(s0) << endl;
    void *bufp = (void *)s.s1v2.buffer;
    size_t size = sizeof(ITypes::s1s[s.s1v2.count]);
    size_t off = (size_t)((char *)&(s.s1v2.buffer)-(char *)&(s));
    _aidl_ret_status = _aidl_data.writeBuffer(bufp,
                                              size,
                                              &child_handle,
                                              parent_handle,
                                              off /* offset_calculator */);
    printf("c: wb vec : buf %p size %ld offset %ld parent %ld child %ld ret %d\n", bufp, size, off, parent_handle, child_handle, _aidl_ret_status);
    uint64_t parent_handle = child_handle; // Yes, this is legal, and works; it's like a static stack.
    if (((_aidl_ret_status) != (::android::OK))) {
      goto _aidl_error;
    }
    for (size_t loop_var = 0; loop_var < s.s1v2.count; loop_var++) {
      /* fixup_write_vec */
      inner_vec_fixup
    }
  }
  // END fixup_write_vec
  // START fixup_write_array
  Array code goes here
  // END fixup_write_array
  // START fixup_write_handle
  Handle code goes here
  // END fixup_write_handle
  // START param_write_vec
  uint64_t parent_handle;
  _aidl_data.writeBuffer((void *)param_name.buffer, vec_size_expr, &parent_handle);
  write_vec_fixups;
  // END param_write_vec

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
  param_name = (Ipackage_name::struct_name *)_aidl_reply.readBuffer();
  fixup_read_struct
// END param_ret_read_struct_type
// END param_ret_read_snips

  // Invoke callback to client  // START callback_invocation
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

/*
auto param_name // ALL return_param_decl_default
const Ipackage_name::type_desc & param_name // ALL return_param_decl_struct_type
*/
#include <namespace_slashes/Ipackage_name.h>
#include <namespace_slashes/Bppackage_name.h>

/*
namespace_open_section

IMPLEMENT_META_INTERFACE(package_name, "namespace_dots.Ipackage_name");

namespace_close_section
*/

#include <iostream>
#include <namespace_slashes/Bnpackage_name.h>
#include <binder/Parcel.h>

namespace_open_section

::android::status_t Bnpackage_name::onTransact(uint32_t _aidl_code, const ::android::hidl::Parcel& _aidl_data, ::android::hidl::Parcel* _aidl_reply, uint32_t _aidl_flags, TransactCallback _cb) {
  ::android::status_t _aidl_ret_status = ::android::OK;
  switch (_aidl_code) {
// START code2_snips
// START code2_for_function
    case Call::func_name_as_enum:
      {
// START param_decls
  Ipackage_name::struct_name *param_name // ALL field_decl_struct_type
  sp<Ipackage_name::import_name> param_name // ALL field_decl_import
        int32_t should_be_replaced; // These will be replaced
        int32_t ret_FOO;   // with auto-generated declarations
// END param_decls
        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }
// START param_read_snips

// START param_read_scalar_int8_t
        _aidl_ret_status = _aidl_data.readByte(&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_scalar_int8_t
// START param_read_scalar_uint8_t
        _aidl_ret_status = _aidl_data.readByte((int8_t*)&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_scalar_uint8_t
// START param_read_scalar_int16_t
        _aidl_ret_status = _aidl_data.readChar((char16_t*)&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_scalar_int16_t
// START param_read_scalar_uint16_t
        _aidl_ret_status = _aidl_data.readChar((char16_t*)&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_scalar_uint16_t

// START param_read_scalar_int32_t
        _aidl_ret_status = _aidl_data.readInt32(&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_scalar_int32_t
// START param_read_scalar_uint32_t
        _aidl_ret_status = _aidl_data.readUint32(&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_scalar_uint32_t
// START param_read_scalar_int64_t
        _aidl_ret_status = _aidl_data.readInt64(&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_scalar_int64_t
// START param_read_scalar_uint64_t
        _aidl_ret_status = _aidl_data.readUint64(&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_scalar_uint64_t

// START param_read_scalar_char
        _aidl_ret_status = _aidl_data.readByte((int8_t*)&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_scalar_char

  // START param_read_import
        _aidl_ret_status = _aidl_data.readStrongBinder(&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          goto _aidl_error;
        }
  // END param_read_import
// START param_read_enum_type_scalar_int8_t
        _aidl_ret_status = _aidl_data.readByte((int8_t *)&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_enum_type_scalar_int8_t
// START param_read_enum_type_scalar_uint8_t
        _aidl_ret_status = _aidl_data.readByte((int8_t *)&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_enum_type_scalar_uint8_t
// START param_read_enum_type_scalar_int16_t
        _aidl_ret_status = _aidl_data.readChar((char16_t *)&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_enum_type_scalar_int16_t
// START param_read_enum_type_scalar_uint16_t
        _aidl_ret_status = _aidl_data.readChar((char16_t *)&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_enum_type_scalar_uint16_t
// START param_read_enum_type_scalar_int32_t
        _aidl_ret_status = _aidl_data.readInt32((int32_t *)&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_enum_type_scalar_int32_t
// START param_read_enum_type_scalar_uint32_t
        _aidl_ret_status = _aidl_data.readUint32((uint32_t *)&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_enum_type_scalar_uint32_t
// START param_read_enum_type_scalar_int64_t
        _aidl_ret_status = _aidl_data.readInt64((int64_t *)&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_enum_type_scalar_int64_t
// START param_read_enum_type_scalar_uint64_t
        _aidl_ret_status = _aidl_data.readUint64((uint64_t *)&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_enum_type_scalar_uint64_t
// START param_read_enum_type_scalar_char
        _aidl_ret_status = _aidl_data.readByte((int8_t *)&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_enum_type_scalar_char
// START param_read_ref_all
        param_name = _aidl_data.readFileDescriptor();
// END param_read_ref_all
  // START param_read_struct_type
        param_name = (Ipackage_name::struct_name *)_aidl_data.readBuffer();
        fixup_read_struct
  // END param_read_struct_type
// END param_read_snips
        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             function_name(stub_arguments));

        // START // Hide callback_code
param_name// ALL stub_param_decl_default
*param_name// ALL stub_param_decl_struct_type
        [&](return_params_stubs) {  // START callback_code
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                             // Serialize
// START param_ret_write_snips

// START param_ret_write_struct_type
                             std::cout << "Server: Struct type passed to cb" << std::endl;
                             _aidl_ret_status = _aidl_reply->writeBuffer((void *)&param_name, sizeof(param_name), nullptr);
                             fixup_ret_write_struct
// END param_ret_write_struct_type

// START param_ret_write_scalar_uint8_t
                             _aidl_reply->writeByte((int8_t)param_name);
// END param_ret_write_scalar_uint8_t
// START param_ret_write_scalar_int8_t
                             _aidl_reply->writeByte(param_name);
// END param_ret_write_scalar_int8_t
// START param_ret_write_scalar_uint16_t
                             _aidl_reply->writeChar((char16_t)param_name);
// END param_ret_write_scalar_uint16_t
// START param_ret_write_scalar_int16_t
                             _aidl_reply->writeChar((char16_t)param_name);
// END param_ret_write_scalar_int16_t

// START param_ret_write_scalar_uint32_t
                             _aidl_reply->writeUint32(param_name);
// END param_ret_write_scalar_uint32_t
// START param_ret_write_scalar_int32_t
                             _aidl_reply->writeInt32(param_name);
// END param_ret_write_scalar_int32_t
// START param_ret_write_scalar_uint64_t
                             _aidl_reply->writeUint64(param_name);
// END param_ret_write_scalar_uint64_t
// START param_ret_write_scalar_int64_t
                             _aidl_reply->writeInt64(param_name);
// END param_ret_write_scalar_int64_t

// START param_ret_write_scalar_char
                             _aidl_reply->writeByte((int8_t)param_name);
// END param_ret_write_scalar_char

// START param_ret_write_enum_type_scalar_int8_t
        _aidl_ret_status = _aidl_reply->writeByte((int8_t)param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          return;
        }
// END param_ret_write_enum_type_scalar_int8_t
// START param_ret_write_enum_type_scalar_uint8_t
        _aidl_ret_status = _aidl_reply->writeByte((int8_t)param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          return;
        }
// END param_ret_write_enum_type_scalar_uint8_t
// START param_ret_write_enum_type_scalar_int16_t
        _aidl_ret_status = _aidl_reply->writeChar((char16_t)param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          return;
        }
// END param_ret_write_enum_type_scalar_int16_t
// START param_ret_write_enum_type_scalar_uint16_t
        _aidl_ret_status = _aidl_reply->writeChar((char16_t)param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          return;
        }
// END param_ret_write_enum_type_scalar_uint16_t
// START param_ret_write_enum_type_scalar_int32_t
        _aidl_ret_status = _aidl_reply->writeInt32((int32_t)param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          return;
        }
// END param_ret_write_enum_type_scalar_int32_t
// START param_ret_write_enum_type_scalar_uint32_t
        _aidl_ret_status = _aidl_reply->writeUint32((uint32_t)param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          return;
        }
// END param_ret_write_enum_type_scalar_uint32_t
// START param_ret_write_enum_type_scalar_int64_t
        _aidl_ret_status = _aidl_reply->writeInt64((int64_t)param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          return;
        }
// END param_ret_write_enum_type_scalar_int64_t
// START param_ret_write_enum_type_scalar_uint64_t
        _aidl_ret_status = _aidl_reply->writeUint64((uint64_t)param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          return;
        }
// END param_ret_write_enum_type_scalar_uint64_t
// START param_ret_write_enum_type_scalar_char
        _aidl_ret_status = _aidl_reply->writeByte((int8_t)param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          return;
        }
// END param_ret_write_enum_type_scalar_char
// START param_ret_write_import
  _aidl_ret_status = _aidl_reply->writeStrongBinder(param_name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
// END param_ret_write_import
// END param_ret_write_snips
                            // Callback
                             _cb(*_aidl_reply);
                           }  // END callback_code
        // END //

        if (!callback_called) {  // START callback_check
          // Callback not called, the call must have returned an error
          // TODO set something like ERR_NO_CALLBACK if the call retuned OK
          _aidl_ret_status = _aidl_status.writeToParcel(_aidl_reply);
        }  // END callback_check
        break;
      }
// END code2_for_function
// END code2_snips
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

namespace_close_section
// END file
