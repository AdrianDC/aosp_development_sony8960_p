// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/ITemplate.h
#ifndef HIDL_GENERATED_android_hardware_tests_I_Expression_H_
#define HIDL_GENERATED_android_hardware_tests_I_Expression_H_

#include <hwbinder/IBinder.h>
#include <hwbinder/IInterface.h>
#include <hwbinder/Status.h>
#include <cstdint>
#include <utils/StrongPointer.h>

namespace android {
namespace hardware {
namespace tests {


template<typename T>
using hidl_ref = int;

#ifndef HIDL_TYPES
#define HIDL_TYPES
template<typename T>
struct hidl_vec {
  T *buffer;
  size_t count;
};
struct hidl_string {
  char *buffer;
  ptrdiff_t length;
};
#endif // HIDL_TYPES

class IExpression : public ::android::hidl::IInterface {
public:

DECLARE_HWBINDER_META_INTERFACE(Expression);

// constexpr SIMPLE_EXPRESSION = 4;
// constexpr BOOLEAN_EXPRESSION = 1 == 7 && !((3 != 4 || (2 < 3 <= 3 > 4)) >= 0);
// constexpr BITWISE_EXPRESSION = ~42 & (1 << 3 | 16 >> 2) ^ 7;
// constexpr ARITHMETIC_EXPRESSION = 2 + 3 - 4 * -7 / (6 % 3);
// constexpr MESSY_EXPRESSION = 2 + (-3 & 4 / 7);
typedef struct {
uint8_t buffer[1 << 10] ;
hidl_vec<hidl_vec<uint8_t> > expmap ;

} expressionist_t;


  using setExpression_cb = std::function<void(int32_t ret)>;


  virtual ::android::hidl::binder::Status setExpression(const IExpression::expressionist_t &state , int32_t mask, setExpression_cb _cb = nullptr ) = 0;

enum Call {
  SETEXPRESSION = ::android::hidl::IBinder::FIRST_CALL_TRANSACTION + 0,


};
};  // class IExpression

}  // namespace tests
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_android_hardware_tests_I_Expression_H_
