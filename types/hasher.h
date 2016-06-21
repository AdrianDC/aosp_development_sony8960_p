#ifndef ANDROID_SYSTEM_TOOLS_HIDL_HASHER
#define ANDROID_SYSTEM_TOOLS_HIDL_HASHER
#include <cstdint>

uint64_t getHash();
void addHash(uint64_t n);
void addHash(int64_t n);
void addHash(uint32_t n);
void addHash(int32_t n);
void addHash(uint16_t n);
void addHash(int16_t n);
void addHash(uint8_t n);
void addHash(int8_t n);
#endif // ANDROID_SYSTEM_TOOLS_HIDL_HASHER
