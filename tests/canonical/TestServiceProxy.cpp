// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/TemplateProxy.cpp
#include <android/hardware/tests/ITestService.h>
#include <android/hardware/tests/BpTestService.h>

namespace android {
namespace hardware {
namespace tests {


IMPLEMENT_META_INTERFACE(TestService, "android.hardware.tests.ITestService");

}  // namespace tests
}  // namespace hardware
}  // namespace android


#include <android/hardware/tests/BpTestService.h>
#include <binder/Parcel.h>

namespace android {
namespace hardware {
namespace tests {


BpTestService::BpTestService(const ::android::sp<::android::hidl::IBinder>& _aidl_impl)
    : BpInterface<ITestService>(_aidl_impl){
}
::android::hidl::binder::Status BpTestService::echoInteger(int32_t echo_me, const ITestService::simple_t* my_struct , ITestService::echoInteger_cb _cb ) {
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

  _aidl_ret_status = remote()->transact(ITestService::ECHOINTEGER, _aidl_data, &_aidl_reply);
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
::android::hidl::binder::Status BpTestService::shareBufferWithRef(hidl_ref<lots_of_data> buffer, ITestService::shareBufferWithRef_cb _cb ) {
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

  _aidl_ret_status = remote()->transact(ITestService::SHAREBUFFERWITHREF, _aidl_data, &_aidl_reply);
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


