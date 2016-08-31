#ifndef HIDL_GENERATED_android_hardware_tests_expression_V1_0_IExpression_H_
#define HIDL_GENERATED_android_hardware_tests_expression_V1_0_IExpression_H_

#include <hidl/HidlSupport.h>
#include <hidl/IServiceManager.h>
#include <hwbinder/Status.h>
#include <utils/NativeHandle.h>

namespace android {
namespace hardware {
namespace tests {
namespace expression {
namespace V1_0 {

struct IExpression : virtual public RefBase {
  enum class UInt64LiteralTypeGuessing : uint64_t {
    noSuffixDec1 = 0ull, // (int32_t)0
    noSuffixDec2 = 1ull, // (int32_t)1
    noSuffixDec3 = 18446744073709551615ull, // (int32_t)(-1)
    noSuffixDec4 = 18446744073709551615ull, // (int32_t)(~0)
    noSuffixDec5 = 2147483647ull, // (int32_t)2147483647
    noSuffixDec6 = 18446744071562067968ull, // (int64_t)(-2147483648)
    noSuffixDec7 = 2147483648ull, // (int64_t)2147483648
    noSuffixDec8 = 18446744071562067967ull, // (int64_t)(-2147483649)
    noSuffixDec9 = 0ull, // (int32_t)(~(-1))
    noSuffixHex1 = 2147483647ull, // (int32_t)0x7fffffff
    noSuffixHex2 = 2147483648ull, // (uint32_t)0x80000000
    noSuffixHex3 = 4294967295ull, // (uint32_t)0xffffffff
    longHex1 = 4294967295ull, // (int64_t)0xffffffffl
    longHex2 = 68719476735ull, // (int64_t)0xfffffffff
    longHex3 = 9223372036854775807ull, // (int64_t)0x7fffffffffffffff
    longHex4 = 9223372036854775808ull, // (uint64_t)0x8000000000000000
    longHex5 = 18446744073709551615ull, // (uint64_t)0xFFFFFFFFFFFFFFFF
  };

  enum class SuffixedLiteralTypeGuessing : int32_t {
    decInt32_1 = 1, // (bool)((~(-1)) == 0)
    decInt32_2 = 1, // (bool)((-(1 << 31)) == (1 << 31))
    decInt64_1 = 1, // (bool)((~(-1l)) == 0)
    decInt64_2 = 1, // (bool)((~4294967295) != 0)
    decInt64_3 = 1, // (bool)((~4294967295l) != 0)
    decInt64_4 = 1, // (bool)((-(1l << 63)) == (1l << 63))
    hexInt32_1 = 1, // (bool)((-0x7fffffff) < 0)
    hexUInt32_1 = 1, // (bool)((-0x80000000) > 0)
    hexUInt32_2 = 1, // (bool)((~0xFFFFFFFF) == 0)
    hexInt64_1 = 1, // (bool)((-0x7FFFFFFFFFFFFFFF) < 0)
    hexUInt64_1 = 1, // (bool)((-0x8000000000000000) > 0)
  };

  enum class Int64LiteralTypeGuessing : int64_t {
    noSuffixDec11 = -2147483648ll, // (int32_t)(1 + 0x7fffffff)
    noSuffixDec12 = 2147483647ll, // (uint32_t)(0x80000000 - 1)
  };

  enum class Int32BitShifting : int32_t {
    int32BitShift1 = -2147483648, // (int32_t)(1 << 31)
  };

  enum class UInt32BitShifting : uint32_t {
    uint32BitShift1 = 2147483648u, // (int32_t)(1 << 31)
  };

  enum class Int64BitShifting : int64_t {
    int64BitShift1 = (int64_t)(-9223372036854775808ull), // (int64_t)(1l << 63)
  };

  enum class UInt64BitShifting : uint64_t {
    uint64BitShift1 = 9223372036854775808ull, // (int64_t)(1l << 63)
  };

  enum class Precedence : int32_t {
    literal = 4, // (int32_t)4
    neg = -4, // (int32_t)(-4)
    literalL = -4, // (int64_t)(-4L)
    hex = -1, // (uint32_t)0xffffffff
    hexLong = -1, // (int64_t)0xffffffffl
    hexLong2 = -1, // (int64_t)0xfffffffff
    simpleArithmetic = 5, // (int32_t)(4 + 1)
    simpleArithmetic2 = 1, // (int32_t)((2 + 3) - 4)
    simpleBoolExpr = 0, // (bool)(1 == 4)
    simpleLogical = 1, // (bool)(1 && 1)
    simpleComp = 1, // (bool)(1 < 2)
    boolExpr1 = 0, // (bool)(!(((3 != 4) || (((2 < 3) <= 3) > 4)) >= 0))
    boolExpr = 0, // (bool)((1 == 7) && (!(((3 != 4) || (((2 < 3) <= 3) > 4)) >= 0)))
    simpleBitShift = 4, // (int32_t)(1 << 2)
    simpleBitShift2 = 2, // (int32_t)(4 >> 1)
    simpleBitShiftNeg = 2, // (int32_t)(4 << (-1))
    simpleArithmeticRightShift = -1, // (int32_t)((1 << 31) >> 31)
    simpleBitExpr = 5, // (int32_t)(1 | (16 >> 2))
    bitExpr = 3, // (int32_t)(((~42) & ((1 << 3) | (16 >> 2))) ^ 7)
    arithmeticExpr = 33, // (int32_t)((2 + 3) - ((4 * (-7)) / (10 % 3)))
    messyExpr = 2, // (int32_t)(2 + ((-3) & (4 / 7)))
    paranExpr = 2, // (int32_t)(1 + 1)
    ternary = 2, // (int32_t)(1?2:3)
    ternary2 = 3, // (int32_t)((1 && 2)?3:4)
    complicatedTernary2 = 56, // (int32_t)((((1 - 1) && (2 + 3)) || 5)?(7 * 8):(-3))
  };

  enum class OperatorSanityCheck : int32_t {
    plus = 1, // (bool)((1 + 2) == 3)
    minus = 1, // (bool)((8 - 9) == (-1))
    product = 1, // (bool)((9 * 9) == 81)
    division = 1, // (bool)((29 / 3) == 9)
    mod = 1, // (bool)((29 % 3) == 2)
    bit_or = 1, // (bool)((0xC0010000 | 0xF00D) == 0xC001F00D)
    bit_or2 = 1, // (bool)((10 | 6) == 14)
    bit_and = 1, // (bool)((10 & 6) == 2)
    bit_xor = 1, // (bool)((10 ^ 6) == 12)
    lt1 = 1, // (bool)(6 < 10)
    lt2 = 1, // (bool)((10 < 10) == 0)
    gt1 = 1, // (bool)((6 > 10) == 0)
    gt2 = 1, // (bool)((10 > 10) == 0)
    gte1 = 1, // (bool)(19 >= 10)
    gte2 = 1, // (bool)(10 >= 10)
    lte1 = 1, // (bool)(5 <= 10)
    lte2 = 1, // (bool)((19 <= 10) == 0)
    ne1 = 1, // (bool)(19 != 10)
    ne2 = 1, // (bool)((10 != 10) == 0)
    lshift = 1, // (bool)((22 << 1) == 44)
    rshift = 1, // (bool)((11 >> 1) == 5)
    logor1 = 1, // (bool)((1 || 0) == 1)
    logor2 = 1, // (bool)((1 || 1) == 1)
    logor3 = 1, // (bool)((0 || 0) == 0)
    logor4 = 1, // (bool)((0 || 1) == 1)
    logand1 = 1, // (bool)((1 && 0) == 0)
    logand2 = 1, // (bool)((1 && 1) == 1)
    logand3 = 1, // (bool)((0 && 0) == 0)
    logand4 = 1, // (bool)((0 && 1) == 0)
  };

  virtual bool isRemote() const { return false; } 

  DECLARE_REGISTER_AND_GET_SERVICE(Expression)
};

}  // namespace V1_0
}  // namespace expression
}  // namespace tests
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_android_hardware_tests_expression_V1_0_IExpression_H_
