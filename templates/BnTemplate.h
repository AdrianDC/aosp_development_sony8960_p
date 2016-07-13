// SECTION bn_h
// START file
// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/BpTemplate.h
#ifndef HIDL_GENERATED_namespace_underscores_BN_header_guard_H_
#define HIDL_GENERATED_namespace_underscores_BN_header_guard_H_

#include <hwbinder/IInterface.h>
//#include <namespace_slashes/Ipackage_name.h>
#include <Ipackage_name.h>

// START namespace_open_section
namespace namespace_name {//ALL namespace_open_line
//END namespace_open_section

class Bnpackage_name : public ::android::hidl::BnInterface<Ipackage_name> {
public:
  ::android::status_t onTransact(uint32_t _aidl_code, const ::android::hidl::Parcel& _aidl_data, ::android::hidl::Parcel* _aidl_reply, uint32_t _aidl_flags = 0, TransactCallback _callback = nullptr) override;
};  // class Bnpackage_name

// START namespace_close_section
}  // namespace namespace_name  //ALL namespace_close_line
//END namespace_close_section

#endif  // HIDL_GENERATED_namespace_underscores_BN_header_guard_H_
// END file
