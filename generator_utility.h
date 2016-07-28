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

#ifndef GENERATOR_UTILITY_H_
#define GENERATOR_UTILITY_H_

#include "snippets.h"
#include <android-base/macros.h>
#include <android-base/strings.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <regex>

using std::string;
using std::vector;
using std::pair;

using Subs = vector<pair<string, string>>;

// Contains utility methods that used to generate the output files by hidl-gen.
class GeneratorUtility {
 public:
  GeneratorUtility() {};
  virtual ~GeneratorUtility() {};

  static string Snip(const string &section, const string &name,
                     const Subs &subs, bool print = false);

  static string Snip(const string &section, const string &name);

  static void PrintSubs(const string &msg, const Subs subs);
  static string upcase(const string in);
  static string make_inline(const string in);
  static string trim(const string in);

 private:

  DISALLOW_COPY_AND_ASSIGN(GeneratorUtility);
};

#endif  // GENERATOR_UTILITY_H_
