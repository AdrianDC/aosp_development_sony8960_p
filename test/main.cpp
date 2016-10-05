#define LOG_TAG "hidl_test"
#include <android-base/logging.h>

#include <android/hardware/tests/foo/1.0/BnFoo.h>
#include <android/hardware/tests/foo/1.0/BnFooCallback.h>
#include <android/hardware/tests/bar/1.0/BnBar.h>
#include <android/hardware/tests/pointer/1.0/BnGraph.h>
#include <android/hardware/tests/pointer/1.0/BnPointer.h>

#include <gtest/gtest.h>
#include <inttypes.h>
#if GTEST_IS_THREADSAFE
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#else
#error "GTest did not detect pthread library."
#endif

#include <vector>
#include <sstream>

#include <hidl/IServiceManager.h>
#include <hidl/Status.h>
#include <hwbinder/IPCThreadState.h>
#include <hwbinder/ProcessState.h>

#include <utils/Condition.h>
#include <utils/Timers.h>

#define PUSH_ERROR_IF(__cond__) if(__cond__) { errors.push_back(std::to_string(__LINE__) + ": " + #__cond__); }
#define EXPECT_OK(__ret__) EXPECT_TRUE(isOk(__ret__))
#define EXPECT_FAIL(__ret__) EXPECT_FALSE(isOk(__ret__))
#define EXPECT_ARRAYEQ(__a1__, __a2__, __size__) EXPECT_TRUE(IsArrayEq(__a1__, __a2__, __size__))

// TODO uncomment this when kernel is patched with pointer changes.
//#define HIDL_RUN_POINTER_TESTS 1

using ::android::hardware::tests::foo::V1_0::IFoo;
using ::android::hardware::tests::foo::V1_0::IFooCallback;
using ::android::hardware::tests::bar::V1_0::IBar;
using ::android::hardware::tests::bar::V1_0::IHwBar;
using ::android::hardware::tests::foo::V1_0::Abc;
using ::android::hardware::tests::pointer::V1_0::IGraph;
using ::android::hardware::tests::pointer::V1_0::IPointer;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_string;
using ::android::sp;
using ::android::Mutex;
using ::android::Condition;

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

static void simpleGraph(IGraph::Graph& g) {
    g.nodes.resize(2);
    g.edges.resize(1);
    g.nodes[0].data = 10;
    g.nodes[1].data = 20;
    g.edges[0].left = &g.nodes[0];
    g.edges[0].right = &g.nodes[1];
}
static bool isSimpleGraph(const IGraph::Graph &g) {
    if(g.nodes.size() != 2) return false;
    if(g.edges.size() != 1) return false;
    if(g.nodes[0].data != 10) return false;
    if(g.nodes[1].data != 20) return false;
    if(g.edges[0].left != &g.nodes[0]) return false;
    if(g.edges[0].right != &g.nodes[1]) return false;
    return true;
}

static void logSimpleGraph(const char *prefix, const IGraph::Graph& g) {
    ALOGI("%s Graph %p, %d nodes, %d edges", prefix, &g, (int)g.nodes.size(), (int)g.edges.size());
    std::ostringstream os;
    for(size_t i = 0; i < g.nodes.size(); i++)
      os << &g.nodes[i] << " = " << g.nodes[i].data << ", ";
    ALOGI("%s Nodes: [%s]", prefix, os.str().c_str());
    os.str("");
    os.clear();
    for(size_t i = 0; i < g.edges.size(); i++)
      os << g.edges[i].left << " -> " << g.edges[i].right << ", ";
    ALOGI("%s Edges: [%s]", prefix, os.str().c_str());
}

struct GraphInterface : public IGraph {
    Return<void> passAGraph(const Graph& e) override;
    Return<void> giveAGraph(giveAGraph_cb _cb) override;
    Return<void> passANode(const IGraph::Node& n) override;
    Return<void> passTwoGraphs(IGraph::Graph const* g1, IGraph::Graph const* g2) override;
    Return<void> passAGamma(const IGraph::Gamma& c) override;
    Return<void> passASimpleRef(const IGraph::Alpha * a) override;
    Return<void> giveASimpleRef(giveASimpleRef_cb _hidl_cb) override;
    Return<void> passASimpleRefS(const IGraph::Theta * s) override;
    Return<int32_t> getErrors() override;
private:
    std::vector<std::string> errors;
};

Return<void> GraphInterface::passAGraph(const Graph& g) {
    ALOGI("SERVER(Graph) passAGraph start.");
    PUSH_ERROR_IF(!isSimpleGraph(g));
    // logSimpleGraph("SERVER(Graph) passAGraph:", g);
    return Void();
}

Return<void> GraphInterface::giveAGraph(giveAGraph_cb _cb) {
    IGraph::Graph g;
    simpleGraph(g);
    _cb(g);
    return Void();
}

Return<void> GraphInterface::passANode(const IGraph::Node& n) {
    PUSH_ERROR_IF(n.data != 10);
    return Void();
}

Return<void> GraphInterface::passTwoGraphs(IGraph::Graph const* g1, IGraph::Graph const* g2) {
    PUSH_ERROR_IF(g1 != g2);
    PUSH_ERROR_IF(!isSimpleGraph(*g1));
    logSimpleGraph("SERVER(Graph): passTwoGraphs", *g2);
    return Void();
}

Return<void> GraphInterface::passAGamma(const IGraph::Gamma& c) {
    if(c.a_ptr == nullptr && c.b_ptr == nullptr)
      return Void();
    ALOGI("SERVER(Graph) passAGamma received c.a = %p, c.b = %p, c.a->s = %p, c.b->s = %p",
        c.a_ptr, c.b_ptr, c.a_ptr->s_ptr, c.b_ptr->s_ptr);
    ALOGI("SERVER(Graph) passAGamma received data %d, %d",
        (int)c.a_ptr->s_ptr->data, (int)c.b_ptr->s_ptr->data);
    PUSH_ERROR_IF(c.a_ptr->s_ptr != c.b_ptr->s_ptr);
    return Void();
}
Return<void> GraphInterface::passASimpleRef(const IGraph::Alpha * a_ptr) {
    ALOGI("SERVER(Graph) passASimpleRef received %d", a_ptr->s_ptr->data);
    PUSH_ERROR_IF(a_ptr->s_ptr->data != 500);
    return Void();
}
Return<void> GraphInterface::passASimpleRefS(const IGraph::Theta * s_ptr) {
    ALOGI("SERVER(Graph) passASimpleRefS received %d @ %p", s_ptr->data, s_ptr);
    PUSH_ERROR_IF(s_ptr->data == 10);
    return Void();
}
Return<void> GraphInterface::giveASimpleRef(giveASimpleRef_cb _cb) {
    IGraph::Theta s; s.data = 500;
    IGraph::Alpha a; a.s_ptr = &s;
    _cb(&a);
    return Void();
}

Return<int32_t> GraphInterface::getErrors() {
    if(!errors.empty()) {
        for(const auto& e : errors)
            ALOGW("SERVER(Graph) error: %s", e.c_str());
    }
    return errors.size();
}

struct PointerInterface : public IPointer {
private:
    std::vector<std::string> errors;
public:
    Return<int32_t> getErrors() {
        if(!errors.empty()) {
            for(const auto& e : errors)
                ALOGW("SERVER(Pointer) error: %s", e.c_str());
        }
        return errors.size();
    }
    Return<void> foo1(const IPointer::Sam& s, IPointer::Sam const* s_ptr) override {
        PUSH_ERROR_IF(!(&s == s_ptr));
        return Void();
    }
    Return<void> foo2(const IPointer::Sam& s, const IPointer::Ada& a) override {
        PUSH_ERROR_IF(!(&s == a.s_ptr));
        return Void();
    }
    Return<void> foo3(const IPointer::Sam& s, const IPointer::Ada& a, const IPointer::Bob& b) override {
        PUSH_ERROR_IF(!(&a == b.a_ptr && a.s_ptr == b.s_ptr && a.s_ptr == &s));
        return Void();
    }
    Return<void> foo4(IPointer::Sam const* s_ptr) override {
        PUSH_ERROR_IF(!(s_ptr->data == 500));
        return Void();
    }
    Return<void> foo5(const IPointer::Ada& a, const IPointer::Bob& b) override {
        PUSH_ERROR_IF(!(a.s_ptr == b.s_ptr && b.a_ptr == &a));
        return Void();
    }
    Return<void> foo6(IPointer::Ada const* a_ptr) override {
        PUSH_ERROR_IF(!(a_ptr->s_ptr->data == 500));
        return Void();
    }
    Return<void> foo7(IPointer::Ada const* a_ptr, IPointer::Bob const* b_ptr) override {
        PUSH_ERROR_IF(!(a_ptr->s_ptr == b_ptr->s_ptr && a_ptr == b_ptr->a_ptr && a_ptr->s_ptr->data == 500));
        return Void();
    }
    Return<void> foo8(const IPointer::Dom& d) override {
        const IPointer::Cin& c = d.c;
        PUSH_ERROR_IF(&c.a != c.b_ptr->a_ptr);
        PUSH_ERROR_IF(c.a.s_ptr != c.b_ptr->s_ptr);
        PUSH_ERROR_IF(c.a.s_ptr->data != 500);
        return Void();
    }
    Return<void> foo9(::android::hardware::hidl_string const* str_ref) override {
        ALOGI("SERVER(Pointer) foo9 got string @ %p (size = %ld) \"%s\"",
            str_ref->c_str(),
            (unsigned long) str_ref->size(), str_ref->c_str());
        PUSH_ERROR_IF(!(strcmp(str_ref->c_str(), "meowmeowmeow") == 0));
        return Void();
    }
    Return<void> foo10(const ::android::hardware::hidl_vec<IPointer::Sam const*>& s_ptr_vec) override {
        PUSH_ERROR_IF(s_ptr_vec[0]->data != 500);
        if(s_ptr_vec.size() != 5) {
            errors.push_back("foo10: s_ptr_vec.size() != 5");
            return Void();
        }
        for(size_t i = 0; i < s_ptr_vec.size(); i++)
            PUSH_ERROR_IF(s_ptr_vec[0] != s_ptr_vec[i]);
        return Void();
    }
    Return<void> foo11(::android::hardware::hidl_vec<IPointer::Sam> const* s_vec_ptr) override {
        if(s_vec_ptr->size() != 5) {
            errors.push_back("foo11: s_vec_ptr->size() != 5");
            return Void();
        }
        for(size_t i = 0; i < 5; i++)
            PUSH_ERROR_IF((*s_vec_ptr)[i].data != 500);
        return Void();
    }
    Return<void> foo12(hidl_array<IPointer::Sam, 5> const* s_array_ref) override {
        for(size_t i = 0; i < 5; ++i)
            PUSH_ERROR_IF((*s_array_ref)[i].data != 500);
        return Void();
    }
    Return<void> foo13(const hidl_array<IPointer::Sam const*, 5>& s_ref_array) override {
        PUSH_ERROR_IF(s_ref_array[0]->data != 500)
        for(size_t i = 0; i < 5; i++)
            PUSH_ERROR_IF(s_ref_array[i] != s_ref_array[0])
        return Void();
    }
    Return<void> foo14(IPointer::Sam const* const* const* s_3ptr) override {
        PUSH_ERROR_IF(!((***s_3ptr).data == 500))
        return Void();
    }
    Return<void> foo15(int32_t const* const* const* i_3ptr) override {
        PUSH_ERROR_IF(!((***i_3ptr) == 500))
        return Void();
    }

    Return<void> foo16(const IPointer::Ptr& p) override {
        PUSH_ERROR_IF((*p.array_ptr)[0].s_ptr->data != 500);
        for(size_t i = 0; i < 5; i++) PUSH_ERROR_IF((*p.array_ptr)[i].s_ptr != (*p.array_ptr)[0].s_ptr);
        PUSH_ERROR_IF(*(p.int_ptr) != 500);
        for(size_t i = 0; i < 5; i++) PUSH_ERROR_IF((*p.int_array_ptr)[i] != 500);
        for(size_t i = 0; i < 5; i++) PUSH_ERROR_IF(p.int_ptr_array[i] != p.int_ptr);
        PUSH_ERROR_IF(p.a_ptr_vec.size() != 5);
        PUSH_ERROR_IF(p.a_ptr_vec[0]->s_ptr->data != 500);
        for(size_t i = 0; i < 5; i++) PUSH_ERROR_IF(p.a_ptr_vec[i]->s_ptr != p.a_ptr_vec[0]->s_ptr);
        PUSH_ERROR_IF(strcmp(p.str_ref->c_str(), "meowmeowmeow") != 0);
        PUSH_ERROR_IF(p.a_vec_ptr->size() != 5);
        PUSH_ERROR_IF((*p.a_vec_ptr)[0].s_ptr->data != 500);
        for(size_t i = 0; i < 5; i++) PUSH_ERROR_IF((*p.a_vec_ptr)[i].s_ptr != (*p.a_vec_ptr)[0].s_ptr);
        return Void();
    };
    Return<void> foo17(IPointer::Ptr const* p) override {
        return foo16(*p);
    };
    Return<void> foo18(hidl_string const* str_ref, hidl_string const* str_ref2, const hidl_string& str) override {
        PUSH_ERROR_IF(&str != str_ref);
        PUSH_ERROR_IF(str_ref != str_ref2);
        PUSH_ERROR_IF(strcmp(str.c_str(), "meowmeowmeow") != 0)
        return Void();
    };
    Return<void> foo19(
                hidl_vec<IPointer::Ada> const* a_vec_ref,
                const hidl_vec<IPointer::Ada>& a_vec,
                hidl_vec<IPointer::Ada> const* a_vec_ref2) {
        PUSH_ERROR_IF(&a_vec != a_vec_ref);
        PUSH_ERROR_IF(a_vec_ref2 != a_vec_ref);
        PUSH_ERROR_IF(a_vec.size() != 5);
        PUSH_ERROR_IF(a_vec[0].s_ptr->data != 500);
        for(size_t i = 0; i < 5; i++)
            PUSH_ERROR_IF(a_vec[i].s_ptr != a_vec[0].s_ptr);
        return Void();
    };

    Return<void> foo20(const hidl_vec<IPointer::Sam const*>&) override {
        return Void();
    }
    Return<void> foo21(hidl_array<IPointer::Ada, 3, 2, 1> const* a_array_ptr) override {
        const hidl_array<IPointer::Ada, 3, 2, 1>& a_array = *a_array_ptr;
        PUSH_ERROR_IF(a_array[0][0][0].s_ptr->data != 500);
        for(size_t i = 0; i < 3; i++)
            for(size_t j = 0; j < 2; j++)
                for(size_t k = 0; k < 1; k++)
                    PUSH_ERROR_IF(a_array[i][j][k].s_ptr != a_array[0][0][0].s_ptr);
        return Void();
    }
    Return<void> foo22(const hidl_array<IPointer::Ada const*, 3, 2, 1>& a_ptr_array) override {
        PUSH_ERROR_IF(a_ptr_array[0][0][0]->s_ptr->data != 500);
        for(size_t i = 0; i < 3; i++)
            for(size_t j = 0; j < 2; j++)
                for(size_t k = 0; k < 1; k++)
                    PUSH_ERROR_IF(a_ptr_array[i][j][k] != a_ptr_array[0][0][0]);
        return Void();
    }

    IPointer::Sam *s;
    IPointer::Ada *a;
    IPointer::Bob *b;
    IPointer::Cin *c;
    IPointer::Dom *d;

    IPointer::Ptr p;
    hidl_array<IPointer::Ada, 5> a_array;
    int32_t someInt;
    hidl_array<int32_t, 5> someIntArray;
    hidl_string str;
    hidl_vec<IPointer::Ada> a_vec;
    PointerInterface() {
        d = new IPointer::Dom();
        s = new IPointer::Sam();
        b = new IPointer::Bob();
        c = &d->c;
        a = &c->a;
        b->s_ptr = a->s_ptr = s;
        b->a_ptr = a;
        c->b_ptr = b;
        s->data = 500;

        someInt = 500;
        for(size_t i = 0; i < 5; i++) someIntArray[i] = 500;

        for(size_t i = 0; i < 5; i++) a_array[i] = *a;

        for(size_t i = 0; i < 5; i++) p.ptr_array[i] = a;
        p.array_ptr = &a_array;
        p.int_ptr = &someInt;
        p.int_array_ptr = &someIntArray;
        for(size_t i = 0; i < 5; i++) p.int_ptr_array[i] = &someInt;
        p.a_ptr_vec.resize(5);
        for(size_t i = 0; i < 5; i++) p.a_ptr_vec[i] = a;
        str = "meowmeowmeow";
        p.str_ref = &str;
        a_vec.resize(5);
        for(size_t i = 0; i < 5; i++) a_vec[i].s_ptr = s;
        p.a_vec_ptr = &a_vec;
    }
    ~PointerInterface() {
        delete d; delete s; delete b;
    }
    Return<void> bar1(bar1_cb _cb) override {
        _cb(*s, s);
        return Void();
    }
    Return<void> bar2(bar2_cb _cb) override {
        _cb(*s, *a);
        return Void();
    }
    Return<void> bar3(bar3_cb _cb) override {
        _cb(*s, *a, *b);
        return Void();
    }
    Return<void> bar4(bar4_cb _cb) override {
        _cb(s);
        return Void();
    }
    Return<void> bar5(bar5_cb _cb) override {
        _cb(*a, *b);
        return Void();
    }
    Return<void> bar6(bar6_cb _cb) override {
        _cb(a);
        return Void();
    }
    Return<void> bar7(bar7_cb _cb) override {
        _cb(a, b);
        return Void();
    }
    Return<void> bar8(bar8_cb _cb) override {
        _cb(*d);
        return Void();
    }
    Return<void> bar9(bar9_cb _cb) override {
        _cb(&str);
        return Void();
    }
    Return<void> bar10(bar10_cb _cb) override {
        hidl_vec<const IPointer::Sam *> v; v.resize(5);
        for(size_t i = 0; i < 5; i++) v[i] = s;
        _cb(v);
        return Void();
    }
    Return<void> bar11(bar11_cb _cb) override {
        hidl_vec<IPointer::Sam> v; v.resize(5);
        for(size_t i = 0; i < 5; i++) v[i].data = 500;
            _cb(&v);
        return Void();
    }
    Return<void> bar12(bar12_cb _cb) override {
        hidl_array<IPointer::Sam, 5> array;
        for(size_t i = 0; i < 5; i++) array[i] = *s;
        _cb(&array);
        return Void();
    }
    Return<void> bar13(bar13_cb _cb) override {
        hidl_array<const IPointer::Sam *, 5> array;
        for(size_t i = 0; i < 5; i++) array[i] = s;
        _cb(array);
        return Void();
    }
    Return<void> bar14(bar14_cb _cb) override {
        IPointer::Sam const* p1 = s;
        IPointer::Sam const* const* p2 = &p1;
        _cb(&p2);
        return Void();
    }
    Return<void> bar15(bar15_cb _cb) override {
        int32_t const* p1 = &someInt;
        int32_t const* const* p2 = &p1;
        _cb(&p2);
        return Void();
    }
    Return<void> bar16(bar16_cb _cb) override {
        _cb(p);
        return Void();
    }
    Return<void> bar17(bar17_cb _cb) override {
        _cb(&p);
        return Void();
    }
    Return<void> bar18(bar18_cb _cb) override {
        _cb(&str, &str, str);
        return Void();
    }
    Return<void> bar19(bar19_cb _cb) override {
        _cb(&a_vec, a_vec, &a_vec);
        return Void();
    }
    Return<void> bar20(bar20_cb _cb) override {
        // 1026 == PARCEL_REF_CAP + 2.
        // 1026 means 1 writeBuffer and 1025 writeReferences. 1025 > PARCEL_REF_CAP.
        hidl_vec<const IPointer::Sam *> v; v.resize(1026);
        for(size_t i = 0; i < 1026; i++) v[i] = s;
        _cb(v);
        return Void();
    }
    Return<void> bar21(bar21_cb _cb) override {
        hidl_array<IPointer::Ada, 3, 2, 1> a_array;
        for(size_t i = 0; i < 3; i++)
            for(size_t j = 0; j < 2; j++)
                for(size_t k = 0; k < 1; k++)
                    a_array[i][j][k] = *a;
        _cb(&a_array);
        return Void();
    }
    Return<void> bar22(bar22_cb _cb) override {
        hidl_array<const IPointer::Ada *, 3, 2, 1> a_ptr_array;
        for(size_t i = 0; i < 3; i++)
            for(size_t j = 0; j < 2; j++)
                for(size_t k = 0; k < 1; k++)
                    a_ptr_array[i][j][k] = a;
        _cb(a_ptr_array);
        return Void();
    }
};

struct FooCallback : public IFooCallback {
    FooCallback() : mLock{}, mCond{} {}
    Return<void> heyItsYou(const sp<IFooCallback> &cb) override;
    Return<bool> heyItsYouIsntIt(const sp<IFooCallback> &cb) override;
    Return<void> heyItsTheMeaningOfLife(uint8_t tmol) override;
    Return<void> reportResults(int64_t ns, reportResults_cb cb) override;
    Return<void> youBlockedMeFor(const hidl_array<int64_t, 3> &ns) override;

    static constexpr nsecs_t DELAY_S = 1;
    static constexpr nsecs_t DELAY_NS = seconds_to_nanoseconds(DELAY_S);
    static constexpr nsecs_t TOLERANCE_NS = milliseconds_to_nanoseconds(10);
    static constexpr nsecs_t ONEWAY_TOLERANCE_NS = milliseconds_to_nanoseconds(1);

    hidl_array<InvokeInfo, 3> invokeInfo;
    Mutex mLock;
    Condition mCond;
};

Return<void> FooCallback::heyItsYou(
        const sp<IFooCallback> &_cb) {
    nsecs_t start = systemTime();
    ALOGI("SERVER(FooCallback) heyItsYou cb = %p", _cb.get());
    mLock.lock();
    invokeInfo[0].invoked = true;
    invokeInfo[0].timeNs = systemTime() - start;
    mCond.signal();
    mLock.unlock();
    return Void();
}

Return<bool> FooCallback::heyItsYouIsntIt(const sp<IFooCallback> &_cb) {
    nsecs_t start = systemTime();
    ALOGI("SERVER(FooCallback) heyItsYouIsntIt cb = %p sleeping for %" PRId64 " seconds", _cb.get(), DELAY_S);
    sleep(DELAY_S);
    ALOGI("SERVER(FooCallback) heyItsYouIsntIt cb = %p responding", _cb.get());
    mLock.lock();
    invokeInfo[1].invoked = true;
    invokeInfo[1].timeNs = systemTime() - start;
    mCond.signal();
    mLock.unlock();
    return true;
}

Return<void> FooCallback::heyItsTheMeaningOfLife(uint8_t tmol) {
    nsecs_t start = systemTime();
    ALOGI("SERVER(FooCallback) heyItsTheMeaningOfLife = %d sleeping for %" PRId64 " seconds", tmol, DELAY_S);
    sleep(DELAY_S);
    ALOGI("SERVER(FooCallback) heyItsTheMeaningOfLife = %d done sleeping", tmol);
    mLock.lock();
    invokeInfo[2].invoked = true;
    invokeInfo[2].timeNs = systemTime() - start;
    mCond.signal();
    mLock.unlock();
    return Void();
}

Return<void> FooCallback::reportResults(int64_t ns, reportResults_cb cb) {
    ALOGI("SERVER(FooCallback) reportResults(%" PRId64 " seconds)", nanoseconds_to_seconds(ns));
    nsecs_t leftToWaitNs = ns;
    mLock.lock();
    while (!(invokeInfo[0].invoked && invokeInfo[1].invoked && invokeInfo[2].invoked) &&
           leftToWaitNs > 0) {
      nsecs_t start = systemTime();
      ::android::status_t rc = mCond.waitRelative(mLock, leftToWaitNs);
      if (rc != ::android::OK) {
          ALOGI("SERVER(FooCallback)::reportResults(%" PRId64 " ns) Condition::waitRelative(%" PRId64 ") returned error (%d)", ns, leftToWaitNs, rc);
          break;
      }
      ALOGI("SERVER(FooCallback)::reportResults(%" PRId64 " ns) Condition::waitRelative was signalled", ns);
      leftToWaitNs -= systemTime() - start;
    }
    mLock.unlock();
    cb(leftToWaitNs, invokeInfo);
    return Void();
}

Return<void> FooCallback::youBlockedMeFor(const hidl_array<int64_t, 3> &ns) {
    for (size_t i = 0; i < 3; i++) {
        invokeInfo[i].callerBlockedNs = ns[i];
    }
    return Void();
}

struct Bar : public IBar {
    Return<void> doThis(float param) override;

    Return<int32_t> doThatAndReturnSomething(int64_t param) override;

    Return<double> doQuiteABit(
            int32_t a,
            int64_t b,
            float c,
            double d) override;

    Return<void> doSomethingElse(
            const hidl_array<int32_t, 15> &param,
            doSomethingElse_cb _cb) override;

    Return<void> doStuffAndReturnAString(
            doStuffAndReturnAString_cb _cb) override;

    Return<void> mapThisVector(
            const hidl_vec<int32_t> &param, mapThisVector_cb _cb) override;

    Return<void> callMe(
            const sp<IFooCallback> &cb) override;

    Return<SomeEnum> useAnEnum(SomeEnum param) override;

    Return<void> haveAGooberVec(const hidl_vec<Goober>& param) override;
    Return<void> haveAGoober(const Goober &g) override;
    Return<void> haveAGooberArray(const hidl_array<Goober, 20> &lots) override;

    Return<void> haveATypeFromAnotherFile(const Abc &def) override;

    Return<void> haveSomeStrings(
            const hidl_array<hidl_string, 3> &array,
            haveSomeStrings_cb _cb) override;

    Return<void> haveAStringVec(
            const hidl_vec<hidl_string> &vector,
            haveAStringVec_cb _cb) override;

    Return<void> transposeMe(
            const hidl_array<float, 3, 5> &in, transposeMe_cb _cb) override;

    Return<void> callingDrWho(
            const MultiDimensional &in,
            callingDrWho_cb _hidl_cb) override;

    Return<void> transpose(
            const StringMatrix5x3 &in, transpose_cb _hidl_cb) override;

    Return<void> transpose2(
            const hidl_array<hidl_string, 5, 3> &in,
            transpose2_cb _hidl_cb) override;

    Return<void> sendVec(
            const hidl_vec<uint8_t> &data, sendVec_cb _hidl_cb) override;

    Return<void> sendVecVec(sendVecVec_cb _hidl_cb) override;

    Return<void> thisIsNew() override;
};

Return<void> Bar::doThis(float param) {
    ALOGI("SERVER(Bar) doThis(%.2f)", param);

    return Void();
}

Return<int32_t> Bar::doThatAndReturnSomething(
        int64_t param) {
    LOG(INFO) << "SERVER(Bar) doThatAndReturnSomething(" << param << ")";

    return 666;
}

Return<double> Bar::doQuiteABit(
        int32_t a,
        int64_t b,
        float c,
        double d) {
    LOG(INFO) << "SERVER(Bar) doQuiteABit("
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

Return<void> Bar::doSomethingElse(
        const hidl_array<int32_t, 15> &param, doSomethingElse_cb _cb) {
    ALOGI("SERVER(Bar) doSomethingElse(...)");

    hidl_array<int32_t, 32> result;
    for (size_t i = 0; i < 15; ++i) {
        result[i] = 2 * param[i];
        result[15 + i] = param[i];
    }
    result[30] = 1;
    result[31] = 2;

    _cb(result);

    return Void();
}

Return<void> Bar::doStuffAndReturnAString(
        doStuffAndReturnAString_cb _cb) {
    ALOGI("SERVER(Bar) doStuffAndReturnAString");

    hidl_string s;
    s = "Hello, world";

    _cb(s);

    return Void();
}

Return<void> Bar::mapThisVector(
        const hidl_vec<int32_t> &param, mapThisVector_cb _cb) {
    ALOGI("SERVER(Bar) mapThisVector");

    hidl_vec<int32_t> out;
    out.resize(param.size());

    for (size_t i = 0; i < out.size(); ++i) {
        out[i] = param[i] * 2;
    }

    _cb(out);

    return Void();
}

Return<void> Bar::callMe(
        const sp<IFooCallback> &cb) {
    ALOGI("SERVER(Bar) callMe %p", cb.get());

    if (cb != NULL) {

        hidl_array<nsecs_t, 3> c;
        ALOGI("SERVER(Bar) callMe %p calling IFooCallback::heyItsYou, " \
              "should return immediately", cb.get());
        c[0] = systemTime();
        cb->heyItsYou(cb);
        c[0] = systemTime() - c[0];
        ALOGI("SERVER(Bar) callMe %p calling IFooCallback::heyItsYou " \
              "returned after %" PRId64 "ns", cb.get(), c[0]);

        ALOGI("SERVER(Bar) callMe %p calling IFooCallback::heyItsYouIsntIt, " \
              "should block for %" PRId64 " seconds", cb.get(),
              FooCallback::DELAY_S);
        c[1] = systemTime();
        bool answer = cb->heyItsYouIsntIt(cb);
        c[1] = systemTime() - c[1];
        ALOGI("SERVER(Bar) callMe %p IFooCallback::heyItsYouIsntIt " \
              "responded with %d after %" PRId64 "ns", cb.get(), answer, c[1]);

        ALOGI("SERVER(Bar) callMe %p calling " \
              "IFooCallback::heyItsTheMeaningOfLife, " \
              "should return immediately ", cb.get());
        c[2] = systemTime();
        cb->heyItsTheMeaningOfLife(42);
        c[2] = systemTime() - c[2];
        ALOGI("SERVER(Bar) callMe %p After call to " \
              "IFooCallback::heyItsTheMeaningOfLife " \
              "responded after %" PRId64 "ns", cb.get(), c[2]);

        ALOGI("SERVER(Bar) callMe %p calling IFooCallback::youBlockedMeFor " \
              "to report times", cb.get());
        cb->youBlockedMeFor(c);
        ALOGI("SERVER(Bar) callMe %p After call to " \
              "IFooCallback::heyYouBlockedMeFor", cb.get());
    }

    return Void();
}

Return<Bar::SomeEnum> Bar::useAnEnum(SomeEnum param) {
    ALOGI("SERVER(Bar) useAnEnum %d", (int)param);

    return SomeEnum::goober;
}

Return<void> Bar::haveAGooberVec(const hidl_vec<Goober>& param) {
    ALOGI("SERVER(Bar) haveAGooberVec &param = %p", &param);

    return Void();
}

Return<void> Bar::haveAGoober(const Goober &g) {
    ALOGI("SERVER(Bar) haveaGoober g=%p", &g);

    return Void();
}

Return<void> Bar::haveAGooberArray(const hidl_array<Goober, 20> & /* lots */) {
    ALOGI("SERVER(Bar) haveAGooberArray");

    return Void();
}

Return<void> Bar::haveATypeFromAnotherFile(const Abc &def) {
    ALOGI("SERVER(Bar) haveATypeFromAnotherFile def=%p", &def);

    return Void();
}

Return<void> Bar::haveSomeStrings(
        const hidl_array<hidl_string, 3> &array,
        haveSomeStrings_cb _cb) {
    ALOGI("SERVER(Bar) haveSomeStrings([\"%s\", \"%s\", \"%s\"])",
          array[0].c_str(),
          array[1].c_str(),
          array[2].c_str());

    hidl_array<hidl_string, 2> result;
    result[0] = "Hello";
    result[1] = "World";

    _cb(result);

    return Void();
}

Return<void> Bar::haveAStringVec(
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

    return Void();
}

using std::to_string;

static std::string to_string(const IFoo::StringMatrix5x3 &M);
static std::string to_string(const IFoo::StringMatrix3x5 &M);
static std::string to_string(const hidl_string &s);

template<typename T>
static std::string to_string(const T *elems, size_t n) {
    std::string out;
    out = "[";
    for (size_t i = 0; i < n; ++i) {
        if (i > 0) {
            out += ", ";
        }
        out += to_string(elems[i]);
    }
    out += "]";

    return out;
}

template<typename T, size_t SIZE>
static std::string to_string(const hidl_array<T, SIZE> &array) {
    return to_string(&array[0], SIZE);
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

template<typename T>
static std::string to_string(const hidl_vec<T> &vec) {
    return to_string(&vec[0], vec.size());
}

static std::string to_string(const IFoo::StringMatrix5x3 &M) {
    return to_string(M.s);
}

static std::string to_string(const IFoo::StringMatrix3x5 &M) {
    return to_string(M.s);
}

static std::string to_string(const hidl_string &s) {
    return std::string("'") + s.c_str() + "'";
}

Return<void> Bar::transposeMe(
        const hidl_array<float, 3, 5> &in, transposeMe_cb _cb) {
    ALOGI("SERVER(Bar) transposeMe(%s)", to_string(in).c_str());

    hidl_array<float, 5, 3> out;
    for (size_t i = 0; i < 5; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            out[i][j] = in[j][i];
        }
    }

    ALOGI("SERVER(Bar) transposeMe returning %s", to_string(out).c_str());

    _cb(out);

    return Void();
}

static std::string QuuxToString(const IFoo::Quux &val) {
    std::string s;

    s = "Quux(first='";
    s += val.first.c_str();
    s += "', last='";
    s += val.last.c_str();
    s += "')";

    return s;
}

static std::string MultiDimensionalToString(const IFoo::MultiDimensional &val) {
    std::string s;

    s += "MultiDimensional(";

    s += "quuxMatrix=[";

    size_t k = 0;
    for (size_t i = 0; i < 5; ++i) {
        if (i > 0) {
            s += ", ";
        }

        s += "[";
        for (size_t j = 0; j < 3; ++j, ++k) {
            if (j > 0) {
                s += ", ";
            }

            s += QuuxToString(val.quuxMatrix[i][j]);
        }
    }
    s += "]";

    s += ")";

    return s;
}

Return<void> Bar::callingDrWho(
        const MultiDimensional &in, callingDrWho_cb _hidl_cb) {
    ALOGI("SERVER(Bar) callingDrWho(%s)", MultiDimensionalToString(in).c_str());

    MultiDimensional out;
    for (size_t i = 0; i < 5; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            out.quuxMatrix[i][j].first = in.quuxMatrix[4 - i][2 - j].last;
            out.quuxMatrix[i][j].last = in.quuxMatrix[4 - i][2 - j].first;
        }
    }

    _hidl_cb(out);

    return Void();
}

Return<void> Bar::transpose(const StringMatrix5x3 &in, transpose_cb _hidl_cb) {
    LOG(INFO) << "SERVER(Bar) transpose " << to_string(in);

    StringMatrix3x5 out;
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 5; ++j) {
            out.s[i][j] = in.s[j][i];
        }
    }

    _hidl_cb(out);

    return Void();
}

Return<void> Bar::transpose2(
        const hidl_array<hidl_string, 5, 3> &in, transpose2_cb _hidl_cb) {
    LOG(INFO) << "SERVER(Bar) transpose2 " << to_string(in);

    hidl_array<hidl_string, 3, 5> out;
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 5; ++j) {
            out[i][j] = in[j][i];
        }
    }

    _hidl_cb(out);

    return Void();
}

Return<void> Bar::sendVec(
        const hidl_vec<uint8_t> &data, sendVec_cb _hidl_cb) {
    _hidl_cb(data);

    return Void();
}

Return<void> Bar::sendVecVec(sendVecVec_cb _hidl_cb) {
    hidl_vec<hidl_vec<uint8_t>> data;
    _hidl_cb(data);

    return Void();
}

Return<void> Bar::thisIsNew() {
    ALOGI("SERVER(Bar) thisIsNew");

    return Void();
}

template <class T>
static void startServer(T server,
                        const char *serviceName,
                        const char *tag) {
    using namespace android::hardware;
    ALOGI("SERVER(%s) registering", tag);
    server->registerAsService(serviceName);
    ALOGI("SERVER(%s) starting", tag);
    ProcessState::self()->setThreadPoolMaxThreadCount(0);
    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool(); // never ends. needs kill().
    ALOGI("SERVER(%s) ends.", tag);
}

static void killServer(pid_t pid, const char *serverName) {
    if(kill(pid, SIGTERM)) {
        ALOGE("Could not kill %s; errno = %d", serverName, errno);
    } else {
        int status;
        ALOGI("Waiting for %s to exit...", serverName);
        waitpid(pid, &status, 0);
        ALOGI("Continuing...");
    }
}


class HidlTest : public ::testing::Test {
public:
    sp<IFoo> foo;
    sp<IBar> bar;
    sp<IFooCallback> fooCb;
    sp<IGraph> graphInterface;
    sp<IPointer> pointerInterface;
    PointerInterface validationPointerInterface;

    virtual void SetUp() override {
        ALOGI("Test setup beginning...");
        using namespace android::hardware;
        foo = IFoo::getService("foo");
        CHECK(foo != NULL);

        bar = IBar::getService("foo");
        CHECK(bar != NULL);

        fooCb = IFooCallback::getService("foo callback");
        CHECK(fooCb != NULL);

        graphInterface = IGraph::getService("graph");
        CHECK(graphInterface != NULL);

        pointerInterface = IPointer::getService("pointer");
        CHECK(graphInterface != NULL);

        ALOGI("Test setup complete");
    }
    virtual void TearDown() override {
    }
};

class HidlEnvironment : public ::testing::Environment {
private:
    pid_t fooCallbackServerPid, barServerPid, graphServerPid, pointerServerPid;
public:
    virtual void SetUp() {
        ALOGI("Environment setup beginning...");
        // use fork to create and kill to destroy server processes.
        if ((barServerPid = fork()) == 0) {
            // Fear me, I am a child.
            startServer(new Bar, "foo", "Bar"); // never returns
            return;
        }

        if ((fooCallbackServerPid = fork()) == 0) {
            // Fear me, I am a second child.
            startServer(new FooCallback, "foo callback", "FooCalback"); // never returns
            return;
        }

        if ((graphServerPid = fork()) == 0) {
            // Fear me, I am a third child.
            startServer(new GraphInterface, "graph", "Graph"); // never returns
            return;
        }

        if ((pointerServerPid = fork()) == 0) {
            // Fear me, I am a forth child.
            startServer(new PointerInterface, "pointer", "Pointer"); // never returns
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
        killServer(barServerPid, "barServer");
        killServer(fooCallbackServerPid, "fooCallbackServer");
        killServer(graphServerPid, "graphServer");
        killServer(pointerServerPid, "pointerServer");
        ALOGI("Servers all killed.");
        ALOGI("Environment tear-down complete.");
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

TEST_F(HidlTest, FooCallMeTest) {
    ALOGI("CLIENT call callMe.");
    // callMe is oneway, should return instantly.
    nsecs_t now;
    now = systemTime();
    EXPECT_OK(foo->callMe(fooCb));
    EXPECT_TRUE(systemTime() - now < FooCallback::ONEWAY_TOLERANCE_NS);
    ALOGI("CLIENT callMe returned.");
}

TEST_F(HidlTest, ForReportResultsTest) {

    // Bar::callMe will invoke three methods on FooCallback; one will return
    // right away (even though it is a two-way method); the second one will
    // block Bar for FooCallback::DELAY_S seconds, and the third one will return
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
        2 * FooCallback::DELAY_NS + FooCallback::TOLERANCE_NS;

    ALOGI("CLIENT: Waiting for up to %" PRId64 " seconds.",
          nanoseconds_to_seconds(reportResultsNs));

    fooCb->reportResults(reportResultsNs,
                [&](int64_t timeLeftNs,
                    const hidl_array<IFooCallback::InvokeInfo, 3> &invokeResults) {
        ALOGI("CLIENT: FooCallback::reportResults() is returning data.");
        ALOGI("CLIENT: Waited for %" PRId64 " milliseconds.",
              nanoseconds_to_milliseconds(reportResultsNs - timeLeftNs));

        EXPECT_TRUE(0 <= timeLeftNs && timeLeftNs <= reportResultsNs);

        // two-way method, was supposed to return right away
        EXPECT_TRUE(invokeResults[0].invoked);
        EXPECT_TRUE(invokeResults[0].timeNs <= invokeResults[0].callerBlockedNs);
        EXPECT_TRUE(invokeResults[0].callerBlockedNs <= FooCallback::TOLERANCE_NS);
        // two-way method, was supposed to block caller for DELAY_NS
        EXPECT_TRUE(invokeResults[1].invoked);
        EXPECT_TRUE(invokeResults[1].timeNs <= invokeResults[1].callerBlockedNs);
        EXPECT_TRUE(invokeResults[1].callerBlockedNs <=
                    FooCallback::DELAY_NS + FooCallback::TOLERANCE_NS);
        // one-way method, do not block caller, but body was supposed to block for DELAY_NS
        EXPECT_TRUE(invokeResults[2].invoked);
        EXPECT_TRUE(invokeResults[2].callerBlockedNs <= FooCallback::ONEWAY_TOLERANCE_NS);
        EXPECT_TRUE(invokeResults[2].timeNs <= FooCallback::DELAY_NS + FooCallback::TOLERANCE_NS);
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
    simpleGraph(g);
    logSimpleGraph("CLIENT", g);
    ALOGI("CLIENT call passAGraph");
    EXPECT_OK(graphInterface->passAGraph(g));
}

TEST_F(HidlTest, GiveAGraphTest) {
    EXPECT_OK(graphInterface->giveAGraph([&](const auto &newGraph) {
        logSimpleGraph("CLIENT", newGraph);
        EXPECT_TRUE(isSimpleGraph(newGraph));
    }));
}
TEST_F(HidlTest, PassANodeTest) {
    IGraph::Node node; node.data = 10;
    EXPECT_OK(graphInterface->passANode(node));
}
TEST_F(HidlTest, PassTwoGraphsTest) {
    IGraph::Graph g;
    simpleGraph(g);
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
    EXPECT_OK(validationPointerInterface.bar1([&](const auto& sptr, const auto& s) {
        EXPECT_OK(pointerInterface->foo1(sptr, s));
    }));
}
TEST_F(HidlTest, PointerPassOldBufferTest2) {
    EXPECT_OK(validationPointerInterface.bar2([&](const auto& s, const auto& a) {
        EXPECT_OK(pointerInterface->foo2(s, a));
    }));
}
TEST_F(HidlTest, PointerPassSameOldBufferPointerTest) {
    EXPECT_OK(validationPointerInterface.bar3([&](const auto& s, const auto& a, const auto& b) {
        EXPECT_OK(pointerInterface->foo3(s, a, b));
    }));
}
TEST_F(HidlTest, PointerPassOnlyTest) {
    EXPECT_OK(validationPointerInterface.bar4([&](const auto& s) {
        EXPECT_OK(pointerInterface->foo4(s));
    }));
}
TEST_F(HidlTest, PointerPassTwoEmbeddedTest) {
    EXPECT_OK(validationPointerInterface.bar5([&](const auto& a, const auto& b) {
        EXPECT_OK(pointerInterface->foo5(a, b));
    }));
}
TEST_F(HidlTest, PointerPassIndirectBufferHasDataTest) {
    EXPECT_OK(validationPointerInterface.bar6([&](const auto& a) {
        EXPECT_OK(pointerInterface->foo6(a));
    }));
}
TEST_F(HidlTest, PointerPassTwoIndirectBufferTest) {
    EXPECT_OK(validationPointerInterface.bar7([&](const auto& a, const auto& b) {
        EXPECT_OK(pointerInterface->foo7(a, b));
    }));
}
TEST_F(HidlTest, PointerPassDeeplyIndirectTest) {
    EXPECT_OK(validationPointerInterface.bar8([&](const auto& d) {
        EXPECT_OK(pointerInterface->foo8(d));
    }));
}
TEST_F(HidlTest, PointerPassStringRefTest) {
    EXPECT_OK(validationPointerInterface.bar9([&](const auto& str) {
        EXPECT_OK(pointerInterface->foo9(str));
    }));
}
TEST_F(HidlTest, PointerPassRefVecTest) {
    EXPECT_OK(validationPointerInterface.bar10([&](const auto& v) {
        EXPECT_OK(pointerInterface->foo10(v));
    }));
}
TEST_F(HidlTest, PointerPassVecRefTest) {
    EXPECT_OK(validationPointerInterface.bar11([&](const auto& v) {
        EXPECT_OK(pointerInterface->foo11(v));
    }));
}
TEST_F(HidlTest, PointerPassArrayRefTest) {
    EXPECT_OK(validationPointerInterface.bar12([&](const auto& array) {
        EXPECT_OK(pointerInterface->foo12(array));
    }));
}
TEST_F(HidlTest, PointerPassRefArrayTest) {
    EXPECT_OK(validationPointerInterface.bar13([&](const auto& array) {
        EXPECT_OK(pointerInterface->foo13(array));
    }));
}
TEST_F(HidlTest, PointerPass3RefTest) {
    EXPECT_OK(validationPointerInterface.bar14([&](const auto& p3) {
        EXPECT_OK(pointerInterface->foo14(p3));
    }));
}
TEST_F(HidlTest, PointerPassInt3RefTest) {
    EXPECT_OK(validationPointerInterface.bar15([&](const auto& p3) {
        EXPECT_OK(pointerInterface->foo15(p3));
    }));
}
TEST_F(HidlTest, PointerPassEmbeddedPointersTest) {
    EXPECT_OK(validationPointerInterface.bar16([&](const auto& p) {
        EXPECT_OK(pointerInterface->foo16(p));
    }));
}
TEST_F(HidlTest, PointerPassEmbeddedPointers2Test) {
    EXPECT_OK(validationPointerInterface.bar17([&](const auto& p) {
        EXPECT_OK(pointerInterface->foo17(p));
    }));
}
TEST_F(HidlTest, PointerPassCopiedStringTest) {
    EXPECT_OK(validationPointerInterface.bar18([&](const auto& str_ref, const auto& str_ref2, const auto& str) {
        EXPECT_OK(pointerInterface->foo18(str_ref, str_ref2, str));
    }));
}
TEST_F(HidlTest, PointerPassCopiedVecTest) {
    EXPECT_OK(validationPointerInterface.bar19([&](const auto& a_vec_ref, const auto& a_vec, const auto& a_vec_ref2) {
        EXPECT_OK(pointerInterface->foo19(a_vec_ref, a_vec, a_vec_ref2));
    }));
}
TEST_F(HidlTest, PointerPassBigRefVecTest) {
    EXPECT_OK(validationPointerInterface.bar20([&](const auto& v) {
        EXPECT_FAIL(pointerInterface->foo20(v));
    }));
}
TEST_F(HidlTest, PointerPassMultidimArrayRefTest) {
    EXPECT_OK(validationPointerInterface.bar21([&](const auto& v) {
        EXPECT_OK(pointerInterface->foo21(v));
    }));
}
TEST_F(HidlTest, PointerPassRefMultidimArrayTest) {
    EXPECT_OK(validationPointerInterface.bar22([&](const auto& v) {
        EXPECT_OK(pointerInterface->foo22(v));
    }));
}
TEST_F(HidlTest, PointerGiveOldBufferTest) {
    EXPECT_OK(pointerInterface->bar1([&](const auto& sptr, const auto& s) {
        EXPECT_OK(validationPointerInterface.foo1(sptr, s));
    }));
}
TEST_F(HidlTest, PointerGiveOldBufferTest2) {
    EXPECT_OK(pointerInterface->bar2([&](const auto& s, const auto& a) {
        EXPECT_OK(validationPointerInterface.foo2(s, a));
    }));
}
TEST_F(HidlTest, PointerGiveSameOldBufferPointerTest) {
    EXPECT_OK(pointerInterface->bar3([&](const auto& s, const auto& a, const auto& b) {
        EXPECT_OK(validationPointerInterface.foo3(s, a, b));
    }));
}
TEST_F(HidlTest, PointerGiveOnlyTest) {
    EXPECT_OK(pointerInterface->bar4([&](const auto& s) {
        EXPECT_OK(validationPointerInterface.foo4(s));
    }));
}
TEST_F(HidlTest, PointerGiveTwoEmbeddedTest) {
    EXPECT_OK(pointerInterface->bar5([&](const auto& a, const auto& b) {
        EXPECT_OK(validationPointerInterface.foo5(a, b));
    }));
}
TEST_F(HidlTest, PointerGiveIndirectBufferHasDataTest) {
    EXPECT_OK(pointerInterface->bar6([&](const auto& a) {
        EXPECT_OK(validationPointerInterface.foo6(a));
    }));
}
TEST_F(HidlTest, PointerGiveTwoIndirectBufferTest) {
    EXPECT_OK(pointerInterface->bar7([&](const auto& a, const auto& b) {
        EXPECT_OK(validationPointerInterface.foo7(a, b));
    }));
}
TEST_F(HidlTest, PointerGiveDeeplyIndirectTest) {
    EXPECT_OK(pointerInterface->bar8([&](const auto& d) {
        EXPECT_OK(validationPointerInterface.foo8(d));
    }));
}
TEST_F(HidlTest, PointerGiveStringRefTest) {
    EXPECT_OK(pointerInterface->bar9([&](const auto& str) {
        EXPECT_OK(validationPointerInterface.foo9(str));
    }));
}
TEST_F(HidlTest, PointerGiveRefVecTest) {
    EXPECT_OK(pointerInterface->bar10([&](const auto& v) {
        EXPECT_OK(validationPointerInterface.foo10(v));
    }));
}
TEST_F(HidlTest, PointerGiveVecRefTest) {
    EXPECT_OK(pointerInterface->bar11([&](const auto& v) {
        EXPECT_OK(validationPointerInterface.foo11(v));
    }));
}
TEST_F(HidlTest, PointerGiveArrayRefTest) {
    EXPECT_OK(pointerInterface->bar12([&](const auto& array) {
        EXPECT_OK(validationPointerInterface.foo12(array));
    }));
}
TEST_F(HidlTest, PointerGiveRefArrayTest) {
    EXPECT_OK(pointerInterface->bar13([&](const auto& array) {
        EXPECT_OK(validationPointerInterface.foo13(array));
    }));
}
TEST_F(HidlTest, PointerGive3RefTest) {
    EXPECT_OK(pointerInterface->bar14([&](const auto& p3) {
        EXPECT_OK(validationPointerInterface.foo14(p3));
    }));
}
TEST_F(HidlTest, PointerGiveInt3RefTest) {
    EXPECT_OK(pointerInterface->bar15([&](const auto& p3) {
        EXPECT_OK(validationPointerInterface.foo15(p3));
    }));
}
TEST_F(HidlTest, PointerGiveEmbeddedPointersTest) {
    EXPECT_OK(pointerInterface->bar16([&](const auto& p) {
        EXPECT_OK(validationPointerInterface.foo16(p));
    }));
}
TEST_F(HidlTest, PointerGiveEmbeddedPointers2Test) {
    EXPECT_OK(pointerInterface->bar17([&](const auto& p) {
        EXPECT_OK(validationPointerInterface.foo17(p));
    }));
}
TEST_F(HidlTest, PointerGiveCopiedStringTest) {
    EXPECT_OK(pointerInterface->bar18([&](const auto& str_ref, const auto& str_ref2, const auto& str) {
        EXPECT_OK(validationPointerInterface.foo18(str_ref, str_ref2, str));
    }));
}
TEST_F(HidlTest, PointerGiveCopiedVecTest) {
    EXPECT_OK(pointerInterface->bar19([&](const auto& a_vec_ref, const auto& a_vec, const auto& a_vec_ref2) {
        EXPECT_OK(validationPointerInterface.foo19(a_vec_ref, a_vec, a_vec_ref2));
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
        EXPECT_OK(validationPointerInterface.foo21(v));
    }));
}
TEST_F(HidlTest, PointerGiveRefMultidimArrayTest) {
    EXPECT_OK(pointerInterface->bar22([&](const auto& v) {
        EXPECT_OK(validationPointerInterface.foo22(v));
    }));
}
TEST_F(HidlTest, PointerReportErrorsTest) {
    Return<int32_t> ret = pointerInterface->getErrors();
    EXPECT_OK(ret);
    EXPECT_EQ(int32_t(ret), 0);
}
#endif

int main(int argc, char **argv) {
    ::testing::AddGlobalTestEnvironment(new HidlEnvironment);
    ::testing::InitGoogleTest(&argc, argv);
    int status = RUN_ALL_TESTS();

    ALOGI("Test result = %d", status);
    return status;
}
