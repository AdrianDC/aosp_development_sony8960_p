#ifndef HIDL_GENERATED_android_hardware_tests_expression_V1_0_IExpression_H_
#define HIDL_GENERATED_android_hardware_tests_expression_V1_0_IExpression_H_

#include <hidl/HidlSupport.h>
#include <hidl/IServiceManager.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <utils/NativeHandle.h>

namespace android {
namespace hardware {
namespace tests {
namespace expression {
namespace V1_0 {

struct IExpression : virtual public RefBase {
    enum class UInt64LiteralTypeGuessing : uint64_t {
        noSuffixDec1 = 0ull, // 0
        noSuffixDec2 = 1ull, // 1
        noSuffixDec3 = 18446744073709551615ull, // (-1)
        noSuffixDec4 = 18446744073709551615ull, // (~0)
        noSuffixDec5 = 2147483647ull, // 2147483647
        noSuffixDec6 = 18446744071562067968ull, // (-2147483648)
        noSuffixDec7 = 2147483648ull, // 2147483648
        noSuffixDec8 = 18446744071562067967ull, // (-2147483649)
        noSuffixDec9 = 0ull, // (~(-1))
        noSuffixHex1 = 2147483647ull, // 0x7fffffff
        noSuffixHex2 = 2147483648ull, // 0x80000000
        noSuffixHex3 = 4294967295ull, // 0xffffffff
        longHex1 = 4294967295ull, // 0xffffffffl
        longHex2 = 68719476735ull, // 0xfffffffff
        longHex3 = 9223372036854775807ull, // 0x7fffffffffffffff
        longHex4 = 9223372036854775808ull, // 0x8000000000000000
        longHex5 = 18446744073709551615ull, // 0xFFFFFFFFFFFFFFFF
    };

    enum class SuffixedLiteralTypeGuessing : int32_t {
        decInt32_1 = 1, // ((~(-1)) == 0)
        decInt32_2 = 1, // ((-(1 << 31)) == (1 << 31))
        decInt64_1 = 1, // ((~(-1l)) == 0)
        decInt64_2 = 1, // ((~4294967295) != 0)
        decInt64_3 = 1, // ((~4294967295l) != 0)
        decInt64_4 = 1, // ((-(1l << 63)) == (1l << 63))
        hexInt32_1 = 1, // ((-0x7fffffff) < 0)
        hexUInt32_1 = 1, // ((-0x80000000) > 0)
        hexUInt32_2 = 1, // ((~0xFFFFFFFF) == 0)
        hexInt64_1 = 1, // ((-0x7FFFFFFFFFFFFFFF) < 0)
        hexUInt64_1 = 1, // ((-0x8000000000000000) > 0)
    };

    enum class Int64LiteralTypeGuessing : int64_t {
        noSuffixDec11 = -2147483648ll, // (1 + 0x7fffffff)
        noSuffixDec12 = 2147483647ll, // (0x80000000 - 1)
    };

    enum class Int32BitShifting : int32_t {
        int32BitShift1 = -2147483648, // (1 << 31)
    };

    enum class UInt32BitShifting : uint32_t {
        uint32BitShift1 = 2147483648u, // (1 << 31)
    };

    enum class Int64BitShifting : int64_t {
        int64BitShift1 = (int64_t)(-9223372036854775808ull), // (1l << 63)
    };

    enum class UInt64BitShifting : uint64_t {
        uint64BitShift1 = 9223372036854775808ull, // (1l << 63)
    };

    enum class Precedence : int32_t {
        literal = 4,
        neg = -4, // (-4)
        literalL = -4, // (-4L)
        hex = -1, // 0xffffffff
        hexLong = -1, // 0xffffffffl
        hexLong2 = -1, // 0xfffffffff
        simpleArithmetic = 5, // (4 + 1)
        simpleArithmetic2 = 1, // ((2 + 3) - 4)
        simpleBoolExpr = 0, // (1 == 4)
        simpleLogical = 1, // (1 && 1)
        simpleComp = 1, // (1 < 2)
        boolExpr1 = 0, // (!(((3 != 4) || (((2 < 3) <= 3) > 4)) >= 0))
        boolExpr = 0, // ((1 == 7) && (!(((3 != 4) || (((2 < 3) <= 3) > 4)) >= 0)))
        simpleBitShift = 4, // (1 << 2)
        simpleBitShift2 = 2, // (4 >> 1)
        simpleBitShiftNeg = 2, // (4 << (-1))
        simpleArithmeticRightShift = -1, // ((1 << 31) >> 31)
        simpleBitExpr = 5, // (1 | (16 >> 2))
        bitExpr = 3, // (((~42) & ((1 << 3) | (16 >> 2))) ^ 7)
        arithmeticExpr = 33, // ((2 + 3) - ((4 * (-7)) / (10 % 3)))
        messyExpr = 2, // (2 + ((-3) & (4 / 7)))
        paranExpr = 2, // (1 + 1)
        ternary = 2, // (1?2:3)
        ternary2 = 3, // ((1 && 2)?3:4)
        complicatedTernary2 = 56, // ((((1 - 1) && (2 + 3)) || 5)?(7 * 8):(-3))
    };

    enum class OperatorSanityCheck : int32_t {
        plus = 1, // ((1 + 2) == 3)
        minus = 1, // ((8 - 9) == (-1))
        product = 1, // ((9 * 9) == 81)
        division = 1, // ((29 / 3) == 9)
        mod = 1, // ((29 % 3) == 2)
        bit_or = 1, // ((0xC0010000 | 0xF00D) == 0xC001F00D)
        bit_or2 = 1, // ((10 | 6) == 14)
        bit_and = 1, // ((10 & 6) == 2)
        bit_xor = 1, // ((10 ^ 6) == 12)
        lt1 = 1, // (6 < 10)
        lt2 = 1, // ((10 < 10) == 0)
        gt1 = 1, // ((6 > 10) == 0)
        gt2 = 1, // ((10 > 10) == 0)
        gte1 = 1, // (19 >= 10)
        gte2 = 1, // (10 >= 10)
        lte1 = 1, // (5 <= 10)
        lte2 = 1, // ((19 <= 10) == 0)
        ne1 = 1, // (19 != 10)
        ne2 = 1, // ((10 != 10) == 0)
        lshift = 1, // ((22 << 1) == 44)
        rshift = 1, // ((11 >> 1) == 5)
        logor1 = 1, // ((1 || 0) == 1)
        logor2 = 1, // ((1 || 1) == 1)
        logor3 = 1, // ((0 || 0) == 0)
        logor4 = 1, // ((0 || 1) == 1)
        logand1 = 1, // ((1 && 0) == 0)
        logand2 = 1, // ((1 && 1) == 1)
        logand3 = 1, // ((0 && 0) == 0)
        logand4 = 1, // ((0 && 1) == 0)
    };

    enum class Grayscale : int8_t {
        WHITE = 126,
        GRAY = 127,
        DARK_GRAY = -128, // 128
        BLACK = -127, // 129
    };

    enum class Color : int8_t {
        WHITE = 126,
        GRAY = 127,
        DARK_GRAY = -128, // 128
        BLACK = -127, // 129
        RED = -126, // 130
        RUBY = 0,
        GREEN = 1,
        BLUE = 5,
        CYAN = 6,
        ORANGE = 7,
        ROSE = 126, // WHITE
    };

    enum class Foo1 : int8_t {
    };

    enum class Foo2 : int8_t {
    };

    enum class Foo3 : int8_t {
        BAR1 = 0,
        BAR2 = 10,
    };

    enum class Foo4 : int8_t {
        BAR1 = 0,
        BAR2 = 10,
        BAR3 = 11,
        BAR4 = 21, // (BAR2 + BAR3)
    };

    enum class Number : uint8_t {
        MAX = 255,
        MAX_PLUS_1 = 0, // 256
        MAX_PLUS_2 = 1, // 257
    };

    enum class Constants : int32_t {
        CONST_FOO = 0,
        CONST_BAR = 70,
        MAX_ARRAY_SIZE = 20,
        MAX_ARRAY_SIZE2 = 21,
        MAX_ARRAY_SIZE3 = 40, // (MAX_ARRAY_SIZE + MAX_ARRAY_SIZE)
        MY_INT32_MAX_MINUS_1 = 2147483646, // 0x7FFFFFFE
        MY_INT32_MAX = 2147483647,
        MY_INT32_MIN = -2147483648,
        MY_INT32_MIN_PLUS_1 = -2147483647,
    };

    constexpr static hidl_version version = {1,0};
    virtual const hidl_version& getInterfaceVersion() const {
        return version;
    }

    virtual bool isRemote() const { return false; }

    // @callflow(key=1 /* (Constants:CONST_FOO + 1) */)
    virtual ::android::hardware::Return<void> foo1(const hidl_array<int32_t, 1 /* (Constants:CONST_FOO + 1) */>& array) = 0;
    virtual ::android::hardware::Return<void> foo2(const hidl_array<int32_t, 13 /* (5 + 8) */>& array) = 0;
    virtual ::android::hardware::Return<void> foo3(const hidl_array<int32_t, 20 /* Constants:MAX_ARRAY_SIZE */>& array) = 0;
    DECLARE_REGISTER_AND_GET_SERVICE(Expression)
};

}  // namespace V1_0
}  // namespace expression
}  // namespace tests
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_android_hardware_tests_expression_V1_0_IExpression_H_
