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

// libbinder:
using android::BnInterface;
using android::defaultServiceManager;
using android::IInterface;
using android::IPCThreadState;
using android::Parcel;
using android::ProcessState;
using android::binder::Status;

// Standard library
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

    virtual ::android::binder::Status echoInteger(int32_t echo_me, int32_t* _aidl_return) {
        *_aidl_return = echo_me;
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

  defaultServiceManager()->addService(service->getInterfaceDescriptor(),
                                      service);

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
