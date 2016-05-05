/*
 * Copyright (C) 2015, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HIDL_HIDL_H_
#define HIDL_HIDL_H_

#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "hidl_language.h"
#include "io_delegate.h"
#include "options.h"
//#include "type_namespace.h"

namespace android {
namespace hidl {

enum class HidlError {
  UNKOWN = std::numeric_limits<int32_t>::min(),
  BAD_PRE_PROCESSED_FILE,
  PARSE_ERROR,
  FOUND_PARCELABLE,
  BAD_PACKAGE,
  BAD_IMPORT,
  BAD_TYPE,
  BAD_METHOD_ID,
  GENERATION_ERROR,

  OK = 0,
};

int Compile_hidl_to_cpp(const CppOptions& options,
                        const IoDelegate& io_delegate);
int Compile_hidl_to_cpp(const CppOptions& options,
                        const IoDelegate& io_delegate);

namespace internals {

bool Parse_preprocessed_file(const IoDelegate& io_delegate,
                             const std::string& filename);

} // namespace internals

}  // namespace android
}  // namespace hidl

#endif  // HIDL_HIDL_H_
