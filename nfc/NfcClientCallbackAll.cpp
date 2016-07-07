// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/TemplateAll.cpp
#include <android/hardware/nfc/INfcClientCallback.h>
#include <android/hardware/nfc/BpNfcClientCallback.h>
#include <iostream>
#include <stdio.h>
using std::cout;
using std::endl;

namespace android {
namespace hardware {
namespace nfc {


IMPLEMENT_META_INTERFACE(NfcClientCallback, "android.hardware.nfc.INfcClientCallback");

}  // namespace nfc
}  // namespace hardware
}  // namespace android


#include <android/hardware/nfc/BpNfcClientCallback.h>
#include <binder/Parcel.h>

namespace android {
namespace hardware {
namespace nfc {


BpNfcClientCallback::BpNfcClientCallback(const ::android::sp<::android::hidl::IBinder>& _aidl_impl)
    : BpInterface<INfcClientCallback>(_aidl_impl){
}
::android::hidl::binder::Status BpNfcClientCallback::sendEvent(nfc_event_t event, nfc_status_t event_status) {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
  
  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeUint32((uint32_t)event);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeUint32((uint32_t)event_status);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }

  _aidl_ret_status = remote()->transact(INfcClientCallback::SENDEVENT, _aidl_data, &_aidl_reply);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_status.readFromParcel(_aidl_reply);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  if (!_aidl_status.isOk()) {
    return _aidl_status;
  }




_aidl_error:
  _aidl_status.setFromStatusT(_aidl_ret_status);
  return _aidl_status;
}
::android::hidl::binder::Status BpNfcClientCallback::sendData(const INfcClientCallback::nfc_data_t& data ) {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
  
  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  {
    uint64_t parent_handle;
    _aidl_ret_status = _aidl_data.writeBuffer((void *)&data, sizeof(data), &parent_handle);
    if (((_aidl_ret_status) != (::android::OK))) {
      goto _aidl_error;
    }
      // struct fixup write_
  {
    uint64_t child_handle;
    void *bufp = (void *)data.data.buffer;
    size_t size = sizeof(uint8_t[data.data.count]);
    size_t off = (size_t)((char *)&(data.data.buffer)-(char *)(&data));
    _aidl_ret_status = _aidl_data.writeBuffer(bufp,
                                              size,
                                              &child_handle,
                                              parent_handle,
                                              off /* offset_calculator */);
    printf("c: wb vec : buf %p size %ld offset %ld parent %ld child %ld ret %d\n", bufp, size, off, parent_handle, child_handle, _aidl_ret_status);
     // Redefining a variable inside braces works like a static stack.
    uint64_t parent_handle = child_handle;
    if (((_aidl_ret_status) != (::android::OK))) {
      goto _aidl_error;
    }
    // Fixup not needed for this vec uint8_t
  }

  }

  _aidl_ret_status = remote()->transact(INfcClientCallback::SENDDATA, _aidl_data, &_aidl_reply);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_status.readFromParcel(_aidl_reply);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  if (!_aidl_status.isOk()) {
    return _aidl_status;
  }




_aidl_error:
  _aidl_status.setFromStatusT(_aidl_ret_status);
  return _aidl_status;
}


}  // namespace nfc
}  // namespace hardware
}  // namespace android


//
#include <android/hardware/nfc/INfcClientCallback.h>
#include <android/hardware/nfc/BpNfcClientCallback.h>


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
INfcClientCallback::nfc_event_t
 event;
INfcClientCallback::nfc_status_t
 event_status;

        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }
        _aidl_ret_status = _aidl_data.readUint32((uint32_t *)&event);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
        _aidl_ret_status = _aidl_data.readUint32((uint32_t *)&event_status);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }

        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             sendEvent(event , event_status ));

//


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
          // struct fixup read_


        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             sendData(*data ));

//


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

