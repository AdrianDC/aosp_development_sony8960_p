// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/TemplateProxy.cpp
#include <android/hardware/tests/ITypes.h>
#include <android/hardware/tests/BpTypes.h>

namespace android {
namespace hardware {
namespace tests {


IMPLEMENT_META_INTERFACE(Types, "android.hardware.tests.ITypes");

}  // namespace tests
}  // namespace hardware
}  // namespace android


#include <android/hardware/tests/BpTypes.h>
#include <binder/Parcel.h>

namespace android {
namespace hardware {
namespace tests {


BpTypes::BpTypes(const ::android::sp<::android::hidl::IBinder>& _aidl_impl)
    : BpInterface<ITypes>(_aidl_impl){
}
::android::hidl::binder::Status BpTypes::echoInteger(int32_t echo_me, const ITypes::simple_t* my_struct , ITypes::echoInteger_cb _cb ) {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
  int32_t _cb_ret;

  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeInt32((int32_t)echo_me);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeBuffer((void *)my_struct, sizeof(my_struct), nullptr);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }

  _aidl_ret_status = remote()->transact(ITypes::ECHOINTEGER, _aidl_data, &_aidl_reply);
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
  _aidl_ret_status = _aidl_reply.readInt32(&_cb_ret);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }


  // Invoke callback to client
  if (_cb != nullptr)
  _cb(_cb_ret );


_aidl_error:
  _aidl_status.setFromStatusT(_aidl_ret_status);
  return _aidl_status;
}
::android::hidl::binder::Status BpTypes::echoEnum(eu8 a, es8 b, eu16 c, es16 d, eu32 e, es32 f, eu64 g, es64 h, ec i, ITypes::echoEnum_cb _cb ) {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
   _cb_a;
 _cb_b;
 _cb_c;
 _cb_d;
 _cb_e;
 _cb_f;
 _cb_g;
 _cb_h;
 _cb_i;

  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeByte((int8_t)a);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeByte((int8_t)b);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeChar((char16_t)c);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeChar((char16_t)d);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeUint32((uint32_t)e);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeInt32((int32_t)f);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeUint64((uint64_t)g);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeInt64((int64_t)h);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeByte((int8_t)i);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }

  _aidl_ret_status = remote()->transact(ITypes::ECHOENUM, _aidl_data, &_aidl_reply);
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
  _aidl_ret_status = _aidl_reply.readByte((int8_t *)&_cb_a);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_reply.readByte((int8_t *)&_cb_b);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_reply.readChar((char16_t *)&_cb_c);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_reply.readChar((char16_t *)&_cb_d);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_reply.readUint32((uint32_t *)&_cb_e);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_reply.readInt32((int32_t *)&_cb_f);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_reply.readUint64((uint64_t *)&_cb_g);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_reply.readInt64((int64_t *)&_cb_h);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_reply.readByte((int8_t *)&_cb_i);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }


  // Invoke callback to client
  if (_cb != nullptr)
  _cb(_cb_a , _cb_b , _cb_c , _cb_d , _cb_e , _cb_f , _cb_g , _cb_h , _cb_i );


_aidl_error:
  _aidl_status.setFromStatusT(_aidl_ret_status);
  return _aidl_status;
}
::android::hidl::binder::Status BpTypes::echoScalar(uint8_t a, int8_t b, uint16_t c, int16_t d, uint32_t e, int32_t f, uint64_t g, int64_t h, char i, ITypes::echoScalar_cb _cb ) {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
  uint8_t _cb_a;
int8_t _cb_b;
uint16_t _cb_c;
int16_t _cb_d;
uint32_t _cb_e;
int32_t _cb_f;
uint64_t _cb_g;
int64_t _cb_h;
char _cb_i;

  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeByte((int8_t)a);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeByte((int8_t)b);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeChar((char16_t)c);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeChar((char16_t)d);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeUint32((uint32_t)e);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeInt32((int32_t)f);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeUint64((uint64_t)g);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeInt64((int64_t)h);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeByte((int8_t)i);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }

  _aidl_ret_status = remote()->transact(ITypes::ECHOSCALAR, _aidl_data, &_aidl_reply);
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
  _aidl_ret_status = _aidl_reply.readByte((int8_t*)&_cb_a);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_reply.readByte(&_cb_b);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_reply.readChar((char16_t*)&_cb_c);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_reply.readChar((char16_t*)&_cb_d);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_reply.readUint32(&_cb_e);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_reply.readInt32(&_cb_f);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_reply.readUint64(&_cb_g);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_reply.readInt64(&_cb_h);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_reply.readByte((int8_t*)&_cb_i);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }


  // Invoke callback to client
  if (_cb != nullptr)
  _cb(_cb_a , _cb_b , _cb_c , _cb_d , _cb_e , _cb_f , _cb_g , _cb_h , _cb_i );


_aidl_error:
  _aidl_status.setFromStatusT(_aidl_ret_status);
  return _aidl_status;
}
::android::hidl::binder::Status BpTypes::echoStruct(const ITypes::s0* s , ITypes::echoStruct_cb _cb ) {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
  s0 *_cb_s; ;

  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeBuffer((void *)s, sizeof(s), nullptr);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }

  _aidl_ret_status = remote()->transact(ITypes::ECHOSTRUCT, _aidl_data, &_aidl_reply);
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
  _cb_s = (ITypes::s0 *)_aidl_data.readBuffer();


  // Invoke callback to client
  if (_cb != nullptr)
  _cb(*_cb_s );


_aidl_error:
  _aidl_status.setFromStatusT(_aidl_ret_status);
  return _aidl_status;
}
::android::hidl::binder::Status BpTypes::shareBufferWithRef(hidl_ref<lots_of_data> buffer, ITypes::shareBufferWithRef_cb _cb ) {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
  int32_t _cb_ret;

  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeFileDescriptor(buffer);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }

  _aidl_ret_status = remote()->transact(ITypes::SHAREBUFFERWITHREF, _aidl_data, &_aidl_reply);
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
  _aidl_ret_status = _aidl_reply.readInt32(&_cb_ret);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }


  // Invoke callback to client
  if (_cb != nullptr)
  _cb(_cb_ret );


_aidl_error:
  _aidl_status.setFromStatusT(_aidl_ret_status);
  return _aidl_status;
}
::android::hidl::binder::Status BpTypes::getHash(ITypes::getHash_cb _cb ) {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
  uint64_t _cb_hash;

  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }

  _aidl_ret_status = remote()->transact(ITypes::GETHASH, _aidl_data, &_aidl_reply);
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
  _aidl_ret_status = _aidl_reply.readUint64(&_cb_hash);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }


  // Invoke callback to client
  if (_cb != nullptr)
  _cb(_cb_hash );


_aidl_error:
  _aidl_status.setFromStatusT(_aidl_ret_status);
  return _aidl_status;
}
::android::hidl::binder::Status BpTypes::quit() {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
  
  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }

  _aidl_ret_status = remote()->transact(ITypes::QUIT, _aidl_data, &_aidl_reply);
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


}  // namespace tests
}  // namespace hardware
}  // namespace android


