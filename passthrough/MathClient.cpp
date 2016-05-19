#include "MathClient.h"
#include <errno.h>
#include <stdio.h>
#include <utils/RefBase.h>
#include <hwbinder/BinderService.h>
#include <functional>
#include "time.h"

/** Base path of the hal modules */
#define HAL_LIBRARY_PATH "./passthrough-hal.so"

namespace android {
namespace hardware {
void use2(int sum, int prod)
{
  printf("sum %d, prod %d\n", sum, prod);
}

status_t localCalc(
    int i, std::function<void(int j, int k)> callback)
{
  printf("localCalc: passed i %d\n", i);

  callback(i+i, i*i);
  return 0;
}

int go()
{
  IMath *service;
  //  hardware::version version = hardware::make_version(4,1);
  if (GetPassthroughService(HAL_LIBRARY_PATH /*, version*/, service) == 0) {
    service->sayHi();
    std::function<void(int,int)> cbk=[](int i, int j){ use2(i, j);};
    localCalc(11, cbk);
    fflush(stdout);
    service->doCalc(23,cbk);
  }
  return 0;
}
} // hardware
} // android
int main(int /*argc*/, char** /*argv*/)
{printf("Starting main\n");

  ::android::hardware::go();
}
