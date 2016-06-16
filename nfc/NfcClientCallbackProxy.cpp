// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/TemplateProxy.cpp
#include <android/hardware/nfc/INfcClientCallback.h>
#include <android/hardware/nfc/BpNfcClientCallback.h>

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
  _aidl_ret_status = _aidl_data.writeUint32(event);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeUint32(event_status);
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
::android::hidl::binder::Status BpNfcClientCallback::sendData(const INfcClientCallback::nfc_data_t* data ) {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
  
  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeBuffer((void *)data, sizeof(data));
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
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


