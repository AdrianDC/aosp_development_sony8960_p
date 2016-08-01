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

#include "generator_utility.h"
#include "parser.h"

extern bool generator_verbose_mode;

// TODO use std::stringstream foo; foo << "string"; foo.str() instead of +...+
using std::cout;
using std::endl;

string GeneratorUtility::Snip(const string &section, const string &name,
                              const Subs &subs, bool print) {
  SnipMapMap::iterator smm_it = snippets_cpp.find(section);
  if (smm_it == snippets_cpp.end()) {
    std::cerr << "Warning: Section " << section << " not found in snippets"
              << endl;
    return "";
  }
  SnipMap snip_map(smm_it->second);

  SnipMap::iterator it = snip_map.find(name);
  if (it == snip_map.end()) {
    if (print)
      std::cout << "  Snip '" << name << "' not found in section '" << section
                << "'" << endl;
    return "";  // This is legal, e.g. many types don't need code
  }
  string snippet(it->second);
  // First, replace all the keys with uglified versions, in case the
  // key also appears in replacement text
  // TODO: Regex-escape the keys, or use a non-regex search/replace
  for (auto &sub : subs) {
    std::regex re(sub.first);
    snippet = std::regex_replace(snippet, re, sub.first + "####");
  }
  // Then, replace the uglified keys with the replacement text
  for (auto &sub : subs) {
    std::regex re(sub.first + "####");
    snippet = std::regex_replace(snippet, re, sub.second);
  }
  if (print)
    cout << "* * Final snippet for '" << name << "': '" << snippet << "' * *"
         << endl;
  if (generator_verbose_mode && snippet != "") {
    snippet = "// START " + name + "\n" + snippet + "\n// END " + name + "\n";
  }
  return snippet;
}

string GeneratorUtility::Snip(const string &section, const string &name) {
  Subs subs{};
  return Snip(section, name, subs);
}

string GeneratorUtility::upcase(const string in) {
  string out{in};
  for (auto &ch : out) ch = toupper(ch);
  return out;
}

string GeneratorUtility::make_inline(const string in) {
  string out{in};
  std::replace(out.begin(), out.end(), '\n', ' ');
  return out;
}

string GeneratorUtility::trim(const string in) {
  string out{in};
  out.erase(out.find_last_not_of("\n") + 1);
  return out;
}

void GeneratorUtility::PrintSubs(const string &msg, const Subs subs)
{
  cout << msg << ": \n";
  for (auto & sub : subs) {
    cout << "  '" << sub.first << "' -> '" << sub.second << "'" << endl;
  }
}
