/*
 * Copyright (C) 2015 The Android Open Source Project
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

#include "hasher.h"
#include <iostream>

#include <android-base/logging.h>
#include <hwbinder/IServiceManager.h>
#include <utils/String16.h>
#include <utils/StrongPointer.h>

#include "android/hardware/tests/ITypes.h"

using std::string;
// libutils:
using android::OK;
using android::sp;
using android::status_t;
using android::String16;

// libbinder:
using android::hidl::getService;
using android::hidl::hidl_version;
using android::hidl::make_hidl_version;
using android::hidl::gen_ref;
using android::hidl::from_ref;

// generated
using android::hardware::tests::ITypes;

using std::cerr;
using std::cout;
using std::endl;

namespace android {
namespace hardware {
namespace tests {
namespace client {

const char kServiceName[] = "android.hardware.tests.ITypes";

bool GetService(sp<ITypes>* service, hidl_version version) {
  //cout << "Retrieving test service binder" << endl;
  status_t status = getService(String16(kServiceName), version, service);
  if (status != OK) {
    cerr << "Failed to get service binder: '" << kServiceName
         << "' status=" << status << endl;
    return false;
  }
  return true;
}

}  // namespace client
}  // namespace tests
}  // namespace aidl
}  // namespace android

void testHash(sp<ITypes> service, string message = "")
{
  static int calls = 0;
  calls++;
  uint64_t hash;
  service->getHash([&hash](auto ret){hash = ret;});
  if (hash != getHash()) {
    cout << "Hash mismatch: " << hash << " vs. " << getHash();
    cout << " on call # " << calls << " - " << message << endl;
  }
}
using eu8 = ::android::hardware::tests::ITypes::eu8;
using es8 = ::android::hardware::tests::ITypes::es8;
using eu16 = ::android::hardware::tests::ITypes::eu16;
using es16 = ::android::hardware::tests::ITypes::es16;
using eu32 = ::android::hardware::tests::ITypes::eu32;
using es32 = ::android::hardware::tests::ITypes::es32;
using eu64 = ::android::hardware::tests::ITypes::eu64;
using es64 = ::android::hardware::tests::ITypes::es64;
using ec = ::android::hardware::tests::ITypes::ec;

void enumTestSmall(sp<ITypes> service)
{
  service->echoEnum(eu8::U8F, es8::S8F, eu16::U16F, es16::S16F, eu32::U32F, es32::S32F, eu64::U64F, es64::S64F, ec::CF,
                    [](eu8 a, es8 b, eu16 c, es16 d, eu32 e, es32 f, eu64 g, es64 h, ec i) {
                      // enum elements have gone through hidl-gen, so I don't fully trust them in a
                      // test program - it's ugly but I'd rather retype all the numbers.
                      if (sizeof(a) != 1 || sizeof(b) != 1 || sizeof(c) != 2 || sizeof(d) != 2 ||
                          sizeof(e) != 4 || sizeof(f) != 4 || sizeof(g) != 8 || sizeof(h) != 8 ||
                          sizeof(i) != 1) {
                        cout << "Wrong size of enum" << endl;
                      }
                      if ((uint8_t)a != 34) cout << "a wrong" << endl;
                      if ((int8_t)b != 104) cout << "b wrong" << endl;
                      if ((uint16_t)c != 234) cout << "c wrong" << endl;
                      if ((int16_t)d != 334) cout << "d wrong" << endl;
                      if ((uint32_t)e != 434) cout << "e wrong" << endl;
                      if ((int32_t)f != 534) cout << "f wrong" << endl;
                      if ((uint64_t)g != 634) cout << "g wrong" << endl;
                      if ((int64_t)h != 734) cout << "h wrong" << endl;
                      if ((uint8_t)i != 43) cout << "i wrong" << endl;
                      addHash((uint8_t)a);
                      addHash((int8_t)b);
                      addHash((uint16_t)c);
                      addHash((int16_t)d);
                      addHash((uint32_t)e);
                      addHash((int32_t)f);
                      addHash((uint64_t)g);
                      addHash((int64_t)h);
                      addHash((uint8_t)i);
                    });
  testHash(service, "after enumTestSmall");
}
void enumTestMin(sp<ITypes> service)
{
  service->echoEnum(eu8::min, es8::min, eu16::min, es16::min, eu32::min, es32::min, eu64::min, es64::min, ec::min,
                    [](eu8 a, es8 b, eu16 c, es16 d, eu32 e, es32 f, eu64 g, es64 h, ec i) {
                      if ((uint8_t)a != 0) cout << "a wrong" << endl;
                      if ((int8_t)b != -128) cout << "b wrong" << endl;
                      if ((uint16_t)c != 0) cout << "c wrong" << endl;
                      if ((int16_t)d != -32768) cout << "d wrong" << endl;
                      if ((uint32_t)e != 0) cout << "e wrong" << endl;
                      if ((int32_t)f != -2147483648) cout << "f wrong" << endl;
                      if ((uint64_t)g != 0) cout << "g wrong" << endl;
                      if ((int64_t)h != -9223372036854775807LL) cout << "h wrong" << endl;
                      if ((uint8_t)i != 0) cout << "i wrong" << endl;
                      addHash((uint8_t)a);
                      addHash((int8_t)b);
                      addHash((uint16_t)c);
                      addHash((int16_t)d);
                      addHash((uint32_t)e);
                      addHash((int32_t)f);
                      addHash((uint64_t)g);
                      addHash((int64_t)h);
                      addHash((uint8_t)i);
                    });
  testHash(service, "after enumTestMin");
}
void enumTestMax(sp<ITypes> service)
{
  service->echoEnum(eu8::max, es8::max, eu16::max, es16::max, eu32::max, es32::max, eu64::max, es64::max, ec::max,
                    [](eu8 a, es8 b, eu16 c, es16 d, eu32 e, es32 f, eu64 g, es64 h, ec i) {
                      if ((uint8_t)a != 255) cout << "a wrong" << endl;
                      if ((int8_t)b != 127) cout << "b wrong" << endl;
                      if ((uint16_t)c != 65535) cout << "c wrong" << endl;
                      if ((int16_t)d != 32767) cout << "d wrong" << endl;
                      if ((uint32_t)e != 0xffffffff) cout << "e wrong" << endl;
                      if ((int32_t)f != 2147483647) cout << "f wrong" << endl;
                      if ((uint64_t)g != 0xffff'ffff'ffffffffLL) cout << "g wrong" << endl;
                      if ((int64_t)h != 0x7fff'ffff'ffffffffLL) cout << "h wrong" << endl;
                      if ((uint8_t)i != 255) cout << "i wrong" << endl;
                      addHash((uint8_t)a);
                      addHash((int8_t)b);
                      addHash((uint16_t)c);
                      addHash((int16_t)d);
                      addHash((uint32_t)e);
                      addHash((int32_t)f);
                      addHash((uint64_t)g);
                      addHash((int64_t)h);
                      addHash((uint8_t)i);
                    });
  testHash(service, "after enumTestMax");
}
void scalarTestSmall(sp<ITypes> service)
{
  service->echoScalar(34, 104, 234, 334, 434, 534, 634, 734, 43,
                    [](uint8_t a, int8_t b, uint16_t c, int16_t d, uint32_t e, int32_t f, uint64_t g, int64_t h, char i) {
                      if (a != 34) cout << "a wrong" << endl;
                      if (b != 104) cout << "b wrong" << endl;
                      if (c != 234) cout << "c wrong" << endl;
                      if (d != 334) cout << "d wrong" << endl;
                      if (e != 434) cout << "e wrong" << endl;
                      if (f != 534) cout << "f wrong" << endl;
                      if (g != 634) cout << "g wrong" << endl;
                      if (h != 734) cout << "h wrong" << endl;
                      if (i != 43) cout << "i wrong" << endl;
                      addHash(a);
                      addHash(b);
                      addHash(c);
                      addHash(d);
                      addHash(e);
                      addHash(f);
                      addHash(g);
                      addHash(h);
                      addHash(i);
                    });
  testHash(service, "after scalarTestSmall");
}
void scalarTestMin(sp<ITypes> service)
{
  service->echoScalar(0, -128, 0, -32768, 0, -2147483648,
                      0, -9223372036854775807LL, 0,
                    [](uint8_t a, int8_t b, uint16_t c, int16_t d, uint32_t e, int32_t f, uint64_t g, int64_t h, char i) {
                      if (a != 0) cout << "a wrong" << endl;
                      if (b != -128) cout << "b wrong" << endl;
                      if (c != 0) cout << "c wrong" << endl;
                      if (d != -32768) cout << "d wrong" << endl;
                      if (e != 0) cout << "e wrong" << endl;
                      if (f != -2147483648) cout << "f wrong" << endl;
                      if (g != 0) cout << "g wrong" << endl;
                      if (h != -9223372036854775807LL) cout << "h wrong" << endl;
                      if (i != 0) cout << "i wrong" << endl;
                      addHash(a);
                      addHash(b);
                      addHash(c);
                      addHash(d);
                      addHash(e);
                      addHash(f);
                      addHash(g);
                      addHash(h);
                      addHash(i);
                    });
  testHash(service, "after scalarTestMin");
}
void scalarTestMax(sp<ITypes> service)
{
  service->echoScalar(255, 127, 65535, 32767, 0xffffffff, 0x7fffffff,
                      0xffff'ffff'ffffffffLL, 0x7fff'ffff'ffffffffLL, 255,
                    [](uint8_t a, int8_t b, uint16_t c, int16_t d, uint32_t e, int32_t f, uint64_t g, int64_t h, char i) {
                      if (a != 255) cout << "a wrong" << endl;
                      if (b != 127) cout << "b wrong" << endl;
                      if (c != 65535) cout << "c wrong" << endl;
                      if (d != 32767) cout << "d wrong" << endl;
                      if (e != 0xffffffff) cout << "e wrong" << endl;
                      if (f != 2147483647) cout << "f wrong" << endl;
                      if (g != 0xffff'ffff'ffffffffLL) cout << "g wrong" << endl;
                      if (h != 0x7fff'ffff'ffffffffLL) cout << "h wrong" << endl;
                      if (i != 255) cout << "i wrong" << endl;
                      addHash(a);
                      addHash(b);
                      addHash(c);
                      addHash(d);
                      addHash(e);
                      addHash(f);
                      addHash(g);
                      addHash(h);
                      addHash(i);
                    });
  testHash(service, "after scalarTestMax");
}

using disc = ::android::hardware::tests::ITypes::disc;

void nestedTest(sp<ITypes> service)
{
  char cs1[]{"HiWorld"};
  char cs2[]{"JiWorldFoo"};
  char cs3[]{"QiWorld"};
  char cs4[]{"FiWorld"};
  char cs5[]{"WiWorld"};
  ITypes::s0 v0;
  ITypes::s1s v1[2];
  v0.s1v2.count = 2;
  v0.s1v2.buffer = v1;
  v0.s1m1.str1.buffer = cs1;
  v0.s1m1.str1.length = -1;
  v1[0].str1.buffer = cs3;
  v1[0].str1.length = -1;
  v1[1].str1.buffer = cs4;
  v1[1].str1.length = 5;

  //  v0.s1m1.dm1 = disc::US;
  //  v0.s1m1.u1m1.s.buffer = cs1;
  //  v0.s1m1.u1m1.s.length = -1;
  //  v1[0].dm1 = disc::UF;
  /*  int32_t *data_ptr;
  int fd = -1;
  gen_ref(&fd, &data_ptr);
  *data_ptr = 0xbeefcafe;
  if (fd >= 0) {
    v1[0].u1m1.fd = fd;
  } else {
    cout << "Couldn't get fd in nestedTest" << endl;
    }*/
  //  v1[1].dm1 = disc::UI;
  //  v1[1].u1m1.i = 0xada;
  //  v0.s1m3.dm1 = disc::UC;
  //  v0.s1m3.u1m1.s2m.i = 0xabcdef;
  cout << "Client: About to call echoStruct" << endl;
  printf("C: &v0 %p v0.s1m1.str1.buf %p v0.s1v2.buf %p v0.s1v2.buf[0].str1.buf %p [1] %p\n",
         &v0, v0.s1m1.str1.buffer, v0.s1v2.buffer, v0.s1v2.buffer[0].str1.buffer, v0.s1v2.buffer[1].str1.buffer);
  service->echoStruct(v0, [](const /*android::hardware::tests::ITypes::s0 */ auto & s){
      cout << "Client: in callback, s is " << &s << endl;
      printf("C: s %p s.s1m1.str1.buf %p s.s1v2.buf %p s.s1v2.buf[0].str1.buf %p [1] %p\n",
         &s, s.s1m1.str1.buffer, s.s1v2.buffer, s.s1v2.buffer[0].str1.buffer, s.s1v2.buffer[1].str1.buffer);
      int i = s.s1v2.count;
      cout << "Client: vec count(2?): " << i << endl;
      printf("s1m1 len is %ld\n", s.s1m1.str1.length);
      printf("s1m1 str is %s\n", s.s1m1.str1.buffer);
      printf("s1v2[0] len is %ld\n", s.s1v2.buffer[0].str1.length);
      printf("s1v2[0] str is %s\n", s.s1v2.buffer[0].str1.buffer);
      printf("s1v2[1] len is %ld\n", s.s1v2.buffer[0].str1.length);
      printf("s1v2[1] bytes is %c%c%c%c%c\n", s.s1v2.buffer[0].str1.buffer[0],
             s.s1v2.buffer[0].str1.buffer[1],s.s1v2.buffer[0].str1.buffer[2],
             s.s1v2.buffer[0].str1.buffer[3],s.s1v2.buffer[0].str1.buffer[4]);
    });
}

int main(int /* argc */, char * argv []) {
  cout << "    Running tests" << endl;
  android::base::InitLogging(argv, android::base::StderrLogger);
  sp<ITypes> service;
  namespace client_tests = android::hardware::tests::client;

  hidl_version version = make_hidl_version(4,0);
  if (!client_tests::GetService(&service, version)) {
    cout << " * * Client couldn't find server * * " << endl;
    return 1;
  }

  ITypes::simple_t sts;
  sts.int1 = sts.int2 = 17;
  service->echoInteger(42, sts, [](int ret){
      if (ret != 42) cout << "Client: echoInteger returned " << ret << " instead of 42" << endl;});
    addHash(42);
    addHash(sts.int1);
    addHash(sts.int2);
  enumTestSmall(service);
  enumTestMin(service);
  enumTestMax(service);
  scalarTestSmall(service);
  scalarTestMin(service);
  scalarTestMax(service);
  nestedTest(service);
  service->quit();
  return 0;
}
