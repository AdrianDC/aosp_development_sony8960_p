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

  CppOptions *options = CppOptions::Parse(argc, argv);
  if (!options) {
    return 1;
  }

  android::hidl::IoDelegate io_delegate;
  Parser p{io_delegate, options->Verbose()};
  Thing::SetParser(&p);
  p.ParseFile(options->InputFileName());
  if (options->PrintStuff()) {
    p.Dump();
  }
  p.WriteDepFileIfNeeded(*options, io_delegate);
  for (auto & job : options->OutputJobs()) {
    android::hidl::CodeWriterPtr writer = android::hidl::GetFileWriter(job.output_file_name);
    printf("OutFileName %s\n", job.output_file_name.c_str());
    p.Write(job.type, std::move(writer));
    if (p.GetErrorCount()) {
      printf("Exit error count: %d writing %s to %s\n",
             p.GetErrorCount(),
             job.type.c_str(),
             job.output_file_name.c_str());
      return p.GetErrorCount();
    }
  }
  delete options;
  return 0;
}
