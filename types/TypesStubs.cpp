// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/TemplateStubs.cpp
#include <android/hardware/tests/ITypes.h>
#include <android/hardware/tests/BpTypes.h>

namespace android {
namespace hardware {
namespace tests {


IMPLEMENT_META_INTERFACE(Types, "android.hardware.tests.ITypes");

}  // namespace tests
}  // namespace hardware
}  // namespace android


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

        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }
        s = (ITypes::s0 *)_aidl_data.readBuffer();

        // Make the call into the server
        ::android::hidl::binder::Status _aidl_status(
             echoStruct(*s ,          [&](auto s ) {
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hidl::binder::Status::ok().writeToParcel(_aidl_reply);
                             // Serialize
                             std::cout << "Struct type passed to cb" << std::endl;

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

