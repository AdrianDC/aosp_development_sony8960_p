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

class Ipackage_name : public hidl::IInterface {
public:
DECLARE_META_INTERFACE(package_name);
  // START declarations
  virtual ::android::hidl::binder::Status function_name(call_param_list return_param_list) = 0; // ALL declare_function
 // START declare_enum_decl
  enum class enum_name { enum_fields };
  const char* GetNameOf(enum_name f) {
    static const char* names[] = {quoted_fields_of_enum }
    //do errror checking
    return names[int(f)];
}
  size_t enum_limits<foo>::max() { return 1; }
// END declare_enum_decl
  struct struct_name { // START declare_struct_decl
    int field_filler; // ALL struct_fields
  }; // END declare_struct_decl
  // END declarations
enum Call {
  // START call_enum_list
  call_enum_name = hidl::IBinder::FIRST_CALL_TRANSACTION + 0, // ALL first_call_enum
  // END call_enum_list
};
};  // class Ipackage_name

// START namespace_close_section
}  // namespace namespace_name  //ALL namespace_close_line
//END namespace_close_section

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_TESTS_I_header_guard_H_
// END file
