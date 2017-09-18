
#include <android/hardware/tests/bar/1.0/IImportRules.h>
#include <android/hardware/tests/foo/1.0/IFoo.h>

using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_array;
using ::android::hardware::tests::bar::V1_0::IImportRules;
using ::android::hardware::tests::foo::V1_0::IFoo;
using ::android::sp;

namespace {

// Check array dimension order
static_assert(std::is_same<IFoo::multidimArrayOne, hidl_array<int32_t, 5, 6, 7>>::value,
              "hidl-gen output array dimention order is incorrect");
static_assert(std::is_same<IFoo::multidimArrayTwo, hidl_array<int32_t, 8, 9, 10, 5, 6, 7>>::value,
              "hidl-gen output array dimention order is incorrect");
static_assert(
    std::is_same<IFoo::multidimArrayThree, hidl_array<int32_t, 2, 3, 4, 8, 9, 10, 5, 6, 7>>::value,
    "hidl-gen output array dimention order is incorrect");

// Check correct type in ambiguous case
static_assert(std::is_same<decltype(IFoo::S1::foo), IFoo::InnerTestStruct>::value,
              "hidl-gen wrong (inner) type in output");
static_assert(!std::is_same<decltype(IFoo::S1::foo), IFoo::S1::InnerTestStruct>::value,
              "hidl-gen wrong (inner) type in output");
static_assert((int32_t) decltype(IFoo::S2::foo)::VALUE == 0,
              "hidl-gen wrong (inner) type in output");

// Ensure (statically) that the types in IImportRules resolves to the correct types by
// overriding the methods with fully namespaced types as arguments.
struct MyImportRules : public IImportRules {
    Return<void> rule0a(
        const ::android::hardware::tests::bar::V1_0::IImportRules::Outer&) override {
        return Void();
    }

    Return<void> rule0a1(
        const ::android::hardware::tests::bar::V1_0::IImportRules::Outer&) override {
        return Void();
    }

    Return<void> rule0b(
        const ::android::hardware::tests::bar::V1_0::IImportRules::Outer&) override {
        return Void();
    }

    Return<void> rule0b1(
        const ::android::hardware::tests::bar::V1_0::IImportRules::Outer&) override {
        return Void();
    }

    Return<void> rule0c(const ::android::hardware::tests::foo::V1_0::Outer&) override {
        return Void();
    }

    Return<void> rule0d(const ::android::hardware::tests::foo::V1_0::Outer&) override {
        return Void();
    }

    Return<void> rule0e(
        const ::android::hardware::tests::bar::V1_0::IImportRules::Outer::Inner&) override {
        return Void();
    }

    Return<void> rule0f(
        const ::android::hardware::tests::bar::V1_0::IImportRules::Outer::Inner&) override {
        return Void();
    }

    Return<void> rule0g(const ::android::hardware::tests::foo::V1_0::Outer::Inner&) override {
        return Void();
    }

    Return<void> rule0h(const ::android::hardware::tests::foo::V1_0::Outer::Inner&) override {
        return Void();
    }

    Return<void> rule1a(const ::android::hardware::tests::bar::V1_0::Def&) override {
        return Void();
    }

    Return<void> rule1b(const ::android::hardware::tests::foo::V1_0::Def&) override {
        return Void();
    }

    Return<void> rule2a(const ::android::hardware::tests::foo::V1_0::Unrelated&) override {
        return Void();
    }

    Return<void> rule2b(const sp<::android::hardware::tests::foo::V1_0::IFooCallback>&) override {
        return Void();
    }
};

}  // anonymous namespace
