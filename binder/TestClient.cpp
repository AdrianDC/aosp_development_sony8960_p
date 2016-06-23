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
using android::hidl::getService;
using android::hidl::hidl_version;
using android::hidl::make_hidl_version;
using android::hidl::gen_ref;
using android::hidl::from_ref;

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

bool GetService(sp<ITestService>* service, hidl_version version) {
  cout << "Retrieving test service binder" << endl;
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

#define DATA_BUFFER_SIZE 32
int main(int /* argc */, char * argv []) {
  uint8_t data_buffer[DATA_BUFFER_SIZE];
  for (int i = 0; i < DATA_BUFFER_SIZE; i++) {
      data_buffer[i] = i;
  }
  android::base::InitLogging(argv, android::base::StderrLogger);
  sp<ITestService> service;
  namespace client_tests = android::hardware::tests::client;

  hidl_version version = make_hidl_version(4,0);
  if (!client_tests::GetService(&service, version)) return 1;

  ITestService::simple_t my_struct;
  my_struct.int1 = 1481;
  my_struct.int2 = 2592;
  my_struct.data.buffer = data_buffer;
  my_struct.data.count = DATA_BUFFER_SIZE;

  service->echoInteger(32, &my_struct, [](auto ret) {
    cout << "Got response from binder service: " << ret << endl;
    }
  );
  // Generate a ref<lots_of_data>
  ITestService::lots_of_data *data_ptr;
  int fd = -1;
  gen_ref(&fd, &data_ptr);
  cout << "gen_ref generated file descriptor " << fd << endl;
  // Now fill up the buffer with fun data
  for (size_t i = 0; i < sizeof(ITestService::lots_of_data); i++) {
     data_ptr->buffer[i] = i & 0xFF;
  }
  if (fd >= 0) {
    service->shareBufferWithRef(fd, [](auto ret) {
      cout << "Got response from binder service: " << ret << endl;
    }
    );
  } else {
    cerr << "Failed to generate a reference" << endl;
  }
  return 0;
}
