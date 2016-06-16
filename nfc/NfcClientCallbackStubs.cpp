// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/TemplateStubs.cpp
#include <android/hardware/nfc/INfcClientCallback.h>
#include <android/hardware/nfc/BpNfcClientCallback.h>

namespace android {
namespace hardware {
namespace nfc {


IMPLEMENT_META_INTERFACE(NfcClientCallback, "android.hardware.nfc.INfcClientCallback");

}  // namespace nfc
}  // namespace hardware
}  // namespace android


#include <iostream>
#include <android/hardware/nfc/BnNfcClientCallback.h>
#include <binder/Parcel.h>

namespace android {
namespace hardware {
namespace nfc {


::android::status_t BnNfcClientCallback::onTransact(uint32_t _aidl_code, const ::android::hidl::Parcel& _aidl_data, ::android::hidl::Parcel* _aidl_reply, uint32_t _aidl_flags, TransactCallback _cb) {
  ::android::status_t _aidl_ret_status = ::android::OK;
  switch (_aidl_code) {
    case Call::SENDEVENT:
      {
nfc_event_t event;
nfc_status_t event_status;

        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }
        _aidl_ret_status = _aidl_data.readInt32(&event);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
        _aidl_ret_status = _aidl_data.readInt32(&event_status);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }

        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             sendEvent(event, event_status,
                           [&]() {
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                             // Serialize

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
    case Call::SENDDATA:
      {
  INfcClientCallback::nfc_data_t *data ;

        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }
        data = (INfcClientCallback::nfc_data_t *)_aidl_data.readBuffer();

        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             sendData(data,
                           [&]() {
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                             // Serialize

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

}  // namespace nfc
}  // namespace hardware
}  // namespace android

