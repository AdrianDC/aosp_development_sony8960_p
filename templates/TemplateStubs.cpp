// SECTION stubs_cpp
/*
auto param_name // ALL return_param_decl
*/
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

#include <iostream>
#include <android/hardware/tests/Bnpackage_name.h>
#include <binder/Parcel.h>

namespace_open_section

::android::status_t Bnpackage_name::onTransact(uint32_t _aidl_code, const ::android::hidl::Parcel& _aidl_data, ::android::hidl::Parcel* _aidl_reply, uint32_t _aidl_flags, TransactCallback _cb) {
  ::android::status_t _aidl_ret_status = ::android::OK;
  switch (_aidl_code) {
// START code_snips
// START code_for_function
    case Call::func_name_as_enum:
      {
// START param_decls
        /*
  Ipackage_name::struct_name *param_name // ALL field_decl_struct_type
         */
        int32_t should_be_replaced; // These will be replaced
        int32_t ret_FOO;   // with auto-generated declarations
// END param_decls
        bool callback_called;
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
// START param_read_enum_type_scalar_int32_t
        _aidl_ret_status = _aidl_data.readInt32(&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_enum_type_scalar_int32_t
// START param_read_enum_type_scalar_uint32_t
        _aidl_ret_status = _aidl_data.readInt32(&param_name);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
// END param_read_enum_type_scalar_uint32_t
// START param_read_ref_all
        param_name = _aidl_data.readFileDescriptor();
// END param_read_ref_all
  // START param_read_struct_type
        param_name = (Ipackage_name::struct_name *)_aidl_data.readBuffer();
  // END param_read_struct_type
// END param_read_snips
        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             function_name(function_params_stubs,
                           [&](return_params_stubs) {
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                             // Serialize
// START param_write_ret_snips
// START param_write_scalar_int32_t
                             _aidl_reply->writeInt32(param_name);
// END param_write_scalar_int32_t
// END param_write_ret_snips
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
// END code_for_function
// END code_snips
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
