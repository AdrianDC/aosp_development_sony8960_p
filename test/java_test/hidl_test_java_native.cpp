//#define LOG_NDEBUG 0
#define LOG_TAG "hidl_test_java_native"

#include <android-base/logging.h>

#include <android/hardware/tests/baz/1.0/IBaz.h>

#include <gtest/gtest.h>
#include <hidl/IServiceManager.h>
#include <hwbinder/IPCThreadState.h>
#include <hwbinder/ProcessState.h>
#include <hwbinder/Status.h>

using ::android::sp;
using ::android::Thread;
using ::android::hardware::tests::baz::V1_0::IBase;
using ::android::hardware::tests::baz::V1_0::IBaz;
using ::android::hardware::tests::baz::V1_0::IBazCallback;

using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_string;
using ::android::hardware::Return;
using ::android::hardware::Status;

struct BazCallback : public IBazCallback {
    Status heyItsMe(const sp<IBazCallback> &cb) override;
};

Status BazCallback::heyItsMe(
        const sp<IBazCallback> &cb) {
    LOG(INFO) << "SERVER: heyItsMe cb = " << cb.get();

    return Status::ok();
}

struct Baz : public IBaz {
    Status someBaseMethod() override;

    Status someOtherBaseMethod(
            const IBaz::Foo &foo, someOtherBaseMethod_cb _hidl_cb) override;

    Return<bool> someBoolMethod(bool x) override;

    Status someBoolArrayMethod(
            const bool x[3], someBoolArrayMethod_cb _hidl_cb) override;

    Status someBoolVectorMethod(
            const hidl_vec<bool> &x, someBoolVectorMethod_cb _hidl_cb) override;

    Status doSomethingElse(
            const int32_t param[15], doSomethingElse_cb _hidl_cb) override;

    Status doThis(float param) override;

    Return<int32_t> doThatAndReturnSomething(int64_t param) override;

    Return<double> doQuiteABit(
            int32_t a,
            int64_t b,
            float c,
            double d) override;

    Status doStuffAndReturnAString(
            doStuffAndReturnAString_cb _hidl_cb) override;

    Status mapThisVector(
            const hidl_vec<int32_t>& param, mapThisVector_cb _hidl_cb) override;

    Status callMe(const sp<IBazCallback>& cb) override;
    Return<IBaz::SomeEnum> useAnEnum(IBaz::SomeEnum zzz) override;

    Status haveSomeStrings(
            const hidl_string array[3], haveSomeStrings_cb _hidl_cb) override;

    Status haveAStringVec(
            const hidl_vec<hidl_string>& vector,
            haveAStringVec_cb _hidl_cb) override;

    Status returnABunchOfStrings(returnABunchOfStrings_cb _hidl_cb) override;
};

Status Baz::someBaseMethod() {
    LOG(INFO) << "Baz::someBaseMethod";

    return Status::ok();
}

using std::to_string;

static std::string to_string(const IBaz::Foo::Bar &bar);
static std::string to_string(const IBaz::Foo &foo);
static std::string to_string(const hidl_string &s);
static std::string to_string(bool x);

template<class T>
static std::string array_to_string(const T *array, size_t size) {
    std::string out;
    out = "[";
    for (size_t i = 0; i < size; ++i) {
        if (i > 0) {
            out += ", ";
        }
        out += to_string(array[i]);
    }
    out += "]";

    return out;
}

template<class T>
static std::string to_string(const hidl_vec<T> &vec) {
    return array_to_string(&vec[0], vec.size());
}

static std::string to_string(bool x) {
    return x ? "true" : "false";
}

static std::string to_string(const hidl_string &s) {
    return std::string("'") + s.c_str() + "'";
}

static std::string to_string(const IBaz::Foo::Bar &bar) {
    std::string out;
    out = "Bar(";
    out += "z = " + to_string(bar.z) + ", ";
    out += "s = '" + std::string(bar.s.c_str()) + "'";
    out += ")";

    return out;
}

static std::string to_string(const IBaz::Foo &foo) {
    std::string out;
    out = "Foo(";
    out += "x = " + to_string(foo.x) + ", ";
    out += "y = " + to_string(foo.y) + ", ";
    out += "aaa = " + to_string(foo.aaa);
    out += ")";

    return out;
}

Status Baz::someOtherBaseMethod(
        const IBaz::Foo &foo, someOtherBaseMethod_cb _hidl_cb) {
    LOG(INFO) << "Baz::someOtherBaseMethod "
              << to_string(foo);

    _hidl_cb(foo);

    return Status::ok();
}

Return<bool> Baz::someBoolMethod(bool x) {
    LOG(INFO) << "Baz::someBoolMethod(" << to_string(x) << ")";

    return !x;
}

Status Baz::someBoolArrayMethod(
        const bool x[3], someBoolArrayMethod_cb _hidl_cb) {
    LOG(INFO) << "Baz::someBoolArrayMethod("
        << to_string(x[0])
        << ", "
        << to_string(x[1])
        << ", "
        << to_string(x[2])
        << ")";

    bool out[4];
    out[0] = !x[0];
    out[1] = !x[1];
    out[2] = !x[2];
    out[3] = true;

    _hidl_cb(out);

    return Status::ok();
}

Status Baz::someBoolVectorMethod(
        const hidl_vec<bool> &x, someBoolVectorMethod_cb _hidl_cb) {
    LOG(INFO) << "Baz::someBoolVectorMethod(" << to_string(x) << ")";

    hidl_vec<bool> out;
    out.resize(x.size());
    for (size_t i = 0; i < x.size(); ++i) {
        out[i] = !x[i];
    }

    _hidl_cb(out);

    return Status::ok();
}

Status Baz::doSomethingElse(
        const int32_t param[15], doSomethingElse_cb _hidl_cb) {
    LOG(INFO) << "Baz::doSomethingElse(...)";

    int32_t result[32] = { 0 };
    for (size_t i = 0; i < 15; ++i) {
        result[i] = 2 * param[i];
        result[15 + i] = param[i];
    }
    result[30] = 1;
    result[31] = 2;

    _hidl_cb(result);

    return Status::ok();
}

Status Baz::doThis(float param) {
    LOG(INFO) << "Baz::doThis(" << param << ")";

    return Status::ok();
}

Return<int32_t> Baz::doThatAndReturnSomething(int64_t param) {
    LOG(INFO) << "Baz::doThatAndReturnSomething(" << param << ")";

    return 666;
}

Return<double> Baz::doQuiteABit(
        int32_t a,
        int64_t b,
        float c,
        double d) {
    LOG(INFO) << "Baz::doQuiteABit("
              << a
              << ", "
              << b
              << ", "
              << c
              << ", "
              << d
              << ")";

    return 666.5;
}

Status Baz::doStuffAndReturnAString(
        doStuffAndReturnAString_cb _hidl_cb) {
    LOG(INFO) << "doStuffAndReturnAString";

    hidl_string s;
    s = "Hello, world!";

    _hidl_cb(s);

    return Status::ok();
}

Status Baz::mapThisVector(
        const hidl_vec<int32_t>& param, mapThisVector_cb _hidl_cb) {
    LOG(INFO) << "mapThisVector";

    hidl_vec<int32_t> out;
    out.resize(param.size());
    for (size_t i = 0; i < param.size(); ++i) {
        out[i] = param[i] * 2;
    }

    _hidl_cb(out);

    return Status::ok();
}

Status Baz::callMe(const sp<IBazCallback>& cb) {
    LOG(INFO) << "callMe " << cb.get();

    if (cb != NULL) {
        sp<IBazCallback> my_cb = new BazCallback;
        cb->heyItsMe(my_cb);
    }

    return Status::ok();
}

Return<IBaz::SomeEnum> Baz::useAnEnum(IBaz::SomeEnum zzz) {
    LOG(INFO) << "useAnEnum " << (int)zzz;

    return SomeEnum::goober;
}

Status Baz::haveSomeStrings(
        const hidl_string array[3], haveSomeStrings_cb _hidl_cb) {
    LOG(INFO) << "haveSomeStrings("
              << array_to_string(array, 3)
              << ")";

    hidl_string result[2];
    result[0] = "Hello";
    result[1] = "World";

    _hidl_cb(result);

    return Status::ok();
}

Status Baz::haveAStringVec(
        const hidl_vec<hidl_string>& vector,
        haveAStringVec_cb _hidl_cb) {
    LOG(INFO) << "haveAStringVec(" << to_string(vector) << ")";

    hidl_vec<hidl_string> result;
    result.resize(2);

    result[0] = "Hello";
    result[1] = "World";

    _hidl_cb(result);

    return Status::ok();
}

Status Baz::returnABunchOfStrings(returnABunchOfStrings_cb _hidl_cb) {
    hidl_string eins; eins = "Eins";
    hidl_string zwei; zwei = "Zwei";
    hidl_string drei; drei = "Drei";
    _hidl_cb(eins, zwei, drei);

    return Status::ok();
}

static void usage(const char *me) {
    fprintf(stderr, "%s [-c]lient | [-s]erver\n", me);
}

struct HidlEnvironment : public ::testing::Environment {
    void SetUp() override {
    }

    void TearDown() override {
    }
};

struct HidlTest : public ::testing::Test {
    sp<IBaz> baz;

    void SetUp() override {
        using namespace ::android::hardware;

        const hidl_version kVersion = make_hidl_version(1, 0);

        baz = IBaz::getService(::android::String16("baz"), kVersion);

        CHECK(baz != NULL);
    }

    void TearDown() override {
    }
};

static void EXPECT_OK(::android::hardware::Status status) {
    EXPECT_TRUE(status.isOk());
}

TEST_F(HidlTest, BazSomeBaseMethodTest) {
    EXPECT_OK(baz->someBaseMethod());
}

TEST_F(HidlTest, BazSomeOtherBaseMethodTest) {
    IBase::Foo foo;
    foo.x = 1;
    foo.y.z = 2.5;
    foo.y.s = "Hello, world";

    foo.aaa.resize(5);
    for (size_t i = 0; i < foo.aaa.size(); ++i) {
        foo.aaa[i].z = 1.0f + (float)i * 0.01f;
        foo.aaa[i].s = ("Hello, world " + std::to_string(i)).c_str();
    }

    EXPECT_OK(
            baz->someOtherBaseMethod(
                foo,
                [&](const auto &result) {
                    EXPECT_EQ(
                        to_string(result),
                        "Foo(x = 1, "
                            "y = Bar(z = 2.500000, s = 'Hello, world'), "
                            "aaa = [Bar(z = 1.000000, s = 'Hello, world 0'), "
                                   "Bar(z = 1.010000, s = 'Hello, world 1'), "
                                   "Bar(z = 1.020000, s = 'Hello, world 2'), "
                                   "Bar(z = 1.030000, s = 'Hello, world 3'), "
                                   "Bar(z = 1.040000, s = 'Hello, world 4')])");
                }));
}

TEST_F(HidlTest, BazSomeBoolMethodTest) {
    auto result = baz->someBoolMethod(true);
    EXPECT_OK(result.status);
    EXPECT_EQ(to_string(result), "false");
}

TEST_F(HidlTest, BazSomeBoolArrayMethodTest) {
    const bool someBoolArray[3] = { true, false, true };

    EXPECT_OK(
            baz->someBoolArrayMethod(
                someBoolArray,
                [&](const auto &result) {
                    EXPECT_EQ(
                        array_to_string(result, 4),
                        "[false, true, false, true]");
                }));
}

TEST_F(HidlTest, BazSomeBoolVectorMethodTest) {
    hidl_vec<bool> someBoolVector;
    someBoolVector.resize(4);
    for (size_t i = 0; i < someBoolVector.size(); ++i) {
        someBoolVector[i] = ((i & 1) == 0);
    }

    EXPECT_OK(
            baz->someBoolVectorMethod(
                someBoolVector,
                [&](const auto &result) {
                    EXPECT_EQ(
                        to_string(result), "[false, true, false, true]");
                }));
}

TEST_F(HidlTest, BazDoThisMethodTest) {
    EXPECT_OK(baz->doThis(1.0f));
}

TEST_F(HidlTest, BazDoThatAndReturnSomethingMethodTest) {
    auto result = baz->doThatAndReturnSomething(1);
    EXPECT_OK(result.status);
    EXPECT_EQ(to_string(result), "666");
}

TEST_F(HidlTest, BazDoQuiteABitMethodTest) {
    auto result = baz->doQuiteABit(1, 2ll, 3.0f, 4.0);

    EXPECT_OK(result.status);
    EXPECT_EQ(to_string(result), "666.500000");
}

TEST_F(HidlTest, BazDoSomethingElseMethodTest) {
    int32_t param[15];
    for (size_t i = 0; i < 15; ++i) {
        param[i] = i;
    }

    EXPECT_OK(
            baz->doSomethingElse(
                param,
                [&](const auto &result) {
                    EXPECT_EQ(
                        array_to_string(result, 32),
                        "[0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, "
                        "28, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, "
                        "1, 2]");
                }));
}

TEST_F(HidlTest, BazDoStuffAndReturnAStringMethodTest) {
    EXPECT_OK(
            baz->doStuffAndReturnAString(
                [&](const auto &result) {
                    EXPECT_EQ(to_string(result), "'Hello, world!'");
                }));
}

TEST_F(HidlTest, BazMapThisVectorMethodTest) {
    hidl_vec<int32_t> vec_param;
    vec_param.resize(15);
    for (size_t i = 0; i < 15; ++i) {
        vec_param[i] = i;
    }

    EXPECT_OK(
            baz->mapThisVector(
                vec_param,
                [&](const auto &result) {
                    EXPECT_EQ(
                        array_to_string(&result[0], result.size()),
                        "[0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, "
                        "28]");
                }));
}

TEST_F(HidlTest, BazCallMeMethodTest) {
    EXPECT_OK(baz->callMe(new BazCallback()));
}

TEST_F(HidlTest, BazUseAnEnumMethodTest) {
    auto result = baz->useAnEnum(IBaz::SomeEnum::bar);

    EXPECT_OK(result.status);
    EXPECT_TRUE(result == IBaz::SomeEnum::quux);
}

TEST_F(HidlTest, BazHaveSomeStringsMethodTest) {
    hidl_string string_params[3];
    string_params[0] = "one";
    string_params[1] = "two";
    string_params[2] = "three";

    EXPECT_OK(
            baz->haveSomeStrings(
                string_params,
                [&](const auto &result) {
                    EXPECT_EQ(
                        array_to_string(result, 2), "['Hello', 'World']");
                }));
}

TEST_F(HidlTest, BazHaveAStringVecMethodTest) {
    hidl_vec<hidl_string> string_vec;
    string_vec.resize(4);
    string_vec[0] = "Uno";
    string_vec[1] = "Dos";
    string_vec[2] = "Tres";
    string_vec[3] = "Cuatro";

    EXPECT_OK(
            baz->haveAStringVec(
                string_vec,
                [&](const auto &result) {
                    EXPECT_EQ(
                        array_to_string(&result[0], result.size()),
                        "['Hello', 'World']");
                }));
}

TEST_F(HidlTest, BazReturnABunchOfStringsMethodTest) {
    EXPECT_OK(
            baz->returnABunchOfStrings(
                [&](const auto &a, const auto &b, const auto &c) {
                    EXPECT_EQ(
                        to_string(a) + ", " + to_string(b) + ", " + to_string(c),
                        "'Eins', 'Zwei', 'Drei'");
                }));
}

int main(int argc, char **argv) {
    using namespace android::hardware;
    using android::String16;

    const char *me = argv[0];

    bool wantClient = false;
    bool wantServer = false;

    int res;
    while ((res = getopt(argc, argv, "chs")) >= 0) {
        switch (res) {
            case 'c':
            {
                wantClient = true;
                break;
            }

            case 's':
            {
                wantServer = true;
                break;
            }

            case '?':
            case 'h':
            default:
            {
                usage(me);
                exit(1);
                break;
            }
        }
    }

    if ((!wantClient && !wantServer) || (wantClient && wantServer)) {
        usage(me);
        exit(1);
    }

    if (wantClient) {
        ::testing::AddGlobalTestEnvironment(new HidlEnvironment);
        ::testing::InitGoogleTest(&argc, argv);
        int status = RUN_ALL_TESTS();
        return status;
    } else {
        sp<Baz> baz = new Baz;

        const hidl_version kVersion = make_hidl_version(1, 0);

        baz->registerAsService(String16("baz"), kVersion);

        ProcessState::self()->startThreadPool();
        ProcessState::self()->setThreadPoolMaxThreadCount(0);
        IPCThreadState::self()->joinThreadPool();
    }

    return 0;
}

