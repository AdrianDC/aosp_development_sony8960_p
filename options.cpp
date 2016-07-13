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

#include "options.h"

#include <cstring>
#include <iostream>
#include <stdio.h>

#include "logging.h"
#include "os.h"

#include "snippets.h"

using std::cerr;
using std::endl;
using std::string;
using std::unique_ptr;
using std::vector;

namespace android {
namespace hidl
{
namespace {

CppOptions *cpp_usage() {
  cerr << "usage: hidl-gen OPTIONS TYPE INPUT_FILE OUTPUT_FILE" << endl
       << "usage: hidl-gen OPTIONS all_cpps INPUT_FILE HEADER_DIR SRC_DIR" << endl
       << endl
       << "OPTIONS:" << endl
      //       << "   -I<DIR>   search path for import statements" << endl
       << "   -d<FILE>  Generate dependency file" << endl
       << "   -p        print info from input file (for debugging)" << endl
       << "   -v        verbose: output files include all snippet names" << endl
       << endl
       << "TYPE:" << endl
       << "   all_cpp     Generate FooAll.cpp (combined proxy/stub)" << endl
       << "   all_cpps    Generate FooAll.cpp, *.h" << endl
       << "   binder      Generate Binder .h file" << endl
       << "   bn_h        Generate BnFoo.h" << endl
       << "   bp_h        Generate BpFoo.h" << endl
       << "   i_h         Generate IFoo.h" << endl
       << "   json        Generate Foo.json" << endl
       << "   proxy_cpp   Generate FooProxy.cpp" << endl
       << "   stubs_cpp   Generate FooStubs.cpp" << endl
       << "   vts         Generate Foo.vts" << endl
       << endl
       << "INPUT_FILE:" << endl
       << "   a hidl interface file" << endl
       << "OUTPUT_FILE:" << endl
       << "   path to Write generated file" << endl;
  return nullptr;
}

}  // namespace

CppOptions *CppOptions::Parse(int argc, const char* const* argv) {
  CppOptions *options(new CppOptions());
  int i = 1;

  // Parse flags, all of which start with '-'
  for ( ; i < argc; ++i) {
    const size_t len = strlen(argv[i]);
    const char *s = argv[i];
    if (s[0] != '-') {
      break;  // On to the positional arguments.
    }
    if (len < 2) {
      cerr << "Invalid argument '" << s << "'." << endl;
      return cpp_usage();
    }
    const string the_rest = s + 2;
    /*    if (s[1] == 'I') {
      options->import_paths_.push_back(the_rest);
      } else*/
    if (s[1] == 'd') {
      options->dep_file_name_ = the_rest;
    } else if (s[1] == 'p') {
      options->print_stuff_ = true;
    } else if (s[1] == 'v') {
      options->verbose_ = true;
    } else {
      cerr << "Invalid argument '" << s << "'." << endl;
      return cpp_usage();
    }
  }

  // There are exactly three positional arguments.
  const int remaining_args = argc - i;
  if (remaining_args == 4 && !strcmp(argv[i], "all_cpps")) {
    string type = argv[i++];
    options->input_file_name_ = argv[i++];
    string header_directory = argv[i++];
    string source_file = argv[i++];
    int slash_pos = options->input_file_name_.find_last_of('/');
    string package_name = options->input_file_name_.substr(
        slash_pos + 2,
        options->input_file_name_.length() - slash_pos - 7);
    options->outputs_.push_back(Job{"bn_h", header_directory+"/Bn"+package_name+".h"});
    options->outputs_.push_back(Job{"bp_h", header_directory+"/Bp"+package_name+".h"});
    options->outputs_.push_back(Job{"i_h", header_directory+"/I"+package_name+".h"});
    options->outputs_.push_back(Job{"all_cpp", source_file});
    printf("options: outputs size %ld\n", options->outputs_.size());
    for (auto & job : options->OutputJobs()) {
      printf("  options: job: %s, %s\n", job.type.c_str(), job.output_file_name.c_str());
    }
  } else if (remaining_args == 3) {
    Job job;
    job.type = argv[i++];
    options->input_file_name_ = argv[i++];
    job.output_file_name = argv[i++];
    options->outputs_.push_back(job);
    SnipMapMap::iterator smm_it = snippets_cpp.find(job.type);
    if (smm_it == snippets_cpp.end()) {
      std::cout << "File type " << job.type << " not found." << endl;
      return cpp_usage();
    }
  } else {
    cerr << "Expected 3 positional arguments but got " << remaining_args << "." << endl;
    return cpp_usage();
  }

  if (!EndsWith(options->input_file_name_, ".hidl")) {
    cerr << "Expected .hidl file for input but got " << options->input_file_name_ << endl;
    return cpp_usage();
  }

  return options;
}

bool EndsWith(const string& str, const string& suffix) {
  if (str.length() < suffix.length()) {
    return false;
  }
  return std::equal(str.crbegin(), str.crbegin() + suffix.length(),
                    suffix.crbegin());
}

bool ReplaceSuffix(const string& old_suffix,
                   const string& new_suffix,
                   string* str) {
  if (!EndsWith(*str, old_suffix)) return false;
  str->replace(str->length() - old_suffix.length(),
               old_suffix.length(),
               new_suffix);
  return true;
}



}  // namespace android
}  // namespace hidl
