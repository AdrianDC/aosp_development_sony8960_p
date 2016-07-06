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

#ifndef HIDL_OPTIONS_H_
#define HIDL_OPTIONS_H_

#include <memory>
#include <string>
#include <vector>

#include <android-base/macros.h>
#include <gtest/gtest_prod.h>

namespace android {
namespace hidl {

class CppOptions final {
 public:
  enum e_out_type {IFOO, BNFOO, BPFOO, FOOSTUBS, FOOPROXY, VTS,
                   BINDER};

  ~CppOptions() = default;

  // Parses the command Line and returns a non-null pointer to an CppOptions
  // object on success.
  // Prints the usage statement on failure.
  static std::unique_ptr<CppOptions> Parse(int argc, const char* const* argv);

  std::string InputFileName() const { return input_file_name_; }
  std::string OutputFileName() const { return output_file_name_; }

  std::vector<std::string> ImportPaths() const { return import_paths_; }
  std::string DependencyFilePath() const { return dep_file_name_; }
  std::string OutputType() const { return section_; }
  bool PrintStuff() const { return print_stuff_; }
  bool Verbose() const { return verbose_; }

 private:
  CppOptions() = default;

  bool print_stuff_ = false;
  bool verbose_ = false;
  std::string input_file_name_;
  std::string section_;
  std::vector<std::string> import_paths_;
  std::string output_file_name_;
  std::string dep_file_name_;

  FRIEND_TEST(CppOptionsTests, ParsesCompileCpp);
  DISALLOW_COPY_AND_ASSIGN(CppOptions);
};

bool EndsWith(const std::string& str, const std::string& suffix);
bool ReplaceSuffix(const std::string& old_suffix,
                   const std::string& new_suffix,
                   std::string* str);

}  // namespace android
}  // namespace hidl

#endif // HIDL_OPTIONS_H_
