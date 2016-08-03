#include <android-base/logging.h>

#include <android/hardware/foo/1.0/BnFoo.h>
#include <android/hardware/foo/1.0/BnFooCallback.h>

#include <hwbinder/IPCThreadState.h>
#include <hwbinder/IServiceManager.h>
#include <hwbinder/ProcessState.h>
#include <hwbinder/Status.h>

using ::android::hardware::foo::V1_0::BnFoo;
using ::android::hardware::foo::V1_0::BnFooCallback;
using ::android::hardware::foo::V1_0::IFoo;
using ::android::hardware::foo::V1_0::IFooCallback;
using ::android::hidl::binder::Status;
using ::android::hidl::hidl_vec;
using ::android::hidl::hidl_string;
using ::android::sp;

struct FooCallback : public BnFooCallback {
    Status heyItsMe(
            const sp<IFooCallback> &cb) override;
};

Status FooCallback::heyItsMe(
        const sp<IFooCallback> &cb) {
    ALOGI("SERVER: heyItsMe cb = %p", cb.get());

    return Status::ok();
}

struct Foo : public BnFoo {
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

    Status hmmm(const hidl_vec<Goober>& param) override;
    Status runningOutOfNames(const Goober &g) override;

    Status aGooberArray(const Goober lots[20]) override;

    Status haveSomeStrings(
            const hidl_string array[3],
            haveSomeStrings_cb _cb) override;

    Status haveAStringVec(
            const hidl_vec<hidl_string> &vector,
            haveAStringVec_cb _cb) override;
};

Status Foo::doThis(float param) {
    ALOGI("Foo::doThis(%.2f)", param);

    return Status::ok();
}

Status Foo::doThatAndReturnSomething(
        int64_t param, doThatAndReturnSomething_cb _cb) {
    ALOGI("Foo::doThatAndReturnSomething(%ld)", param);

    _cb(666);

    return Status::ok();
}

Status Foo::doQuiteABit(
        int32_t a,
        int64_t b,
        float c,
        double d,
        doQuiteABit_cb _cb) {
    ALOGI("Foo::doQuiteABit(%d, %ld, %.2f, %.2f)", a, b, c, d);

    _cb(666.5);

    return Status::ok();
}

Status Foo::doSomethingElse(
        const int32_t param[15], doSomethingElse_cb _cb) {
    ALOGI("Foo::doSomethingElse(...)");

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

Status Foo::doStuffAndReturnAString(
        doStuffAndReturnAString_cb _cb) {
    ALOGI("Foo::doStuffAndReturnAString");

    hidl_string s;
    s = "Hello, world";

    _cb(s);

    return Status::ok();
}

Status Foo::mapThisVector(
        const hidl_vec<int32_t> &param, mapThisVector_cb _cb) {
    ALOGI("Foo::mapThisVector");

    hidl_vec<int32_t> out;
    out.count = param.count;
    out.buffer = new int32_t[out.count];
    for (size_t i = 0; i < out.count; ++i) {
        out.buffer[i] = param.buffer[i] * 2;
    }

    _cb(out);

    delete[] out.buffer;

    return Status::ok();
}

Status Foo::callMe(
        const sp<IFooCallback> &cb) {
    ALOGI("Foo::callMe %p", cb.get());

    if (cb != NULL) {
        sp<IFooCallback> my_cb = new FooCallback;
        cb->heyItsMe(my_cb);
    }

    return Status::ok();
}

Status Foo::useAnEnum(
        SomeEnum param, useAnEnum_cb _cb) {
    ALOGI("Foo::useAnEnum %d", (int)param);

    _cb(SomeEnum::goober);

    return Status::ok();
}

Status Foo::haveSomeStrings(
        const hidl_string array[3],
        haveSomeStrings_cb _cb) {
    ALOGI("Foo::haveSomeStrings([\"%s\", \"%s\", \"%s\"])",
          array[0].c_str(),
          array[1].c_str(),
          array[2].c_str());

    hidl_string result[2];
    result[0] = "Hello";
    result[1] = "World";

    _cb(result);

    return Status::ok();
}

Status Foo::haveAStringVec(
        const hidl_vec<hidl_string> &vector,
        haveAStringVec_cb _cb) {
    ALOGI("Foo::haveAStringVec([\"%s\", \"%s\", \"%s\"])",
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

Status Foo::hmmm(const hidl_vec<Goober>& param) {
    ALOGI("Foo::hmmm &param = %p", &param);

    return Status::ok();
}

Status Foo::runningOutOfNames(const Goober &g) {
    ALOGI("Foo::runningOutOfNames g=%p", &g);

    return Status::ok();
}

Status Foo::aGooberArray(const Goober lots[20]) {
    ALOGI("Foo::aGooberArray lots = %p", lots);

    return Status::ok();
}

static std::string arrayToString(const int32_t *data, size_t size) {
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
    return arrayToString(vec.buffer, vec.count);
}

static void client() {
    using namespace android::hidl;
    using android::String16;

    const hidl_version kVersion = make_hidl_version(1, 0);

    sp<IBinder> service =
        defaultServiceManager()->getService(String16("foo"), kVersion);

    CHECK(service != NULL);

    sp<IFoo> foo = IFoo::asInterface(service);
    CHECK(foo != NULL);

    foo->doThis(1.0f);

    ALOGI("=> doThis returned.");

    foo->doThatAndReturnSomething(
            2.0f, [&](auto result) {
                ALOGI("=> doThatAndReturnSomething returned %d.", result);
            });

    foo->doQuiteABit(
            1, 2, 3.0f, 4.0, [&](auto something) {
                ALOGI("=> doQuiteABit returned %f.", something);
            });

    int32_t param[15];
    for (size_t i = 0; i < sizeof(param) / sizeof(param[0]); ++i) {
        param[i] = i;
    }
    foo->doSomethingElse(param, [&](const auto &something) {
            ALOGI("=> doSomethingElse returned %s.",
                  arrayToString(something, 32).c_str());
        });

    foo->doStuffAndReturnAString([&](const auto &something) {
            ALOGI("=> doStuffAndReturnAString returned '%s'.",
                  something.c_str());
        });

    hidl_vec<int32_t> vecParam;
    vecParam.resize(10);
    for (size_t i = 0; i < 10; ++i) {
        vecParam[i] = i;
    }

    foo->mapThisVector(vecParam, [&](const auto &something) {
            ALOGI("=> mapThisVector returned %s.",
                  vecToString(something).c_str());
        });

    foo->callMe(new FooCallback);
    ALOGI("=> callMe returned.");

    foo->useAnEnum(IFoo::SomeEnum::quux, [&](auto kkk) {
                ALOGI("=> useAnEnum returned %u", (unsigned)kkk);
            });

    hidl_vec<IFoo::Goober> gooberVecParam;
    gooberVecParam.resize(2);
    gooberVecParam[0].name = "Hello";
    gooberVecParam[1].name = "World";

    foo->hmmm(gooberVecParam);
    ALOGI("=> hmmm returned.");

    foo->runningOutOfNames(gooberVecParam[0]);
    ALOGI("=> runningOutOfNames returned.");

    IFoo::Goober gooberArrayParam[20];
    foo->aGooberArray(gooberArrayParam);
    ALOGI("=> aGooberArray returned.");

    hidl_string stringArrayParam[3];
    stringArrayParam[0] = "What";
    stringArrayParam[1] = "a";
    stringArrayParam[2] = "disaster";
    foo->haveSomeStrings(stringArrayParam);
    ALOGI("haveSomeStrings returned.");

    hidl_vec<hidl_string> stringVecParam;
    stringVecParam.resize(3);
    stringVecParam[0] = "What";
    stringVecParam[1] = "a";
    stringVecParam[2] = "disaster";
    foo->haveAStringVec(stringVecParam);
    ALOGI("haveAStringVec returned.");
}

int main() {
    using namespace android::hidl;
    using android::String16;

    if (fork() == 0) {
        sleep(1);

        // Fear me, I am child.
        client();

        return 0;
    }

    sp<Foo> foo = new Foo;

    const hidl_version kVersion = make_hidl_version(1, 0);
    defaultServiceManager()->addService(String16("foo"), foo, kVersion);

    ProcessState::self()->setThreadPoolMaxThreadCount(0);
    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();

    return 0;
}
