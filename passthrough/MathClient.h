#include "IMath.h"
#include <string>

namespace android {
namespace hardware {
status_t GetPassthroughService(std::string name, IMath* &iObject);
}
}
