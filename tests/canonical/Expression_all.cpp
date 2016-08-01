// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/TemplateAll.cpp
//#include <android/hardware/tests/IExpression.h>
//#include <android/hardware/tests/BpExpression.h>
#include <IExpression.h>
#include <BpExpression.h>
#include <iostream>
#include <stdio.h>
using std::cout;
using std::endl;

namespace android {
namespace hardware {
namespace tests {


IMPLEMENT_HWBINDER_META_INTERFACE(Expression, "android.hardware.tests.IExpression");

}  // namespace tests
}  // namespace hardware
}  // namespace android


//#include <android/hardware/tests/BpExpression.h>
#include <BpExpression.h>
#include <binder/Parcel.h>

namespace android {
namespace hardware {
namespace tests {


BpExpression::BpExpression(const ::android::sp<::android::hardware::IBinder>& _aidl_impl)
    : BpInterface<IExpression>(_aidl_impl){
}
::android::hardware::Status BpExpression::setExpression(const IExpression::expressionist_t& state , int32_t mask, IExpression::setExpression_cb _cb ) {
  ::android::hardware::Parcel _aidl_data;
  ::android::hardware::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::hardware::Status _aidl_status;
  const void *_aidl_ret_pointer; // For checking if all the nested buffers are OK
  int32_t _cb_ret;

  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  {
    size_t parent_handle;
    _aidl_ret_status = _aidl_data.writeBuffer((void *)&state, sizeof(state), &parent_handle);
    if (((_aidl_ret_status) != (::android::OK))) {
      goto _aidl_error;
    }
      // struct fixup write_

//field buffer doesn't need fixup
  {
    size_t child_handle;
    void *bufp = (void *)state.expmap.buffer;
    size_t size = sizeof(hidl_vec<uint8_t>
[state.expmap.count]);
    size_t off = (size_t)((char *)&(state.expmap.buffer)-(char *)(&state));
    _aidl_ret_status = _aidl_data.writeEmbeddedBuffer(bufp,
                                              size,
                                              &child_handle,
                                              parent_handle,
                                              off /* offset_calculator */);
     // Redefining a variable inside braces works like a static stack.
    size_t parent_handle = child_handle;
    if (((_aidl_ret_status) != (::android::OK))) {
      goto _aidl_error;
    }
        for (size_t i1 = 0; i1 < state.expmap.count; i1++) {
      /* fixup_write_vec */
        {
    size_t child_handle;
    void *bufp = (void *)state.expmap.buffer[i1].buffer;
    size_t size = sizeof(uint8_t[state.expmap.buffer[i1].count]);
    size_t off = (size_t)((char *)&(state.expmap.buffer[i1].buffer)-(char *)(state.expmap.buffer));
    _aidl_ret_status = _aidl_data.writeEmbeddedBuffer(bufp,
                                              size,
                                              &child_handle,
                                              parent_handle,
                                              off /* offset_calculator */);
     // Redefining a variable inside braces works like a static stack.
    size_t parent_handle = child_handle;
    if (((_aidl_ret_status) != (::android::OK))) {
      goto _aidl_error;
    }
    // Fixup not needed for this vec uint8_t
  }

    }

  }

  }
  _aidl_ret_status = _aidl_data.writeInt32((int32_t)mask);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }

  _aidl_ret_status = remote()->transact(IExpression::SETEXPRESSION, _aidl_data, &_aidl_reply);
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


}  // namespace tests
}  // namespace hardware
}  // namespace android


//
//#include <android/hardware/tests/IExpression.h>
//#include <android/hardware/tests/BpExpression.h>
#include <IExpression.h>
#include <BpExpression.h>


#include <iostream>
//#include <android/hardware/tests/BnExpression.h>
#include <BnExpression.h>
#include <binder/Parcel.h>

namespace android {
namespace hardware {
namespace tests {


::android::status_t BnExpression::onTransact(uint32_t _aidl_code, const ::android::hardware::Parcel& _aidl_data, ::android::hardware::Parcel* _aidl_reply, uint32_t _aidl_flags, TransactCallback _cb) {
  ::android::status_t _aidl_ret_status = ::android::OK;
  const void *_aidl_ret_pointer; // For checking if all the nested buffers are OK
  switch (_aidl_code) {
    case Call::SETEXPRESSION:
      {
  IExpression::expressionist_t *state ;
int32_t mask;

        bool callback_called;
        if (!(_aidl_data.checkInterface(this))) {
          _aidl_ret_status = ::android::BAD_TYPE;
          break;
        }
  {
    size_t parent_handle;
    state = (IExpression::expressionist_t *)_aidl_data.readBuffer(&parent_handle);
    if ((state) == nullptr) {
      break;
    }
      // struct fixup read_

//field buffer doesn't need fixup
  {
    size_t child_handle;
    _aidl_ret_pointer = _aidl_data.readEmbeddedBuffer(&child_handle, parent_handle,
                             (size_t)((char *)&((*state).expmap.buffer)-(char *)(&(*state))));
     // Redefining a variable inside braces works like a static stack.
    size_t parent_handle = child_handle;
    if (_aidl_ret_pointer == nullptr) {
      break;
    }
        for (size_t i1 = 0; i1 < (*state).expmap.count; i1++) {
      /* fixup_write_vec */
        {
    size_t child_handle;
    _aidl_ret_pointer = _aidl_data.readEmbeddedBuffer(&child_handle, parent_handle,
                             (size_t)((char *)&((*state).expmap.buffer[i1].buffer)-(char *)((*state).expmap.buffer)));
     // Redefining a variable inside braces works like a static stack.
    size_t parent_handle = child_handle;
    if (_aidl_ret_pointer == nullptr) {
      break;
    }
    // Fixup not needed for this vec uint8_t
  }

    }

  }

  }
        _aidl_ret_status = _aidl_data.readInt32(&mask);
        if (((_aidl_ret_status) != (::android::OK))) {
          break;
        }

        // Make the call into the server
        ::android::hardware::Status _aidl_status(
             setExpression(*state , mask ,          [&](auto ret ) {
                             callback_called = true;
                             // Write "OK" to parcel
                             ::android::hardware::Status::ok().writeToParcel(_aidl_reply);
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

    default:
      {
        _aidl_ret_status = ::android::hardware::BBinder::onTransact(_aidl_code, _aidl_data, _aidl_reply, _aidl_flags);
      }
      break;
  }
  if (_aidl_ret_status == ::android::UNEXPECTED_NULL) {
    _aidl_ret_status = ::android::hardware::Status::fromExceptionCode(::android::hardware::Status::EX_NULL_POINTER).writeToParcel(_aidl_reply);
  }
  return _aidl_ret_status;
}

}  // namespace tests
}  // namespace hardware
}  // namespace android

