#define LOG_TAG "hidl_test"
#include <android-base/logging.h>

#include <android/hardware/tests/foo/1.0/BnFoo.h>
#include <android/hardware/tests/foo/1.0/BnFooCallback.h>
#include <android/hardware/tests/bar/1.0/BnBar.h>

#include <hwbinder/IPCThreadState.h>
#include <hwbinder/IServiceManager.h>
#include <hwbinder/ProcessState.h>
#include <hwbinder/Status.h>

using ::android::hardware::tests::foo::V1_0::BnFoo;
using ::android::hardware::tests::foo::V1_0::BnFooCallback;
using ::android::hardware::tests::bar::V1_0::BnBar;
using ::android::hardware::tests::foo::V1_0::IFoo;
using ::android::hardware::tests::foo::V1_0::IFooCallback;
using ::android::hardware::tests::bar::V1_0::IBar;
using ::android::hardware::tests::foo::V1_0::Abc;
using ::android::hardware::Status;
using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_string;
using ::android::sp;

struct FooCallback : public BnFooCallback {
    Status heyItsYou(
            const sp<IFooCallback> &cb) override;
    Status heyItsYouIsntIt(const sp<IFooCallback> &cb, heyItsYouIsntIt_cb _cb) override;
    Status heyItsTheMeaningOfLife(uint8_t tmol) override;
};

Status FooCallback::heyItsYou(
        const sp<IFooCallback> &_cb) {
    ALOGI("SERVER(FooCallback) heyItsYou cb = %p", _cb.get());
    return Status::ok();
}

Status FooCallback::heyItsYouIsntIt(const sp<IFooCallback> &_cb,
        heyItsYouIsntIt_cb _hidl_cb) {
    ALOGI("SERVER(FooCallback) heyItsYouIsntIt cb = %p sleeping for 10 seconds", _cb.get());
    sleep(10);
    ALOGI("SERVER(FooCallback) heyItsYouIsntIt cb = %p responding", _cb.get());
    if (_hidl_cb)
      _hidl_cb(true);
    return Status::ok();
}

Status FooCallback::heyItsTheMeaningOfLife(uint8_t tmol) {
    ALOGI("SERVER(FooCallback) heyItsTheMeaningOfLife = %d sleeping for 10 seconds", tmol);
    sleep(10);
    ALOGI("SERVER(FooCallback) heyItsTheMeaningOfLife = %d done sleeping", tmol);
    return Status::ok();
}

struct Bar : public BnBar {
    Status doThis(float param) override;

    Status doThatAndReturnSomething(
            int64_t param, doThatAndReturnSomething_cb _cb) override;

    Status doQuiteABit(
            int32_t a,
            int64_t b,
            float c,
            double d,
            doQuiteABit_cb _cb) override;

    Status doSomethingElse(
            const int32_t param[15], doSomethingElse_cb _cb) override;

    Status doStuffAndReturnAString(
            doStuffAndReturnAString_cb _cb) override;

    Status mapThisVector(
            const hidl_vec<int32_t> &param, mapThisVector_cb _cb) override;

    Status callMe(
            const sp<IFooCallback> &cb) override;

    Status useAnEnum(
            SomeEnum param, useAnEnum_cb _cb) override;

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

Status Bar::doThatAndReturnSomething(
        int64_t param, doThatAndReturnSomething_cb _cb) {
    ALOGI("SERVER(Bar) doThatAndReturnSomething(%ld)", param);

    _cb(666);

    return Status::ok();
}

Status Bar::doQuiteABit(
        int32_t a,
        int64_t b,
        float c,
        double d,
        doQuiteABit_cb _cb) {
    ALOGI("SERVER(Bar) doQuiteABit(%d, %ld, %.2f, %.2f)", a, b, c, d);

    _cb(666.5);

    return Status::ok();
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
        uint8_t answer = false;
        ALOGI("SERVER(Bar) callMe %p calling IFooCallback::heyItsYouIsntIt, should block for 10 seconds", cb.get());
        cb->heyItsYouIsntIt(cb, [&answer](uint8_t response) {answer = response;});
        ALOGI("SERVER(Bar) callMe %p IFooCallback::heyItsYouIsntIt responded with %d", cb.get(), answer);
        ALOGI("SERVER(Bar) callMe %p calling IFooCallback::heyItsTheMeaningOfLife, should return immediately", cb.get());
        cb->heyItsTheMeaningOfLife(42);
        ALOGI("SERVER(Bar) callMe %p After call to IFooCallback::heyItsTheMeaningOfLife", cb.get());
    }

    return Status::ok();
}

Status Bar::useAnEnum(
        SomeEnum param, useAnEnum_cb _cb) {
    ALOGI("SERVER(Bar) useAnEnum %d", (int)param);

    _cb(SomeEnum::goober);

    return Status::ok();
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

static void client() {
    using namespace android::hardware;
    using android::String16;

    const hidl_version kVersion = make_hidl_version(1, 0);

    sp<IBinder> service =
        defaultServiceManager()->getService(String16("foo"), kVersion);

    CHECK(service != NULL);

    sp<IFoo> foo = IFoo::asInterface(service);
    CHECK(foo != NULL);

    ALOGI("CLIENT call doThis.");
    foo->doThis(1.0f);
    ALOGI("CLIENT doThis returned.");

    ALOGI("CLIENT call doThatAndReturnSomething.");
    foo->doThatAndReturnSomething(
            2.0f, [&](auto result) {
                ALOGI("CLIENT doThatAndReturnSomething returned %d.", result);
            });

    ALOGI("CLIENT call doQuiteABit");
    foo->doQuiteABit(
            1, 2, 3.0f, 4.0, [&](auto something) {
                ALOGI("CLIENT doQuiteABit returned %f.", something);
            });

    ALOGI("CLIENT call doSomethingElse");
    int32_t param[15];
    for (size_t i = 0; i < sizeof(param) / sizeof(param[0]); ++i) {
        param[i] = i;
    }
    foo->doSomethingElse(param, [&](const auto &something) {
            ALOGI("CLIENT doSomethingElse returned %s.",
                  arraylikeToString(something, 32).c_str());
        });

    ALOGI("CLIENT call doStuffAndReturnAString");
    foo->doStuffAndReturnAString([&](const auto &something) {
            ALOGI("CLIENT doStuffAndReturnAString returned '%s'.",
                  something.c_str());
        });

    hidl_vec<int32_t> vecParam;
    vecParam.resize(10);
    for (size_t i = 0; i < 10; ++i) {
        vecParam[i] = i;
    }
    foo->mapThisVector(vecParam, [&](const auto &something) {
            ALOGI("CLIENT mapThisVector returned %s.",
                  vecToString(something).c_str());
        });

    ALOGI("CLIENT call callMe.");
    sp<IBinder> cbService = defaultServiceManager()->getService(String16("foo callback"), kVersion);
    foo->callMe(IFooCallback::asInterface(cbService));
    ALOGI("CLIENT callMe returned.");

    ALOGI("CLIENT call userAnEnum.");
    foo->useAnEnum(IFoo::SomeEnum::quux, [&](auto sleepy) {
                ALOGI("CLIENT useAnEnum returned %u", (unsigned)sleepy);
            });

    hidl_vec<IFoo::Goober> gooberVecParam;
    gooberVecParam.resize(2);
    gooberVecParam[0].name = "Hello";
    gooberVecParam[1].name = "World";

    ALOGI("CLIENT call haveAGooberVec.");
    foo->haveAGooberVec(gooberVecParam);
    ALOGI("CLIENT haveAGooberVec returned.");

    ALOGI("CLIENT call haveaGoober.");
    foo->haveAGoober(gooberVecParam[0]);
    ALOGI("CLIENT haveaGoober returned.");

    ALOGI("CLIENT call haveAGooberArray.");
    IFoo::Goober gooberArrayParam[20];
    foo->haveAGooberArray(gooberArrayParam);
    ALOGI("CLIENT haveAGooberArray returned.");

    ALOGI("CLIENT call haveATypeFromAnotherFile.");
    Abc abcParam{};
    abcParam.x = "alphabet";
    abcParam.y = 3.14f;
    abcParam.z = new native_handle_t();
    foo->haveATypeFromAnotherFile(abcParam);
    ALOGI("CLIENT haveATypeFromAnotherFile returned.");
    delete abcParam.z;
    abcParam.z = NULL;

    ALOGI("CLIENT call haveSomeStrings.");
    hidl_string stringArrayParam[3];
    stringArrayParam[0] = "What";
    stringArrayParam[1] = "a";
    stringArrayParam[2] = "disaster";
    foo->haveSomeStrings(stringArrayParam);
    ALOGI("CLIENT haveSomeStrings returned.");

    ALOGI("CLIENT call haveAStringVec.");
    hidl_vec<hidl_string> stringVecParam;
    stringVecParam.resize(3);
    stringVecParam[0] = "What";
    stringVecParam[1] = "a";
    stringVecParam[2] = "disaster";
    foo->haveAStringVec(stringVecParam);
    ALOGI("CLIENT haveAStringVec returned.");

    // Now the tricky part, get access to the derived interface.

    sp<IBar> bar = IBar::asInterface(service);
    CHECK(bar != NULL);

    ALOGI("CLIENT call thisIsNew.");
    bar->thisIsNew();
    ALOGI("CLIENT thisIsNew returned.");
}

int main() {
    using namespace android::hardware;
    using android::String16;

    if (fork() == 0) {
        sleep(1);

        // Fear me, I am child.
        client();

        return 0;
    }

    if (fork() == 0) {
        ALOGI("SERVER(FooCallback) registering");
        sp<FooCallback> fcb = new FooCallback;
        const hidl_version kVersion = make_hidl_version(1, 0);
        defaultServiceManager()->addService(String16("foo callback"), fcb, kVersion);
        ALOGI("SERVER(FooCallback) starting");
        ProcessState::self()->setThreadPoolMaxThreadCount(0);
        ProcessState::self()->startThreadPool();
        IPCThreadState::self()->joinThreadPool();

        return 0;
    }

    ALOGI("SERVER(Bar) registering");
    sp<Bar> bar = new Bar;
    const hidl_version kVersion = make_hidl_version(1, 0);
    defaultServiceManager()->addService(String16("foo"), bar, kVersion);
    ALOGI("SERVER(Bar) starting");
    ProcessState::self()->setThreadPoolMaxThreadCount(0);
    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();

    return 0;
}
