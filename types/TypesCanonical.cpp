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
      /*fixup_write_struct*/

//field int1 doesn't need fixup

//field int2 doesn't need fixup
;
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
  eu8 _cb_a;
es8 _cb_b;
eu16 _cb_c;
es16 _cb_d;
eu32 _cb_e;
es32 _cb_f;
eu64 _cb_g;
es64 _cb_h;
ec _cb_i;

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
    ITypes::s0 *_cb_s ;

  cout << "Called echoStruct" << endl;
  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  {
    uint64_t parent_handle;
    cout << "Client: calling writeBuffer in echoStruct. ptr " << &s << " size " << sizeof(s0) << endl;
    _aidl_ret_status = _aidl_data.writeBuffer((void *)&s, sizeof(s), &parent_handle);
    cout << " parent handle " << parent_handle << endl;
    if (((_aidl_ret_status) != (::android::OK))) {
      goto _aidl_error;
    }
      /*fixup_write_struct*/
  /*fixup_write_struct*/
  _aidl_ret_status = _aidl_data.writeBuffer((void *)s.s1m1.str1.buffer,
                       (s.s1m1.str1.length < 0 ? strlen(s.s1m1.str1.buffer)+1 : s.s1m1.str1.length),
                       nullptr, parent_handle,
                       (size_t)((char *)&(s.s1m1.str1.buffer)-(char *)(&s)));
    printf("CL: Writing first string buffer - ptr %p, len %ld, handle %ld, offset %ld ret_status %d\n", (void *)s.s1m1.str1.buffer, (s.s1m1.str1.length < 0 ? strlen(s.s1m1.str1.buffer)+1 : s.s1m1.str1.length),
           parent_handle, (size_t)((char *)&(s.s1m1.str1.buffer)-(char *)&(s)) , _aidl_ret_status);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  {
    uint64_t child_handle;
    cout << "Client: calling writeBuffer in echoStruct. ptr " << &s << " size " << sizeof(s0) << endl;
    void *bufp = (void *)s.s1v2.buffer;
    size_t size = sizeof(ITypes::s1s[s.s1v2.count]);
    size_t off = (size_t)((char *)&(s.s1v2.buffer)-(char *)&(s));
    _aidl_ret_status = _aidl_data.writeBuffer(bufp,
                                              size,
                                              &child_handle,
                                              parent_handle,
                                              off /* offset_calculator */);
    printf("c: wb vec : buf %p size %ld offset %ld parent %ld child %ld ret %d\n", bufp, size, off, parent_handle, child_handle, _aidl_ret_status);
    uint64_t parent_handle = child_handle; // Yes, this is legal, and works; it's like a static stack.
    if (((_aidl_ret_status) != (::android::OK))) {
      goto _aidl_error;
    }
    for (size_t i1 = 0; i1 < s.s1v2.count; i1++) {
      /* fixup_write_vec */
        /*fixup_write_struct*/
  _aidl_ret_status = _aidl_data.writeBuffer((void *)s.s1v2.buffer[i1].str1.buffer,
                       (s.s1v2.buffer[i1].str1.length < 0 ? strlen(s.s1v2.buffer[i1].str1.buffer)+1 : s.s1v2.buffer[i1].str1.length),
                       nullptr, parent_handle,
                       (size_t)((char *)&(s.s1v2.buffer[i1].str1.buffer)-(char *)(s.s1v2.buffer)));
        printf("c: wb vec-Str%ld : buf %p size %ld offset %ld parent %ld ret %d\n", i1, bufp, size, off, parent_handle, _aidl_ret_status);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }

    }
  }
  cout << "Client: Got to transact" << endl;
  }

  _aidl_ret_status = remote()->transact(ITypes::ECHOSTRUCT, _aidl_data, &_aidl_reply);
  printf("Client: Survived transact, status 0x%x\n", _aidl_ret_status);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_status.readFromParcel(_aidl_reply);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  cout << "Client: Survived ReadFromParcel" << endl;
  if (!_aidl_status.isOk()) {
    return _aidl_status;
  }
  _cb_s = (ITypes::s0 *)_aidl_reply.readBuffer();
  cout << "Client: Survived ReadBuffer, _cb_s is " << _cb_s << endl;
  (void)0; /*kilroy was here, reading*/


  // Invoke callback to client
  _cb(*_cb_s );

  cout << "Client: Survived callback" << endl;

_aidl_error:
  cout << "Client: At end, status is " << _aidl_ret_status << endl;
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


/*
return_param_decl_default
return_param_decl_struct_type
*/
#include <android/hardware/tests/ITypes.h>
#include <android/hardware/tests/BpTypes.h>

/*
namespace android {
namespace hardware {
namespace tests {


IMPLEMENT_META_INTERFACE(Types, "android.hardware.tests.ITypes");

}  // namespace tests
}  // namespace hardware
}  // namespace android

*/

#include <iostream>
#include <android/hardware/tests/BnTypes.h>
#include <binder/Parcel.h>

namespace android {
namespace hardware {
namespace tests {


::android::status_t BnTypes::onTransact(uint32_t _aidl_code, const ::android::hidl::Parcel& _aidl_data, ::android::hidl::Parcel* _aidl_reply, uint32_t _aidl_flags, TransactCallback _cb) {
  ::android::status_t _aidl_ret_status = ::android::OK;
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
        my_struct = (ITypes::simple_t *)_aidl_data.readBuffer();
        (void)0; /*kilroy was here, reading*/

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
eu8 a;
es8 b;
eu16 c;
es16 d;
eu32 e;
es32 f;
eu64 g;
es64 h;
ec i;

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

  cout << "Server: got echoStruct call" << endl;
        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }
        s = (ITypes::s0 *)_aidl_data.readBuffer();
        (void)0; /*kilroy was here, reading*/

        void *p = (void*)_aidl_data.readBuffer();
        void *q = (void*)_aidl_data.readBuffer();
        cout <<"server ptrs: "<<s <<" and " <<p <<" and " <<q <<endl;
        // Make the call into the server
        cout << "Server: about to call echoStruct impl" << endl;
        ::android::hidl::binder::Status _aidl_status(
             echoStruct(*s ,          [&](const /*ITypes::s0*/ auto & s ) {
                 std::cout << "Server: In callback" << endl;
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                             // Serialize
                             std::cout << "Server: Struct type passed to cb" << std::endl;
                             _aidl_ret_status = _aidl_reply->writeBuffer((void *)&s, sizeof(s), nullptr);
                             (void)0; /*kilroy_ret was here*/

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
