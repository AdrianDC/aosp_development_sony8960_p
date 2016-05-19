//#include <dlfcn.h>
//#include <errno.h>
//#include <stdio.h>
//#include <iostream>
//#include <functional>
//#include "hidl_client.h"

#include "MathImplementations.h"
#include "BnMath.h"
#include "BpMath.h"

namespace android {
namespace hardware {

status_t MathImplementation::sayHi()
{
  printf("HiWorld\n");
  return 0;
}

status_t MathImplementation::doCalc(
    int i, std::function<void(int j, int k)> callback)
{
  printf("doCalc: passed i %d\n", i);

  callback(i+i, i*i);
  return 0;
}

} // namespace hardware
} // namespace android
