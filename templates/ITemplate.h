#define param_list ::android::String16* _aidl_return
// SECTION i_h
// START file
#ifndef HIDL_GENERATED_ANDROID_HARDWARE_TESTS_I_header_guard_H_
#define HIDL_GENERATED_ANDROID_HARDWARE_TESTS_I_header_guard_H_

#include <hwbinder/IBinder.h>
#include <hwbinder/IInterface.h>
#include <hwbinder/Status.h>
#include <cstdint>
#include <utils/StrongPointer.h>

namespace android {

namespace hardware {

namespace tests {

class Ipackage_name : public ::android::IInterface {
public:
DECLARE_META_INTERFACE(package_name);
  // START declarations
  virtual ::android::binder::Status function_name(param_list) = 0; // ALL declare_function
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
  call_enum_name = ::android::IBinder::FIRST_CALL_TRANSACTION + 0, // ALL first_call_enum
  // END call_enum_list
};
};  // class Ipackage_name

}  // namespace tests

}  // namespace hardware

}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_TESTS_I_header_guard_H_
// END file
