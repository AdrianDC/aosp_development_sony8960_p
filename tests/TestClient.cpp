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

#include <iostream>

#include <android-base/logging.h>
#include <hwbinder/IServiceManager.h>
#include <utils/String16.h>
#include <utils/StrongPointer.h>

#include "android/hardware/tests/ITestService.h"

// libutils:
using android::OK;
using android::sp;
using android::status_t;
using android::String16;

// libbinder:
using android::getService;

// generated
using android::hardware::tests::ITestService;

using std::cerr;
using std::cout;
using std::endl;

namespace android {
namespace hardware {
namespace tests {
namespace client {

const char kServiceName[] = "android.hardware.tests.ITestService";

bool GetService(sp<ITestService>* service) {
  cout << "Retrieving test service binder" << endl;
  status_t status = getService(String16(kServiceName), service);
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

/* Runs all the test cases in aidl_test_client_*.cpp files. */
int main(int /* argc */, char * argv []) {
  android::base::InitLogging(argv, android::base::StderrLogger);
  sp<ITestService> service;
  namespace client_tests = android::hardware::tests::client;


  if (!client_tests::GetService(&service)) return 1;

  int echo_value;
  service->echoInteger(32, &echo_value);
  cout << "Got response from binder service: " << echo_value << endl;

  return 0;
}
