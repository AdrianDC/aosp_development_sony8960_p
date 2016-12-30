#define LOG_TAG "hidl_test"
#include <android-base/logging.h>

#include <android/hidl/manager/1.0/IServiceManager.h>
#include <android/hidl/manager/1.0/IServiceNotification.h>

#include <android/hidl/memory/1.0/IAllocator.h>
#include <android/hidl/memory/1.0/IMemory.h>

#include <android/hidl/token/1.0/ITokenManager.h>

#include <android/hardware/tests/foo/1.0/IFoo.h>
#include <android/hardware/tests/foo/1.0/IFooCallback.h>
#include <android/hardware/tests/foo/1.0/BnSimple.h>
// TODO(b/33669138): remove
#include <cutils/trace.h>
#include <android/hardware/tests/foo/1.0/BsSimple.h>
#include <android/hardware/tests/foo/1.0/BpSimple.h>

#include <android/hardware/tests/bar/1.0/IBar.h>
#include <android/hardware/tests/bar/1.0/IComplicated.h>
#include <android/hardware/tests/inheritance/1.0/IFetcher.h>
#include <android/hardware/tests/inheritance/1.0/IGrandparent.h>
#include <android/hardware/tests/inheritance/1.0/IParent.h>
#include <android/hardware/tests/inheritance/1.0/IChild.h>
#include <android/hardware/tests/memory/1.0/IMemoryTest.h>
#include <android/hardware/tests/pointer/1.0/IGraph.h>
#include <android/hardware/tests/pointer/1.0/IPointer.h>

#include <gtest/gtest.h>
#if GTEST_IS_THREADSAFE
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#else
#error "GTest did not detect pthread library."
#endif

#include <algorithm>
#include <condition_variable>
#include <getopt.h>
#include <inttypes.h>
#include <mutex>
#include <set>
#include <sstream>
#include <utility>
#include <vector>

#include <hidl-test/FooHelper.h>
#include <hidl-test/PointerHelper.h>

#include <hidl/Status.h>
#include <hidlmemory/mapping.h>

#include <hwbinder/IPCThreadState.h>

#include <utils/Condition.h>
#include <utils/Timers.h>

#define EXPECT_OK(__ret__) EXPECT_TRUE(isOk(__ret__))
#define EXPECT_FAIL(__ret__) EXPECT_FALSE(isOk(__ret__))
#define EXPECT_ARRAYEQ(__a1__, __a2__, __size__) EXPECT_TRUE(isArrayEqual(__a1__, __a2__, __size__))

// TODO uncomment this when kernel is patched with pointer changes.
//#define HIDL_RUN_POINTER_TESTS 1

// forward declarations.
class PassthroughEnvironment;
class BinderizedEnvironment;

// static storage
static enum TestMode {
    BINDERIZED,
    PASSTHROUGH
} gMode;
static PassthroughEnvironment *gPassthroughEnvironment = nullptr;
static BinderizedEnvironment *gBinderizedEnvironment = nullptr;
// per process tag
static std::string gServiceName;
// end static storage

using ::android::hardware::tests::foo::V1_0::Abc;
using ::android::hardware::tests::foo::V1_0::IFoo;
using ::android::hardware::tests::foo::V1_0::IFooCallback;
using ::android::hardware::tests::foo::V1_0::ISimple;
using ::android::hardware::tests::bar::V1_0::IBar;
using ::android::hardware::tests::bar::V1_0::IComplicated;
using ::android::hardware::tests::inheritance::V1_0::IFetcher;
using ::android::hardware::tests::inheritance::V1_0::IGrandparent;
using ::android::hardware::tests::inheritance::V1_0::IParent;
using ::android::hardware::tests::inheritance::V1_0::IChild;
using ::android::hardware::tests::pointer::V1_0::IGraph;
using ::android::hardware::tests::pointer::V1_0::IPointer;
using ::android::hardware::tests::memory::V1_0::IMemoryTest;
using ::android::hardware::IPCThreadState;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::configureRpcThreadpool;
using ::android::hardware::joinRpcThreadpool;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_death_recipient;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hidl::base::V1_0::IBase;
using ::android::hidl::manager::V1_0::IServiceManager;
using ::android::hidl::manager::V1_0::IServiceNotification;
using ::android::hidl::memory::V1_0::IAllocator;
using ::android::hidl::memory::V1_0::IMemory;
using ::android::hidl::token::V1_0::ITokenManager;
using ::android::sp;
using ::android::wp;
using ::android::to_string;
using ::android::Mutex;
using ::android::MultiDimensionalToString;
using ::android::Condition;
using ::android::DELAY_S;
using ::android::DELAY_NS;
using ::android::TOLERANCE_NS;
using ::android::ONEWAY_TOLERANCE_NS;
using std::to_string;

template <typename T>
static inline ::testing::AssertionResult isOk(::android::hardware::Return<T> ret) {
    return ret.getStatus().isOk()
        ? (::testing::AssertionSuccess() << ret.getStatus())
        : (::testing::AssertionFailure() << ret.getStatus());
}

template<typename T, typename S>
static inline bool isArrayEqual(const T arr1, const S arr2, size_t size) {
    for(size_t i = 0; i < size; i++)
        if(arr1[i] != arr2[i])
            return false;
    return true;
}

template<typename T>
std::string to_string(std::set<T> set) {
    std::stringstream ss;
    ss << "{";

    bool first = true;
    for (const T &item : set) {
        if (first) {
            first = false;
        } else {
            ss << ", ";
        }

        ss << to_string(item);
    }

    ss << "}";

    return ss.str();
}

struct Simple : public ISimple {
    Simple(int32_t cookie)
        : mCookie(cookie) {
    }

    Return<int32_t> getCookie() override {
        return mCookie;
    }

    Return<void> customVecInt(customVecInt_cb _cb) override {
        _cb(hidl_vec<int32_t>());
        return Void();
    }

    Return<void> customVecStr(customVecStr_cb _cb) override {
        hidl_vec<hidl_string> vec;
        vec.resize(2);
        _cb(vec);
        return Void();
    }

    Return<void> mystr(mystr_cb _cb) override {
        _cb(hidl_string());
        return Void();
    }

    Return<void> myhandle(myhandle_cb _cb) override {
        auto h = native_handle_create(0, 1);
        _cb(h);
        native_handle_delete(h);
        return Void();
    }

private:
    int32_t mCookie;
};

struct Complicated : public IComplicated {
    Complicated(int32_t cookie)
        : mCookie(cookie) {
    }

    Return<int32_t> getCookie() override {
        return mCookie;
    }

    Return<void> customVecInt(customVecInt_cb _cb) override {
        _cb(hidl_vec<int32_t>());
        return Void();
    }
    Return<void> customVecStr(customVecStr_cb _cb) override {
        hidl_vec<hidl_string> vec;
        vec.resize(2);
        _cb(vec);
        return Void();
    }

    Return<void> mystr(mystr_cb _cb) override {
        _cb(hidl_string());
        return Void();
    }

    Return<void> myhandle(myhandle_cb _cb) override {
        auto h = native_handle_create(0, 1);
        _cb(h);
        native_handle_delete(h);
        return Void();
    }

private:
    int32_t mCookie;
};

struct ServiceNotification : public IServiceNotification {
    std::mutex mutex;
    std::condition_variable condition;

    Return<void> onRegistration(const hidl_string &fqName,
                                const hidl_string &name,
                                bool preexisting) override {
        if (preexisting) {
            // not interested in things registered from previous runs of hidl_test
            return Void();
        }

        std::unique_lock<std::mutex> lock(mutex);

        mRegistered.push_back(std::string(fqName.c_str()) + "/" + name.c_str());

        lock.unlock();
        condition.notify_one();

        return Void();
    }

    const std::vector<std::string> &getRegistrations() const {
        return mRegistered;
    }

private:
    std::vector<std::string> mRegistered{};
};

void signal_handler(int signal)
{
    if (signal == SIGTERM) {
        ALOGD("SERVER %s shutting down...", gServiceName.c_str());
        IPCThreadState::shutdown();
        ALOGD("SERVER %s shutdown.", gServiceName.c_str());
        exit(0);
    }
}

template <class T>
static pid_t forkServer(const std::string &serviceName) {
    pid_t pid;

    // use fork to create and kill to destroy server processes.
    // getStub = true to get the passthrough version as the backend for the
    // binderized service.
    if ((pid = fork()) == 0) {
        // in child process
        configureRpcThreadpool(1, true /*callerWillJoin*/);
        sp<T> server = T::getService(serviceName, true);
        gServiceName = serviceName;
        signal(SIGTERM, signal_handler);
        ALOGD("SERVER registering %s", serviceName.c_str());
        ::android::status_t status = server->registerAsService(serviceName);
        if (status != ::android::OK) {
            ALOGE("SERVER could not register %s", serviceName.c_str());
            exit(-1);
        }
        ALOGD("SERVER starting %s", serviceName.c_str());
        joinRpcThreadpool();
        ALOGD("SERVER %s ends.", serviceName.c_str());
        exit(0);
    }

    // in main process
    return pid;
}

static void killServer(pid_t pid, const char *serverName) {
    if(kill(pid, SIGTERM)) {
        ALOGE("Could not kill %s; errno = %d", serverName, errno);
    } else {
        int status;
        ALOGD("Waiting for %s to exit...", serverName);
        waitpid(pid, &status, 0);
        if (status != 0) {
            ALOGE("%s terminates abnormally with status %d", serverName, status);
        }
        ALOGD("Continuing...");
    }
}

class HidlEnvironmentBase : public ::testing::Environment {
protected:
    std::vector<std::pair<std::string, pid_t>> mPids;

public:
    sp<IServiceManager> manager;
    sp<ITokenManager> tokenManager;
    sp<IAllocator> ashmemAllocator;
    sp<IMemoryTest> memoryTest;
    sp<IFetcher> fetcher;
    sp<IFoo> foo;
    sp<IFoo> dyingFoo;
    sp<IBar> bar;
    sp<IFooCallback> fooCb;
    sp<IGraph> graphInterface;
    sp<IPointer> pointerInterface;
    sp<IPointer> validationPointerInterface;

    template <class T>
    void addServer(const std::string &name) {
        mPids.push_back({name, forkServer<T>(name)});
    }

    void getServices() {
        manager = IServiceManager::getService("manager");

        // alternatively:
        // manager = defaultServiceManager()

        ASSERT_NE(manager, nullptr);
        ASSERT_TRUE(manager->isRemote()); // manager is always remote

        tokenManager = ITokenManager::getService("manager");
        ASSERT_NE(tokenManager, nullptr);
        ASSERT_TRUE(tokenManager->isRemote()); // tokenManager is always remote

        ashmemAllocator = IAllocator::getService("ashmem");
        ASSERT_NE(ashmemAllocator, nullptr);
        ASSERT_TRUE(ashmemAllocator->isRemote()); // allocator is always remote

        // getStub is true if we are in passthrough mode to skip checking
        // binderized server, false for binderized mode.

        memoryTest = IMemoryTest::getService("memory", gMode == PASSTHROUGH /* getStub */);
        ASSERT_NE(memoryTest, nullptr);
        ASSERT_EQ(memoryTest->isRemote(), gMode == BINDERIZED);

        fetcher = IFetcher::getService("fetcher", gMode == PASSTHROUGH /* getStub */);
        ASSERT_NE(fetcher, nullptr);
        ASSERT_EQ(fetcher->isRemote(), gMode == BINDERIZED);

        foo = IFoo::getService("foo", gMode == PASSTHROUGH /* getStub */);
        ASSERT_NE(foo, nullptr);
        ASSERT_EQ(foo->isRemote(), gMode == BINDERIZED);

        dyingFoo = IFoo::getService("dyingFoo", gMode == PASSTHROUGH /* getStub */);
        ASSERT_NE(foo, nullptr);
        ASSERT_EQ(foo->isRemote(), gMode == BINDERIZED);

        bar = IBar::getService("foo", gMode == PASSTHROUGH /* getStub */);
        ASSERT_NE(bar, nullptr);
        ASSERT_EQ(bar->isRemote(), gMode == BINDERIZED);

        fooCb = IFooCallback::getService("foo callback", gMode == PASSTHROUGH /* getStub */);
        ASSERT_NE(fooCb, nullptr);
        ASSERT_EQ(fooCb->isRemote(), gMode == BINDERIZED);

        graphInterface = IGraph::getService("graph", gMode == PASSTHROUGH /* getStub */);
        ASSERT_NE(graphInterface, nullptr);
        ASSERT_EQ(graphInterface->isRemote(), gMode == BINDERIZED);

        pointerInterface = IPointer::getService("pointer", gMode == PASSTHROUGH /* getStub */);
        ASSERT_NE(pointerInterface, nullptr);
        ASSERT_EQ(pointerInterface->isRemote(), gMode == BINDERIZED);

        // use passthrough mode as the validation object.
        validationPointerInterface = IPointer::getService("pointer", true /* getStub */);
        ASSERT_NE(validationPointerInterface, nullptr);
    }

    void killServer(const char *serverName) {
        for (const auto &pair : mPids) {
            if (pair.first == serverName) {
                ::killServer(pair.second, pair.first.c_str());
            }
        }
    }

    virtual void TearDown() {
        // clean up by killing server processes.
        ALOGI("Environment tear-down beginning...");
        ALOGI("Killing servers...");
        size_t i = 0;
        for (const auto &pair : mPids) {
            ::killServer(pair.second, pair.first.c_str());
        }
        ALOGI("Servers all killed.");
        ALOGI("Environment tear-down complete.");
    }
};

class PassthroughEnvironment : public HidlEnvironmentBase {
private:
    virtual void SetUp() {
        ALOGI("Environment setup beginning...");
        // starts this even for passthrough mode.
        // this is used in Bar's default implementation
        addServer<IChild>("child");
        sleep(1);
        getServices();
        ALOGI("Environment setup complete.");
    }
};

class BinderizedEnvironment : public HidlEnvironmentBase {
public:
    virtual void SetUp() {
        ALOGI("Environment setup beginning...");

        size_t i = 0;
        addServer<IMemoryTest>("memory");
        addServer<IChild>("child");
        addServer<IParent>("parent");
        addServer<IFetcher>("fetcher");
        addServer<IBar>("foo");
        addServer<IFoo>("dyingFoo");
        addServer<IFooCallback>("foo callback");
        addServer<IGraph>("graph");
        addServer<IPointer>("pointer");

        sleep(1);
        getServices();
        ALOGI("Environment setup complete.");
    }
};

class HidlTest : public ::testing::Test {
public:
    sp<IServiceManager> manager;
    sp<ITokenManager> tokenManager;
    sp<IAllocator> ashmemAllocator;
    sp<IMemoryTest> memoryTest;
    sp<IFetcher> fetcher;
    sp<IFoo> foo;
    sp<IFoo> dyingFoo;
    sp<IBar> bar;
    sp<IFooCallback> fooCb;
    sp<IGraph> graphInterface;
    sp<IPointer> pointerInterface;
    sp<IPointer> validationPointerInterface;

    void killServer(const char *serverName) {
        HidlEnvironmentBase *env;
        if (gMode == BINDERIZED) {
            env = gBinderizedEnvironment;
        } else {
            env = gPassthroughEnvironment;
        }
        env->killServer(serverName);
    }

    virtual void SetUp() override {
        ALOGI("Test setup beginning...");
        HidlEnvironmentBase *env;
        if (gMode == BINDERIZED) {
            env = gBinderizedEnvironment;
        } else {
            env = gPassthroughEnvironment;
        }
        manager = env->manager;
        tokenManager = env->tokenManager;
        ashmemAllocator = env->ashmemAllocator;
        memoryTest = env->memoryTest;
        fetcher = env->fetcher;
        foo = env->foo;
        dyingFoo = env->dyingFoo;
        bar = env->bar;
        fooCb = env->fooCb;
        graphInterface = env->graphInterface;
        pointerInterface = env->pointerInterface;
        validationPointerInterface = env->validationPointerInterface;
        ALOGI("Test setup complete");
    }
};

TEST_F(HidlTest, ServiceListTest) {
    static const std::set<std::string> binderizedSet = {
        "android.hardware.tests.pointer@1.0::IPointer/pointer",
        "android.hardware.tests.bar@1.0::IBar/foo",
        "android.hardware.tests.inheritance@1.0::IFetcher/fetcher",
        "android.hardware.tests.foo@1.0::IFooCallback/foo callback",
        "android.hardware.tests.inheritance@1.0::IParent/parent",
        "android.hardware.tests.inheritance@1.0::IParent/child",
        "android.hardware.tests.inheritance@1.0::IChild/child",
        "android.hardware.tests.pointer@1.0::IGraph/graph",
        "android.hardware.tests.inheritance@1.0::IGrandparent/child",
        "android.hardware.tests.foo@1.0::IFoo/foo",
        "android.hidl.manager@1.0::IServiceManager/manager",
    };

    static const std::set<std::string> passthroughSet = {
        "android.hidl.manager@1.0::IServiceManager/manager"
    };

    std::set<std::string> activeSet;

    switch(gMode) {
        case BINDERIZED: {
            activeSet = binderizedSet;
        } break;

        case PASSTHROUGH: {
            activeSet = passthroughSet;
        } break;
        default:
            EXPECT_TRUE(false) << "unrecognized mode";
    }

    EXPECT_OK(manager->list([&activeSet](const hidl_vec<hidl_string> &registered){
        std::set<std::string> registeredSet;

        for (size_t i = 0; i < registered.size(); i++) {
            registeredSet.insert(registered[i]);
        }

        std::set<std::string> difference;
        std::set_difference(activeSet.begin(), activeSet.end(),
                            registeredSet.begin(), registeredSet.end(),
                            std::inserter(difference, difference.begin()));

        EXPECT_EQ(difference.size(), 0u) << "service(s) not registered " << to_string(difference);
    }));
}

// passthrough TODO(b/32747392)
TEST_F(HidlTest, ServiceListByInterfaceTest) {
    if (gMode == BINDERIZED) {
        EXPECT_OK(manager->listByInterface(IParent::descriptor,
            [](const hidl_vec<hidl_string> &registered) {
                std::set<std::string> registeredSet;

                for (size_t i = 0; i < registered.size(); i++) {
                    registeredSet.insert(registered[i]);
                }

                std::set<std::string> activeSet = {
                    "parent", "child"
                };
                std::set<std::string> difference;
                std::set_difference(activeSet.begin(), activeSet.end(),
                                    registeredSet.begin(), registeredSet.end(),
                                    std::inserter(difference, difference.begin()));

                EXPECT_EQ(difference.size(), 0u) << "service(s) not registered " << to_string(difference);
            }));
    }
}

// passthrough TODO(b/32747392)
TEST_F(HidlTest, ServiceParentTest) {
    if (gMode == BINDERIZED) {
        sp<IParent> parent = IParent::getService("child");

        EXPECT_NE(parent, nullptr);
    }
}

// passthrough TODO(b/32747392)
TEST_F(HidlTest, ServiceNotificationTest) {
    if (gMode == BINDERIZED) {
        ServiceNotification *notification = new ServiceNotification();

        std::string instanceName = "test-instance";
        EXPECT_TRUE(ISimple::registerForNotifications(instanceName, notification));

        Simple* instance = new Simple(1);
        EXPECT_EQ(::android::OK, instance->registerAsService(instanceName));

        std::unique_lock<std::mutex> lock(notification->mutex);

        notification->condition.wait_for(
                lock,
                std::chrono::milliseconds(2),
                [&notification]() {
                   return notification->getRegistrations().size() >= 1;
                });

        std::vector<std::string> registrations = notification->getRegistrations();

        EXPECT_EQ(registrations.size(), 1u);

        EXPECT_EQ(to_string(registrations.data(), registrations.size()),
                  std::string("['") + Simple::descriptor + "/" + instanceName + "']");
    }
}

// passthrough TODO(b/32747392)
TEST_F(HidlTest, ServiceAllNotificationTest) {
    if (gMode == BINDERIZED) {
        ServiceNotification *notification = new ServiceNotification();

        std::string instanceOne = "test-instance-one";
        std::string instanceTwo = "test-instance-two";
        EXPECT_TRUE(ISimple::registerForNotifications("", notification));

        Simple* instanceA = new Simple(1);
        EXPECT_EQ(::android::OK, instanceA->registerAsService(instanceOne));
        Simple* instanceB = new Simple(2);
        EXPECT_EQ(::android::OK, instanceB->registerAsService(instanceTwo));

        std::unique_lock<std::mutex> lock(notification->mutex);

        notification->condition.wait_for(
                lock,
                std::chrono::milliseconds(2),
                [&notification]() {
                   return notification->getRegistrations().size() >= 2;
                });

        std::vector<std::string> registrations = notification->getRegistrations();
        std::sort(registrations.begin(), registrations.end());

        EXPECT_EQ(registrations.size(), 2u);

        std::string descriptor = ISimple::descriptor;

        EXPECT_EQ(to_string(registrations.data(), registrations.size()),
                  "['" + descriptor + "/" + instanceOne + "', '"
                       + descriptor + "/" + instanceTwo + "']");
    }
}

TEST_F(HidlTest, TestToken) {
    Return<uint64_t> ret = tokenManager->createToken(manager);
    EXPECT_OK(ret);
    uint64_t token = ret;

    EXPECT_OK(tokenManager->get(token, [&](const auto &store) {
        EXPECT_NE(nullptr, store.get());
        sp<IServiceManager> retManager = IServiceManager::castFrom(store);

        // TODO(b/33818800): should have only one Bp per process
        // EXPECT_EQ(manager, retManager);

        EXPECT_NE(nullptr, retManager.get());
    }));

    Return<bool> unregisterRet = tokenManager->unregister(token);

    EXPECT_OK(unregisterRet);
    if (unregisterRet.isOk()) {
        EXPECT_TRUE(ret);
    }
}

TEST_F(HidlTest, TestSharedMemory) {
    const uint8_t kValue = 0xCA;
    hidl_memory mem_copy;
    EXPECT_OK(ashmemAllocator->allocate(1024, [&](bool success, const hidl_memory& mem) {
        EXPECT_EQ(success, true);

        sp<IMemory> memory = mapMemory(mem);

        EXPECT_NE(memory, nullptr);

        uint8_t* data = static_cast<uint8_t*>(static_cast<void*>(memory->getPointer()));
        EXPECT_NE(data, nullptr);

        EXPECT_EQ(memory->getSize(), mem.size());

        memory->update();
        memset(data, 0, memory->getSize());
        memory->commit();

        mem_copy = mem;
        memoryTest->fillMemory(mem, kValue);
        for (size_t i = 0; i < mem.size(); i++) {
            EXPECT_EQ(kValue, data[i]);
        }
    }));

    // Test the memory persists after the call
    sp<IMemory> memory = mapMemory(mem_copy);

    EXPECT_NE(memory, nullptr);

    uint8_t* data = static_cast<uint8_t*>(static_cast<void*>(memory->getPointer()));
    EXPECT_NE(data, nullptr);

    for (size_t i = 0; i < mem_copy.size(); i++) {
        EXPECT_EQ(kValue, data[i]);
    }
}

TEST_F(HidlTest, FooDoThisTest) {
    ALOGI("CLIENT call doThis.");
    EXPECT_OK(foo->doThis(1.0f));
    ALOGI("CLIENT doThis returned.");
}

TEST_F(HidlTest, FooDoThatAndReturnSomethingTest) {
    ALOGI("CLIENT call doThatAndReturnSomething.");
    int32_t result = foo->doThatAndReturnSomething(2.0f);
    ALOGI("CLIENT doThatAndReturnSomething returned %d.", result);
    EXPECT_EQ(result, 666);
}

TEST_F(HidlTest, FooDoQuiteABitTest) {
    ALOGI("CLIENT call doQuiteABit");
    double something = foo->doQuiteABit(1, 2, 3.0f, 4.0);
    ALOGI("CLIENT doQuiteABit returned %f.", something);
    EXPECT_DOUBLE_EQ(something, 666.5);
}

TEST_F(HidlTest, FooDoSomethingElseTest) {

    ALOGI("CLIENT call doSomethingElse");
    hidl_array<int32_t, 15> param;
    for (size_t i = 0; i < sizeof(param) / sizeof(param[0]); ++i) {
        param[i] = i;
    }
    EXPECT_OK(foo->doSomethingElse(param, [&](const auto &something) {
            ALOGI("CLIENT doSomethingElse returned %s.",
                  to_string(something).c_str());
            int32_t expect[] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24,
                26, 28, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 1, 2};
            EXPECT_TRUE(isArrayEqual(something, expect, 32));
        }));
}

TEST_F(HidlTest, FooDoStuffAndReturnAStringTest) {
    ALOGI("CLIENT call doStuffAndReturnAString");
    EXPECT_OK(foo->doStuffAndReturnAString([&](const auto &something) {
            ALOGI("CLIENT doStuffAndReturnAString returned '%s'.",
                  something.c_str());
            EXPECT_STREQ(something.c_str(), "Hello, world");
        }));
}

TEST_F(HidlTest, FooMapThisVectorTest) {
    hidl_vec<int32_t> vecParam;
    vecParam.resize(10);
    for (size_t i = 0; i < 10; ++i) {
        vecParam[i] = i;
    }
    EXPECT_OK(foo->mapThisVector(vecParam, [&](const auto &something) {
            ALOGI("CLIENT mapThisVector returned %s.",
                  to_string(something).c_str());
            int32_t expect[] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18};
            EXPECT_TRUE(isArrayEqual(something, expect, something.size()));
        }));
}

TEST_F(HidlTest, WrapTest) {
    using ::android::hardware::tests::foo::V1_0::BnSimple;
    using ::android::hardware::tests::foo::V1_0::BsSimple;
    using ::android::hardware::tests::foo::V1_0::BpSimple;
    using ::android::hardware::HidlInstrumentor;
    nsecs_t now;
    int i = 0;

    now = systemTime();
    new BnSimple(new Simple(1));
    EXPECT_LT(systemTime() - now, 2000000) << "    for BnSimple(nonnull)";

    now = systemTime();
    new BnSimple(nullptr);
    EXPECT_LT(systemTime() - now, 2000000) << "    for BnSimple(null)";

    now = systemTime();
    new BsSimple(new Simple(1));
    EXPECT_LT(systemTime() - now, 2000000) << "    for BsSimple(nonnull)";

    now = systemTime();
    new BsSimple(nullptr);
    EXPECT_LT(systemTime() - now, 2000000) << "    for BsSimple(null)";

    now = systemTime();
    new BpSimple(nullptr);
    EXPECT_LT(systemTime() - now, 2000000) << "    for BpSimple(null)";

    now = systemTime();
    new ::android::hardware::HidlInstrumentor("");
    EXPECT_LT(systemTime() - now, 2000000) << "    for HidlInstrumentor";

    now = systemTime();
    i++;
    EXPECT_LT(systemTime() - now,    1000) << "    for nothing";
}

// TODO: b/31819198
TEST_F(HidlTest, FooCallMeTest) {
    ALOGI("CLIENT call callMe.");
    // callMe is oneway, should return instantly.
    nsecs_t now;
    now = systemTime();
    EXPECT_OK(foo->callMe(fooCb));
    EXPECT_LT(systemTime() - now, ONEWAY_TOLERANCE_NS);
    ALOGI("CLIENT callMe returned.");
}

// TODO: b/31819198
TEST_F(HidlTest, ForReportResultsTest) {

    // Bar::callMe will invoke three methods on FooCallback; one will return
    // right away (even though it is a two-way method); the second one will
    // block Bar for DELAY_S seconds, and the third one will return
    // to Bar right away (is oneway) but will itself block for DELAY_S seconds.
    // We need a way to make sure that these three things have happened within
    // 2*DELAY_S seconds plus some small tolerance.
    //
    // Method FooCallback::reportResults() takes a timeout parameter.  It blocks for
    // that length of time, while waiting for the three methods above to
    // complete.  It returns the information of whether each method was invoked,
    // as well as how long the body of the method took to execute.  We verify
    // the information returned by reportResults() against the timeout we pass (which
    // is long enough for the method bodies to execute, plus tolerance), and
    // verify that eachof them executed, as expected, and took the length of
    // time to execute that we also expect.

    const nsecs_t reportResultsNs =
        2 * DELAY_NS + TOLERANCE_NS;

    ALOGI("CLIENT: Waiting for up to %" PRId64 " seconds.",
          nanoseconds_to_seconds(reportResultsNs));

    fooCb->reportResults(reportResultsNs,
                [&](int64_t timeLeftNs,
                    const hidl_array<IFooCallback::InvokeInfo, 3> &invokeResults) {
        ALOGI("CLIENT: FooCallback::reportResults() is returning data.");
        ALOGI("CLIENT: Waited for %" PRId64 " milliseconds.",
              nanoseconds_to_milliseconds(reportResultsNs - timeLeftNs));

        EXPECT_LE(0, timeLeftNs);
        EXPECT_LE(timeLeftNs, reportResultsNs);

        // two-way method, was supposed to return right away
        EXPECT_TRUE(invokeResults[0].invoked);
        EXPECT_LE(invokeResults[0].timeNs, invokeResults[0].callerBlockedNs);
        EXPECT_LE(invokeResults[0].callerBlockedNs, TOLERANCE_NS);
        // two-way method, was supposed to block caller for DELAY_NS
        EXPECT_TRUE(invokeResults[1].invoked);
        EXPECT_LE(invokeResults[1].timeNs, invokeResults[1].callerBlockedNs);
        EXPECT_LE(invokeResults[1].callerBlockedNs,
                    DELAY_NS + TOLERANCE_NS);
        // one-way method, do not block caller, but body was supposed to block for DELAY_NS
        EXPECT_TRUE(invokeResults[2].invoked);
        EXPECT_LE(invokeResults[2].callerBlockedNs, ONEWAY_TOLERANCE_NS);
        EXPECT_LE(invokeResults[2].timeNs, DELAY_NS + TOLERANCE_NS);
    });
}



TEST_F(HidlTest, FooUseAnEnumTest) {
    ALOGI("CLIENT call useAnEnum.");
    IFoo::SomeEnum sleepy = foo->useAnEnum(IFoo::SomeEnum::quux);
    ALOGI("CLIENT useAnEnum returned %u", (unsigned)sleepy);
    EXPECT_EQ(sleepy, IFoo::SomeEnum::goober);
}

TEST_F(HidlTest, FooHaveAGooberTest) {
    hidl_vec<IFoo::Goober> gooberVecParam;
    gooberVecParam.resize(2);
    gooberVecParam[0].name = "Hello";
    gooberVecParam[1].name = "World";

    ALOGI("CLIENT call haveAGooberVec.");
    EXPECT_OK(foo->haveAGooberVec(gooberVecParam));
    ALOGI("CLIENT haveAGooberVec returned.");

    ALOGI("CLIENT call haveaGoober.");
    EXPECT_OK(foo->haveAGoober(gooberVecParam[0]));
    ALOGI("CLIENT haveaGoober returned.");

    ALOGI("CLIENT call haveAGooberArray.");
    hidl_array<IFoo::Goober, 20> gooberArrayParam;
    EXPECT_OK(foo->haveAGooberArray(gooberArrayParam));
    ALOGI("CLIENT haveAGooberArray returned.");
}

TEST_F(HidlTest, FooHaveATypeFromAnotherFileTest) {
    ALOGI("CLIENT call haveATypeFromAnotherFile.");
    Abc abcParam{};
    abcParam.x = "alphabet";
    abcParam.y = 3.14f;
    native_handle_t *handle = native_handle_create(0, 0);
    abcParam.z = handle;
    EXPECT_OK(foo->haveATypeFromAnotherFile(abcParam));
    ALOGI("CLIENT haveATypeFromAnotherFile returned.");
    native_handle_delete(handle);
    abcParam.z = NULL;
}

TEST_F(HidlTest, FooHaveSomeStringsTest) {
    ALOGI("CLIENT call haveSomeStrings.");
    hidl_array<hidl_string, 3> stringArrayParam;
    stringArrayParam[0] = "What";
    stringArrayParam[1] = "a";
    stringArrayParam[2] = "disaster";
    EXPECT_OK(foo->haveSomeStrings(
                stringArrayParam,
                [&](const auto &out) {
                    ALOGI("CLIENT haveSomeStrings returned %s.",
                          to_string(out).c_str());

                    EXPECT_EQ(to_string(out), "['Hello', 'World']");
                }));
    ALOGI("CLIENT haveSomeStrings returned.");
}

TEST_F(HidlTest, FooHaveAStringVecTest) {
    ALOGI("CLIENT call haveAStringVec.");
    hidl_vec<hidl_string> stringVecParam;
    stringVecParam.resize(3);
    stringVecParam[0] = "What";
    stringVecParam[1] = "a";
    stringVecParam[2] = "disaster";
    EXPECT_OK(foo->haveAStringVec(
                stringVecParam,
                [&](const auto &out) {
                    ALOGI("CLIENT haveAStringVec returned %s.",
                          to_string(out).c_str());

                    EXPECT_EQ(to_string(out), "['Hello', 'World']");
                }));
    ALOGI("CLIENT haveAStringVec returned.");
}

TEST_F(HidlTest, FooTransposeMeTest) {
    hidl_array<float, 3, 5> in;
    float k = 1.0f;
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 5; ++j, ++k) {
            in[i][j] = k;
        }
    }

    ALOGI("CLIENT call transposeMe(%s).", to_string(in).c_str());

    EXPECT_OK(foo->transposeMe(
                in,
                [&](const auto &out) {
                    ALOGI("CLIENT transposeMe returned %s.",
                          to_string(out).c_str());

                    for (size_t i = 0; i < 3; ++i) {
                        for (size_t j = 0; j < 5; ++j) {
                            EXPECT_EQ(out[j][i], in[i][j]);
                        }
                    }
                }));
}

TEST_F(HidlTest, FooCallingDrWhoTest) {
    IFoo::MultiDimensional in;

    size_t k = 0;
    for (size_t i = 0; i < 5; ++i) {
        for (size_t j = 0; j < 3; ++j, ++k) {
            in.quuxMatrix[i][j].first = ("First " + std::to_string(k)).c_str();
            in.quuxMatrix[i][j].last = ("Last " + std::to_string(15-k)).c_str();
        }
    }

    ALOGI("CLIENT call callingDrWho(%s).",
          MultiDimensionalToString(in).c_str());

    EXPECT_OK(foo->callingDrWho(
                in,
                [&](const auto &out) {
                    ALOGI("CLIENT callingDrWho returned %s.",
                          MultiDimensionalToString(out).c_str());

                    size_t k = 0;
                    for (size_t i = 0; i < 5; ++i) {
                        for (size_t j = 0; j < 3; ++j, ++k) {
                            EXPECT_STREQ(
                                out.quuxMatrix[i][j].first.c_str(),
                                in.quuxMatrix[4 - i][2 - j].last.c_str());

                            EXPECT_STREQ(
                                out.quuxMatrix[i][j].last.c_str(),
                                in.quuxMatrix[4 - i][2 - j].first.c_str());
                        }
                    }
                }));
}

static std::string numberToEnglish(int x) {
    static const char *const kDigits[] = {
        "zero",
        "one",
        "two",
        "three",
        "four",
        "five",
        "six",
        "seven",
        "eight",
        "nine",
    };

    if (x < 0) {
        return "negative " + numberToEnglish(-x);
    }

    if (x < 10) {
        return kDigits[x];
    }

    if (x <= 15) {
        static const char *const kSpecialTens[] = {
            "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen",
        };

        return kSpecialTens[x - 10];
    }

    if (x < 20) {
        return std::string(kDigits[x % 10]) + "teen";
    }

    if (x < 100) {
        static const char *const kDecades[] = {
            "twenty", "thirty", "forty", "fifty", "sixty", "seventy",
            "eighty", "ninety",
        };

        return std::string(kDecades[x / 10 - 2]) + kDigits[x % 10];
    }

    return "positively huge!";
}

TEST_F(HidlTest, FooTransposeTest) {
    IFoo::StringMatrix5x3 in;

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 3; ++j) {
            in.s[i][j] = numberToEnglish(3 * i + j + 1).c_str();
        }
    }

    EXPECT_OK(foo->transpose(
                in,
                [&](const auto &out) {
                    EXPECT_EQ(
                        to_string(out),
                        "[['one', 'four', 'seven', 'ten', 'thirteen'], "
                         "['two', 'five', 'eight', 'eleven', 'fourteen'], "
                         "['three', 'six', 'nine', 'twelve', 'fifteen']]");
                }));
}

TEST_F(HidlTest, FooTranspose2Test) {
    hidl_array<hidl_string, 5, 3> in;

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 3; ++j) {
            in[i][j] = numberToEnglish(3 * i + j + 1).c_str();
        }
    }

    EXPECT_OK(foo->transpose2(
                in,
                [&](const auto &out) {
                    EXPECT_EQ(
                        to_string(out),
                        "[['one', 'four', 'seven', 'ten', 'thirteen'], "
                         "['two', 'five', 'eight', 'eleven', 'fourteen'], "
                         "['three', 'six', 'nine', 'twelve', 'fifteen']]");
                }));
}

// TODO: enable for passthrough mode after b/30814137
TEST_F(HidlTest, FooNullNativeHandleTest) {
    if (gMode == BINDERIZED) {
        Abc xyz;
        xyz.z = nullptr;
        EXPECT_FAIL(bar->expectNullHandle(nullptr, xyz, [](bool hIsNull, bool xyzHasNull) {
            EXPECT_TRUE(hIsNull);
            EXPECT_TRUE(xyzHasNull);
        }));
    }
}

TEST_F(HidlTest, FooNullCallbackTest) {
    EXPECT_OK(foo->echoNullInterface(nullptr,
                [](const auto receivedNull, const auto &intf) {
                   EXPECT_TRUE(receivedNull);
                   EXPECT_EQ(intf, nullptr);
                }));
}

TEST_F(HidlTest, FooNonNullCallbackTest) {
    hidl_array<hidl_string, 5, 3> in;

    EXPECT_FAIL(foo->transpose2(in, nullptr /* _hidl_cb */));
}

TEST_F(HidlTest, FooSendVecTest) {
    hidl_vec<uint8_t> in;
    in.resize(16);
    for (size_t i = 0; i < in.size(); ++i) {
        in[i] = i;
    }

    EXPECT_OK(foo->sendVec(
                in,
                [&](const auto &out) {
                    EXPECT_EQ(to_string(in), to_string(out));
                }));
}

TEST_F(HidlTest, FooHaveAVectorOfInterfacesTest) {
    hidl_vec<sp<ISimple> > in;
    in.resize(16);
    for (size_t i = 0; i < in.size(); ++i) {
        in[i] = new Simple(i);
    }

    EXPECT_OK(foo->haveAVectorOfInterfaces(
                in,
                [&](const auto &out) {
                    EXPECT_EQ(in.size(), out.size());
                    for (size_t i = 0; i < in.size(); ++i) {
                        int32_t inCookie = in[i]->getCookie();
                        int32_t outCookie = out[i]->getCookie();
                        EXPECT_EQ(inCookie, outCookie);
                    }
                }));
}

TEST_F(HidlTest, FooHaveAVectorOfGenericInterfacesTest) {

    hidl_vec<sp<::android::hidl::base::V1_0::IBase> > in;
    in.resize(16);
    for (size_t i = 0; i < in.size(); ++i) {
        sp<ISimple> s = new Simple(i);
        in[i] = s;
    }

    EXPECT_OK(foo->haveAVectorOfGenericInterfaces(
                in,
                [&](const auto &out) {
                    EXPECT_EQ(in.size(), out.size());

                    EXPECT_OK(out[0]->interfaceChain([](const auto &names) {
                        ASSERT_GT(names.size(), 0u);
                        ASSERT_STREQ(names[0].c_str(), ISimple::descriptor);
                    }));
                    for (size_t i = 0; i < in.size(); ++i) {
                        sp<ISimple> inSimple = ISimple::castFrom(in[i]);
                        sp<ISimple> outSimple = ISimple::castFrom(out[i]);

                        ASSERT_NE(inSimple.get(), nullptr);
                        ASSERT_NE(outSimple.get(), nullptr);
                        EXPECT_EQ(in[i], inSimple.get()); // pointers must be equal!
                        int32_t inCookie = inSimple->getCookie();
                        int32_t outCookie = outSimple->getCookie();
                        EXPECT_EQ(inCookie, outCookie);
                    }
                }));
}

TEST_F(HidlTest, FooStructEmbeddedHandleTest) {
    EXPECT_OK(foo->createMyHandle([&](const auto &myHandle) {
        EXPECT_EQ(myHandle.guard, 666);
        const native_handle_t* handle = myHandle.h.getNativeHandle();
        EXPECT_EQ(handle->numInts, 10);
        EXPECT_EQ(handle->numFds, 0);
        int data[] = {2,3,5,7,11,13,17,19,21,23};
        EXPECT_ARRAYEQ(handle->data, data, 10);
    }));

    EXPECT_OK(foo->closeHandles());
}

TEST_F(HidlTest, FooHandleVecTest) {
    EXPECT_OK(foo->createHandles(3, [&](const auto &handles) {
        EXPECT_EQ(handles.size(), 3ull);
        int data[] = {2,3,5,7,11,13,17,19,21,23};
        for (size_t i = 0; i < 3; i++) {
            const native_handle_t *h = handles[i];
            EXPECT_EQ(h->numInts, 10) << " for element " << i;
            EXPECT_EQ(h->numFds, 0) << " for element " << i;
            EXPECT_ARRAYEQ(h->data, data, 10);
        }
    }));

    EXPECT_OK(foo->closeHandles());
}

struct HidlDeathRecipient : hidl_death_recipient {
    std::mutex mutex;
    std::condition_variable condition;
    wp<IBase> who;
    bool fired = false;
    uint64_t cookie = 0;

    virtual void serviceDied(uint64_t cookie, const wp<IBase>& who) {
        std::unique_lock<std::mutex> lock(mutex);
        fired = true;
        this->cookie = cookie;
        this->who = who;
        condition.notify_one();
    };
};

TEST_F(HidlTest, DeathRecipientTest) {
    sp<HidlDeathRecipient> recipient = new HidlDeathRecipient();
    sp<HidlDeathRecipient> recipient2 = new HidlDeathRecipient();

    EXPECT_TRUE(dyingFoo->linkToDeath(recipient, 0x1481));
    EXPECT_TRUE(dyingFoo->linkToDeath(recipient2, 0x2592));
    EXPECT_TRUE(dyingFoo->unlinkToDeath(recipient2));

    if (gMode != BINDERIZED) {
        // Passthrough doesn't fire, nor does it keep state of
        // registered death recipients (so it won't fail unlinking
        // the same recipient twice).
        return;
    }

    EXPECT_FALSE(dyingFoo->unlinkToDeath(recipient2));
    killServer("dyingFoo");

    std::unique_lock<std::mutex> lock(recipient->mutex);
    recipient->condition.wait_for(lock, std::chrono::milliseconds(1000), [&recipient]() {
            return recipient->fired;
    });
    EXPECT_TRUE(recipient->fired);
    EXPECT_EQ(recipient->cookie, 0x1481u);
    EXPECT_EQ(recipient->who, dyingFoo);
    std::unique_lock<std::mutex> lock2(recipient2->mutex);
    recipient2->condition.wait_for(lock2, std::chrono::milliseconds(1000), [&recipient2]() {
            return recipient2->fired;
    });
    EXPECT_FALSE(recipient2->fired);

    // Verify servicemanager dropped its reference too
    sp<IFoo> deadFoo = IFoo::getService("dyingFoo", false);
    if (deadFoo != nullptr) {
        // Got a passthrough
        EXPECT_FALSE(deadFoo->isRemote());
    }
}

TEST_F(HidlTest, BarThisIsNewTest) {
    // Now the tricky part, get access to the derived interface.
    ALOGI("CLIENT call thisIsNew.");
    EXPECT_OK(bar->thisIsNew());
    ALOGI("CLIENT thisIsNew returned.");
}

static void expectGoodChild(const sp<IChild> &child) {
    ASSERT_NE(child.get(), nullptr);
    EXPECT_OK(child->doGrandparent());
    EXPECT_OK(child->doParent());
    EXPECT_OK(child->doChild());
}

static void expectGoodParent(const sp<IParent> &parent) {
    ASSERT_NE(parent.get(), nullptr);
    EXPECT_OK(parent->doGrandparent());
    EXPECT_OK(parent->doParent());
    sp<IChild> child = IChild::castFrom(parent);
    expectGoodChild(child);
}

static void expectGoodGrandparent(const sp<IGrandparent> &grandparent) {
    ASSERT_NE(grandparent.get(), nullptr);
    EXPECT_OK(grandparent->doGrandparent());
    sp<IParent> parent = IParent::castFrom(grandparent);
    expectGoodParent(parent);
}

TEST_F(HidlTest, FooHaveAnInterfaceTest) {

    sp<ISimple> in = new Complicated(42);

    EXPECT_OK(bar->haveAInterface(
                in,
                [&](const auto &out) {
                    ASSERT_NE(out.get(), nullptr);
                    EXPECT_EQ(out->getCookie(), 42);
                    EXPECT_OK(out->customVecInt([&](const auto &) { }));
                    EXPECT_OK(out->customVecStr([&](const auto &) { }));
                    EXPECT_OK(out->interfaceChain([&](const auto &) { }));
                    EXPECT_OK(out->mystr([&](const auto &) { }));
                    EXPECT_OK(out->myhandle([&](const auto &) { }));
                }));
}

TEST_F(HidlTest, InheritRemoteGrandparentTest) {
    EXPECT_OK(fetcher->getGrandparent(true, [&](const sp<IGrandparent>& grandparent) {
        expectGoodGrandparent(grandparent);
    }));
}

TEST_F(HidlTest, InheritLocalGrandparentTest) {
    EXPECT_OK(fetcher->getGrandparent(false, [&](const sp<IGrandparent>& grandparent) {
        expectGoodGrandparent(grandparent);
    }));
}

TEST_F(HidlTest, InheritRemoteParentTest) {
    EXPECT_OK(fetcher->getParent(true, [&](const sp<IParent>& parent) {
        expectGoodParent(parent);
    }));
}

TEST_F(HidlTest, InheritLocalParentTest) {
    EXPECT_OK(fetcher->getParent(false, [&](const sp<IParent>& parent) {
        expectGoodParent(parent);
    }));
}

TEST_F(HidlTest, InheritRemoteChildTest) {
    EXPECT_OK(fetcher->getChild(true, [&](const sp<IChild>& child) {
        expectGoodChild(child);
    }));
}

TEST_F(HidlTest, InheritLocalChildTest) {
    EXPECT_OK(fetcher->getChild(false, [&](const sp<IChild>& child) {
        expectGoodChild(child);
    }));
}

TEST_F(HidlTest, TestArrayDimensionality) {
    hidl_array<int, 2> oneDim;
    hidl_array<int, 2, 3> twoDim;
    hidl_array<int, 2, 3, 4> threeDim;

    EXPECT_EQ(oneDim.size(), 2u);
    EXPECT_EQ(twoDim.size(), std::make_tuple(2u, 3u));
    EXPECT_EQ(threeDim.size(), std::make_tuple(2u, 3u, 4u));
}

TEST_F(HidlTest, StructEqualTest) {
    using G = IFoo::Goober;
    using F = IFoo::Fumble;
    G g1{
        .q = 42,
        .name = "The Ultimate Question of Life, the Universe, and Everything",
        .address = "North Pole",
        .numbers = std::array<double, 10>{ {1, 2, 3, 4, 5, 6, 7, 8, 9, 10} },
        .fumble = F{.data = {.data = 50}},
        .gumble = F{.data = {.data = 60}}
    };
    G g2{
        .q = 42,
        .name = "The Ultimate Question of Life, the Universe, and Everything",
        .address = "North Pole",
        .numbers = std::array<double, 10>{ {1, 2, 3, 4, 5, 6, 7, 8, 9, 10} },
        .fumble = F{.data = {.data = 50}},
        .gumble = F{.data = {.data = 60}}
    };
    G g3{
        .q = 42,
        .name = "The Ultimate Question of Life, the Universe, and Everything",
        .address = "North Pole",
        .numbers = std::array<double, 10>{ {1, 2, 3, 4, 5, 6, 7, 8, 9, 10} },
        .fumble = F{.data = {.data = 50}},
        .gumble = F{.data = {.data = 61}}
    };
    // explicitly invoke operator== here.
    EXPECT_TRUE(g1 == g2);
    EXPECT_TRUE(g1 != g3);
}

TEST_F(HidlTest, EnumEqualTest) {
    using E = IFoo::SomeEnum;
    E e1 = E::quux;
    E e2 = E::quux;
    E e3 = E::goober;
    // explicitly invoke operator== here.
    EXPECT_TRUE(e1 == e2);
    EXPECT_TRUE(e1 != e3);
}

#if HIDL_RUN_POINTER_TESTS

TEST_F(HidlTest, PassAGraphTest) {
    IGraph::Graph g;
    ::android::simpleGraph(g);
    ::android::logSimpleGraph("CLIENT", g);
    ALOGI("CLIENT call passAGraph");
    EXPECT_OK(graphInterface->passAGraph(g));
}

TEST_F(HidlTest, GiveAGraphTest) {
    EXPECT_OK(graphInterface->giveAGraph([&](const auto &newGraph) {
        ::android::logSimpleGraph("CLIENT", newGraph);
        EXPECT_TRUE(::android::isSimpleGraph(newGraph));
    }));
}
TEST_F(HidlTest, PassANodeTest) {
    IGraph::Node node; node.data = 10;
    EXPECT_OK(graphInterface->passANode(node));
}
TEST_F(HidlTest, PassTwoGraphsTest) {
    IGraph::Graph g;
    ::android::simpleGraph(g);
    EXPECT_OK(graphInterface->passTwoGraphs(&g, &g));
}
TEST_F(HidlTest, PassAGammaTest) {
    IGraph::Theta s; s.data = 500;
    IGraph::Alpha a; a.s_ptr = &s;
    IGraph::Beta  b; b.s_ptr = &s;
    IGraph::Gamma c; c.a_ptr = &a; c.b_ptr = &b;
    ALOGI("CLIENT calling passAGamma: c.a = %p, c.b = %p, c.a->s = %p, c.b->s = %p",
        c.a_ptr, c.b_ptr, c.a_ptr->s_ptr, c.b_ptr->s_ptr);
    EXPECT_OK(graphInterface->passAGamma(c));
}
TEST_F(HidlTest, PassNullTest) {
    IGraph::Gamma c;
    c.a_ptr = nullptr;
    c.b_ptr = nullptr;
    EXPECT_OK(graphInterface->passAGamma(c));
}
TEST_F(HidlTest, PassASimpleRefTest) {
    IGraph::Theta s;
    s.data = 500;
    IGraph::Alpha a;
    a.s_ptr = &s;
    EXPECT_OK(graphInterface->passASimpleRef(&a));
}
TEST_F(HidlTest, PassASimpleRefSTest) {
    IGraph::Theta s;
    s.data = 500;
    ALOGI("CLIENT call passASimpleRefS with %p", &s);
    EXPECT_OK(graphInterface->passASimpleRefS(&s));
}
TEST_F(HidlTest, GiveASimpleRefTest) {
    EXPECT_OK(graphInterface->giveASimpleRef([&](const auto & a_ptr) {
        EXPECT_EQ(a_ptr->s_ptr->data, 500);
    }));
}
TEST_F(HidlTest, GraphReportErrorsTest) {
    Return<int32_t> ret = graphInterface->getErrors();
    EXPECT_OK(ret);
    EXPECT_EQ(int32_t(ret), 0);
}

TEST_F(HidlTest, PointerPassOldBufferTest) {
    EXPECT_OK(validationPointerInterface->bar1([&](const auto& sptr, const auto& s) {
        EXPECT_OK(pointerInterface->foo1(sptr, s));
    }));
}
TEST_F(HidlTest, PointerPassOldBufferTest2) {
    EXPECT_OK(validationPointerInterface->bar2([&](const auto& s, const auto& a) {
        EXPECT_OK(pointerInterface->foo2(s, a));
    }));
}
TEST_F(HidlTest, PointerPassSameOldBufferPointerTest) {
    EXPECT_OK(validationPointerInterface->bar3([&](const auto& s, const auto& a, const auto& b) {
        EXPECT_OK(pointerInterface->foo3(s, a, b));
    }));
}
TEST_F(HidlTest, PointerPassOnlyTest) {
    EXPECT_OK(validationPointerInterface->bar4([&](const auto& s) {
        EXPECT_OK(pointerInterface->foo4(s));
    }));
}
TEST_F(HidlTest, PointerPassTwoEmbeddedTest) {
    EXPECT_OK(validationPointerInterface->bar5([&](const auto& a, const auto& b) {
        EXPECT_OK(pointerInterface->foo5(a, b));
    }));
}
TEST_F(HidlTest, PointerPassIndirectBufferHasDataTest) {
    EXPECT_OK(validationPointerInterface->bar6([&](const auto& a) {
        EXPECT_OK(pointerInterface->foo6(a));
    }));
}
TEST_F(HidlTest, PointerPassTwoIndirectBufferTest) {
    EXPECT_OK(validationPointerInterface->bar7([&](const auto& a, const auto& b) {
        EXPECT_OK(pointerInterface->foo7(a, b));
    }));
}
TEST_F(HidlTest, PointerPassDeeplyIndirectTest) {
    EXPECT_OK(validationPointerInterface->bar8([&](const auto& d) {
        EXPECT_OK(pointerInterface->foo8(d));
    }));
}
TEST_F(HidlTest, PointerPassStringRefTest) {
    EXPECT_OK(validationPointerInterface->bar9([&](const auto& str) {
        EXPECT_OK(pointerInterface->foo9(str));
    }));
}
TEST_F(HidlTest, PointerPassRefVecTest) {
    EXPECT_OK(validationPointerInterface->bar10([&](const auto& v) {
        EXPECT_OK(pointerInterface->foo10(v));
    }));
}
TEST_F(HidlTest, PointerPassVecRefTest) {
    EXPECT_OK(validationPointerInterface->bar11([&](const auto& v) {
        EXPECT_OK(pointerInterface->foo11(v));
    }));
}
TEST_F(HidlTest, PointerPassArrayRefTest) {
    EXPECT_OK(validationPointerInterface->bar12([&](const auto& array) {
        EXPECT_OK(pointerInterface->foo12(array));
    }));
}
TEST_F(HidlTest, PointerPassRefArrayTest) {
    EXPECT_OK(validationPointerInterface->bar13([&](const auto& array) {
        EXPECT_OK(pointerInterface->foo13(array));
    }));
}
TEST_F(HidlTest, PointerPass3RefTest) {
    EXPECT_OK(validationPointerInterface->bar14([&](const auto& p3) {
        EXPECT_OK(pointerInterface->foo14(p3));
    }));
}
TEST_F(HidlTest, PointerPassInt3RefTest) {
    EXPECT_OK(validationPointerInterface->bar15([&](const auto& p3) {
        EXPECT_OK(pointerInterface->foo15(p3));
    }));
}
TEST_F(HidlTest, PointerPassEmbeddedPointersTest) {
    EXPECT_OK(validationPointerInterface->bar16([&](const auto& p) {
        EXPECT_OK(pointerInterface->foo16(p));
    }));
}
TEST_F(HidlTest, PointerPassEmbeddedPointers2Test) {
    EXPECT_OK(validationPointerInterface->bar17([&](const auto& p) {
        EXPECT_OK(pointerInterface->foo17(p));
    }));
}
TEST_F(HidlTest, PointerPassCopiedStringTest) {
    EXPECT_OK(validationPointerInterface->bar18([&](const auto& str_ref, const auto& str_ref2, const auto& str) {
        EXPECT_OK(pointerInterface->foo18(str_ref, str_ref2, str));
    }));
}
TEST_F(HidlTest, PointerPassCopiedVecTest) {
    EXPECT_OK(validationPointerInterface->bar19([&](const auto& a_vec_ref, const auto& a_vec, const auto& a_vec_ref2) {
        EXPECT_OK(pointerInterface->foo19(a_vec_ref, a_vec, a_vec_ref2));
    }));
}
TEST_F(HidlTest, PointerPassBigRefVecTest) {
    EXPECT_OK(validationPointerInterface->bar20([&](const auto& v) {
        EXPECT_FAIL(pointerInterface->foo20(v));
    }));
}
TEST_F(HidlTest, PointerPassMultidimArrayRefTest) {
    EXPECT_OK(validationPointerInterface->bar21([&](const auto& v) {
        EXPECT_OK(pointerInterface->foo21(v));
    }));
}
TEST_F(HidlTest, PointerPassRefMultidimArrayTest) {
    EXPECT_OK(validationPointerInterface->bar22([&](const auto& v) {
        EXPECT_OK(pointerInterface->foo22(v));
    }));
}
TEST_F(HidlTest, PointerGiveOldBufferTest) {
    EXPECT_OK(pointerInterface->bar1([&](const auto& sptr, const auto& s) {
        EXPECT_OK(validationPointerInterface->foo1(sptr, s));
    }));
}
TEST_F(HidlTest, PointerGiveOldBufferTest2) {
    EXPECT_OK(pointerInterface->bar2([&](const auto& s, const auto& a) {
        EXPECT_OK(validationPointerInterface->foo2(s, a));
    }));
}
TEST_F(HidlTest, PointerGiveSameOldBufferPointerTest) {
    EXPECT_OK(pointerInterface->bar3([&](const auto& s, const auto& a, const auto& b) {
        EXPECT_OK(validationPointerInterface->foo3(s, a, b));
    }));
}
TEST_F(HidlTest, PointerGiveOnlyTest) {
    EXPECT_OK(pointerInterface->bar4([&](const auto& s) {
        EXPECT_OK(validationPointerInterface->foo4(s));
    }));
}
TEST_F(HidlTest, PointerGiveTwoEmbeddedTest) {
    EXPECT_OK(pointerInterface->bar5([&](const auto& a, const auto& b) {
        EXPECT_OK(validationPointerInterface->foo5(a, b));
    }));
}
TEST_F(HidlTest, PointerGiveIndirectBufferHasDataTest) {
    EXPECT_OK(pointerInterface->bar6([&](const auto& a) {
        EXPECT_OK(validationPointerInterface->foo6(a));
    }));
}
TEST_F(HidlTest, PointerGiveTwoIndirectBufferTest) {
    EXPECT_OK(pointerInterface->bar7([&](const auto& a, const auto& b) {
        EXPECT_OK(validationPointerInterface->foo7(a, b));
    }));
}
TEST_F(HidlTest, PointerGiveDeeplyIndirectTest) {
    EXPECT_OK(pointerInterface->bar8([&](const auto& d) {
        EXPECT_OK(validationPointerInterface->foo8(d));
    }));
}
TEST_F(HidlTest, PointerGiveStringRefTest) {
    EXPECT_OK(pointerInterface->bar9([&](const auto& str) {
        EXPECT_OK(validationPointerInterface->foo9(str));
    }));
}
TEST_F(HidlTest, PointerGiveRefVecTest) {
    EXPECT_OK(pointerInterface->bar10([&](const auto& v) {
        EXPECT_OK(validationPointerInterface->foo10(v));
    }));
}
TEST_F(HidlTest, PointerGiveVecRefTest) {
    EXPECT_OK(pointerInterface->bar11([&](const auto& v) {
        EXPECT_OK(validationPointerInterface->foo11(v));
    }));
}
TEST_F(HidlTest, PointerGiveArrayRefTest) {
    EXPECT_OK(pointerInterface->bar12([&](const auto& array) {
        EXPECT_OK(validationPointerInterface->foo12(array));
    }));
}
TEST_F(HidlTest, PointerGiveRefArrayTest) {
    EXPECT_OK(pointerInterface->bar13([&](const auto& array) {
        EXPECT_OK(validationPointerInterface->foo13(array));
    }));
}
TEST_F(HidlTest, PointerGive3RefTest) {
    EXPECT_OK(pointerInterface->bar14([&](const auto& p3) {
        EXPECT_OK(validationPointerInterface->foo14(p3));
    }));
}
TEST_F(HidlTest, PointerGiveInt3RefTest) {
    EXPECT_OK(pointerInterface->bar15([&](const auto& p3) {
        EXPECT_OK(validationPointerInterface->foo15(p3));
    }));
}
TEST_F(HidlTest, PointerGiveEmbeddedPointersTest) {
    EXPECT_OK(pointerInterface->bar16([&](const auto& p) {
        EXPECT_OK(validationPointerInterface->foo16(p));
    }));
}
TEST_F(HidlTest, PointerGiveEmbeddedPointers2Test) {
    EXPECT_OK(pointerInterface->bar17([&](const auto& p) {
        EXPECT_OK(validationPointerInterface->foo17(p));
    }));
}
TEST_F(HidlTest, PointerGiveCopiedStringTest) {
    EXPECT_OK(pointerInterface->bar18([&](const auto& str_ref, const auto& str_ref2, const auto& str) {
        EXPECT_OK(validationPointerInterface->foo18(str_ref, str_ref2, str));
    }));
}
TEST_F(HidlTest, PointerGiveCopiedVecTest) {
    EXPECT_OK(pointerInterface->bar19([&](const auto& a_vec_ref, const auto& a_vec, const auto& a_vec_ref2) {
        EXPECT_OK(validationPointerInterface->foo19(a_vec_ref, a_vec, a_vec_ref2));
    }));
}
// This cannot be enabled until _hidl_error is not ignored when
// the remote writeEmbeddedReferencesToParcel.
// TEST_F(HidlTest, PointerGiveBigRefVecTest) {
//     EXPECT_FAIL(pointerInterface->bar20([&](const auto& v) {
//     }));
// }
TEST_F(HidlTest, PointerGiveMultidimArrayRefTest) {
    EXPECT_OK(pointerInterface->bar21([&](const auto& v) {
        EXPECT_OK(validationPointerInterface->foo21(v));
    }));
}
TEST_F(HidlTest, PointerGiveRefMultidimArrayTest) {
    EXPECT_OK(pointerInterface->bar22([&](const auto& v) {
        EXPECT_OK(validationPointerInterface->foo22(v));
    }));
}
TEST_F(HidlTest, PointerReportErrorsTest) {
    Return<int32_t> ret = pointerInterface->getErrors();
    EXPECT_OK(ret);
    EXPECT_EQ(int32_t(ret), 0);
}
#endif

int forkAndRunTests(TestMode mode) {
    pid_t child;
    int status;

    const char* modeText = (mode == BINDERIZED) ? "BINDERIZED" : "PASSTHROUGH";
    ALOGI("Start running tests in %s mode...", modeText);
    fprintf(stdout, "Start running tests in %s mode...\n", modeText);
    fflush(stdout);

    if ((child = fork()) == 0) {
        gMode = mode;
        if (gMode == PASSTHROUGH) {
            gPassthroughEnvironment = static_cast<PassthroughEnvironment *>(
                ::testing::AddGlobalTestEnvironment(new PassthroughEnvironment));
        } else if (gMode == BINDERIZED) {
            gBinderizedEnvironment = static_cast<BinderizedEnvironment *>(
                ::testing::AddGlobalTestEnvironment(new BinderizedEnvironment));
        }
        int testStatus = RUN_ALL_TESTS();
        if(testStatus == 0) {
            exit(0);
        }
        int failed = ::testing::UnitTest::GetInstance()->failed_test_count();
        if (failed == 0) {
            exit(-testStatus);
        }
        exit(failed);
    }
    waitpid(child, &status, 0 /* options */);
    ALOGI("All tests finished in %s mode.", modeText);
    fprintf(stdout, "All tests finished in %s mode.\n", modeText);
    fflush(stdout);
    return status;
}

void handleStatus(int status, const char *mode) {
    if (status != 0) {
        if (WIFEXITED(status)) {
            status = WEXITSTATUS(status);
            if (status < 0) {
                fprintf(stdout, "    RUN_ALL_TESTS returns %d for %s mode.\n", -status, mode);
            } else {
                fprintf(stdout, "    %d test(s) failed for %s mode.\n", status, mode);
            }
        } else {
            fprintf(stdout, "    ERROR: %s child process exited abnormally with %d\n", mode, status);
        }
    }
}

static void usage(const char *me) {
    fprintf(stderr,
            "usage: %s [-b] [-p] [GTEST_OPTIONS]\n",
            me);

    fprintf(stderr, "         -b binderized mode only\n");
    fprintf(stderr, "         -p passthrough mode only\n");
    fprintf(stderr, "            (if -b and -p are both missing or both present, "
                                 "both modes are tested.)\n");
}

int main(int argc, char **argv) {
    const char *me = argv[0];
    bool b = false;
    bool p = false;
    struct option longopts[] = {{0,0,0,0}};
    int res;
    while ((res = getopt_long(argc, argv, "hbp", longopts, NULL)) >= 0) {
        switch (res) {
            case 'h': {
                usage(me);
                exit(1);
            } break;

            case 'b': {
                b = true;
            } break;

            case 'p': {
                p = true;
            } break;

            case '?':
            default: {
                // ignore. pass to gTest.
            } break;
        }
    }
    if (!b && !p) {
        b = p = true;
    }

    ::testing::InitGoogleTest(&argc, argv);
    // put test in child process because RUN_ALL_TESTS
    // should not be run twice.
    int pStatus = p ? forkAndRunTests(PASSTHROUGH) : 0;
    int bStatus = b ? forkAndRunTests(BINDERIZED)  : 0;

    fprintf(stdout, "\n=========================================================\n\n"
                    "    Summary:\n\n");
    if (p) {
        ALOGI("PASSTHROUGH Test result = %d", pStatus);
        handleStatus(pStatus, "PASSTHROUGH");
    }
    if (b) {
        ALOGI("BINDERIZED Test result = %d", bStatus);
        handleStatus(bStatus, "BINDERIZED ");
    }

    if (pStatus == 0 && bStatus == 0) {
        fprintf(stdout, "    Hooray! All tests passed.\n");
    }
    fprintf(stdout, "\n=========================================================\n\n");

    return pStatus + bStatus;
}
