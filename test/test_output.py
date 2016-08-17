#!/usr/bin/env python
#
# Copyright 2016 - The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import filecmp
import logging
import os
import shutil
import subprocess
import sys
import unittest

class Tester(unittest.TestCase):
    """Integration test runner for hidl-gen.
    Runs hidl-gen on a bunch of packages/files and compares the out results with
    canonical ones. Exit code is 0 iff all tests pass.

    Usage:
        python test_output.py path_to_hidl_gen canonical_dir output_dir (interface_root)
    If interface_root is not provided, the default package "andorid.hardwre" and
    the path root defined in $TOP will be used.

    Attributes:
        _hidl_gen_path: the path to run hidl-gen.
        _canonical_dir: root directory contains canonical files for comparison.
        _output_dir: root directory that stores all output files.
        _interface_root: package:path root for the interface.
                         e.g., android.hardware:hardware/interfaces
        _errors: number of errors generates during the test.
    """
    def __init__(self, testName, hidl_gen_path, canonical_dir, output_dir, interface_root=''):
        super(Tester, self).__init__(testName)
        self._hidl_gen_path = hidl_gen_path
        self._canonical_dir = canonical_dir
        self._output_dir = output_dir
        self._interface_root = interface_root
        self._errors = 0

    def setUp(self):
        """Removes output dir to prevent interference from previous runs."""
        self.RemoveOutputDir()

    def tearDown(self):
        """If successful, removes the output dir for clean-up."""
        if self._errors == 0:
            self.RemoveOutputDir()

    def testAll(self):
        """Run all tests. """
        self.TestVtsOutput()
        # Uncomment once we want to test cpp output.
        self.TestCppOutput()

    def TestVtsOutput(self):
        """Runs hidl-gen to generate vts file and verify the output."""
        self.RunTest("vts", "android.hardware.nfc@1.0",
                     "android/hardware/nfc/1.0")
        self.assertEqual(self._errors, 0)

    def TestCppOutput(self):
        """Runs hidl-gen to generate c++ file and verify the output."""
        # self.RunTest("c++", "android.hardware.nfc@1.0",
        #              "android/hardware/nfc/1.0")
        self.RunTest("c++", "android.hardware.tests.expression@1.0",
                     "android/hardware/tests/expression/1.0")
        self.assertEqual(self._errors, 0)

    def RunTest(self, language, fqname, package_path):
        """Run hidl-gen with given language and fqname, and compare the results.

        Args:
            language: the target language for the output code. e.g. c++, vts.
            fqname: fully qualified name of the input files.
                For single file input, follow the format:
                package@version::fileName.
                For directory input, follow the format: package@version.
            example: android.hardware.nfc@1.0::INfc.hal
                package_path: path for the package in fqname.
                example: android/hardware/nfc/1.0
        """
        if self._interface_root != '':
            hidl_gen_cmd = [self._hidl_gen_path, "-o", self._output_dir, "-L",
                            language, "-r", self._interface_root, fqname]
        else:
            hidl_gen_cmd = [self._hidl_gen_path, "-o", self._output_dir, "-L",
                            language, fqname]
        self.RunCommand(hidl_gen_cmd)

        output_dir = os.path.join(self._output_dir, package_path)
        canonical_dir = os.path.join(self._canonical_dir, package_path)
        self.CompareOutput(output_dir, canonical_dir)

    def RunCommand(self, command):
        """Runs a unix command and stashes the result."""
        proc = subprocess.Popen(command,
                                stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE)
        (stdout, stderr) = proc.communicate()
        if proc.returncode != 0:
            self.Error(("Fail to execute command: %s" % command +
                        ("  (stdout: %s\n  stderr: %s\n)" % (stdout, stderr))))

    def CompareOutput(self, output_dir, canonical_dir):
        """Compare the files under output_dir with those under canonical_dir.

        Args:
            output_dir: directory that stores output files.
            canonical_dir: directory contains canonical files for comparison.
        """
        for file_name in os.listdir(output_dir):
            logging.info("comparing file: %s" % file)
            self.CompareFile(file_name, output_dir, canonical_dir)

    def CompareFile(self, file_name, output_dir, canonical_dir):
        """Compares a given file and the corresponding one under canonical_dir.

        Args:
            file_name: name of file for comparison.
            output_dir: directory that stores output files.
            canonical_dir: directory contains canonical files for comparison.
        """
        canonical_file = os.path.join(canonical_dir, file_name)
        output_file = os.path.join(output_dir, file_name)
        if not os.path.isfile(canonical_file):
            self.Error("Generated unexpected file: %s" % output_file)
        else:
            if not filecmp.cmp(output_file, canonical_file):
                self.Error("output file: %s does not match the canonical_file: "
                           "%s" % (output_file, canonical_file))

    def Error(self, string):
        """Prints an error message and increments error count."""
        logging.error(string)
        self._errors += 1

    def RemoveOutputDir(self):
        """Remove the output_dir if it exists."""
        if os.path.exists(self._output_dir):
            shutil.rmtree(self._output_dir)

if __name__ == "__main__":
    if len(sys.argv) != 5 and len(sys.argv) !=4:
        print "Usage: python test_output.py hidl_gen_path canonical_dir " \
              "output_dir (interface_root)"
        sys.exit(1)
    suite = unittest.TestSuite()
    if len(sys.argv) == 5:
        suite.addTest(Tester('testAll', sys.argv[1],sys.argv[2], sys.argv[3], sys.argv[4]))
    else:
        suite.addTest(Tester('testAll', sys.argv[1],sys.argv[2], sys.argv[3]))
    result = unittest.TextTestRunner(verbosity=2).run(suite)
    if not result.wasSuccessful():
      sys.exit(-1)
