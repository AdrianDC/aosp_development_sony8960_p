#include "hasher.h"

#include <iostream>
#include <map>

#include <unistd.h>

#include <hwbinder/IInterface.h>
#include <hwbinder/IPCThreadState.h>
#include <hwbinder/IServiceManager.h>
#include <hwbinder/ProcessState.h>
#include <hwbinder/Status.h>
#include <nativehelper/ScopedFd.h>
#include <utils/Errors.h>
#include <utils/Log.h>
#include <utils/Looper.h>
#include <utils/StrongPointer.h>

#include <android/hardware/tests/BnTypes.h>
// libutils:
using android::Looper;
using android::LooperCallback;
using android::OK;
using android::sp;
using android::String16;

// libhwbinder:
using android::hidl::BnInterface;
using android::hidl::defaultServiceManager;
using android::hidl::IInterface;
using android::hidl::IPCThreadState;
using android::hidl::Parcel;
using android::hidl::ProcessState;
using android::hidl::binder::Status;
using android::hidl::hidl_version;
using android::hidl::make_hidl_version;
using android::hidl::from_ref;

// Standard library
using std::cerr;
using std::cout;
using std::endl;
using std::map;
using std::string;
using std::unique_ptr;
using std::vector;

// Generated HIDL files
using android::hardware::tests::BnTypes;

namespace {

class BinderCallback : public LooperCallback {
 public:
  BinderCallback() {}
  ~BinderCallback() override {}

  int handleEvent(int /* fd */, int /* events */, void* /* data */) override {
    IPCThreadState::self()->handlePolledCommands();
    return 1;  // Continue receiving callbacks.
  }
};

class Types : public BnTypes {
 public:
  Types() {}
  virtual ~Types() = default;

  virtual Status echoInteger(int32_t echo_me, const ITypes::simple_t* my_struct, ITypes::echoInteger_cb callback) {
    //std::cout << "struct at addr " << (void *)my_struct << std::endl;
    //std::cout << "int1 " << my_struct->int1 << " int 2 " << my_struct->int2 << std::endl;
    addHash(echo_me);
    addHash(my_struct->int1);
    addHash(my_struct->int2);
    callback(echo_me);
    return Status::ok();
  }
  virtual Status shareBufferWithRef(int buffer, ITypes::shareBufferWithRef_cb callback) {
    // Map back to struct lots_of_data
    ITypes::lots_of_data* dataPtr;
    if (from_ref(buffer, &dataPtr)) {
      // Failed
      callback(-1);
    } else {
      // cout << "Data element at position 1481 is " << unsigned(dataPtr->buffer[1481]) << endl;
      addHash(dataPtr->buffer[1481]);
      callback(int32_t(dataPtr->buffer[1481]));
    }
    return Status::ok();
  }
  virtual Status echoEnum(eu8 a, es8 b, eu16 c, es16 d, eu32 e, es32 f, eu64 g, es64 h, ec i, ITypes::echoEnum_cb cb) {
    addHash((uint8_t)a);
    addHash((int8_t)b);
    addHash((uint16_t)c);
    addHash((int16_t)d);
    addHash((uint32_t)e);
    addHash((int32_t)f);
    addHash((uint64_t)g);
    addHash((int64_t)h);
    addHash((uint8_t)i);
    cb(a, b, c, d, e, f, g, h, i);
    return Status::ok();
  }

  virtual Status echoScalar(uint8_t a, int8_t b, uint16_t c, int16_t d, uint32_t e, int32_t f, uint64_t g, int64_t h, char i, ITypes::echoScalar_cb cb) {
    addHash(a);
    addHash(b);
    addHash(c);
    addHash(d);
    addHash(e);
    addHash(f);
    addHash(g);
    addHash(h);
    addHash(i);
    cb(a, b, c, d, e, f, g, h, i);
    return Status::ok();
  }

  virtual Status echoStruct(const ITypes::s0 *s, ITypes::echoStruct_cb cb) {
    cout << "echoStruct called" << endl;
    cb(s);
    return Status::ok();
  }

  virtual Status getHash(ITypes::getHash_cb cb) {
    cb(::getHash());
    return Status::ok();
  }

  virtual Status quit() {
    //cout << "Service: quitting" << endl;
    fflush(stdout);
    exit(0);
  }
  private:
};

int Run() {
  android::sp<Types> service = new Types;
  //cout << "Service started." << endl;
  //fflush(stdout);

  sp<Looper> looper(Looper::prepare(0 /* opts */));

  int binder_fd = -1;
  ProcessState::self()->setThreadPoolMaxThreadCount(0);
  IPCThreadState::self()->disableBackgroundScheduling(true);
  IPCThreadState::self()->setupPolling(&binder_fd);
  ALOGI("Got binder FD %d", binder_fd);
  if (binder_fd < 0) return -1;

  sp<BinderCallback> cb(new BinderCallback);
  if (looper->addFd(binder_fd, Looper::POLL_CALLBACK, Looper::EVENT_INPUT, cb,
                    nullptr) != 1) {
    ALOGE("Failed to add binder FD to Looper");
    return -1;
  }
  hidl_version version = android::hidl::make_hidl_version(4,1);
  defaultServiceManager()->addService(service->getInterfaceDescriptor(),
                                      service, version);

  ALOGI("Entering loop");
  while (true) {
    const int result = looper->pollAll(-1 /* timeoutMillis */);
    ALOGI("Looper returned %d", result);
  }
  return 0;

}

} // namespace

int main(int /* argc */, char* /* argv */ []) {
    return Run();
}
