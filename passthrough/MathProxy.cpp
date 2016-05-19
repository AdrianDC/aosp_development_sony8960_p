#include "PpMath.h"
#include "MathClient.h"
#include <cstring>
#include <dlfcn.h>

namespace android {
namespace hardware {

//IMPLEMENT_META_INTERFACE(Math, "android.hardware.IMath");
IMath::IMath(){}
IMath::~IMath(){}

struct hidl_functions {
  status_t (*sayHi)(ptrdiff_t g_connection);
  status_t (*doCalc)(ptrdiff_t g_connection, int32_t i,
                     std::function<void(int32_t, int32_t)> cb);
};

hidl_functions *g_funs;
ptrdiff_t g_connection;

status_t IMath::doCalc(int32_t i, std::function<void(int32_t, int32_t)> cb)
{
  g_funs->doCalc(g_connection, i, cb);
  return 0;
}

status_t IMath::sayHi(void)
{
  g_funs->sayHi(g_connection);
  return 0;
}

struct serverInitFunctionHolder {
  ptrdiff_t (*findService)(std::string name);
};

template<typename T>
status_t GetSymbol(void *handle, std::string name, T* &pAddress)
{
  pAddress = (T *)dlsym(handle, name.c_str());
  if (pAddress == NULL) {
    printf("load: couldn't find symbol %s", name.c_str());
    return -EINVAL;
  }
  return 0;
}

/**
 * Load the file defined by the variant and if successful
 * return the dlopen handle and the hmi.
 * @return 0 = success, !0 = failure.
 */
status_t GetPassthroughService(std::string name, IMath *&bpObject)
{
  static PpMath theBpObject; // singleton, thread-safe
  bpObject = &theBpObject;

  int status = -EINVAL;
  void *handle = NULL;

  /*
   * load the symbols resolving undefined symbols before
   * dlopen returns. Since RTLD_GLOBAL is not or'd in with
   * RTLD_NOW the external symbols will not be global
   */
  handle = dlopen(name.c_str(), RTLD_NOW);
  const char *serviceFinderHolderName = "hidlServiceFinderFunctionHolder";
  const char *hidlFunctionHolderName = "hidlFunctionTable";
  if (handle == NULL) {
    char const *err_str = dlerror();
    printf("load: module=%s\n%s\n", name.c_str(), err_str?err_str:"unknown");
    status = -EINVAL;
    goto done;
  }

  serverInitFunctionHolder *initHolder;

  if ((status = GetSymbol(handle, serviceFinderHolderName, initHolder)) ||
      (status = GetSymbol(handle, hidlFunctionHolderName, g_funs))) {
    goto done;
  }
  g_connection = (initHolder->findService)("GiveMeService!");

  if (!g_connection) {
    printf("Couldn't get a connection\n");
    status = -EINVAL;
  }

  /* success */
  status = 0;

done:
  if (status != 0) {
    if (handle != NULL) {
      dlclose(handle);
      handle = NULL;
    }
  } else {
    printf("loaded HAL path=%s handle=%p\n", name.c_str(), handle);
  }

  return status;
}
} //namespace hardware
} // namespace android
