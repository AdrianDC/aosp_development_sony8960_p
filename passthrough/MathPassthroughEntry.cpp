// Boilerplate code for the passthrough client to get a service
// proxy object. Not auto-generated, but probably cut-and-pasted.

#include "MathImplementations.h"
#include <string>

using android::hardware::MathImplementation;

ptrdiff_t findServiceSingleton(std::string name)
{
  (void)name;
  static MathImplementation serviceSingleton;
  // We don't need this anymore, since the handle we're returning is the object we're calling into
  //  registerService(name, serviceSingleton);
  return (ptrdiff_t) &serviceSingleton;
}
extern "C" {
struct {
  ptrdiff_t (*findService)(std::string name) = findServiceSingleton;
} hidlServiceFinderFunctionHolder; // This name is magic
}
