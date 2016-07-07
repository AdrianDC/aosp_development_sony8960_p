// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/TemplateAll.cpp
#include <android/hardware/tests/ITypes.h>
#include <android/hardware/tests/BpTypes.h>
#include <iostream>
#include <stdio.h>
using std::cout;
using std::endl;

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
::android::hidl::binder::Status BpTypes::echoInteger(int32_t echo_me, const ITypes::simple_t& my_struct , ITypes::echoInteger_cb _cb ) {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
  const void *_aidl_ret_pointer; // For checking if all the nested buffers are OK
  int32_t _cb_ret;

  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeInt32((int32_t)echo_me);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  {
    uint64_t parent_handle;
    _aidl_ret_status = _aidl_data.writeBuffer((void *)&my_struct, sizeof(my_struct), &parent_handle);
    if (((_aidl_ret_status) != (::android::OK))) {
      goto _aidl_error;
    }
      // struct fixup write_

//field int1 doesn't need fixup

//field int2 doesn't need fixup

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
  const void *_aidl_ret_pointer; // For checking if all the nested buffers are OK
  ITypes::eu8
 _cb_a;
ITypes::es8
 _cb_b;
ITypes::eu16
 _cb_c;
ITypes::es16
 _cb_d;
ITypes::eu32
 _cb_e;
ITypes::es32
 _cb_f;
ITypes::eu64
 _cb_g;
ITypes::es64
 _cb_h;
ITypes::ec
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
  const void *_aidl_ret_pointer; // For checking if all the nested buffers are OK
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
::android::hidl::binder::Status BpTypes::echoStruct(const ITypes::s0& s , ITypes::echoStruct_cb _cb ) {
  ::android::hidl::Parcel _aidl_data;
  ::android::hidl::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hidl::binder::Status _aidl_status;
  const void *_aidl_ret_pointer; // For checking if all the nested buffers are OK
    ITypes::s0 *_cb_s ;

  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  {
    uint64_t parent_handle;
    _aidl_ret_status = _aidl_data.writeBuffer((void *)&s, sizeof(s), &parent_handle);
    if (((_aidl_ret_status) != (::android::OK))) {
      goto _aidl_error;
    }
      // struct fixup write_
  // struct fixup write_
  _aidl_ret_status = _aidl_data.writeEmbeddedBuffer((void *)s.s1m1.str1.buffer,
                       (s.s1m1.str1.length < 0 ? strlen(s.s1m1.str1.buffer)+1 : s.s1m1.str1.length),
                       nullptr, parent_handle,
                       (size_t)((char *)&(s.s1m1.str1.buffer)-(char *)(&s)));
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  {
    uint64_t child_handle;
    void *bufp = (void *)s.s1v2.buffer;
    size_t size = sizeof(ITypes::s1s
[s.s1v2.count]);
    size_t off = (size_t)((char *)&(s.s1v2.buffer)-(char *)(&s));
    _aidl_ret_status = _aidl_data.writeEmbeddedBuffer(bufp,
                                              size,
                                              &child_handle,
                                              parent_handle,
                                              off /* offset_calculator */);
     // Redefining a variable inside braces works like a static stack.
    uint64_t parent_handle = child_handle;
    if (((_aidl_ret_status) != (::android::OK))) {
      goto _aidl_error;
    }
        for (size_t i1 = 0; i1 < s.s1v2.count; i1++) {
      /* fixup_write_vec */
        // struct fixup write_
  _aidl_ret_status = _aidl_data.writeEmbeddedBuffer((void *)s.s1v2.buffer[i1].str1.buffer,
                       (s.s1v2.buffer[i1].str1.length < 0 ? strlen(s.s1v2.buffer[i1].str1.buffer)+1 : s.s1v2.buffer[i1].str1.length),
                       nullptr, parent_handle,
                       (size_t)((char *)&(s.s1v2.buffer[i1].str1.buffer)-(char *)(s.s1v2.buffer)));
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }

    }

  }

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
  {
    uint64_t parent_handle;
    _cb_s = (ITypes::s0 *)_aidl_reply.readBuffer(&parent_handle);
    if ((_cb_s) == nullptr) {
      goto _aidl_error;
    }
      // struct fixup read_ret_
  // struct fixup read_ret_
  _aidl_ret_pointer = _aidl_data.readEmbeddedBuffer(nullptr, parent_handle,
                       (size_t)((char *)&((*_cb_s).s1m1.str1.buffer)-(char *)(&(*_cb_s))));
  if (_aidl_ret_pointer == nullptr) {
      goto _aidl_error;
  }
  {
    uint64_t child_handle;
    _aidl_ret_pointer = _aidl_data.readEmbeddedBuffer(&child_handle, parent_handle,
                             (size_t)((char *)&((*_cb_s).s1v2.buffer)-(char *)(&(*_cb_s))));
     // Redefining a variable inside braces works like a static stack.
    uint64_t parent_handle = child_handle;
    if (_aidl_ret_pointer == nullptr) {
      goto _aidl_error;
    }
        for (size_t i1 = 0; i1 < (*_cb_s).s1v2.count; i1++) {
      /* fixup_write_vec */
        // struct fixup read_ret_
  _aidl_ret_pointer = _aidl_data.readEmbeddedBuffer(nullptr, parent_handle,
                       (size_t)((char *)&((*_cb_s).s1v2.buffer[i1].str1.buffer)-(char *)((*_cb_s).s1v2.buffer)));
  if (_aidl_ret_pointer == nullptr) {
      goto _aidl_error;
  }

    }

  }

  }


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
  const void *_aidl_ret_pointer; // For checking if all the nested buffers are OK
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
  const void *_aidl_ret_pointer; // For checking if all the nested buffers are OK
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
  const void *_aidl_ret_pointer; // For checking if all the nested buffers are OK
  
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


//
#include <android/hardware/tests/ITypes.h>
#include <android/hardware/tests/BpTypes.h>


#include <iostream>
#include <android/hardware/tests/BnTypes.h>
#include <binder/Parcel.h>

namespace android {
namespace hardware {
namespace tests {


::android::status_t BnTypes::onTransact(uint32_t _aidl_code, const ::android::hidl::Parcel& _aidl_data, ::android::hidl::Parcel* _aidl_reply, uint32_t _aidl_flags, TransactCallback _cb) {
  ::android::status_t _aidl_ret_status = ::android::OK;
  const void *_aidl_ret_pointer; // For checking if all the nested buffers are OK
  switch (_aidl_code) {
    case Call::ECHOINTEGER:
      {
int32_t echo_me;
  ITypes::simple_t *my_struct ;

        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }
        _aidl_ret_status = _aidl_data.readInt32(&echo_me);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
  {
    uint64_t parent_handle;
    my_struct = (ITypes::simple_t *)_aidl_data.readBuffer(&parent_handle);
    if ((my_struct) == nullptr) {
      break;
    }
      // struct fixup read_

//field int1 doesn't need fixup

//field int2 doesn't need fixup

  }

        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             echoInteger(echo_me , *my_struct ,          [&](auto ret ) {
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                             // Serialize
                             _aidl_reply->writeInt32(ret);

                            // Callback
                             _cb(*_aidl_reply);
                           }
));

//

        if (!callback_called) {
          // Callback not called, the call must have returned an error
          // TODO set something like ERR_NO_CALLBACK if the call retuned OK
          _aidl_ret_status = _aidl_status.writeToParcel(_aidl_reply);
        }

        break;
      }
    case Call::ECHOENUM:
      {
ITypes::eu8
 a;
ITypes::es8
 b;
ITypes::eu16
 c;
ITypes::es16
 d;
ITypes::eu32
 e;
ITypes::es32
 f;
ITypes::eu64
 g;
ITypes::es64
 h;
ITypes::ec
 i;

        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }
        _aidl_ret_status = _aidl_data.readByte((int8_t *)&a);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
        _aidl_ret_status = _aidl_data.readByte((int8_t *)&b);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
        _aidl_ret_status = _aidl_data.readChar((char16_t *)&c);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
        _aidl_ret_status = _aidl_data.readChar((char16_t *)&d);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
        _aidl_ret_status = _aidl_data.readUint32((uint32_t *)&e);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
        _aidl_ret_status = _aidl_data.readInt32((int32_t *)&f);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
        _aidl_ret_status = _aidl_data.readUint64((uint64_t *)&g);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
        _aidl_ret_status = _aidl_data.readInt64((int64_t *)&h);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
        _aidl_ret_status = _aidl_data.readByte((int8_t *)&i);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }

        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             echoEnum(a , b , c , d , e , f , g , h , i ,          [&](auto a , auto b , auto c , auto d , auto e , auto f , auto g , auto h , auto i ) {
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                             // Serialize
        _aidl_ret_status = _aidl_reply->writeByte((int8_t)a);
        if (((_aidl_ret_status) != (::android::OK))) {
          return;
        }
        _aidl_ret_status = _aidl_reply->writeByte((int8_t)b);
        if (((_aidl_ret_status) != (::android::OK))) {
          return;
        }
        _aidl_ret_status = _aidl_reply->writeChar((char16_t)c);
        if (((_aidl_ret_status) != (::android::OK))) {
          return;
        }
        _aidl_ret_status = _aidl_reply->writeChar((char16_t)d);
        if (((_aidl_ret_status) != (::android::OK))) {
          return;
        }
        _aidl_ret_status = _aidl_reply->writeUint32((uint32_t)e);
        if (((_aidl_ret_status) != (::android::OK))) {
          return;
        }
        _aidl_ret_status = _aidl_reply->writeInt32((int32_t)f);
        if (((_aidl_ret_status) != (::android::OK))) {
          return;
        }
        _aidl_ret_status = _aidl_reply->writeUint64((uint64_t)g);
        if (((_aidl_ret_status) != (::android::OK))) {
          return;
        }
        _aidl_ret_status = _aidl_reply->writeInt64((int64_t)h);
        if (((_aidl_ret_status) != (::android::OK))) {
          return;
        }
        _aidl_ret_status = _aidl_reply->writeByte((int8_t)i);
        if (((_aidl_ret_status) != (::android::OK))) {
          return;
        }

                            // Callback
                             _cb(*_aidl_reply);
                           }
));

//

        if (!callback_called) {
          // Callback not called, the call must have returned an error
          // TODO set something like ERR_NO_CALLBACK if the call retuned OK
          _aidl_ret_status = _aidl_status.writeToParcel(_aidl_reply);
        }

        break;
      }
    case Call::ECHOSCALAR:
      {
uint8_t a;
int8_t b;
uint16_t c;
int16_t d;
uint32_t e;
int32_t f;
uint64_t g;
int64_t h;
char i;

        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }
        _aidl_ret_status = _aidl_data.readByte((int8_t*)&a);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
        _aidl_ret_status = _aidl_data.readByte(&b);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
        _aidl_ret_status = _aidl_data.readChar((char16_t*)&c);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
        _aidl_ret_status = _aidl_data.readChar((char16_t*)&d);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
        _aidl_ret_status = _aidl_data.readUint32(&e);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
        _aidl_ret_status = _aidl_data.readInt32(&f);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
        _aidl_ret_status = _aidl_data.readUint64(&g);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
        _aidl_ret_status = _aidl_data.readInt64(&h);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }
        _aidl_ret_status = _aidl_data.readByte((int8_t*)&i);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }

        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             echoScalar(a , b , c , d , e , f , g , h , i ,          [&](auto a , auto b , auto c , auto d , auto e , auto f , auto g , auto h , auto i ) {
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                             // Serialize
                             _aidl_reply->writeByte((int8_t)a);
                             _aidl_reply->writeByte(b);
                             _aidl_reply->writeChar((char16_t)c);
                             _aidl_reply->writeChar((char16_t)d);
                             _aidl_reply->writeUint32(e);
                             _aidl_reply->writeInt32(f);
                             _aidl_reply->writeUint64(g);
                             _aidl_reply->writeInt64(h);
                             _aidl_reply->writeByte((int8_t)i);

                            // Callback
                             _cb(*_aidl_reply);
                           }
));

//

        if (!callback_called) {
          // Callback not called, the call must have returned an error
          // TODO set something like ERR_NO_CALLBACK if the call retuned OK
          _aidl_ret_status = _aidl_status.writeToParcel(_aidl_reply);
        }

        break;
      }
    case Call::ECHOSTRUCT:
      {
  ITypes::s0 *s ;

        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }
  {
    uint64_t parent_handle;
    s = (ITypes::s0 *)_aidl_data.readBuffer(&parent_handle);
    if ((s) == nullptr) {
      break;
    }
      // struct fixup read_
  // struct fixup read_
  _aidl_ret_pointer = _aidl_data.readEmbeddedBuffer(nullptr, parent_handle,
                       (size_t)((char *)&((*s).s1m1.str1.buffer)-(char *)(&(*s))));
  if (_aidl_ret_pointer == nullptr) {
      break;
  }
  {
    uint64_t child_handle;
    _aidl_ret_pointer = _aidl_data.readEmbeddedBuffer(&child_handle, parent_handle,
                             (size_t)((char *)&((*s).s1v2.buffer)-(char *)(&(*s))));
     // Redefining a variable inside braces works like a static stack.
    uint64_t parent_handle = child_handle;
    if (_aidl_ret_pointer == nullptr) {
      break;
    }
        for (size_t i1 = 0; i1 < (*s).s1v2.count; i1++) {
      /* fixup_write_vec */
        // struct fixup read_
  _aidl_ret_pointer = _aidl_data.readEmbeddedBuffer(nullptr, parent_handle,
                       (size_t)((char *)&((*s).s1v2.buffer[i1].str1.buffer)-(char *)((*s).s1v2.buffer)));
  if (_aidl_ret_pointer == nullptr) {
      break;
  }

    }

  }

  }

        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             echoStruct(*s ,          [&](const ITypes::s0 & s ) {
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                             // Serialize
  {
    uint64_t parent_handle;
    _aidl_ret_status = _aidl_reply->writeBuffer((void *)&s, sizeof(s), &parent_handle);
      // struct fixup write_ret_
  // struct fixup write_ret_
  _aidl_ret_status = _aidl_reply->writeEmbeddedBuffer((void *)s.s1m1.str1.buffer,
                       (s.s1m1.str1.length < 0 ? strlen(s.s1m1.str1.buffer)+1 : s.s1m1.str1.length),
                       nullptr, parent_handle,
                       (size_t)((char *)&(s.s1m1.str1.buffer)-(char *)(&s)));
  {
    uint64_t child_handle;
    void *bufp = (void *)s.s1v2.buffer;
    size_t size = sizeof(ITypes::s1s
[s.s1v2.count]);
    size_t off = (size_t)((char *)&(s.s1v2.buffer)-(char *)(&s));
    _aidl_ret_status = _aidl_reply->writeEmbeddedBuffer(bufp,
                                              size,
                                              &child_handle,
                                              parent_handle,
                                              off /* offset_calculator */);
     // Redefining a variable inside braces works like a static stack.
    uint64_t parent_handle = child_handle;
        for (size_t i1 = 0; i1 < s.s1v2.count; i1++) {
      /* fixup_write_vec */
        // struct fixup write_ret_
  _aidl_ret_status = _aidl_reply->writeEmbeddedBuffer((void *)s.s1v2.buffer[i1].str1.buffer,
                       (s.s1v2.buffer[i1].str1.length < 0 ? strlen(s.s1v2.buffer[i1].str1.buffer)+1 : s.s1v2.buffer[i1].str1.length),
                       nullptr, parent_handle,
                       (size_t)((char *)&(s.s1v2.buffer[i1].str1.buffer)-(char *)(s.s1v2.buffer)));

    }

  }

  }

                            // Callback
                             _cb(*_aidl_reply);
                           }
));

//

        if (!callback_called) {
          // Callback not called, the call must have returned an error
          // TODO set something like ERR_NO_CALLBACK if the call retuned OK
          _aidl_ret_status = _aidl_status.writeToParcel(_aidl_reply);
        }

        break;
      }
    case Call::SHAREBUFFERWITHREF:
      {
hidl_ref<lots_of_data> buffer;

        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }
        buffer = _aidl_data.readFileDescriptor();

        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             shareBufferWithRef(buffer ,          [&](auto ret ) {
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                             // Serialize
                             _aidl_reply->writeInt32(ret);

                            // Callback
                             _cb(*_aidl_reply);
                           }
));

//

        if (!callback_called) {
          // Callback not called, the call must have returned an error
          // TODO set something like ERR_NO_CALLBACK if the call retuned OK
          _aidl_ret_status = _aidl_status.writeToParcel(_aidl_reply);
        }

        break;
      }
    case Call::GETHASH:
      {

        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }

        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             getHash(        [&](auto hash ) {
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                             // Serialize
                             _aidl_reply->writeUint64(hash);

                            // Callback
                             _cb(*_aidl_reply);
                           }
));

//

        if (!callback_called) {
          // Callback not called, the call must have returned an error
          // TODO set something like ERR_NO_CALLBACK if the call retuned OK
          _aidl_ret_status = _aidl_status.writeToParcel(_aidl_reply);
        }

        break;
      }
    case Call::QUIT:
      {

        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }

        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             quit());

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

}  // namespace tests
}  // namespace hardware
}  // namespace android

