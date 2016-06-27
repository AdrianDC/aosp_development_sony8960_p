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

#include <asm-generic/mman.h>
#include <cutils/ashmem.h>
#include <gtest/gtest.h>
#include <cstdlib>
#include <sstream>
#include "common/MessageQueue.h"

static const int queue_size = 1024;
typedef uint64_t mq_position_t;

class MQTests : public ::testing::Test {
 protected:
  virtual void TearDown() {
    if (fmsgq) {
      delete fmsgq;
    }
  }

  virtual void SetUp() {
    size_t eventQueueTotal = 4096;
    int ashmemFd = ashmem_create_region("MessageQueue", eventQueueTotal);
    ashmem_set_prot_region(ashmemFd, PROT_READ | PROT_WRITE);
    ASSERT_TRUE(ashmemFd >= 0);
    native_handle_t* mq_handle = native_handle_create(1, 0);
    ASSERT_TRUE(mq_handle != nullptr);
    std::vector<android::hidl::GrantorDescriptor> Grantors(
        MINIMUM_GRANTOR_COUNT);
    /*
     * The native handle will contain the fds to be
     * mapped.
     */
    mq_handle->data[0] = ashmemFd;

    /*
     * Create Grantor Descriptors for read, write pointers and the data
     * buffer.
     */
    Grantors[android::hidl::READPTRPOS] = {0, 0, 0, sizeof(mq_position_t)};
    Grantors[android::hidl::WRITEPTRPOS] = {0, 0, sizeof(mq_position_t),
                                            sizeof(mq_position_t)};
    Grantors[android::hidl::DATAPTRPOS] = {0, 0, 2 * sizeof(mq_position_t),
                                           queue_size};
    android::hidl::MQDescriptor mydesc(Grantors, mq_handle, 0, sizeof(uint8_t));
    fmsgq = new android::hidl::MessageQueue<uint8_t>(mydesc);
    ASSERT_TRUE(fmsgq != nullptr);
    ASSERT_TRUE(fmsgq->isValid());
  }

  android::hidl::MessageQueue<uint8_t>* fmsgq;
};

/*
 * Verify that a few bytes of data can be successfully written and read.
 */
TEST_F(MQTests, SmallInputTest1) {
  const int data_len = 16;
  int write_count = -1;
  ASSERT_TRUE(data_len <= queue_size);
  uint8_t data[data_len];
  for (int i = 0; i < data_len; i++) {
    data[i] = i & 0xFF;
  }
  ASSERT_TRUE(fmsgq->write(data, data_len));
  uint8_t read_data[data_len] = {};
  ASSERT_TRUE(fmsgq->read(read_data, data_len));
  ASSERT_TRUE(memcmp(data, read_data, data_len) == 0);
}

/*
 * Verify that read() returns false when trying to read from an empty queue.
 */
TEST_F(MQTests, ReadWhenEmpty) {
  ASSERT_TRUE(fmsgq->availableToRead() == 0);
  const int data_len = 2;
  ASSERT_TRUE(data_len < queue_size);
  uint8_t read_data[data_len];
  ASSERT_FALSE(fmsgq->read(read_data, data_len));
}

/*
 * Write the queue when full. Verify that subsequent writes fail.
 * Verify that availableToWrite() returns 0 as expected.
 */

TEST_F(MQTests, WriteWhenFull) {
  ASSERT_TRUE(fmsgq->availableToRead() == 0);
  uint8_t* data = new uint8_t[queue_size];
  for (int i = 0; i < queue_size; i++) {
    data[i] = i & 0xFF;
  }
  ASSERT_TRUE(fmsgq->write(data, queue_size));
  ASSERT_TRUE(fmsgq->availableToWrite() == 0);
  ASSERT_FALSE(fmsgq->write(data, 1));

  uint8_t* read_data = new uint8_t[queue_size]();
  ASSERT_TRUE(fmsgq->read(read_data, queue_size));
  ASSERT_TRUE(memcmp(data, read_data, queue_size) == 0);

  delete[] data;
  delete[] read_data;
}

/*
 * Write a chunk of data equal to the queue size.
 * Verify that the write is successful and the subsequent read
 * returns the expected data.
 */
TEST_F(MQTests, LargeInputTest1) {
  uint8_t* data = new uint8_t[queue_size];
  for (int i = 0; i < queue_size; i++) {
    data[i] = i & 0xFF;
  }
  ASSERT_TRUE(fmsgq->write(data, queue_size));
  uint8_t* read_data = new uint8_t[queue_size]();
  ASSERT_TRUE(fmsgq->read(read_data, queue_size));
  ASSERT_TRUE(memcmp(data, read_data, queue_size) == 0);
  delete[] data;
  delete[] read_data;
}

/*
 * Attempt to write a chunk of data larger than the queue size.
 * Verify that it fails. Verify that a subsequent read fails and
 * the queue is still empty.
 */
TEST_F(MQTests, LargeInputTest2) {
  ASSERT_TRUE(fmsgq->availableToRead() == 0);
  const int data_len = 4096;
  ASSERT_TRUE(data_len > queue_size);
  uint8_t* data = new uint8_t[data_len];
  for (int i = 0; i < data_len; i++) {
    data[i] = i & 0xFF;
  }
  ASSERT_FALSE(fmsgq->write(data, data_len));
  uint8_t* read_data = new uint8_t[queue_size]();
  ASSERT_FALSE(fmsgq->read(read_data, queue_size));
  ASSERT_FALSE(memcmp(data, read_data, queue_size) == 0);
  ASSERT_TRUE(fmsgq->availableToRead() == 0);
  delete[] data;
  delete[] read_data;
}

/*
 * After the queue is full, try to write more data. Verify that
 * the attempt returns false. Verify that the attempt did not
 * affect the pre-existing data in the queue.
 */
TEST_F(MQTests, LargeInputTest3) {
  uint8_t* data = new uint8_t[queue_size];
  for (int i = 0; i < queue_size; i++) {
    data[i] = i & 0xFF;
  }
  ASSERT_TRUE(fmsgq->write(data, queue_size));
  ASSERT_FALSE(fmsgq->write(data, 1));
  uint8_t* read_data = new uint8_t[queue_size];
  ASSERT_TRUE(fmsgq->read(read_data, queue_size));
  ASSERT_TRUE(memcmp(read_data, data, queue_size) == 0);
  delete[] data;
  delete[] read_data;
}
/*
 * Verify that multiple reads one after the other return expected data.
 */
TEST_F(MQTests, MultipleRead) {
  const int chunkSize = 100;
  const int chunkNum = 5;
  const size_t data_len = chunkSize * chunkNum;
  ASSERT_TRUE(data_len <= queue_size);
  uint8_t data[data_len];
  for (unsigned int i = 0; i < data_len; i++) {
    data[i] = i & 0xFF;
  }
  ASSERT_TRUE(fmsgq->write(data, data_len));
  uint8_t read_data[data_len] = {};
  for (unsigned int i = 0; i < chunkNum; i++) {
    ASSERT_TRUE(fmsgq->read(read_data + i * chunkSize, chunkSize));
  }
  ASSERT_TRUE(memcmp(read_data, data, data_len) == 0);
}

/*
 * Verify that multiple writes one after the other happens correctly.
 */
TEST_F(MQTests, MultipleWrite) {
  const int chunkSize = 100;
  const int chunkNum = 5;
  const size_t data_len = chunkSize * chunkNum;
  ASSERT_TRUE(data_len <= queue_size);
  uint8_t data[data_len];
  for (unsigned int i = 0; i < data_len; i++) {
    data[i] = i & 0xFF;
  }
  for (unsigned int i = 0; i < chunkNum; i++) {
    ASSERT_TRUE(fmsgq->write(data + i * chunkSize, chunkSize));
  }
  uint8_t read_data[data_len] = {};
  ASSERT_TRUE(fmsgq->read(read_data, data_len));
  ASSERT_TRUE(memcmp(read_data, data, data_len) == 0);
}

/*
 * Write enough messages into the FMQ to fill half of it
 * and read back the same.
 * Write queue_size messages into the queue. This will cause a
 * wrap around. Read and verify the data.
 */
TEST_F(MQTests, ReadWriteWrapAround) {
  size_t numMessages = queue_size / 2;
  uint8_t* data = new uint8_t[queue_size];
  uint8_t* read_data = new uint8_t[queue_size]();
  for (int i = 0; i < queue_size; i++) {
    data[i] = i & 0xFF;
  }
  ASSERT_TRUE(fmsgq->write(data, numMessages));
  ASSERT_TRUE(fmsgq->read(read_data, numMessages));
  ASSERT_TRUE(fmsgq->write(data, queue_size));
  ASSERT_TRUE(fmsgq->read(read_data, queue_size));
  ASSERT_TRUE(memcmp(read_data, data, queue_size) == 0);
  delete[] data;
  delete[] read_data;
}
