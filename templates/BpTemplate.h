#define param_list ::android::String16* _aidl_return
// SECTION bp_h
// START file
#ifndef HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BP_header_guard_H_
#define HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BP_header_guard_H_

#include <hwbinder/IBinder.h>
#include <hwbinder/IInterface.h>
#include <utils/Errors.h>
#include <android/hardware/tests/Ipackage_name.h>

namespace android {

namespace hardware {

namespace tests {

class Bppackage_name : public ::android::BpInterface<Ipackage_name> {
public:
explicit Bppackage_name(const ::android::sp<::android::IBinder>& _aidl_impl);
virtual ~Bppackage_name() = default;
  // START declarations
::android::binder::Status function_name(param_list) override; // ALL declare_function
  // END declarations
};  // class Bppackage_name

}  // namespace tests

}  // namespace hardware

}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_TESTS_BP_header_guard_H_
// END file
