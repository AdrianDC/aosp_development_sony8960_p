// AUTO_GENERATED FILE - DO NOT EDIT
// see system/tools/hidl/templates/BpTemplate.h
#ifndef HIDL_GENERATED_android_hardware_tests_BP_Types_H_
#define HIDL_GENERATED_android_hardware_tests_BP_Types_H_

#include <hwbinder/IBinder.h>
#include <hwbinder/IInterface.h>
#include <utils/Errors.h>
#include <android/hardware/tests/ITypes.h>

namespace android {
namespace hardware {
namespace tests {


class BpTypes : public ::android::hidl::BpInterface<ITypes> {
public:
explicit BpTypes(const ::android::sp<::android::hidl::IBinder>& _aidl_impl);
virtual ~BpTypes() = default;
::android::hidl::binder::Status echoInteger(int32_t echo_me, const ITypes::simple_t *my_struct , ITypes::echoInteger_cb _cb ) override;
::android::hidl::binder::Status echoEnum(eu8 a, es8 b, eu16 c, es16 d, eu32 e, es32 f, eu64 g, es64 h, ec i, ITypes::echoEnum_cb _cb ) override;
::android::hidl::binder::Status echoScalar(uint8_t a, int8_t b, uint16_t c, int16_t d, uint32_t e, int32_t f, uint64_t g, int64_t h, char i, ITypes::echoScalar_cb _cb ) override;
::android::hidl::binder::Status echoStruct(const ITypes::s0 *s , ITypes::echoStruct_cb _cb ) override;
::android::hidl::binder::Status shareBufferWithRef(hidl_ref<lots_of_data> buffer, ITypes::shareBufferWithRef_cb _cb ) override;
::android::hidl::binder::Status getHash(ITypes::getHash_cb _cb ) override;
::android::hidl::binder::Status quit() override;

};  // class BpTypes

}  // namespace tests
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_android_hardware_tests_BP_Types_H_
