#define LOG_TAG "hidl_test"
#include <android-base/logging.h>

#include <android/hardware/tests/foo/1.0/BnFoo.h>
#include <android/hardware/tests/foo/1.0/BnFooCallback.h>
#include <android/hardware/tests/bar/1.0/BnBar.h>
#include <android/hardware/tests/pointer/1.0/BnGraph.h>
#include <android/hardware/tests/pointer/1.0/BnPointer.h>

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

#include <getopt.h>
#include <inttypes.h>
#include <sstream>
#include <vector>

#include <hidl-test/FooHelper.h>
#include <hidl-test/PointerHelper.h>

#include <hidl/IServiceManager.h>
#include <hidl/Status.h>
#include <hwbinder/IPCThreadState.h>
#include <hwbinder/ProcessState.h>

#include <utils/Condition.h>
#include <utils/Timers.h>

#define EXPECT_OK(__ret__) EXPECT_TRUE(isOk(__ret__))
#define EXPECT_FAIL(__ret__) EXPECT_FALSE(isOk(__ret__))
#define EXPECT_ARRAYEQ(__a1__, __a2__, __size__) EXPECT_TRUE(IsArrayEq(__a1__, __a2__, __size__))

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
using ::android::hardware::tests::bar::V1_0::IHwBar;
using ::android::hardware::tests::pointer::V1_0::IGraph;
using ::android::hardware::tests::pointer::V1_0::IPointer;
using ::android::hardware::IPCThreadState;
using ::android::hardware::ProcessState;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_string;
using ::android::sp;
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

struct Simple : public ISimple {
    Simple(int32_t cookie)
        : mCookie(cookie) {
    }

    Return<int32_t> getCookie() override {
        return mCookie;
    }

private:
    int32_t mCookie;
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
static pid_t forkServer(const std::string &serviceName,
                       const char *tag) {
    pid_t pid;
    // use fork to create and kill to destroy server processes.
    // getStub = true to get the passthrough version as the backend for the
    // binderized service.
    if ((pid = fork()) == 0) {
        // in child process
        sp<T> server = T::getService(serviceName, true);
        gServiceName = serviceName;
        signal(SIGTERM, signal_handler);
        ALOGD("SERVER(%s) registering %s", tag, serviceName.c_str());
        server->registerAsService(serviceName);
        ALOGD("SERVER(%s) starting %s", tag, serviceName.c_str());
        ProcessState::self()->setThreadPoolMaxThreadCount(0);
        ProcessState::self()->startThreadPool();
        IPCThreadState::self()->joinThreadPool();
        ALOGD("SERVER(%s) %s ends.", tag, serviceName.c_str());
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
public:
    sp<IFoo> foo;
    sp<IBar> bar;
    sp<IFooCallback> fooCb;
    sp<IGraph> graphInterface;
    sp<IPointer> pointerInterface;
    sp<IPointer> validationPointerInterface;

    void getServices() {
        // getStub is true if we are in passthrough mode to skip checking
        // binderized server, false for binderized mode.
        foo = IFoo::getService("foo", gMode == PASSTHROUGH /* getStub */);
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
};

class PassthroughEnvironment : public HidlEnvironmentBase {
private:
    pid_t fooCallbackServerPid, barServerPid, graphServerPid, pointerServerPid;

    virtual void SetUp() {
        ALOGI("Environment setup beginning...");
        getServices();
        ALOGI("Environment setup complete.");
    }
};

class BinderizedEnvironment : public HidlEnvironmentBase {
private:
    pid_t fooCallbackServerPid, barServerPid, graphServerPid, pointerServerPid;
public:
    virtual void SetUp() {
        ALOGI("Environment setup beginning...");

        barServerPid = forkServer<IBar>("foo", "Bar");
        fooCallbackServerPid = forkServer<IFooCallback>("foo callback", "FooCallback");
        graphServerPid = forkServer<IGraph>("graph", "Graph");
        pointerServerPid = forkServer<IPointer>("pointer", "Pointer");

        sleep(1);

        getServices();
        ALOGI("Environment setup complete.");
    }

    virtual void TearDown() {
        // clean up by killing server processes.
        ALOGI("Environment tear-down beginning...");
        ALOGI("Killing servers...");
        killServer(barServerPid, "barServer");
        killServer(fooCallbackServerPid, "fooCallbackServer");
        killServer(graphServerPid, "graphServer");
        killServer(pointerServerPid, "pointerServer");
        ALOGI("Servers all killed.");
        ALOGI("Environment tear-down complete.");
    }
};

class HidlTest : public ::testing::Test {
public:
    sp<IFoo> foo;
    sp<IBar> bar;
    sp<IFooCallback> fooCb;
    sp<IGraph> graphInterface;
    sp<IPointer> pointerInterface;
    sp<IPointer> validationPointerInterface;

    virtual void SetUp() override {
        ALOGI("Test setup beginning...");
        HidlEnvironmentBase *env;
        if (gMode == BINDERIZED) {
            env = gBinderizedEnvironment;
        } else {
            env = gPassthroughEnvironment;
        }
        foo = env->foo;
        bar = env->bar;
        fooCb = env->fooCb;
        graphInterface = env->graphInterface;
        pointerInterface = env->pointerInterface;
        validationPointerInterface = env->validationPointerInterface;
        ALOGI("Test setup complete");
    }
};

TEST_F(HidlTest, FooDoThisTest) {
    ALOGI("CLIENT call doThis.");
    EXPECT_OK(foo->doThis(1.0f));
    ALOGI("CLIENT doThis returned.");
    EXPECT_EQ(true, true);
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
    abcParam.z = new native_handle_t();
    EXPECT_OK(foo->haveATypeFromAnotherFile(abcParam));
    ALOGI("CLIENT haveATypeFromAnotherFile returned.");
    delete abcParam.z;
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
        EXPECT_FAIL(foo->haveATypeFromAnotherFile(xyz));
    }
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
    using ::android::hardware::tests::foo::V1_0::IHwSimple;
    using ::android::hardware::tests::foo::V1_0::BnSimple;

    hidl_vec<sp<android::hardware::IBinder> > in;
    in.resize(16);
    for (size_t i = 0; i < in.size(); ++i) {
        sp<BnSimple> simpleStub = new BnSimple(new Simple(i));
        in[i] = IHwSimple::asBinder(simpleStub);
    }

    EXPECT_OK(foo->haveAVectorOfGenericInterfaces(
                in,
                [&](const auto &out) {
                    EXPECT_EQ(in.size(), out.size());
                    for (size_t i = 0; i < in.size(); ++i) {
                        sp<ISimple> inSimple = IHwSimple::asInterface(in[i]);
                        sp<ISimple> outSimple = IHwSimple::asInterface(out[i]);

                        int32_t inCookie = inSimple->getCookie();
                        int32_t outCookie = outSimple->getCookie();
                        EXPECT_EQ(inCookie, outCookie);
                    }
                }));
}

TEST_F(HidlTest, BarThisIsNewTest) {
    // Now the tricky part, get access to the derived interface.
    ALOGI("CLIENT call thisIsNew.");
    EXPECT_OK(bar->thisIsNew());
    ALOGI("CLIENT thisIsNew returned.");
}

TEST_F(HidlTest, TestArrayDimensionality) {
    hidl_array<int, 2> oneDim;
    hidl_array<int, 2, 3> twoDim;
    hidl_array<int, 2, 3, 4> threeDim;

    EXPECT_EQ(oneDim.size(), 2u);
    EXPECT_EQ(twoDim.size(), std::make_tuple(2u, 3u));
    EXPECT_EQ(threeDim.size(), std::make_tuple(2u, 3u, 4u));
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
