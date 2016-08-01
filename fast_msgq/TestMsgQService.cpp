#include <iostream>
#include <map>

#include <unistd.h>

#include <BnTestMsgQ.h>
#include <common/MessageQueue.h>
#include <hwbinder/IInterface.h>
#include <hwbinder/IPCThreadState.h>
#include <hwbinder/IServiceManager.h>
#include <hwbinder/ProcessState.h>
#include <hwbinder/Status.h>
#include <utils/Errors.h>
#include <utils/Log.h>
#include <utils/Looper.h>
#include <utils/StrongPointer.h>
// libutils:
using android::Looper;
using android::LooperCallback;
using android::OK;
using android::sp;
using android::String16;

// libhwbinder:
using android::hardware::BnInterface;
using android::hardware::defaultServiceManager;
using android::hardware::IInterface;
using android::hardware::IPCThreadState;
using android::hardware::Parcel;
using android::hardware::ProcessState;
using android::hardware::Status;
using android::hardware::hidl_version;
using android::hardware::make_hidl_version;
using android::hardware::from_ref;

// Standard library
using std::cerr;
using std::cout;
using std::endl;
using std::map;
using std::string;
using std::unique_ptr;
using std::vector;

// Generated HIDL files
using android::hardware::tests::BnTestMsgQ;

typedef uint64_t ringbuffer_position_t;

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

class TestMsgQ : public BnTestMsgQ {
 public:
  TestMsgQ() : fmsg_queue(nullptr) {}
  virtual ~TestMsgQ() {
    if (fmsg_queue) {
      delete fmsg_queue;
    }
  }

  // TODO:: Change callback argument to bool.
  virtual Status requestWrite(int count, ITestMsgQ::requestWrite_cb callback) {
    uint16_t* data = new uint16_t[count];
    for (int i = 0; i < count; i++) {
      data[i] = i;
    }
    if (fmsg_queue->write(data, count)) {
      callback(count);
    } else {
      callback(0);
    }
    delete[] data;
    return Status::ok();
  }
  // TODO:: Change callback argument to bool.
  virtual Status requestRead(int count, ITestMsgQ::requestRead_cb callback) {
    uint16_t* data = new uint16_t[count];
    if (fmsg_queue->read(data, count) && verifyData(data, count)) {
      callback(count);
    } else {
      callback(0);
    }
    delete[] data;
    return Status::ok();
  }

  virtual Status configure(ITestMsgQ::configure_cb callback) {
    size_t eventQueueTotal = 4096;
    const size_t eventQueueDataSize = 2048;
    int ashmemFd = ashmem_create_region("MessageQueue", eventQueueTotal);
    ashmem_set_prot_region(ashmemFd, PROT_READ | PROT_WRITE);
    /*
     * The native handle will contain the fds to be mapped.
     */
    native_handle_t* mq_handle = native_handle_create(1, 0);
    if (!mq_handle) {
      ALOGE("Unable to create native_handle_t");
      return Status::fromExceptionCode(Status::EX_ILLEGAL_STATE);
    }

    std::vector<android::hardware::GrantorDescriptor> Grantors(
        MINIMUM_GRANTOR_COUNT);

    mq_handle->data[0] = ashmemFd;

    /*
     * Create Grantor Descriptors for read, write pointers and the data buffer.
     */
    Grantors[android::hardware::READPTRPOS] = {0, 0, 0,
                                           sizeof(ringbuffer_position_t)};
    Grantors[android::hardware::WRITEPTRPOS] = {0, 0, sizeof(ringbuffer_position_t),
                                            sizeof(ringbuffer_position_t)};
    Grantors[android::hardware::DATAPTRPOS] = {
        0, 0, 2 * sizeof(ringbuffer_position_t), eventQueueDataSize};

    android::hardware::MQDescriptor mydesc(Grantors, mq_handle, 0,
                                       sizeof(uint16_t));
    if (fmsg_queue) {
      delete fmsg_queue;
    }
    fmsg_queue = new android::hardware::MessageQueue<uint16_t>(mydesc);
    ITestMsgQ::WireMQDescriptor* wmsgq_desc = CreateWireMQDescriptor(mydesc);
    callback(*wmsgq_desc);
    delete[] wmsgq_desc->grantors.buffer;
    delete wmsgq_desc;
    return Status::ok();
  }
  android::hardware::MessageQueue<uint16_t>* fmsg_queue;

 private:
  /*
   * Utility function to verify data read from the fast message queue.
   */
  bool verifyData(uint16_t* data, int count) {
    for (int i = 0; i < count; i++) {
      if (data[i] != i) return false;
    }

    return true;
  }

  /*
   * Create WireMQDescriptor from MQDescriptor.
   */
  ITestMsgQ::WireMQDescriptor* CreateWireMQDescriptor(
      android::hardware::MQDescriptor& rb_desc) {
    ITestMsgQ::WireMQDescriptor* wmq_desc = new ITestMsgQ::WireMQDescriptor;
    const vector<android::hardware::GrantorDescriptor>& vec_gd =
        rb_desc.getGrantors();
    wmq_desc->grantors.count = vec_gd.size();
    wmq_desc->grantors.buffer =
        new ITestMsgQ::WireGrantorDescriptor[wmq_desc->grantors.count];
    for (size_t i = 0; i < wmq_desc->grantors.count; i++) {
      wmq_desc->grantors.buffer[i] = {
          0, {vec_gd[i].fdIndex, vec_gd[i].offset, vec_gd[i].extent}};
    }
    wmq_desc->mq_handle = (rb_desc.getHandle())->handle();
    wmq_desc->quantum = rb_desc.getQuantum();
    wmq_desc->flags = rb_desc.getFlags();
    return wmq_desc;
  }
};

int Run() {
  android::sp<TestMsgQ> service = new TestMsgQ;
  sp<Looper> looper(Looper::prepare(0 /* opts */));

  int binder_fd = -1;
  ProcessState::self()->setThreadPoolMaxThreadCount(0);
  IPCThreadState::self()->disableBackgroundScheduling(true);
  IPCThreadState::self()->setupPolling(&binder_fd);
  if (binder_fd < 0) return -1;

  sp<BinderCallback> cb(new BinderCallback);
  if (looper->addFd(binder_fd, Looper::POLL_CALLBACK, Looper::EVENT_INPUT, cb,
                    nullptr) != 1) {
    ALOGE("Failed to add binder FD to Looper");
    return -1;
  }
  hidl_version version = android::hardware::make_hidl_version(4, 0);
  defaultServiceManager()->addService(service->getInterfaceDescriptor(),
                                      service, version);

  ALOGI("Entering loop");
  while (true) {
    const int result = looper->pollAll(-1 /* timeoutMillis */);
    ALOGI("Looper returned %d", result);
  }
  return 0;
}

}  // namespace

int main(int /* argc */, char* /* argv */ []) { return Run(); }
