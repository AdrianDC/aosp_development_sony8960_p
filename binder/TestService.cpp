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

#include <android/hardware/tests/BnTestService.h>
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
using android::hardware::tests::BnTestService;

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

class TestService : public BnTestService {
 public:
  TestService() {}
  virtual ~TestService() = default;

  virtual Status echoInteger(int32_t echo_me, const ITestService::simple_t* my_struct, ITestService::echoInteger_cb callback) {
            std::cout << "struct at addr " << (void *)my_struct << std::endl;
            std::cout << "int1 " << my_struct->int1 << " int2 " << my_struct->int2 << std::endl;
            std::cout << "uint8 at position 18 is " << static_cast<int>(my_struct->data.buffer[18]) << std::endl;
        callback(echo_me);
        printf("Value is %d\n", my_struct->data.buffer[18]);
        return Status::ok();
    }
    virtual Status shareBufferWithRef(int buffer, ITestService::shareBufferWithRef_cb callback) {
        // Map back to struct lots_of_data
        ITestService::lots_of_data* dataPtr;
        if (from_ref(buffer, &dataPtr)) {
            // Failed
            callback(1);
        } else {
            cout << "Data element at position 1481 is " << unsigned(dataPtr->buffer[1481]) << endl;
            callback(0);
        }

        return Status::ok();
    }
  private:
};

int Run() {
  android::sp<TestService> service = new TestService;
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
