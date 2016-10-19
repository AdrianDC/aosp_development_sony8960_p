//#define LOG_NDEBUG 0
#define LOG_TAG "hidl_test_java_native"

#include <android-base/logging.h>

#include <android/hardware/tests/baz/1.0/IBaz.h>

#include <gtest/gtest.h>
#include <hidl/IServiceManager.h>
#include <hidl/Status.h>
#include <hwbinder/IPCThreadState.h>
#include <hwbinder/ProcessState.h>

using ::android::sp;
using ::android::Thread;
using ::android::hardware::tests::baz::V1_0::IBase;
using ::android::hardware::tests::baz::V1_0::IBaz;
using ::android::hardware::tests::baz::V1_0::IBazCallback;

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_string;
using ::android::hardware::Return;
using ::android::hardware::Void;

struct BazCallback : public IBazCallback {
    Return<void> heyItsMe(const sp<IBazCallback> &cb) override;
};

Return<void> BazCallback::heyItsMe(
        const sp<IBazCallback> &cb) {
    LOG(INFO) << "SERVER: heyItsMe cb = " << cb.get();

    return Void();
}

struct Baz : public IBaz {
    Return<void> someBaseMethod() override;

    Return<void> someOtherBaseMethod(
            const IBaz::Foo &foo, someOtherBaseMethod_cb _hidl_cb) override;

    Return<void> someMethodWithFooArrays(
            const hidl_array<IBaz::Foo, 2> &fooInput,
            someMethodWithFooArrays_cb _hidl_cb) override;

    Return<void> someMethodWithFooVectors(
            const hidl_vec<IBaz::Foo> &fooInput,
            someMethodWithFooVectors_cb _hidl_cb) override;

    Return<void> someMethodWithVectorOfArray(
            const IBase::VectorOfArray &in,
            someMethodWithVectorOfArray_cb _hidl_cb) override;

    Return<void> someMethodTakingAVectorOfArray(
            const hidl_vec<hidl_array<uint8_t, 6> > &in,
            someMethodTakingAVectorOfArray_cb _hidl_cb) override;

    Return<void> transpose(
            const IBase::StringMatrix5x3 &in,
            transpose_cb _hidl_cb) override;

    Return<void> transpose2(
            const hidl_array<hidl_string, 5, 3> &in,
            transpose2_cb _hidl_cb) override;

    Return<bool> someBoolMethod(bool x) override;

    Return<void> someBoolArrayMethod(
            const hidl_array<bool, 3> &x,
            someBoolArrayMethod_cb _hidl_cb) override;

    Return<void> someBoolVectorMethod(
            const hidl_vec<bool> &x, someBoolVectorMethod_cb _hidl_cb) override;

    Return<void> doSomethingElse(
            const hidl_array<int32_t, 15> &param,
            doSomethingElse_cb _hidl_cb) override;

    Return<void> doThis(float param) override;

    Return<int32_t> doThatAndReturnSomething(int64_t param) override;

    Return<double> doQuiteABit(
            int32_t a,
            int64_t b,
            float c,
            double d) override;

    Return<void> doStuffAndReturnAString(
            doStuffAndReturnAString_cb _hidl_cb) override;

    Return<void> mapThisVector(
            const hidl_vec<int32_t>& param, mapThisVector_cb _hidl_cb) override;

    Return<void> callMe(const sp<IBazCallback>& cb) override;
    Return<IBaz::SomeEnum> useAnEnum(IBaz::SomeEnum zzz) override;

    Return<void> haveSomeStrings(
            const hidl_array<hidl_string, 3> &array,
            haveSomeStrings_cb _hidl_cb) override;

    Return<void> haveAStringVec(
            const hidl_vec<hidl_string>& vector,
            haveAStringVec_cb _hidl_cb) override;

    Return<void> returnABunchOfStrings(returnABunchOfStrings_cb _hidl_cb) override;
};

Return<void> Baz::someBaseMethod() {
    LOG(INFO) << "Baz::someBaseMethod";

    return Void();
}

using std::to_string;

static std::string to_string(const IBaz::Foo::Bar &bar);
static std::string to_string(const IBaz::Foo &foo);
static std::string to_string(const hidl_string &s);
static std::string to_string(bool x);
static std::string to_string(const IBase::StringMatrix5x3 &M);
static std::string to_string(const IBase::StringMatrix3x5 &M);

template<typename T, size_t SIZE>
static std::string to_string(const hidl_array<T, SIZE> &array);

template<size_t SIZE>
static std::string to_string(const hidl_array<uint8_t, SIZE> &array);

template<typename T>
static std::string to_string(const hidl_vec<T> &vec) {
    std::string out;
    out = "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) {
            out += ", ";
        }
        out += to_string(vec[i]);
    }
    out += "]";

    return out;
}

template<typename T, size_t SIZE>
static std::string to_string(const hidl_array<T, SIZE> &array) {
    std::string out;
    out = "[";
    for (size_t i = 0; i < SIZE; ++i) {
        if (i > 0) {
            out += ", ";
        }
        out += to_string(array[i]);
    }
    out += "]";

    return out;
}

template<size_t SIZE>
static std::string to_string(const hidl_array<uint8_t, SIZE> &array) {
    std::string out;
    for (size_t i = 0; i < SIZE; ++i) {
        if (i > 0) {
            out += ":";
        }

        char tmp[3];
        sprintf(tmp, "%02x", array[i]);

        out += tmp;
    }

    return out;
}

template<typename T, size_t SIZE1, size_t SIZE2>
static std::string to_string(const hidl_array<T, SIZE1, SIZE2> &array) {
    std::string out;
    out = "[";
    for (size_t i = 0; i < SIZE1; ++i) {
        if (i > 0) {
            out += ", ";
        }

        out += "[";
        for (size_t j = 0; j < SIZE2; ++j) {
            if (j > 0) {
                out += ", ";
            }

            out += to_string(array[i][j]);
        }
        out += "]";
    }
    out += "]";

    return out;
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

static std::string to_string(const IBase::StringMatrix5x3 &M) {
    return to_string(M.s);
}

static std::string to_string(const IBase::StringMatrix3x5 &M) {
    return to_string(M.s);
}

Return<void> Baz::someOtherBaseMethod(
        const IBaz::Foo &foo, someOtherBaseMethod_cb _hidl_cb) {
    LOG(INFO) << "Baz::someOtherBaseMethod "
              << to_string(foo);

    _hidl_cb(foo);

    return Void();
}

Return<void> Baz::someMethodWithFooArrays(
        const hidl_array<IBaz::Foo, 2> &fooInput,
        someMethodWithFooArrays_cb _hidl_cb) {
    LOG(INFO) << "Baz::someMethodWithFooArrays "
              << to_string(fooInput);

    hidl_array<IBaz::Foo, 2> fooOutput;
    fooOutput[0] = fooInput[1];
    fooOutput[1] = fooInput[0];

    _hidl_cb(fooOutput);

    return Void();
}

Return<void> Baz::someMethodWithFooVectors(
        const hidl_vec<IBaz::Foo> &fooInput,
        someMethodWithFooVectors_cb _hidl_cb) {
    LOG(INFO) << "Baz::someMethodWithFooVectors "
              << to_string(fooInput);

    hidl_vec<IBaz::Foo> fooOutput;
    fooOutput.resize(2);
    fooOutput[0] = fooInput[1];
    fooOutput[1] = fooInput[0];

    _hidl_cb(fooOutput);

    return Void();
}

static std::string VectorOfArray_to_string(const IBase::VectorOfArray &in) {
    std::string out;
    out += "VectorOfArray(";

    for (size_t i = 0; i < in.addresses.size(); ++i) {
        if (i > 0) {
            out += ", ";
        }

        for (size_t j = 0; j < 6; ++j) {
            if (j > 0) {
                out += ":";
            }

            char tmp[3];
            sprintf(tmp, "%02x", in.addresses[i][j]);

            out += tmp;
        }
    }

    out += ")";

    return out;
}

Return<void> Baz::someMethodWithVectorOfArray(
        const IBase::VectorOfArray &in,
        someMethodWithVectorOfArray_cb _hidl_cb) {
    LOG(INFO) << "Baz::someMethodWithVectorOfArray "
              << VectorOfArray_to_string(in);

    IBase::VectorOfArray out;

    const size_t n = in.addresses.size();
    out.addresses.resize(n);

    for (size_t i = 0; i < n; ++i) {
        out.addresses[i] = in.addresses[n - 1 - i];
    }

    _hidl_cb(out);

    return Void();
}

Return<void> Baz::someMethodTakingAVectorOfArray(
        const hidl_vec<hidl_array<uint8_t, 6> > &in,
        someMethodTakingAVectorOfArray_cb _hidl_cb) {
    LOG(INFO) << "Baz::someMethodTakingAVectorOfArray "
              << to_string(in);

    const size_t n = in.size();

    hidl_vec<hidl_array<uint8_t, 6> > out;
    out.resize(n);

    for (size_t i = 0; i < n; ++i) {
        out[i] = in[n - 1 - i];
    }

    _hidl_cb(out);

    return Void();
}

Return<void> Baz::transpose(
        const IBase::StringMatrix5x3 &in, transpose_cb _hidl_cb) {
    LOG(INFO) << "Baz::transpose " << to_string(in);

    IBase::StringMatrix3x5 out;
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 5; ++j) {
            out.s[i][j] = in.s[j][i];
        }
    }

    _hidl_cb(out);

    return Void();
}

Return<void> Baz::transpose2(
        const hidl_array<hidl_string, 5, 3> &in, transpose2_cb _hidl_cb) {
    LOG(INFO) << "Baz::transpose2 " << to_string(in);

    hidl_array<hidl_string, 3, 5> out;
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 5; ++j) {
            out[i][j] = in[j][i];
        }
    }

    _hidl_cb(out);

    return Void();
}

Return<bool> Baz::someBoolMethod(bool x) {
    LOG(INFO) << "Baz::someBoolMethod(" << to_string(x) << ")";

    return !x;
}

Return<void> Baz::someBoolArrayMethod(
        const hidl_array<bool, 3> &x, someBoolArrayMethod_cb _hidl_cb) {
    LOG(INFO) << "Baz::someBoolArrayMethod("
        << to_string(x[0])
        << ", "
        << to_string(x[1])
        << ", "
        << to_string(x[2])
        << ")";

    hidl_array<bool, 4> out;
    out[0] = !x[0];
    out[1] = !x[1];
    out[2] = !x[2];
    out[3] = true;

    _hidl_cb(out);

    return Void();
}

Return<void> Baz::someBoolVectorMethod(
        const hidl_vec<bool> &x, someBoolVectorMethod_cb _hidl_cb) {
    LOG(INFO) << "Baz::someBoolVectorMethod(" << to_string(x) << ")";

    hidl_vec<bool> out;
    out.resize(x.size());
    for (size_t i = 0; i < x.size(); ++i) {
        out[i] = !x[i];
    }

    _hidl_cb(out);

    return Void();
}

Return<void> Baz::doSomethingElse(
        const hidl_array<int32_t, 15> &param, doSomethingElse_cb _hidl_cb) {
    LOG(INFO) << "Baz::doSomethingElse(...)";

    hidl_array<int32_t, 32> result;
    for (size_t i = 0; i < 15; ++i) {
        result[i] = 2 * param[i];
        result[15 + i] = param[i];
    }
    result[30] = 1;
    result[31] = 2;

    _hidl_cb(result);

    return Void();
}

Return<void> Baz::doThis(float param) {
    LOG(INFO) << "Baz::doThis(" << param << ")";

    return Void();
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

Return<void> Baz::doStuffAndReturnAString(
        doStuffAndReturnAString_cb _hidl_cb) {
    LOG(INFO) << "doStuffAndReturnAString";

    hidl_string s;
    s = "Hello, world!";

    _hidl_cb(s);

    return Void();
}

Return<void> Baz::mapThisVector(
        const hidl_vec<int32_t>& param, mapThisVector_cb _hidl_cb) {
    LOG(INFO) << "mapThisVector";

    hidl_vec<int32_t> out;
    out.resize(param.size());
    for (size_t i = 0; i < param.size(); ++i) {
        out[i] = param[i] * 2;
    }

    _hidl_cb(out);

    return Void();
}

Return<void> Baz::callMe(const sp<IBazCallback>& cb) {
    LOG(INFO) << "callMe " << cb.get();

    if (cb != NULL) {
        sp<IBazCallback> my_cb = new BazCallback;
        cb->heyItsMe(my_cb);
    }

    return Void();
}

Return<IBaz::SomeEnum> Baz::useAnEnum(IBaz::SomeEnum zzz) {
    LOG(INFO) << "useAnEnum " << (int)zzz;

    return SomeEnum::goober;
}

Return<void> Baz::haveSomeStrings(
        const hidl_array<hidl_string, 3> &array, haveSomeStrings_cb _hidl_cb) {
    LOG(INFO) << "haveSomeStrings("
              << to_string(array)
              << ")";

    hidl_array<hidl_string, 2> result;
    result[0] = "Hello";
    result[1] = "World";

    _hidl_cb(result);

    return Void();
}

Return<void> Baz::haveAStringVec(
        const hidl_vec<hidl_string>& vector,
        haveAStringVec_cb _hidl_cb) {
    LOG(INFO) << "haveAStringVec(" << to_string(vector) << ")";

    hidl_vec<hidl_string> result;
    result.resize(2);

    result[0] = "Hello";
    result[1] = "World";

    _hidl_cb(result);

    return Void();
}

Return<void> Baz::returnABunchOfStrings(returnABunchOfStrings_cb _hidl_cb) {
    hidl_string eins; eins = "Eins";
    hidl_string zwei; zwei = "Zwei";
    hidl_string drei; drei = "Drei";
    _hidl_cb(eins, zwei, drei);

    return Void();
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

        baz = IBaz::getService("baz");

        CHECK(baz != NULL);
    }

    void TearDown() override {
    }
};

template <typename T>
static void EXPECT_OK(::android::hardware::Return<T> ret) {
    EXPECT_TRUE(ret.getStatus().isOk());
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

TEST_F(HidlTest, BazSomeMethodWithFooArraysTest) {
    hidl_array<IBase::Foo, 2> foo;

    foo[0].x = 1;
    foo[0].y.z = 2.5;
    foo[0].y.s = "Hello, world";

    foo[0].aaa.resize(5);
    for (size_t i = 0; i < foo[0].aaa.size(); ++i) {
        foo[0].aaa[i].z = 1.0f + (float)i * 0.01f;
        foo[0].aaa[i].s = ("Hello, world " + std::to_string(i)).c_str();
    }

    foo[1].x = 2;
    foo[1].y.z = -2.5;
    foo[1].y.s = "Morituri te salutant";

    foo[1].aaa.resize(3);
    for (size_t i = 0; i < foo[1].aaa.size(); ++i) {
        foo[1].aaa[i].z = 2.0f - (float)i * 0.01f;
        foo[1].aaa[i].s = ("Alea iacta est: " + std::to_string(i)).c_str();
    }

    EXPECT_OK(
            baz->someMethodWithFooArrays(
                foo,
                [&](const auto &result) {
                    EXPECT_EQ(
                        to_string(result),
                        "[Foo(x = 2, "
                             "y = Bar(z = -2.500000, s = 'Morituri te salutant'), "
                             "aaa = [Bar(z = 2.000000, s = 'Alea iacta est: 0'), "
                                    "Bar(z = 1.990000, s = 'Alea iacta est: 1'), "
                                    "Bar(z = 1.980000, s = 'Alea iacta est: 2')]), "
                        "Foo(x = 1, "
                            "y = Bar(z = 2.500000, s = 'Hello, world'), "
                            "aaa = [Bar(z = 1.000000, s = 'Hello, world 0'), "
                                   "Bar(z = 1.010000, s = 'Hello, world 1'), "
                                   "Bar(z = 1.020000, s = 'Hello, world 2'), "
                                   "Bar(z = 1.030000, s = 'Hello, world 3'), "
                                   "Bar(z = 1.040000, s = 'Hello, world 4')])]");
                }));
}

TEST_F(HidlTest, BazSomeMethodWithFooVectorsTest) {
    hidl_vec<IBase::Foo> foo;
    foo.resize(2);

    foo[0].x = 1;
    foo[0].y.z = 2.5;
    foo[0].y.s = "Hello, world";

    foo[0].aaa.resize(5);
    for (size_t i = 0; i < foo[0].aaa.size(); ++i) {
        foo[0].aaa[i].z = 1.0f + (float)i * 0.01f;
        foo[0].aaa[i].s = ("Hello, world " + std::to_string(i)).c_str();
    }

    foo[1].x = 2;
    foo[1].y.z = -2.5;
    foo[1].y.s = "Morituri te salutant";

    foo[1].aaa.resize(3);
    for (size_t i = 0; i < foo[1].aaa.size(); ++i) {
        foo[1].aaa[i].z = 2.0f - (float)i * 0.01f;
        foo[1].aaa[i].s = ("Alea iacta est: " + std::to_string(i)).c_str();
    }

    EXPECT_OK(
            baz->someMethodWithFooVectors(
                foo,
                [&](const auto &result) {
                    EXPECT_EQ(
                        to_string(result),
                        "[Foo(x = 2, "
                             "y = Bar(z = -2.500000, s = 'Morituri te salutant'), "
                             "aaa = [Bar(z = 2.000000, s = 'Alea iacta est: 0'), "
                                    "Bar(z = 1.990000, s = 'Alea iacta est: 1'), "
                                    "Bar(z = 1.980000, s = 'Alea iacta est: 2')]), "
                        "Foo(x = 1, "
                            "y = Bar(z = 2.500000, s = 'Hello, world'), "
                            "aaa = [Bar(z = 1.000000, s = 'Hello, world 0'), "
                                   "Bar(z = 1.010000, s = 'Hello, world 1'), "
                                   "Bar(z = 1.020000, s = 'Hello, world 2'), "
                                   "Bar(z = 1.030000, s = 'Hello, world 3'), "
                                   "Bar(z = 1.040000, s = 'Hello, world 4')])]");
                }));
}

TEST_F(HidlTest, BazSomeMethodWithVectorOfArray) {
    IBase::VectorOfArray in;
    in.addresses.resize(3);

    size_t k = 0;
    for (size_t i = 0; i < in.addresses.size(); ++i) {
        for (size_t j = 0; j < 6; ++j, ++k) {
            in.addresses[i][j] = k;
        }
    }

    EXPECT_OK(
            baz->someMethodWithVectorOfArray(
                in,
                [&](const auto &out) {
                    EXPECT_EQ(
                        VectorOfArray_to_string(out),
                        "VectorOfArray("
                          "0c:0d:0e:0f:10:11, "
                          "06:07:08:09:0a:0b, "
                          "00:01:02:03:04:05)");
                }));
}

TEST_F(HidlTest, BazSomeMethodTakingAVectorOfArray) {
    hidl_vec<hidl_array<uint8_t, 6> > in;
    in.resize(3);

    size_t k = 0;
    for (size_t i = 0; i < in.size(); ++i) {
        for (size_t j = 0; j < 6; ++j, ++k) {
            in[i][j] = k;
        }
    }

    EXPECT_OK(
            baz->someMethodTakingAVectorOfArray(
                in,
                [&](const auto &out) {
                    EXPECT_EQ(
                        to_string(out),
                        "[0c:0d:0e:0f:10:11, 06:07:08:09:0a:0b, 00:01:02:03:04:05]");
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

TEST_F(HidlTest, BazTransposeTest) {
    IBase::StringMatrix5x3 in;

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 3; ++j) {
            in.s[i][j] = numberToEnglish(3 * i + j + 1).c_str();
        }
    }

    EXPECT_OK(baz->transpose(
                in,
                [&](const auto &out) {
                    EXPECT_EQ(
                        to_string(out),
                        "[['one', 'four', 'seven', 'ten', 'thirteen'], "
                         "['two', 'five', 'eight', 'eleven', 'fourteen'], "
                         "['three', 'six', 'nine', 'twelve', 'fifteen']]");
                }));
}

TEST_F(HidlTest, BazTranspose2Test) {
    hidl_array<hidl_string, 5, 3> in;

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 3; ++j) {
            in[i][j] = numberToEnglish(3 * i + j + 1).c_str();
        }
    }

    EXPECT_OK(baz->transpose2(
                in,
                [&](const auto &out) {
                    EXPECT_EQ(
                        to_string(out),
                        "[['one', 'four', 'seven', 'ten', 'thirteen'], "
                         "['two', 'five', 'eight', 'eleven', 'fourteen'], "
                         "['three', 'six', 'nine', 'twelve', 'fifteen']]");
                }));
}

TEST_F(HidlTest, BazSomeBoolMethodTest) {
    auto result = baz->someBoolMethod(true);
    EXPECT_OK(result);
    EXPECT_EQ(to_string(result), "false");
}

TEST_F(HidlTest, BazSomeBoolArrayMethodTest) {
    hidl_array<bool, 3> someBoolArray;
    someBoolArray[0] = true;
    someBoolArray[1] = false;
    someBoolArray[2] = true;

    EXPECT_OK(
            baz->someBoolArrayMethod(
                someBoolArray,
                [&](const auto &result) {
                    EXPECT_EQ(
                        to_string(result),
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
    EXPECT_OK(result);
    EXPECT_EQ(to_string(result), "666");
}

TEST_F(HidlTest, BazDoQuiteABitMethodTest) {
    auto result = baz->doQuiteABit(1, 2ll, 3.0f, 4.0);

    EXPECT_OK(result);
    EXPECT_EQ(to_string(result), "666.500000");
}

TEST_F(HidlTest, BazDoSomethingElseMethodTest) {
    hidl_array<int32_t, 15> param;
    for (size_t i = 0; i < 15; ++i) {
        param[i] = i;
    }

    EXPECT_OK(
            baz->doSomethingElse(
                param,
                [&](const auto &result) {
                    EXPECT_EQ(
                        to_string(result),
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
                        to_string(result),
                        "[0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, "
                        "28]");
                }));
}

TEST_F(HidlTest, BazCallMeMethodTest) {
    EXPECT_OK(baz->callMe(new BazCallback()));
}

TEST_F(HidlTest, BazUseAnEnumMethodTest) {
    auto result = baz->useAnEnum(IBaz::SomeEnum::bar);

    EXPECT_OK(result);
    EXPECT_TRUE(result == IBaz::SomeEnum::quux);
}

TEST_F(HidlTest, BazHaveSomeStringsMethodTest) {
    hidl_array<hidl_string, 3> string_params;
    string_params[0] = "one";
    string_params[1] = "two";
    string_params[2] = "three";

    EXPECT_OK(
            baz->haveSomeStrings(
                string_params,
                [&](const auto &result) {
                    EXPECT_EQ(to_string(result), "['Hello', 'World']");
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
                    EXPECT_EQ(to_string(result), "['Hello', 'World']");
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

        baz->registerAsService("baz");

        ProcessState::self()->startThreadPool();
        ProcessState::self()->setThreadPoolMaxThreadCount(0);
        IPCThreadState::self()->joinThreadPool();
    }

    return 0;
}

