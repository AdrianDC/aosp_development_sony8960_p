#define param_list ::android::String16* _aidl_return
// SECTION i_h
// START file
// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/ITemplate.h
#ifndef HIDL_GENERATED_ANDROID_HARDWARE_TESTS_I_header_guard_H_
#define HIDL_GENERATED_ANDROID_HARDWARE_TESTS_I_header_guard_H_

#include <hwbinder/IBinder.h>
#include <hwbinder/IInterface.h>
#include <hwbinder/Status.h>
#include <cstdint>
#include <utils/StrongPointer.h>

// START namespace_open_section
namespace namespace_name {//ALL namespace_open_line
//END namespace_open_section

template<typename T>
using hidl_ref = int;

class Ipackage_name : public ::android::hidl::IInterface {
public:
DECLARE_META_INTERFACE(package_name);
  // START code_snips
typedef struct { // START code_for_struct_decl
    int field_filler; // ALL struct_fields
} struct_name;

  // END code_for_struct_decl
  // END code_snips
/*
, function_name_cb _cb = nullptr // ALL callback_param // Used in callback_description
const Ipackage_name::struct_name *param_name // ALL param_decl_named_type_struct_decl
base_type_name param_name[array_size] // ALL field_decl_array
*/
  // START callback_decls
  using function_name_cb = std::function<void(return_param_list)>; // ALL callback_decl_line
  // END callback_decls

  // START declarations
  // START declare_function
  virtual ::android::hidl::binder::Status function_name(call_param_list callback_description) = 0;
  // END declare_function
  // START declare_enum_decl
  enum class enum_name { enum_fields };
  const char* GetNameOf(enum_name f) {
    static const char* names[] = {quoted_fields_of_enum }
    //do errror checking
    return names[int(f)];
}
  size_t enum_limits<foo>::max() { return 1; }
// END declare_enum_decl
  // END declarations
enum Call {
  // START call_enum_list
  call_enum_name = ::android::hidl::IBinder::FIRST_CALL_TRANSACTION + 0, // ALL first_call_enum
  // END call_enum_list
};
};  // class Ipackage_name

// START namespace_close_section
}  // namespace namespace_name  //ALL namespace_close_line
//END namespace_close_section

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_TESTS_I_header_guard_H_
// END file
