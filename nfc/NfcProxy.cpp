// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/TemplateProxy.cpp
#include <android/hardware/nfc/INfc.h>
#include <android/hardware/nfc/BpNfc.h>

namespace android {
namespace hardware {
namespace nfc {


IMPLEMENT_META_INTERFACE(Nfc, "android.hardware.nfc.INfc");

}  // namespace nfc
}  // namespace hardware
}  // namespace android


#include <android/hardware/nfc/BpNfc.h>
#include <binder/Parcel.h>

namespace android {
namespace hardware {
namespace nfc {


BpNfc::BpNfc(const ::android::sp<::android::hidl::IBinder>& _aidl_impl)
    : BpInterface<INfc>(_aidl_impl){
}
::android::hidl::binder::Status BpNfc::open(sp<INfc::INfcClientCallback> clientCallback , INfc::open_cb _cb ) {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
  int32_t retval;

  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeStrongBinder(clientCallback);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }

  _aidl_ret_status = remote()->transact(INfc::OPEN, _aidl_data, &_aidl_reply);
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
  _aidl_ret_status = _aidl_reply.readInt32(&retval);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }


  // Invoke callback to client
  _cb(retval);


_aidl_error:
  _aidl_status.setFromStatusT(_aidl_ret_status);
  return _aidl_status;
}
::android::hidl::binder::Status BpNfc::write(const INfc::nfc_data_t* data , INfc::write_cb _cb ) {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
  int32_t retval;

  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeBuffer((void *)data, sizeof(data));
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }

  _aidl_ret_status = remote()->transact(INfc::WRITE, _aidl_data, &_aidl_reply);
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
  _aidl_ret_status = _aidl_reply.readInt32(&retval);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }


  // Invoke callback to client
  _cb(retval);


_aidl_error:
  _aidl_status.setFromStatusT(_aidl_ret_status);
  return _aidl_status;
}
::android::hidl::binder::Status BpNfc::core_initialized(hidl_vec<uint8_t> data, INfc::core_initialized_cb _cb ) {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
  int32_t retval;

  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }

  _aidl_ret_status = remote()->transact(INfc::CORE_INITIALIZED, _aidl_data, &_aidl_reply);
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
  _aidl_ret_status = _aidl_reply.readInt32(&retval);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }


  // Invoke callback to client
  _cb(retval);


_aidl_error:
  _aidl_status.setFromStatusT(_aidl_ret_status);
  return _aidl_status;
}
::android::hidl::binder::Status BpNfc::pre_discover(INfc::pre_discover_cb _cb ) {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
  int32_t retval;

  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }

  _aidl_ret_status = remote()->transact(INfc::PRE_DISCOVER, _aidl_data, &_aidl_reply);
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
  _aidl_ret_status = _aidl_reply.readInt32(&retval);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }


  // Invoke callback to client
  _cb(retval);


_aidl_error:
  _aidl_status.setFromStatusT(_aidl_ret_status);
  return _aidl_status;
}
::android::hidl::binder::Status BpNfc::close(INfc::close_cb _cb ) {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
  int32_t retval;

  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }

  _aidl_ret_status = remote()->transact(INfc::CLOSE, _aidl_data, &_aidl_reply);
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
  _aidl_ret_status = _aidl_reply.readInt32(&retval);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }


  // Invoke callback to client
  _cb(retval);


_aidl_error:
  _aidl_status.setFromStatusT(_aidl_ret_status);
  return _aidl_status;
}
::android::hidl::binder::Status BpNfc::control_granted(INfc::control_granted_cb _cb ) {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
  int32_t retval;

  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }

  _aidl_ret_status = remote()->transact(INfc::CONTROL_GRANTED, _aidl_data, &_aidl_reply);
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
  _aidl_ret_status = _aidl_reply.readInt32(&retval);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }


  // Invoke callback to client
  _cb(retval);


_aidl_error:
  _aidl_status.setFromStatusT(_aidl_ret_status);
  return _aidl_status;
}
::android::hidl::binder::Status BpNfc::power_cycle(INfc::power_cycle_cb _cb ) {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
  int32_t retval;

  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }

  _aidl_ret_status = remote()->transact(INfc::POWER_CYCLE, _aidl_data, &_aidl_reply);
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
  _aidl_ret_status = _aidl_reply.readInt32(&retval);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }


  // Invoke callback to client
  _cb(retval);


_aidl_error:
  _aidl_status.setFromStatusT(_aidl_ret_status);
  return _aidl_status;
}


}  // namespace nfc
}  // namespace hardware
}  // namespace android


