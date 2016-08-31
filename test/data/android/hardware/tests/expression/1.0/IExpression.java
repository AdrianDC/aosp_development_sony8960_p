package android.hardware.tests.expression.V1_0;

import android.os.IHwBinder;
import android.os.IHwInterface;
import android.os.HwBinder;
import android.os.HwBlob;
import android.os.HwParcel;

import java.util.Vector;

public interface IExpression extends IHwInterface {
  public static final String kInterfaceName = "android.hardware.tests.expression@1.0::IExpression";

  public static IExpression asInterface(IHwBinder binder) {
    if (binder == null) {
      return null;
    }

    IHwInterface iface =
        binder.queryLocalInterface(kInterfaceName);

    if ((iface != null) && (iface instanceof IExpression)) {
      return (IExpression)iface;
    }

    return new IExpression.Proxy(binder);
  }

  public IHwBinder asBinder();

  public final class UInt64LiteralTypeGuessing {
    public static final long noSuffixDec1 = 0L; // (int32_t)0
    public static final long noSuffixDec2 = 1L; // (int32_t)1
    public static final long noSuffixDec3 = -1L; // (int32_t)(-1)
    public static final long noSuffixDec4 = -1L; // (int32_t)(~0)
    public static final long noSuffixDec5 = 2147483647L; // (int32_t)2147483647
    public static final long noSuffixDec6 = -2147483648L; // (int64_t)(-2147483648)
    public static final long noSuffixDec7 = 2147483648L; // (int64_t)2147483648
    public static final long noSuffixDec8 = -2147483649L; // (int64_t)(-2147483649)
    public static final long noSuffixDec9 = 0L; // (int32_t)(~(-1))
    public static final long noSuffixHex1 = 2147483647L; // (int32_t)0x7fffffff
    public static final long noSuffixHex2 = 2147483648L; // (uint32_t)0x80000000
    public static final long noSuffixHex3 = 4294967295L; // (uint32_t)0xffffffff
    public static final long longHex1 = 4294967295L; // (int64_t)0xffffffffl
    public static final long longHex2 = 68719476735L; // (int64_t)0xfffffffff
    public static final long longHex3 = 9223372036854775807L; // (int64_t)0x7fffffffffffffff
    public static final long longHex4 = -9223372036854775808L; // (uint64_t)0x8000000000000000
    public static final long longHex5 = -1L; // (uint64_t)0xFFFFFFFFFFFFFFFF
  };

  public final class SuffixedLiteralTypeGuessing {
    public static final int decInt32_1 = 1; // (bool)((~(-1)) == 0)
    public static final int decInt32_2 = 1; // (bool)((-(1 << 31)) == (1 << 31))
    public static final int decInt64_1 = 1; // (bool)((~(-1l)) == 0)
    public static final int decInt64_2 = 1; // (bool)((~4294967295) != 0)
    public static final int decInt64_3 = 1; // (bool)((~4294967295l) != 0)
    public static final int decInt64_4 = 1; // (bool)((-(1l << 63)) == (1l << 63))
    public static final int hexInt32_1 = 1; // (bool)((-0x7fffffff) < 0)
    public static final int hexUInt32_1 = 1; // (bool)((-0x80000000) > 0)
    public static final int hexUInt32_2 = 1; // (bool)((~0xFFFFFFFF) == 0)
    public static final int hexInt64_1 = 1; // (bool)((-0x7FFFFFFFFFFFFFFF) < 0)
    public static final int hexUInt64_1 = 1; // (bool)((-0x8000000000000000) > 0)
  };

  public final class Int64LiteralTypeGuessing {
    public static final long noSuffixDec11 = -2147483648L; // (int32_t)(1 + 0x7fffffff)
    public static final long noSuffixDec12 = 2147483647L; // (uint32_t)(0x80000000 - 1)
  };

  public final class Int32BitShifting {
    public static final int int32BitShift1 = -2147483648; // (int32_t)(1 << 31)
  };

  public final class UInt32BitShifting {
    public static final int uint32BitShift1 = -2147483648; // (int32_t)(1 << 31)
  };

  public final class Int64BitShifting {
    public static final long int64BitShift1 = -9223372036854775808L; // (int64_t)(1l << 63)
  };

  public final class UInt64BitShifting {
    public static final long uint64BitShift1 = -9223372036854775808L; // (int64_t)(1l << 63)
  };

  public final class Precedence {
    public static final int literal = 4; // (int32_t)4
    public static final int neg = -4; // (int32_t)(-4)
    public static final int literalL = -4; // (int64_t)(-4L)
    public static final int hex = -1; // (uint32_t)0xffffffff
    public static final int hexLong = -1; // (int64_t)0xffffffffl
    public static final int hexLong2 = -1; // (int64_t)0xfffffffff
    public static final int simpleArithmetic = 5; // (int32_t)(4 + 1)
    public static final int simpleArithmetic2 = 1; // (int32_t)((2 + 3) - 4)
    public static final int simpleBoolExpr = 0; // (bool)(1 == 4)
    public static final int simpleLogical = 1; // (bool)(1 && 1)
    public static final int simpleComp = 1; // (bool)(1 < 2)
    public static final int boolExpr1 = 0; // (bool)(!(((3 != 4) || (((2 < 3) <= 3) > 4)) >= 0))
    public static final int boolExpr = 0; // (bool)((1 == 7) && (!(((3 != 4) || (((2 < 3) <= 3) > 4)) >= 0)))
    public static final int simpleBitShift = 4; // (int32_t)(1 << 2)
    public static final int simpleBitShift2 = 2; // (int32_t)(4 >> 1)
    public static final int simpleBitShiftNeg = 2; // (int32_t)(4 << (-1))
    public static final int simpleArithmeticRightShift = -1; // (int32_t)((1 << 31) >> 31)
    public static final int simpleBitExpr = 5; // (int32_t)(1 | (16 >> 2))
    public static final int bitExpr = 3; // (int32_t)(((~42) & ((1 << 3) | (16 >> 2))) ^ 7)
    public static final int arithmeticExpr = 33; // (int32_t)((2 + 3) - ((4 * (-7)) / (10 % 3)))
    public static final int messyExpr = 2; // (int32_t)(2 + ((-3) & (4 / 7)))
    public static final int paranExpr = 2; // (int32_t)(1 + 1)
    public static final int ternary = 2; // (int32_t)(1?2:3)
    public static final int ternary2 = 3; // (int32_t)((1 && 2)?3:4)
    public static final int complicatedTernary2 = 56; // (int32_t)((((1 - 1) && (2 + 3)) || 5)?(7 * 8):(-3))
  };

  public final class OperatorSanityCheck {
    public static final int plus = 1; // (bool)((1 + 2) == 3)
    public static final int minus = 1; // (bool)((8 - 9) == (-1))
    public static final int product = 1; // (bool)((9 * 9) == 81)
    public static final int division = 1; // (bool)((29 / 3) == 9)
    public static final int mod = 1; // (bool)((29 % 3) == 2)
    public static final int bit_or = 1; // (bool)((0xC0010000 | 0xF00D) == 0xC001F00D)
    public static final int bit_or2 = 1; // (bool)((10 | 6) == 14)
    public static final int bit_and = 1; // (bool)((10 & 6) == 2)
    public static final int bit_xor = 1; // (bool)((10 ^ 6) == 12)
    public static final int lt1 = 1; // (bool)(6 < 10)
    public static final int lt2 = 1; // (bool)((10 < 10) == 0)
    public static final int gt1 = 1; // (bool)((6 > 10) == 0)
    public static final int gt2 = 1; // (bool)((10 > 10) == 0)
    public static final int gte1 = 1; // (bool)(19 >= 10)
    public static final int gte2 = 1; // (bool)(10 >= 10)
    public static final int lte1 = 1; // (bool)(5 <= 10)
    public static final int lte2 = 1; // (bool)((19 <= 10) == 0)
    public static final int ne1 = 1; // (bool)(19 != 10)
    public static final int ne2 = 1; // (bool)((10 != 10) == 0)
    public static final int lshift = 1; // (bool)((22 << 1) == 44)
    public static final int rshift = 1; // (bool)((11 >> 1) == 5)
    public static final int logor1 = 1; // (bool)((1 || 0) == 1)
    public static final int logor2 = 1; // (bool)((1 || 1) == 1)
    public static final int logor3 = 1; // (bool)((0 || 0) == 0)
    public static final int logor4 = 1; // (bool)((0 || 1) == 1)
    public static final int logand1 = 1; // (bool)((1 && 0) == 0)
    public static final int logand2 = 1; // (bool)((1 && 1) == 1)
    public static final int logand3 = 1; // (bool)((0 && 0) == 0)
    public static final int logand4 = 1; // (bool)((0 && 1) == 0)
  };

  public static final int kOpEnd = IHwBinder.FIRST_CALL_TRANSACTION + 0;


  public static final class Proxy implements IExpression {
    private IHwBinder mRemote;

    public Proxy(IHwBinder remote) {
      mRemote = remote;
    }

    public IHwBinder asBinder() {
      return mRemote;
    }

    // Methods from ::android::hardware::tests::expression::V1_0::IExpression follow.
  }

  public static abstract class Stub extends HwBinder implements IExpression {
    public IHwBinder asBinder() {
      return this;
    }

    public IHwInterface queryLocalInterface(String descriptor) {
      if (kInterfaceName.equals(descriptor)) {
        return this;
      }
      return null;
    }

    public void onTransact(int code, HwParcel request, final HwParcel reply, int flags) {
      switch (code) {
      }
    }
  }
}
