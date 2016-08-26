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

import java.util.Vector;

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

    public static String toString(Vector<IBase.Foo.Bar> vec) {
        StringBuilder builder = new StringBuilder();
        builder.append("[");
        for (int i = 0; i < vec.size(); ++i) {
            if (i > 0) {
                builder.append(", ");
            }
            builder.append(toString(vec.elementAt(i)));
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

        Expect(toString(proxy.someBoolMethod(true)), "false");

        {
            boolean[] someBoolArray = new boolean[3];
            someBoolArray[0] = true;
            someBoolArray[1] = false;
            someBoolArray[2] = true;

            Expect(toString(proxy.someBoolArrayMethod(someBoolArray)),
                   "[false, true, false, true]");

            Expect(toString(proxy.someBoolVectorMethod(someBoolArray)),
                   "[false, true, false]");
        }

        proxy.doThis(1.0f);

        Expect(toString(proxy.doThatAndReturnSomething(1)), "666");
        Expect(toString(proxy.doQuiteABit(1, 2L, 3.0f, 4.0)), "666.5");

        {
            int[] param = new int[15];
            for (int i = 0; i < param.length; ++i) {
                param[i] = i;
            }

            Expect(toString(proxy.doSomethingElse(param)),
                   "[0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, " +
                   "0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 1, 2]");

            Expect(toString(proxy.mapThisVector(param)),
                   "[0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28]");
        }

        Expect(toString(proxy.doStuffAndReturnAString()), "'Hello, world!'");

        BazCallback cb = new BazCallback();
        ExpectTrue(!cb.wasCalled());
        proxy.callMe(cb);
        ExpectTrue(cb.wasCalled());

        Expect(toString(proxy.useAnEnum(IBaz.SomeEnum.goober)), "-64");

        {
            String[] strings = new String[3];
            strings[0] = "one";
            strings[1] = "two";
            strings[2] = "three";

            Expect(toString(proxy.haveSomeStrings(strings)),
                   "['Hello', 'World']");

            Expect(toString(proxy.haveAStringVec(strings)),
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

        public boolean[] someBoolVectorMethod(boolean[] x) {
            Log.d(TAG, "Baz someBoolVectorMethod(" + HidlTestJava.toString(x) + ")");

            boolean[] out = new boolean[x.length];
            for (int i = 0; i < x.length; ++i) {
                out[i] = !x[i];
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

        public int[] mapThisVector(int[] param) {
            Log.d(TAG, "mapThisVector " + HidlTestJava.toString(param));

            for (int i = 0; i < param.length; ++i) {
                param[i] *= 2;
            }

            return param;
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

        public String[] haveAStringVec(String[] vector) {
            Log.d(TAG, "haveAStringVec ["
                        + "\"" + vector[0] + "\", "
                        + "\"" + vector[1] + "\", "
                        + "\"" + vector[2] + "\"]");

            String[] result = new String[2];
            result[0] = "Hello";
            result[1] = "World";

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
