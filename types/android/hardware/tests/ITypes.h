// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/ITemplate.h
#ifndef HIDL_GENERATED_android_hardware_tests_I_Types_H_
#define HIDL_GENERATED_android_hardware_tests_I_Types_H_

#include <hwbinder/IBinder.h>
#include <hwbinder/IInterface.h>
#include <hwbinder/Status.h>
#include <cstdint>
#include <utils/StrongPointer.h>

namespace android {
namespace hardware {
namespace tests {


template<typename T>
using hidl_ref = int;

class ITypes : public ::android::hidl::IInterface {
public:
template<typename T>
struct hidl_vec {
  T *buffer;
  size_t count;
};
struct hidl_string {
  char *buffer;
  ptrdiff_t length;
};

DECLARE_META_INTERFACE(Types);
typedef struct {
uint32_t int1;
uint32_t int2;

} simple_t;

  enum class eu8 : uint8_t {  U8F = 34,  max = 255,  min = 0 };
  const char* GetNameOf(eu8 f) {
    static const char* names[] = {  "U8F" ,   "max" ,   "min"  };
    //do errror checking
    return names[int(f)];
}
  //  size_t enum_limits<foo>::max() { return 1; }
  enum class es8 : int8_t {  S8F = 104,  max = 127,  min = -128 };
  const char* GetNameOf(es8 f) {
    static const char* names[] = {  "S8F" ,   "max" ,   "min"  };
    //do errror checking
    return names[int(f)];
}
  //  size_t enum_limits<foo>::max() { return 1; }
  enum class eu16 : uint16_t {  U16F = 234,  max = 65535,  min = 0 };
  const char* GetNameOf(eu16 f) {
    static const char* names[] = {  "U16F" ,   "max" ,   "min"  };
    //do errror checking
    return names[int(f)];
}
  //  size_t enum_limits<foo>::max() { return 1; }
  enum class es16 : int16_t {  S16F = 334,  max = 32767,  min = -32768 };
  const char* GetNameOf(es16 f) {
    static const char* names[] = {  "S16F" ,   "max" ,   "min"  };
    //do errror checking
    return names[int(f)];
}
  //  size_t enum_limits<foo>::max() { return 1; }
  enum class eu32 : uint32_t {  U32F = 434,  max = 0xffff'f'fff,  min = 0 };
  const char* GetNameOf(eu32 f) {
    static const char* names[] = {  "U32F" ,   "max" ,   "min"  };
    //do errror checking
    return names[int(f)];
}
  //  size_t enum_limits<foo>::max() { return 1; }
  enum class es32 : int32_t {  S32F = 534,  max = 0x7fff'f'fff,  min = -2147483648 };
  const char* GetNameOf(es32 f) {
    static const char* names[] = {  "S32F" ,   "max" ,   "min"  };
    //do errror checking
    return names[int(f)];
}
  //  size_t enum_limits<foo>::max() { return 1; }
  enum class eu64 : uint64_t {  U64F = 634,  max = 0xffff'ffff'ffff'f'fffuLL,  min = 0 };
  const char* GetNameOf(eu64 f) {
    static const char* names[] = {  "U64F" ,   "max" ,   "min"  };
    //do errror checking
    return names[int(f)];
}
  //  size_t enum_limits<foo>::max() { return 1; }
  enum class es64 : int64_t {  S64F = 734,  max = 9223372036854775807LL,  min = -9223372036854775807LL };
  const char* GetNameOf(es64 f) {
    static const char* names[] = {  "S64F" ,   "max" ,   "min"  };
    //do errror checking
    return names[int(f)];
}
  //  size_t enum_limits<foo>::max() { return 1; }
  enum class ec : char {  CF = 43,  min = 0,  max = 255 };
  const char* GetNameOf(ec f) {
    static const char* names[] = {  "CF" ,   "min" ,   "max"  };
    //do errror checking
    return names[int(f)];
}
  //  size_t enum_limits<foo>::max() { return 1; }
typedef struct {
uint8_t buffer[4096] ;

} lots_of_data;

  enum class disc : uint32_t {  US,  UF,  UI,  UC };
  const char* GetNameOf(disc f) {
    static const char* names[] = {  "US" ,   "UF" ,   "UI" ,   "UC"  };
    //do errror checking
    return names[int(f)];
}
  //  size_t enum_limits<foo>::max() { return 1; }
typedef struct {
int32_t i;

} s2;

typedef union {
hidl_string s;
hidl_ref<uint8_t> fd;
int64_t i;
s2 s2m;

} u1;

typedef struct {
hidl_string str1;

} s1s;

typedef struct {
disc dm1;
u1 u1m1;

} s1u;

typedef struct {
s1s s1m1;
hidl_vec<s1s> s1v2;

} s0;

typedef struct {
uint32_t fdIndex;
uint32_t offset;
uint32_t extent;

} shm_t;

typedef struct {
uint32_t flags;
shm_t shm;

} GrantorDescriptor;

typedef struct {
hidl_vec<GrantorDescriptor> grantors;
native_handle mq_handles;
uint32_t quantum;
uint32_t nQuanta;
uint32_t flags;

} MQDescriptor;


  using echoInteger_cb = std::function<void(int32_t ret)>;
  using echoEnum_cb = std::function<void(eu8 a, es8 b, eu16 c, es16 d, eu32 e, es32 f, eu64 g, es64 h, ec i)>;
  using echoScalar_cb = std::function<void(uint8_t a, int8_t b, uint16_t c, int16_t d, uint32_t e, int32_t f, uint64_t g, int64_t h, char i)>;
  using echoStruct_cb = std::function<void(const ITypes::s0 &s )>;
  using shareBufferWithRef_cb = std::function<void(int32_t ret)>;
  using getHash_cb = std::function<void(uint64_t hash)>;
  using quit_cb = std::function<void()>;


  virtual ::android::hidl::binder::Status echoInteger(int32_t echo_me, const ITypes::simple_t &my_struct , echoInteger_cb _cb = nullptr ) = 0;
  virtual ::android::hidl::binder::Status echoEnum(eu8 a, es8 b, eu16 c, es16 d, eu32 e, es32 f, eu64 g, es64 h, ec i, echoEnum_cb _cb = nullptr ) = 0;
  virtual ::android::hidl::binder::Status echoScalar(uint8_t a, int8_t b, uint16_t c, int16_t d, uint32_t e, int32_t f, uint64_t g, int64_t h, char i, echoScalar_cb _cb = nullptr ) = 0;
  virtual ::android::hidl::binder::Status echoStruct(const ITypes::s0 &s , echoStruct_cb _cb = nullptr ) = 0;
  virtual ::android::hidl::binder::Status shareBufferWithRef(hidl_ref<lots_of_data> buffer, shareBufferWithRef_cb _cb = nullptr ) = 0;
  virtual ::android::hidl::binder::Status getHash(getHash_cb _cb = nullptr ) = 0;
  virtual ::android::hidl::binder::Status quit() = 0;

enum Call {
  ECHOINTEGER = ::android::hidl::IBinder::FIRST_CALL_TRANSACTION + 0,

  ECHOENUM,   ECHOSCALAR,   ECHOSTRUCT,   SHAREBUFFERWITHREF,   GETHASH,   QUIT, 
};
};  // class ITypes

}  // namespace tests
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_android_hardware_tests_I_Types_H_
