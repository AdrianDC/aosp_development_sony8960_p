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

#include <memory>

#include "hidl.h"
#include "io_delegate.h"
#include "logging.h"
#include "options.h"

using android::hidl::CppOptions;

int main(int argc, char** argv) {
  android::base::InitLogging(argv);
  LOG(DEBUG) << "hidl starting";

  std::unique_ptr<CppOptions> options = CppOptions::Parse(argc, argv);
  if (!options) {
    return 1;
  }

  android::hidl::IoDelegate io_delegate;
  android::hidl::CodeWriterPtr writer = android::hidl::GetFileWriter(options->OutputFileName());
  Parser p{io_delegate, options->OutputType(), options->Verbose()};
  Thing::SetParser(&p);
  p.SetWriter(std::move(writer));
  p.ParseFile(options->InputFileName());
  if (options->PrintStuff()) {
    p.Dump();
  }
  p.Write();
  p.WriteDepFileIfNeeded(std::move(options), io_delegate);
  if (p.GetErrorCount()) {
    printf("Exit error count: %d\n", p.GetErrorCount());
  }
  return p.GetErrorCount();
  //  return android::hidl::Compile_hidl_to_cpp(*options, io_delegate);
}
