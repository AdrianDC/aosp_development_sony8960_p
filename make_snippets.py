#
# Copyright (C) 2015 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import re, sys
snips = {}
labels = []

def read_file(fname):
  global snips, labels
  f = open(fname)
  text = f.read()
  lines = text.split("\n")
  f.close()
  section = ""
  snips[section] = {}
  for line in lines:
    start = re.search(r"//\s*START\s+(\S+)", line)
    alls = re.search(r"//\s*ALL\s+(\S+)", line)
    if start or alls:
      name = (start and start.group(1)) or alls.group(1)
      if len(labels) > 0:
        snips[labels[-1][0]][labels[-1][1]] += name + "\\n"
      labels.append( (section, name) )
      #print "after append sec %s name %s, labels is %s"%(section, name, labels)
    skip = re.search(r"//\s*SKIP", line)
    if len(labels) > 0 and not skip:
      if labels[-1][1] not in snips[labels[-1][0]]:
        snips[labels[-1][0]][labels[-1][1]] = ""
      match_magic = re.compile(r"\s*//\s*(START|END|SECTION|ALL|SKIP).*?$")
      line_text = match_magic.sub("", line)
      if re.match(r"\s*$", line_text) and not re.match(r"\s*$", line):
        line_text = ""
      else:
        line_text = line_text + "\\n"
        line_text = line_text.replace("\"", "\\\"")
      snips[labels[-1][0]][labels[-1][1]] += line_text
    sec_match = re.search(r"//\s*SECTION\s+(\S+)", line)
    if sec_match:
      section = sec_match.group(1)
      snips[section] = {}
    end = re.search(r"//\s*END\s+(\S+)", line)
    alls = re.search(r"//\s*ALL\s+(\S+)", line)
    if end or alls:
      name = (end and end.group(1)) or alls.group(1)
      if labels[-1] != (section, name):
        print "Error, found END '[%s, %s]' but expected '%s'"%(section, name, labels[-1])
        sys.exit(1)

      labels.pop()
  if len(labels) > 0:
    print "Error, labels %s left over"%labels
    sys.exit(1)

def write(fname):
  global snips
  f = open(fname, "w")
  f.write("// AUTO GENERATED - python snippets.py outfile infile infile...\n")
  f.write("#include \"snippets.h\"\n")
  f.write("SnipMapMap snippets_cpp {\n")
  for section, snip_dict in snips.iteritems():
    f.write("  {\"%s\",{\n"%section)
    for name, snip in snip_dict.iteritems():
      f.write("    {std::string(\"%s\"),\"%s\"},\n"%
        (name, snip.replace('"','\"')))
    f.write("  }},")
  f.write("};\n")

for file in sys.argv[2:]:
  read_file(file)
write(sys.argv[1])
sys.exit(0)
