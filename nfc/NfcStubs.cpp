// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/TemplateStubs.cpp
#include <android/hardware/nfc/INfc.h>
#include <android/hardware/nfc/BpNfc.h>

namespace android {
namespace hardware {
namespace nfc {


IMPLEMENT_META_INTERFACE(Nfc, "android.hardware.nfc.INfc");

}  // namespace nfc
}  // namespace hardware
}  // namespace android


#include <iostream>
#include <android/hardware/nfc/BnNfc.h>
#include <binder/Parcel.h>

namespace android {
namespace hardware {
namespace nfc {


::android::status_t BnNfc::onTransact(uint32_t _aidl_code, const ::android::hidl::Parcel& _aidl_data, ::android::hidl::Parcel* _aidl_reply, uint32_t _aidl_flags, TransactCallback _cb) {
  ::android::status_t _aidl_ret_status = ::android::OK;
  switch (_aidl_code) {
    case Call::OPEN:
      {
  sp<INfc::INfcClientCallback> clientCallback ;

        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }
        _aidl_ret_status = _aidl_data.readStrongBinder(&clientCallback);
        if (((_aidl_ret_status) != (::android::OK))) {
          goto _aidl_error;
        }

        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             open(clientCallback,          [&](auto retval ) {
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                             // Serialize
                             _aidl_reply->writeInt32(retval);

                            // Callback
                             _cb(*_aidl_reply);
                           }
));
        /*
callback_code
        */
        if (!callback_called) {
          // Callback not called, the call must have returned an error
          // TODO set something like ERR_NO_CALLBACK if the call retuned OK
          _aidl_ret_status = _aidl_status.writeToParcel(_aidl_reply);
        }

        break;
      }
    case Call::WRITE:
      {
  INfc::nfc_data_t *data ;

        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }
        data = (INfc::nfc_data_t *)_aidl_data.readBuffer();

        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             write(data,          [&](auto retval ) {
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                             // Serialize
                             _aidl_reply->writeInt32(retval);

                            // Callback
                             _cb(*_aidl_reply);
                           }
));
        /*
callback_code
        */
        if (!callback_called) {
          // Callback not called, the call must have returned an error
          // TODO set something like ERR_NO_CALLBACK if the call retuned OK
          _aidl_ret_status = _aidl_status.writeToParcel(_aidl_reply);
        }

        break;
      }
    case Call::CORE_INITIALIZED:
      {
hidl_vec<uint8_t> data;

        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }

        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             core_initialized(data,          [&](auto retval ) {
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                             // Serialize
                             _aidl_reply->writeInt32(retval);

                            // Callback
                             _cb(*_aidl_reply);
                           }
));
        /*
callback_code
        */
        if (!callback_called) {
          // Callback not called, the call must have returned an error
          // TODO set something like ERR_NO_CALLBACK if the call retuned OK
          _aidl_ret_status = _aidl_status.writeToParcel(_aidl_reply);
        }

        break;
      }
    case Call::PRE_DISCOVER:
      {

        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }

        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             pre_discover(        [&](auto retval ) {
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                             // Serialize
                             _aidl_reply->writeInt32(retval);

                            // Callback
                             _cb(*_aidl_reply);
                           }
));
        /*
callback_code
        */
        if (!callback_called) {
          // Callback not called, the call must have returned an error
          // TODO set something like ERR_NO_CALLBACK if the call retuned OK
          _aidl_ret_status = _aidl_status.writeToParcel(_aidl_reply);
        }

        break;
      }
    case Call::CLOSE:
      {

        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }

        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             close(        [&](auto retval ) {
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                             // Serialize
                             _aidl_reply->writeInt32(retval);

                            // Callback
                             _cb(*_aidl_reply);
                           }
));
        /*
callback_code
        */
        if (!callback_called) {
          // Callback not called, the call must have returned an error
          // TODO set something like ERR_NO_CALLBACK if the call retuned OK
          _aidl_ret_status = _aidl_status.writeToParcel(_aidl_reply);
        }

        break;
      }
    case Call::CONTROL_GRANTED:
      {

        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }

        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             control_granted(        [&](auto retval ) {
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                             // Serialize
                             _aidl_reply->writeInt32(retval);

                            // Callback
                             _cb(*_aidl_reply);
                           }
));
        /*
callback_code
        */
        if (!callback_called) {
          // Callback not called, the call must have returned an error
          // TODO set something like ERR_NO_CALLBACK if the call retuned OK
          _aidl_ret_status = _aidl_status.writeToParcel(_aidl_reply);
        }

        break;
      }
    case Call::POWER_CYCLE:
      {

        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }

        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             power_cycle(        [&](auto retval ) {
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                             // Serialize
                             _aidl_reply->writeInt32(retval);

                            // Callback
                             _cb(*_aidl_reply);
                           }
));
        /*
callback_code
        */
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

