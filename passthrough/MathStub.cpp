// This file will be auto-generated.
#include "IMath.h"
#include "BpMath.h"

#include "BnMath.h"

using android::hardware::BnMath;
using android::status_t;

namespace android {

namespace hardware {

IMPLEMENT_META_INTERFACE(Math, "android.hardware.tests.IMath");

}  // namespace hardware

}  // namespace android

#include <binder/Parcel.h>

namespace android {

namespace hardware {

::android::status_t BnMath::onTransact(uint32_t _aidl_code, const hidl::Parcel& _aidl_data, hidl::Parcel* _aidl_reply, uint32_t _aidl_flags) {
::android::status_t _aidl_ret_status = ::android::OK;
(void)_aidl_data;
(void)_aidl_code;
(void)_aidl_flags;
(void)_aidl_reply;
return _aidl_ret_status;
}

}  // namespace hardware

}  // namespace android


extern "C" {

struct funs_t {
  status_t (*sayHi)(BnMath *obj);
  status_t (*doCalc)(BnMath *obj, int, std::function<void(int32_t, int32_t)> cb);
};

status_t sayHi(BnMath *obj)
{
  return obj->sayHi();
}
status_t doCalc(BnMath *obj, int i, std::function<void(int32_t, int32_t)> cb)
{
  return obj->doCalc(i, cb);
}

// The name "hidlFunctionTable" is magic
funs_t hidlFunctionTable {sayHi, doCalc};

} // extern "C"
