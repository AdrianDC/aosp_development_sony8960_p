""" Integration test runner for hidl-cpp

Runs hidl-cpp on a bunch of files and tests the result.
Exit code is 0 iff all tests pass.

Usage (intended to be called from Android.mk):

testAll.py path_to_hidl_cpp output_dir test_dir

The output files will all go in output_dir
The input files come from test_dir
Some output files are compared with files of the same
 name in test_dir/canonical

All tests are called from Tester::RunTests(). Available
 checks include the value of the return code,
 any text printed, and whether an output and canonical file
 are equal.
"""

import os
import subprocess
import sys


class Tester(object):
  """ The entirety of the program """

  def __init__(self, hidl_cpp_path, test_dir, output_dir):
    """ Stashes the paths the program will need"""
    self.hidl_cpp_path = hidl_cpp_path
    self.output_dir = output_dir + "/"
    self.test_dir = test_dir + "/"
    self.errors = 0  # Number of failed tests
    # Many of these are for convenience in writing small
    # function calls in the RunTests() function.
    self.last_stdout = ""  # Return string from last command
    self.last_stderr = ""  # Return string from last command
    self.last_return = 0  # Exit code from last command
    self.last_out_path = ""  # Last output file generated
    self.last_out_text = ""  # Last output file's text
    self.last_hidl_name = ""  # Last .hidl file processed
    self.last_out_type = ""  # last file-type parameter

  def RunTests(self):
    """ Runs all the tests """
    self.ProcessFile("IBadType.hidl", "foo", "n")
    self.Test(self.last_return != 0)
    self.Test(has(self.last_stdout, "int48_t"))
    self.ProcessFile("ITest.hidl", "foo", "n")
    self.Test(self.last_return == 0)
    self.TestOutput("vts_gps.hidl", "vts_gps.vts", "v")
    self.TestOutput("vts_lights.hidl", "vts_lights.vts", "v")
    self.TestOutput("ITestService.hidl", "ITestService.h", "i")
    self.TestOutput("ITestService.hidl", "BnTestService.h", "n")
    self.TestOutput("ITestService.hidl", "BpTestService.h", "p")
    self.TestOutput("ITestService.hidl", "TestServiceProxy.cpp", "x")
    self.TestOutput("ITestService.hidl", "TestServiceStubs.cpp", "s")
#    self.TestOutput("IanBinder.hidl", "IanBinder.h", "b")

  def RunCommand(self, command):
    """ Runs a unix command and stashes the result """
#    print command
    proc = subprocess.Popen(command,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE)
    self.last_stdout, self.last_stderr = proc.communicate()
    self.last_return = proc.returncode
#    print "Ret from RunC is %d" % self.last_return

  def TestOutput(self, hidl_name, out_name, out_type):
    self.ProcessFile(hidl_name, out_name, out_type)
    if self.last_return != 0:
      self.Error("hidl-cpp error on file %s (type %s): %s" % (
          hidl_name, out_type, self.last_stdout + self.last_stderr))
      return
    canonical_path = self.test_dir + "canonical/" + out_name
    with open(canonical_path) as canonical:
      good_text = canonical.read()
    if good_text != self.last_out_text:
      self.Error(("Output file %s (%s) is wrong for hidl file %s\n" +
                  "  meld %s %s") % (out_name, out_type, hidl_name,
                                   canonical_path,
                                   self.last_out_path))

  def Error(self, string):
    """ Prints an error message and increments error count """
    print string
    self.errors += 1


  def ProcessFile(self, hidl_name, out_name, out_type):
    self.last_hidl_name = hidl_name
    self.last_out_type = out_type
    hidl_path = self.test_dir + hidl_name
    self.last_out_path = self.output_dir + out_name
    self.RunCommand([self.hidl_cpp_path, "-O%s" % out_type,
                     hidl_path, self.last_out_path])
    if self.last_return == 0:
      with open(self.last_out_path) as out_file:
        self.last_out_text = out_file.read()
    else:
      self.last_out_text = " * * INVALID * * "

  def Test(self, bool, text=""):
    if not bool:
      self.Error(("File %s (type %s) error: %s\n" % (
          self.last_hidl_name, self.last_out_type, text)) +
                 ("  (stdout: %s\n  stderr: %s\n)" % (
                     self.last_stdout, self.last_stderr)))


def has(str, substr):
  """ Syntactic sugar: true iff str contains substr"""
  return str.find(substr) != -1


if __name__ == "__main__":
  if len(sys.argv) < 4:
    print "Usage: python testAll.py hidl-cpp-path test-dir out-dir"
    sys.exit(1)

  tester = Tester(sys.argv[1], sys.argv[2], sys.argv[3])
  tester.RunTests()
  if tester.errors:
    print "hidl_cpp test: %d errors" % tester.errors
  sys.exit(tester.errors)
