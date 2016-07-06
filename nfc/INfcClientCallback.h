// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/ITemplate.h
#ifndef HIDL_GENERATED_android_hardware_nfc_I_NfcClientCallback_H_
#define HIDL_GENERATED_android_hardware_nfc_I_NfcClientCallback_H_

#include <hwbinder/IBinder.h>
#include <hwbinder/IInterface.h>
#include <hwbinder/Status.h>
#include <cstdint>
#include <utils/StrongPointer.h>

namespace android {
namespace hardware {
namespace nfc {


template<typename T>
using hidl_ref = int;

#ifndef HIDL_TYPES
#define HIDL_TYPES
template<typename T>
struct hidl_vec {
  T *buffer;
  size_t count;
};
struct hidl_string {
  char *buffer;
  ptrdiff_t length;
};
#endif // HIDL_TYPES

class INfcClientCallback : public ::android::hidl::IInterface {
public:

DECLARE_META_INTERFACE(NfcClientCallback);
typedef struct {
hidl_vec<uint8_t> data ;

} nfc_data_t;

  enum class nfc_event_t : uint32_t {  HAL_NFC_OPEN_CPLT_EVT = 0,  HAL_NFC_CLOSE_CPLT_EVT = 1,  HAL_NFC_POST_INIT_CPLT_EVT = 2,  HAL_NFC_PRE_DISCOVER_CPLT_EVT = 3,  HAL_NFC_REQUEST_CONTROL_EVT = 4,  HAL_NFC_RELEASE_CONTROL_EVT = 5,  HAL_NFC_ERROR_EVT = 6 };
  const char* GetNameOf(nfc_event_t f) {
    static const char* names[] = {  "HAL_NFC_OPEN_CPLT_EVT" ,   "HAL_NFC_CLOSE_CPLT_EVT" ,   "HAL_NFC_POST_INIT_CPLT_EVT" ,   "HAL_NFC_PRE_DISCOVER_CPLT_EVT" ,   "HAL_NFC_REQUEST_CONTROL_EVT" ,   "HAL_NFC_RELEASE_CONTROL_EVT" ,   "HAL_NFC_ERROR_EVT"  };
    //do errror checking
    return names[int(f)];
}
  //  size_t enum_limits<foo>::max() { return 1; }
  enum class nfc_status_t : uint32_t {  HAL_NFC_STATUS_OK = 0,  HAL_NFC_STATUS_FAILED = 1,  HAL_NFC_STATUS_ERR_TRANSPORT = 2,  HAL_NFC_STATUS_ERR_CMD_TIMEOUT = 3,  HAL_NFC_STATUS_REFUSED = 4 };
  const char* GetNameOf(nfc_status_t f) {
    static const char* names[] = {  "HAL_NFC_STATUS_OK" ,   "HAL_NFC_STATUS_FAILED" ,   "HAL_NFC_STATUS_ERR_TRANSPORT" ,   "HAL_NFC_STATUS_ERR_CMD_TIMEOUT" ,   "HAL_NFC_STATUS_REFUSED"  };
    //do errror checking
    return names[int(f)];
}
  //  size_t enum_limits<foo>::max() { return 1; }

  using sendEvent_cb = std::function<void()>;
  using sendData_cb = std::function<void()>;


  virtual ::android::hidl::binder::Status sendEvent(nfc_event_t event, nfc_status_t event_status) = 0;
  virtual ::android::hidl::binder::Status sendData(const INfcClientCallback::nfc_data_t &data ) = 0;

enum Call {
  SENDEVENT = ::android::hidl::IBinder::FIRST_CALL_TRANSACTION + 0,

  SENDDATA, 
};
};  // class INfcClientCallback

}  // namespace nfc
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_android_hardware_nfc_I_NfcClientCallback_H_
