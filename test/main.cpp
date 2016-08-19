#define LOG_TAG "hidl_test"
#include <android-base/logging.h>

#include <android/hardware/tests/foo/1.0/BnFoo.h>
#include <android/hardware/tests/foo/1.0/BnFooCallback.h>
#include <android/hardware/tests/bar/1.0/BnBar.h>

#include <gtest/gtest.h>
#if GTEST_IS_THREADSAFE
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#else
#error "GTest did not detect pthread library."
#endif

#include <hidl/IServiceManager.h>
#include <hwbinder/IPCThreadState.h>
#include <hwbinder/ProcessState.h>
#include <hwbinder/Status.h>

using ::android::hardware::tests::foo::V1_0::BnFoo;
using ::android::hardware::tests::foo::V1_0::BnFooCallback;
using ::android::hardware::tests::bar::V1_0::BnBar;
using ::android::hardware::tests::foo::V1_0::IFoo;
using ::android::hardware::tests::foo::V1_0::IFooCallback;
using ::android::hardware::tests::bar::V1_0::IBar;
using ::android::hardware::tests::foo::V1_0::Abc;
using ::android::hardware::Return;
using ::android::hardware::Status;
using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_string;
using ::android::sp;

struct FooCallback : public BnFooCallback {
    Status heyItsYou(const sp<IFooCallback> &cb) override;
    Return<bool> heyItsYouIsntIt(const sp<IFooCallback> &cb) override;
    Status heyItsTheMeaningOfLife(uint8_t tmol) override;
};

Status FooCallback::heyItsYou(
        const sp<IFooCallback> &_cb) {
    ALOGI("SERVER(FooCallback) heyItsYou cb = %p", _cb.get());
    return Status::ok();
}

Return<bool> FooCallback::heyItsYouIsntIt(const sp<IFooCallback> &_cb) {
    ALOGI("SERVER(FooCallback) heyItsYouIsntIt cb = %p sleeping for 10 seconds", _cb.get());
    sleep(10);
    ALOGI("SERVER(FooCallback) heyItsYouIsntIt cb = %p responding", _cb.get());
    return true;
}

Status FooCallback::heyItsTheMeaningOfLife(uint8_t tmol) {
    ALOGI("SERVER(FooCallback) heyItsTheMeaningOfLife = %d sleeping for 10 seconds", tmol);
    sleep(10);
    ALOGI("SERVER(FooCallback) heyItsTheMeaningOfLife = %d done sleeping", tmol);
    return Status::ok();
}

struct Bar : public BnBar {
    Status doThis(float param) override;

    Return<int32_t> doThatAndReturnSomething(int64_t param) override;

    Return<double> doQuiteABit(
            int32_t a,
            int64_t b,
            float c,
            double d) override;

    Status doSomethingElse(
            const int32_t param[15], doSomethingElse_cb _cb) override;

    Status doStuffAndReturnAString(
            doStuffAndReturnAString_cb _cb) override;

    Status mapThisVector(
            const hidl_vec<int32_t> &param, mapThisVector_cb _cb) override;

    Status callMe(
            const sp<IFooCallback> &cb) override;

    Return<SomeEnum> useAnEnum(SomeEnum param) override;

    Status haveAGooberVec(const hidl_vec<Goober>& param) override;
    Status haveAGoober(const Goober &g) override;
    Status haveAGooberArray(const Goober lots[20]) override;

    Status haveATypeFromAnotherFile(const Abc &def) override;

    Status haveSomeStrings(
            const hidl_string array[3],
            haveSomeStrings_cb _cb) override;

    Status haveAStringVec(
            const hidl_vec<hidl_string> &vector,
            haveAStringVec_cb _cb) override;

    Status thisIsNew() override;
};

Status Bar::doThis(float param) {
    ALOGI("SERVER(Bar) doThis(%.2f)", param);

    return Status::ok();
}

Return<int32_t> Bar::doThatAndReturnSomething(
        int64_t param) {
    ALOGI("SERVER(Bar) doThatAndReturnSomething(%ld)", param);

    return 666;
}

Return<double> Bar::doQuiteABit(
        int32_t a,
        int64_t b,
        float c,
        double d) {
    ALOGI("SERVER(Bar) doQuiteABit(%d, %ld, %.2f, %.2f)", a, b, c, d);

    return 666.5;
}

Status Bar::doSomethingElse(
        const int32_t param[15], doSomethingElse_cb _cb) {
    ALOGI("SERVER(Bar) doSomethingElse(...)");

    int32_t result[32] = { 0 };
    for (size_t i = 0; i < 15; ++i) {
        result[i] = 2 * param[i];
        result[15 + i] = param[i];
    }
    result[30] = 1;
    result[31] = 2;

    _cb(result);

    return Status::ok();
}

Status Bar::doStuffAndReturnAString(
        doStuffAndReturnAString_cb _cb) {
    ALOGI("SERVER(Bar) doStuffAndReturnAString");

    hidl_string s;
    s = "Hello, world";

    _cb(s);

    return Status::ok();
}

Status Bar::mapThisVector(
        const hidl_vec<int32_t> &param, mapThisVector_cb _cb) {
    ALOGI("SERVER(Bar) mapThisVector");

    hidl_vec<int32_t> out;
    out.resize(param.size());

    for (size_t i = 0; i < out.size(); ++i) {
        out[i] = param[i] * 2;
    }

    _cb(out);

    return Status::ok();
}

Status Bar::callMe(
        const sp<IFooCallback> &cb) {
    ALOGI("SERVER(Bar) callMe %p", cb.get());

    if (cb != NULL) {
        ALOGI("SERVER(Bar) callMe %p calling IFooCallback::heyItsYou, should return immediately", cb.get());
        cb->heyItsYou(cb);
        ALOGI("SERVER(Bar) callMe %p calling IFooCallback::heyItsYouIsntIt, should block for 10 seconds", cb.get());
        bool answer = cb->heyItsYouIsntIt(cb);
        ALOGI("SERVER(Bar) callMe %p IFooCallback::heyItsYouIsntIt responded with %d", cb.get(), answer);
        ALOGI("SERVER(Bar) callMe %p calling IFooCallback::heyItsTheMeaningOfLife, should return immediately", cb.get());
        cb->heyItsTheMeaningOfLife(42);
        ALOGI("SERVER(Bar) callMe %p After call to IFooCallback::heyItsTheMeaningOfLife", cb.get());
    }

    return Status::ok();
}

Return<Bar::SomeEnum> Bar::useAnEnum(SomeEnum param) {
    ALOGI("SERVER(Bar) useAnEnum %d", (int)param);

    return SomeEnum::goober;
}

Status Bar::haveAGooberVec(const hidl_vec<Goober>& param) {
    ALOGI("SERVER(Bar) haveAGooberVec &param = %p", &param);

    return Status::ok();
}

Status Bar::haveAGoober(const Goober &g) {
    ALOGI("SERVER(Bar) haveaGoober g=%p", &g);

    return Status::ok();
}

Status Bar::haveAGooberArray(const Goober lots[20]) {
    ALOGI("SERVER(Bar) haveAGooberArray lots = %p", lots);

    return Status::ok();
}

Status Bar::haveATypeFromAnotherFile(const Abc &def) {
    ALOGI("SERVER(Bar) haveATypeFromAnotherFile def=%p", &def);

    return Status::ok();
}

Status Bar::haveSomeStrings(
        const hidl_string array[3],
        haveSomeStrings_cb _cb) {
    ALOGI("SERVER(Bar) haveSomeStrings([\"%s\", \"%s\", \"%s\"])",
          array[0].c_str(),
          array[1].c_str(),
          array[2].c_str());

    hidl_string result[2];
    result[0] = "Hello";
    result[1] = "World";

    _cb(result);

    return Status::ok();
}

Status Bar::haveAStringVec(
        const hidl_vec<hidl_string> &vector,
        haveAStringVec_cb _cb) {
    ALOGI("SERVER(Bar) haveAStringVec([\"%s\", \"%s\", \"%s\"])",
          vector[0].c_str(),
          vector[1].c_str(),
          vector[2].c_str());

    hidl_vec<hidl_string> result;
    result.resize(2);

    result[0] = "Hello";
    result[1] = "World";

    _cb(result);

    return Status::ok();
}

Status Bar::thisIsNew() {
    ALOGI("SERVER(Bar) thisIsNew");

    return Status::ok();
}

template<typename I>
static std::string arraylikeToString(const I data, size_t size) {
    std::string out = "[";
    for (size_t i = 0; i < size; ++i) {
        if (i > 0) {
            out += ", ";
        }

        out += ::android::String8::format("%d", data[i]).string();
    }
    out += "]";

    return out;
}


static std::string vecToString(const hidl_vec<int32_t> &vec) {
    return arraylikeToString(vec, vec.size());
}

static inline void EXPECT_OK(::android::hardware::Status status) {
    EXPECT_TRUE(status.isOk());
}

template<typename T, typename S>
static inline bool EXPECT_ARRAYEQ(const T arr1, const S arr2, size_t size) {
    for(size_t i = 0; i < size; i++)
        if(arr1[i] != arr2[i])
            return false;
    return true;
}


template <class T>
static void startServer(T server, const android::hardware::hidl_version kVersion,
                        const char *serviceName,
                        const char *tag) {
    using namespace android::hardware;
    using android::String16;
    ALOGI("SERVER(%s) registering", tag);
    defaultServiceManager()->addService(String16(serviceName), server, kVersion);
    ALOGI("SERVER(%s) starting", tag);
    ProcessState::self()->setThreadPoolMaxThreadCount(0);
    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool(); // never ends. needs kill().
    ALOGI("SERVER(%s) ends.", tag);
}


class HidlTest : public ::testing::Test {
public:
    sp<::android::hardware::IBinder> service;
    sp<IFoo> foo;
    sp<IBar> bar;
    sp<::android::hardware::IBinder> cbService;
    virtual void SetUp() override {
        ALOGI("Test setup beginning...");
        using namespace android::hardware;
        using android::String16;
        const hidl_version kVersion = make_hidl_version(1, 0);

        service =
            defaultServiceManager()->getService(String16("foo"), kVersion);
        ALOGI("CLIENT Found service foo.");

        CHECK(service != NULL);

        foo = IFoo::asInterface(service);
        CHECK(foo != NULL);

        bar = IBar::asInterface(service);
        CHECK(bar != NULL);

        cbService = defaultServiceManager()->getService(String16("foo callback"), kVersion);
        ALOGI("Test setup complete");
    }
    virtual void TearDown() override {
    }
};

class HidlEnvironment : public ::testing::Environment {
private:
    pid_t fooCallbackServerPid, barServerPid;
public:
    virtual void SetUp() {
        ALOGI("Environment setup beginning...");
        // use fork to create and kill to destroy server processes.
        if ((barServerPid = fork()) == 0) {
            // Fear me, I am a child.
            startServer(new Bar, android::hardware::make_hidl_version(1, 0),
                "foo", "Bar"); // never returns
            return;
        }

        if ((fooCallbackServerPid = fork()) == 0) {
            // Fear me, I am a second child.
            startServer(new FooCallback, android::hardware::make_hidl_version(1, 0),
                "foo callback", "FooCalback"); // never returns
            return;
        }

        // Fear you not, I am parent.
        sleep(1);
        ALOGI("Environment setup complete.");
    }

    virtual void TearDown() {
        // clean up by killing server processes.
        ALOGI("Environment tear-down beginning...");
        ALOGI("Killing servers...");
        if(kill(barServerPid, SIGTERM)) {
            ALOGE("Could not kill barServer; errno = %d", errno);
        } else {
            int status;
            ALOGI("Waiting for barServer to exit...");
            waitpid(barServerPid, &status, 0);
            ALOGI("Continuing...");
        }
        if(kill(fooCallbackServerPid, SIGTERM)) {
            ALOGE("Could not kill fooCallbackServer; errno = %d", errno);
        } else {
            int status;
            ALOGI("Waiting for fooCallbackServer to exit...");
            waitpid(barServerPid, &status, 0);
            ALOGI("Continuing...");
        }
        ALOGI("Servers all killed.");
        ALOGI("Environment tear-down complete.");
    }
};

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
    int32_t param[15];
    for (size_t i = 0; i < sizeof(param) / sizeof(param[0]); ++i) {
        param[i] = i;
    }
    EXPECT_OK(foo->doSomethingElse(param, [&](const auto &something) {
            ALOGI("CLIENT doSomethingElse returned %s.",
                  arraylikeToString(something, 32).c_str());
            int32_t expect[] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24,
                26, 28, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 1, 2};
            EXPECT_ARRAYEQ(something, expect, 32);
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
                  vecToString(something).c_str());
            int32_t expect[] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18};
            EXPECT_ARRAYEQ(something, expect, something.size());
        }));
}

TEST_F(HidlTest, FooCallMeTest) {
    ALOGI("CLIENT call callMe.");
    EXPECT_OK(foo->callMe(IFooCallback::asInterface(cbService)));
    ALOGI("CLIENT callMe returned.");
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
    IFoo::Goober gooberArrayParam[20];
    EXPECT_OK(foo->haveAGooberArray(gooberArrayParam));
    ALOGI("CLIENT haveAGooberArray returned.");
}

TEST_F(HidlTest, FooHaveATypeFromAnotherFileTest) {
    ALOGI("CLIENT call haveATypeFromAnotherFile.");
    Abc abcParam{};
    abcParam.x = "alphabet";
    abcParam.y = 3.14f;
    abcParam.z = new native_handle_t();
    EXPECT_OK(foo->haveATypeFromAnotherFile(abcParam));
    ALOGI("CLIENT haveATypeFromAnotherFile returned.");
    delete abcParam.z;
    abcParam.z = NULL;
}

TEST_F(HidlTest, FooHaveSomeStringsTest) {
    ALOGI("CLIENT call haveSomeStrings.");
    hidl_string stringArrayParam[3];
    stringArrayParam[0] = "What";
    stringArrayParam[1] = "a";
    stringArrayParam[2] = "disaster";
    EXPECT_OK(foo->haveSomeStrings(stringArrayParam));
    ALOGI("CLIENT haveSomeStrings returned.");
}

TEST_F(HidlTest, FooHaveAStringVecTest) {
    ALOGI("CLIENT call haveAStringVec.");
    hidl_vec<hidl_string> stringVecParam;
    stringVecParam.resize(3);
    stringVecParam[0] = "What";
    stringVecParam[1] = "a";
    stringVecParam[2] = "disaster";
    EXPECT_OK(foo->haveAStringVec(stringVecParam));
    ALOGI("CLIENT haveAStringVec returned.");
}

TEST_F(HidlTest, BarThisIsNewTest) {
// Now the tricky part, get access to the derived interface.
    ALOGI("CLIENT call thisIsNew.");
    EXPECT_OK(bar->thisIsNew());
    ALOGI("CLIENT thisIsNew returned.");
}

int main(int argc, char **argv) {

    ::testing::AddGlobalTestEnvironment(new HidlEnvironment);
    ::testing::InitGoogleTest(&argc, argv);
    int status = RUN_ALL_TESTS();

    ALOGI("Test result = %d", status);
    return status;
}
