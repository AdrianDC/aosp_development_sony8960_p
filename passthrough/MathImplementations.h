// Human-written file, declaring class(es) that implement BnPassthrough

#include "BnMath.h"
#include <string>

namespace android {
namespace hardware {

class MathImplementation : public BnMath {
  virtual status_t sayHi( );
  virtual status_t doCalc(int32_t i , std::function<void(int32_t sum, int32_t product)> callback = nullptr);

};  // class MathImplementation

}  // namespace hardware

}  // namespace android


  // This, suitably templatized, will move to the HIDL-provided header files when
  // the interface stabilizes.
void registerService(std::string name, ::android::hardware::IMath& service);
