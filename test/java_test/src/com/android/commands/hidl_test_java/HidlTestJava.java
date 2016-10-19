/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.commands.hidl_test_java;

import android.hardware.tests.baz.V1_0.IBase;
import android.hardware.tests.baz.V1_0.IBaz;
import android.hardware.tests.baz.V1_0.IBazCallback;
import android.os.HwBinder;
import android.util.Log;

import java.util.ArrayList;

public final class HidlTestJava {
    private static final String TAG = "HidlTestJava";

    public static void main(String[] args) {
        int exitCode = 1;
        try {
            exitCode = new HidlTestJava().run(args);
        } catch (Exception e) {
            Log.e(TAG, "Error ", e);
        }
        System.exit(exitCode);
    }

    public int run(String[] args) {
        if (args[0].equals("-c")) {
            client();
        } else if (args[0].equals("-s")) {
            server();
        } else {
            Log.e(TAG, "Usage: HidlTestJava  -c(lient) | -s(erver)");
            return 1;
        }

        return 0;
    }

    public static String toString(IBase.Foo.Bar bar) {
        StringBuilder builder = new StringBuilder();
        builder.append("Bar(");
        builder.append("z = ");
        builder.append(bar.z);
        builder.append(", ");
        builder.append("s = ");
        builder.append(toString(bar.s));
        builder.append(")");

        return builder.toString();
    }

    public static String toString(IBase.Foo foo) {
        StringBuilder builder = new StringBuilder();
        builder.append("Foo(");
        builder.append("x = ");
        builder.append(foo.x);
        builder.append(", ");
        builder.append("y = ");
        builder.append(toString(foo.y));
        builder.append(", ");
        builder.append("aaa = ");
        builder.append(toString(foo.aaa));
        builder.append(")");

        return builder.toString();
    }

    public static String toString(ArrayList<IBase.Foo.Bar> vec) {
        StringBuilder builder = new StringBuilder();
        builder.append("[");
        for (int i = 0; i < vec.size(); ++i) {
            if (i > 0) {
                builder.append(", ");
            }
            builder.append(toString(vec.get(i)));
        }
        builder.append("]");

        return builder.toString();
    }

    public static String fooVecToString(ArrayList<IBase.Foo> vec) {
        StringBuilder builder = new StringBuilder();
        builder.append("[");
        for (int i = 0; i < vec.size(); ++i) {
            if (i > 0) {
                builder.append(", ");
            }
            builder.append(toString(vec.get(i)));
        }
        builder.append("]");

        return builder.toString();
    }

    public static String macAddressVecToString(ArrayList<byte[]> vec) {
        StringBuilder builder = new StringBuilder();
        builder.append("[");
        for (int i = 0; i < vec.size(); ++i) {
            if (i > 0) {
                builder.append(", ");
            }
            builder.append(toString(vec.get(i)));
        }
        builder.append("]");

        return builder.toString();
    }

    public static String booleanVecToString(ArrayList<Boolean> vec) {
        StringBuilder builder = new StringBuilder();
        builder.append("[");
        for (int i = 0; i < vec.size(); ++i) {
            if (i > 0) {
                builder.append(", ");
            }
            builder.append(toString(vec.get(i)));
        }
        builder.append("]");

        return builder.toString();
    }

    public static String integerVecToString(ArrayList<Integer> vec) {
        StringBuilder builder = new StringBuilder();
        builder.append("[");
        for (int i = 0; i < vec.size(); ++i) {
            if (i > 0) {
                builder.append(", ");
            }
            builder.append(toString(vec.get(i)));
        }
        builder.append("]");

        return builder.toString();
    }

    public static String stringVecToString(ArrayList<String> vec) {
        StringBuilder builder = new StringBuilder();
        builder.append("[");
        for (int i = 0; i < vec.size(); ++i) {
            if (i > 0) {
                builder.append(", ");
            }
            builder.append(toString(vec.get(i)));
        }
        builder.append("]");

        return builder.toString();
    }

    public static String toString(IBase.Foo[] array) {
        StringBuilder builder = new StringBuilder();
        builder.append("[");
        for (int i = 0; i < array.length; ++i) {
            if (i > 0) {
                builder.append(", ");
            }
            builder.append(toString(array[i]));
        }
        builder.append("]");

        return builder.toString();
    }

    public static String toString(IBase.Foo.Bar[] array) {
        StringBuilder builder = new StringBuilder();
        builder.append("[");
        for (int i = 0; i < array.length; ++i) {
            if (i > 0) {
                builder.append(", ");
            }
            builder.append(toString(array[i]));
        }
        builder.append("]");

        return builder.toString();
    }

    public static String toString(int[] val) {
        StringBuilder builder = new StringBuilder();
        builder.append("[");
        for (int i = 0; i < val.length; ++i) {
            if (i > 0) {
                builder.append(", ");
            }
            builder.append(val[i]);
        }
        builder.append("]");

        return builder.toString();
    }

    public static String toString(boolean[] val) {
        StringBuilder builder = new StringBuilder();
        builder.append("[");
        for (int i = 0; i < val.length; ++i) {
            if (i > 0) {
                builder.append(", ");
            }
            builder.append(val[i] ? "true" : "false");
        }
        builder.append("]");

        return builder.toString();
    }

    public static String toString(String[] val) {
        StringBuilder builder = new StringBuilder();
        builder.append("[");
        for (int i = 0; i < val.length; ++i) {
            if (i > 0) {
                builder.append(", ");
            }
            builder.append(toString(val[i]));
        }
        builder.append("]");

        return builder.toString();
    }

    public static String toString(byte[] val) {
        StringBuilder builder = new StringBuilder();
        for (int i = 0; i < val.length; ++i) {
            if (i > 0) {
                builder.append(":");
            }

            byte b = val[i];
            if (b < 16) {
                builder.append("0");
            }
            builder.append(Integer.toHexString(b));
        }

        return builder.toString();
    }

    public static String toString(boolean x) {
        return x ? "true" : "false";
    }

    public static String toString(int x) {
        return Integer.toString(x);
    }

    public static String toString(double x) {
        return Double.toString(x);
    }

    public static String toString(String s) {
        return "'" + s + "'";
    }

    public static String toString(String[][] M) {
        StringBuilder builder = new StringBuilder();

        builder.append("[");
        for (int i = 0; i < M.length; ++i) {
            if (i > 0) {
                builder.append(", ");
            }
            builder.append(toString(M[i]));
        }
        builder.append("]");

        return builder.toString();
    }

    public static String toString(IBase.StringMatrix5x3 M) {
        return toString(M.s);
    }

    public static String toString(IBase.StringMatrix3x5 M) {
        return toString(M.s);
    }

    public static String toString(IBase.VectorOfArray vec) {
        StringBuilder out = new StringBuilder();

        out.append("VectorOfArray(");
        for (int i = 0; i < vec.addresses.size(); ++i) {
            if (i > 0) {
                out.append(", ");
            }

            byte[] address = vec.addresses.get(i);

            for (int j = 0; j < 6; ++j) {
                if (j > 0) {
                    out.append(":");
                }

                byte b = address[j];
                if (b < 16) {
                    out.append("0");
                }
                out.append(Integer.toHexString(b));
            }
        }

        out.append(")");
        return out.toString();
    }

    private void ExpectTrue(boolean x) {
        if (x) {
            return;
        }

        throw new RuntimeException();
    }

    private void Expect(String result, String s) {
        if (result.equals(s)) {
            return;
        }

        Log.e(TAG, "Expected '" + s + "', got '" + result + "'");
        throw new RuntimeException();
    }

    class BazCallback extends IBazCallback.Stub {
        private boolean mCalled;

        public BazCallback() {
            mCalled = false;
        }

        boolean wasCalled() {
            return mCalled;
        }

        public void heyItsMe(IBazCallback cb) {
            mCalled = true;

            cb.heyItsMe(null);
        }
    }

    private String numberToEnglish(int x) {
        final String[] kDigits = {
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
            final String[] kSpecialTens = {
                "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen",
            };

            return kSpecialTens[x - 10];
        }

        if (x < 20) {
            return kDigits[x % 10] + "teen";
        }

        if (x < 100) {
            final String[] kDecades = {
                "twenty", "thirty", "forty", "fifty", "sixty", "seventy",
                "eighty", "ninety",
            };

            return kDecades[x / 10 - 2] + kDigits[x % 10];
        }

        return "positively huge!";
    }

    private void client() {
        {
            // Test access through base interface binder.
            IBase baseProxy = IBase.asInterface(HwBinder.getService("baz"));
            baseProxy.someBaseMethod();
        }

        IBaz proxy = IBaz.asInterface(HwBinder.getService("baz"));

        proxy.someBaseMethod();

        {
            IBase.Foo foo = new IBase.Foo();
            foo.x = 1;

            for (int i = 0; i < 5; ++i) {
                IBase.Foo.Bar bar = new IBase.Foo.Bar();
                bar.z = 1.0f + (float)i * 0.01f;
                bar.s = "Hello, world " + i;
                foo.aaa.add(bar);
            }

            foo.y.z = 3.14f;
            foo.y.s = "Lorem ipsum...";

            IBase.Foo result = proxy.someOtherBaseMethod(foo);

            Expect(toString(result),
                   "Foo(x = 1, " +
                       "y = Bar(z = 3.14, s = 'Lorem ipsum...'), " +
                       "aaa = [Bar(z = 1.0, s = 'Hello, world 0'), " +
                              "Bar(z = 1.01, s = 'Hello, world 1'), " +
                              "Bar(z = 1.02, s = 'Hello, world 2'), " +
                              "Bar(z = 1.03, s = 'Hello, world 3'), " +
                              "Bar(z = 1.04, s = 'Hello, world 4')])");
        }

        {
            IBase.Foo[] inputArray = new IBase.Foo[2];

            IBase.Foo foo = new IBase.Foo();
            foo.x = 1;

            for (int i = 0; i < 5; ++i) {
                IBase.Foo.Bar bar = new IBase.Foo.Bar();
                bar.z = 1.0f + (float)i * 0.01f;
                bar.s = "Hello, world " + i;
                foo.aaa.add(bar);
            }

            foo.y.z = 3.14f;
            foo.y.s = "Lorem ipsum...";

            inputArray[0] = foo;

            foo = new IBase.Foo();
            foo.x = 2;

            for (int i = 0; i < 3; ++i) {
                IBase.Foo.Bar bar = new IBase.Foo.Bar();
                bar.z = 2.0f - (float)i * 0.01f;
                bar.s = "Lorem ipsum " + i;
                foo.aaa.add(bar);
            }

            foo.y.z = 1.1414f;
            foo.y.s = "Et tu brute?";

            inputArray[1] = foo;

            IBase.Foo[] outputArray = proxy.someMethodWithFooArrays(inputArray);

            Expect(toString(outputArray),
                   "[Foo(x = 2, " +
                        "y = Bar(z = 1.1414, s = 'Et tu brute?'), " +
                        "aaa = [Bar(z = 2.0, s = 'Lorem ipsum 0'), " +
                               "Bar(z = 1.99, s = 'Lorem ipsum 1'), " +
                               "Bar(z = 1.98, s = 'Lorem ipsum 2')]), " +
                     "Foo(x = 1, " +
                         "y = Bar(z = 3.14, s = 'Lorem ipsum...'), " +
                         "aaa = [Bar(z = 1.0, s = 'Hello, world 0'), " +
                                "Bar(z = 1.01, s = 'Hello, world 1'), " +
                                "Bar(z = 1.02, s = 'Hello, world 2'), " +
                                "Bar(z = 1.03, s = 'Hello, world 3'), " +
                                "Bar(z = 1.04, s = 'Hello, world 4')])]");
        }

        {
            ArrayList<IBase.Foo> inputVec = new ArrayList<IBase.Foo>();

            IBase.Foo foo = new IBase.Foo();
            foo.x = 1;

            for (int i = 0; i < 5; ++i) {
                IBase.Foo.Bar bar = new IBase.Foo.Bar();
                bar.z = 1.0f + (float)i * 0.01f;
                bar.s = "Hello, world " + i;
                foo.aaa.add(bar);
            }

            foo.y.z = 3.14f;
            foo.y.s = "Lorem ipsum...";

            inputVec.add(foo);

            foo = new IBase.Foo();
            foo.x = 2;

            for (int i = 0; i < 3; ++i) {
                IBase.Foo.Bar bar = new IBase.Foo.Bar();
                bar.z = 2.0f - (float)i * 0.01f;
                bar.s = "Lorem ipsum " + i;
                foo.aaa.add(bar);
            }

            foo.y.z = 1.1414f;
            foo.y.s = "Et tu brute?";

            inputVec.add(foo);

            ArrayList<IBase.Foo> outputVec =
                proxy.someMethodWithFooVectors(inputVec);

            Expect(fooVecToString(outputVec),
                   "[Foo(x = 2, " +
                        "y = Bar(z = 1.1414, s = 'Et tu brute?'), " +
                        "aaa = [Bar(z = 2.0, s = 'Lorem ipsum 0'), " +
                               "Bar(z = 1.99, s = 'Lorem ipsum 1'), " +
                               "Bar(z = 1.98, s = 'Lorem ipsum 2')]), " +
                     "Foo(x = 1, " +
                         "y = Bar(z = 3.14, s = 'Lorem ipsum...'), " +
                         "aaa = [Bar(z = 1.0, s = 'Hello, world 0'), " +
                                "Bar(z = 1.01, s = 'Hello, world 1'), " +
                                "Bar(z = 1.02, s = 'Hello, world 2'), " +
                                "Bar(z = 1.03, s = 'Hello, world 3'), " +
                                "Bar(z = 1.04, s = 'Hello, world 4')])]");
        }

        {
            IBase.VectorOfArray in = new IBase.VectorOfArray();

            int k = 0;
            for (int i = 0; i < 3; ++i) {
                byte[] mac = new byte[6];
                for (int j = 0; j < 6; ++j, ++k) {
                    mac[j] = (byte)k;
                }

                in.addresses.add(mac);
            }

            IBase.VectorOfArray out = proxy.someMethodWithVectorOfArray(in);

            Expect(toString(out),
                   "VectorOfArray("
                     + "0c:0d:0e:0f:10:11, "
                     + "06:07:08:09:0a:0b, "
                     + "00:01:02:03:04:05)");
        }

        {
            ArrayList<byte[]> in = new ArrayList<byte[]>();

            int k = 0;
            for (int i = 0; i < 3; ++i) {
                byte[] mac = new byte[6];
                for (int j = 0; j < 6; ++j, ++k) {
                    mac[j] = (byte)k;
                }

                in.add(mac);
            }

            ArrayList<byte[]> out = proxy.someMethodTakingAVectorOfArray(in);

            Expect(macAddressVecToString(out),
                   "[0c:0d:0e:0f:10:11, 06:07:08:09:0a:0b, 00:01:02:03:04:05]");
        }

        {
            IBase.StringMatrix5x3 in = new IBase.StringMatrix5x3();
            for (int i = 0; i < 5; ++i) {
                for (int j = 0; j < 3; ++j) {
                    in.s[i][j] = numberToEnglish(3 * i + j + 1);
                }
            }

            IBase.StringMatrix3x5 out = proxy.transpose(in);

            // [[1 2 3] [4 5 6] [7 8 9] [10 11 12] [13 14 15]]^T
            // = [[1 4 7 10 13] [2 5 8 11 14] [3 6 9 12 15]]
            Expect(toString(out),
                   "[['one', 'four', 'seven', 'ten', 'thirteen'], "
                   +"['two', 'five', 'eight', 'eleven', 'fourteen'], "
                   +"['three', 'six', 'nine', 'twelve', 'fifteen']]");
        }

        {
            String[][] in = new String[5][3];
            for (int i = 0; i < 5; ++i) {
                for (int j = 0; j < 3; ++j) {
                    in[i][j] = numberToEnglish(3 * i + j + 1);
                }
            }

            String[][] out = proxy.transpose2(in);

            // [[1 2 3] [4 5 6] [7 8 9] [10 11 12] [13 14 15]]^T
            // = [[1 4 7 10 13] [2 5 8 11 14] [3 6 9 12 15]]
            Expect(toString(out),
                   "[['one', 'four', 'seven', 'ten', 'thirteen'], "
                   +"['two', 'five', 'eight', 'eleven', 'fourteen'], "
                   +"['three', 'six', 'nine', 'twelve', 'fifteen']]");
        }

        Expect(toString(proxy.someBoolMethod(true)), "false");

        {
            boolean[] someBoolArray = new boolean[3];
            someBoolArray[0] = true;
            someBoolArray[1] = false;
            someBoolArray[2] = true;

            Expect(toString(proxy.someBoolArrayMethod(someBoolArray)),
                   "[false, true, false, true]");

            ArrayList<Boolean> someBoolVec = new ArrayList<Boolean>();
            someBoolVec.add(true);
            someBoolVec.add(false);
            someBoolVec.add(true);

            Expect(booleanVecToString(proxy.someBoolVectorMethod(someBoolVec)),
                   "[false, true, false]");
        }

        proxy.doThis(1.0f);

        Expect(toString(proxy.doThatAndReturnSomething(1)), "666");
        Expect(toString(proxy.doQuiteABit(1, 2L, 3.0f, 4.0)), "666.5");

        {
            int[] paramArray = new int[15];
            ArrayList<Integer> paramVec = new ArrayList<Integer>();
            for (int i = 0; i < paramArray.length; ++i) {
                paramArray[i] = i;
                paramVec.add(i);
            }

            Expect(toString(proxy.doSomethingElse(paramArray)),
                   "[0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, " +
                   "0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 1, 2]");

            Expect(integerVecToString(proxy.mapThisVector(paramVec)),
                   "[0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28]");
        }

        Expect(toString(proxy.doStuffAndReturnAString()), "'Hello, world!'");

        BazCallback cb = new BazCallback();
        ExpectTrue(!cb.wasCalled());
        proxy.callMe(cb);
        ExpectTrue(cb.wasCalled());

        Expect(toString(proxy.useAnEnum(IBaz.SomeEnum.goober)), "-64");

        {
            String[] stringArray = new String[3];
            stringArray[0] = "one";
            stringArray[1] = "two";
            stringArray[2] = "three";

            Expect(toString(proxy.haveSomeStrings(stringArray)),
                   "['Hello', 'World']");

            ArrayList<String> stringVec = new ArrayList<String>();
            stringVec.add("one");
            stringVec.add("two");
            stringVec.add("three");

            Expect(stringVecToString(proxy.haveAStringVec(stringVec)),
                   "['Hello', 'World']");
        }

        proxy.returnABunchOfStrings(
                new IBaz.returnABunchOfStringsCallback() {
                    @Override
                    public void onValues(String a, String b, String c) {
                        Expect(a, "Eins");
                        Expect(b, "Zwei");
                        Expect(c, "Drei");
                    }
                });
    }

    class Baz extends IBaz.Stub {
        // from IBase
        public void someBaseMethod() {
            Log.d(TAG, "Baz someBaseMethod");
        }

        public IBase.Foo someOtherBaseMethod(IBase.Foo foo) {
            Log.d(TAG, "Baz someOtherBaseMethod " + HidlTestJava.toString(foo));
            return foo;
        }

        public IBase.Foo[] someMethodWithFooArrays(IBase.Foo[] fooInput) {
            Log.d(TAG, "Baz someMethodWithFooArrays " + HidlTestJava.toString(fooInput));

            IBase.Foo[] fooOutput = new IBase.Foo[2];
            fooOutput[0] = fooInput[1];
            fooOutput[1] = fooInput[0];

            return fooOutput;
        }

        public ArrayList<IBase.Foo> someMethodWithFooVectors(
                ArrayList<IBase.Foo> fooInput) {
            Log.d(TAG, "Baz someMethodWithFooVectors " + HidlTestJava.fooVecToString(fooInput));

            ArrayList<IBase.Foo> fooOutput = new ArrayList<IBase.Foo>();
            fooOutput.add(fooInput.get(1));
            fooOutput.add(fooInput.get(0));

            return fooOutput;
        }

        public IBase.VectorOfArray someMethodWithVectorOfArray(
                IBase.VectorOfArray in) {
            Log.d(TAG, "Baz someMethodWithVectorOfArray " + HidlTestJava.toString(in));

            IBase.VectorOfArray out = new IBase.VectorOfArray();
            int n = in.addresses.size();
            for (int i = 0; i < n; ++i) {
                out.addresses.add(in.addresses.get(n - i - 1));
            }

            return out;
        }

        public ArrayList<byte[/* 6 */]> someMethodTakingAVectorOfArray(
                ArrayList<byte[/* 6 */]> in) {
            Log.d(TAG, "Baz someMethodTakingAVectorOfArray");

            int n = in.size();
            ArrayList<byte[]> out = new ArrayList<byte[]>();
            for (int i = 0; i < n; ++i) {
                out.add(in.get(n - i - 1));
            }

            return out;
        }

        public IBase.StringMatrix3x5 transpose(IBase.StringMatrix5x3 in) {
            Log.d(TAG, "Baz transpose " + HidlTestJava.toString(in));

            IBase.StringMatrix3x5 out = new IBase.StringMatrix3x5();
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 5; ++j) {
                    out.s[i][j] = in.s[j][i];
                }
            }

            return out;
        }

        public String[][] transpose2(String[][] in) {
            Log.d(TAG, "Baz transpose2 " + HidlTestJava.toString(in));

            String[][] out = new String[3][5];
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 5; ++j) {
                    out[i][j] = in[j][i];
                }
            }

            return out;
        }

        public boolean someBoolMethod(boolean x) {
            Log.d(TAG, "Baz someBoolMethod(" + x + ")");

            return !x;
        }

        public boolean[] someBoolArrayMethod(boolean[] x) {
            Log.d(TAG, "Baz someBoolArrayMethod("
                    + HidlTestJava.toString(x) + ")");

            boolean[] out = new boolean[4];
            out[0] = !x[0];
            out[1] = !x[1];
            out[2] = !x[2];
            out[3] = true;

            return out;
        }

        public ArrayList<Boolean> someBoolVectorMethod(ArrayList<Boolean> x) {
            Log.d(TAG, "Baz someBoolVectorMethod(" + HidlTestJava.booleanVecToString(x) + ")");

            ArrayList<Boolean> out = new ArrayList<Boolean>();
            for (int i = 0; i < x.size(); ++i) {
                out.add(!x.get(i));
            }

            return out;
        }

        public void doThis(float param) {
            Log.d(TAG, "Baz doThis " + param);
        }

        public int doThatAndReturnSomething(long param) {
            Log.d(TAG, "Baz doThatAndReturnSomething " + param);
            return 666;
        }

        public double doQuiteABit(int a, long b, float c, double d) {
            Log.d(TAG, "Baz doQuiteABit " + a + ", " + b + ", " + c + ", " + d);
            return 666.5;
        }

        public int[] doSomethingElse(int[] param) {
            Log.d(TAG, "Baz doSomethingElse " + HidlTestJava.toString(param));

            int[] something = new int[32];
            for (int i = 0; i < 15; ++i) {
                something[i] = 2 * param[i];
                something[15 + i] = param[i];
            }
            something[30] = 1;
            something[31] = 2;

            return something;
        }

        public String doStuffAndReturnAString() {
            Log.d(TAG, "doStuffAndReturnAString");
            return "Hello, world!";
        }

        public ArrayList<Integer> mapThisVector(ArrayList<Integer> param) {
            Log.d(TAG, "mapThisVector " + HidlTestJava.integerVecToString(param));

            ArrayList<Integer> out = new ArrayList<Integer>();

            for (int i = 0; i < param.size(); ++i) {
                out.add(2 * param.get(i));
            }

            return out;
        }

        class BazCallback extends IBazCallback.Stub {
            public void heyItsMe(IBazCallback cb) {
                Log.d(TAG, "SERVER: heyItsMe");
            }
        }

        public void callMe(IBazCallback cb) {
            Log.d(TAG, "callMe");
            cb.heyItsMe(new BazCallback());
        }

        public byte useAnEnum(byte zzz) {
            Log.d(TAG, "useAnEnum " + zzz);
            return SomeEnum.quux;
        }

        public String[] haveSomeStrings(String[] array) {
            Log.d(TAG, "haveSomeStrings ["
                        + "\"" + array[0] + "\", "
                        + "\"" + array[1] + "\", "
                        + "\"" + array[2] + "\"]");

            String[] result = new String[2];
            result[0] = "Hello";
            result[1] = "World";

            return result;
        }

        public ArrayList<String> haveAStringVec(ArrayList<String> vector) {
            Log.d(TAG, "haveAStringVec ["
                        + "\"" + vector.get(0) + "\", "
                        + "\"" + vector.get(1) + "\", "
                        + "\"" + vector.get(2) + "\"]");

            ArrayList<String> result = new ArrayList<String>();
            result.add("Hello");
            result.add("World");

            return result;
        }

        public void returnABunchOfStrings(returnABunchOfStringsCallback cb) {
            cb.onValues("Eins", "Zwei", "Drei");
        }
    }

    private void server() {
        Baz baz = new Baz();
        baz.registerService("baz");

        try {
            Thread.sleep(20000);
        } catch (InterruptedException e) {
        }
    }
}
