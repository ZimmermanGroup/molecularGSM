# @HEADER
# ************************************************************************
#
#            TriBITS: Tribal Build, Integrate, and Test System
#                    Copyright 2013 Sandia Corporation
#
# Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
# the U.S. Government retains certain rights in this software.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
# 1. Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# 3. Neither the name of the Corporation nor the names of the
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# ************************************************************************
# @HEADER

from FindDevtoolsInstall import *
from install_devtools import *
import unittest


#
# Test getCmndLineOptions()
#


class test_getCmndLineOptions(unittest.TestCase):


  def test_empty(self):
    self.assertRaises(Exception, getCmndLineOptions, ([], True), None)


  def test_install_dir_only(self):
    inOptions = getCmndLineOptions(["--install-dir=/dev_env_base"], True)
    self.assertEqual(inOptions.installDir, "/dev_env_base")
    self.assertEqual(inOptions.sourceGitUrlBase, "https://github.com/tribitsdevtools/")
    self.assertEqual(inOptions.commonTools, "gitdist,autoconf,cmake")
    self.assertEqual(inOptions.compilerToolset, "gcc,mpich")
    self.assertEqual(inOptions.skipOp, False)
    self.assertEqual(inOptions.showDefaults, False)
    self.assertEqual(inOptions.doInitialSetup, False)
    self.assertEqual(inOptions.doDownload, False)
    self.assertEqual(inOptions.doInstall, False)
    self.assertEqual(inOptions.showFinalInstructions, False)
    self.assertEqual(inOptions.doAll, False)


  def test_explicit_all(self):
    inOptions = getCmndLineOptions(
      ["--install-dir=/dev_env_base", "--common-tools=all", "--compiler-toolset=all"],
      True)
    self.assertEqual(inOptions.commonTools, "gitdist,autoconf,cmake")
    self.assertEqual(inOptions.compilerToolset, "gcc,mpich")


  def test_explict_tools_one_only(self):
    inOptions = getCmndLineOptions(
      ["--install-dir=/dev_env_base", "--common-tools=cmake", "--compiler-toolset=gcc"],
      True)
    self.assertEqual(inOptions.commonTools, "cmake")
    self.assertEqual(inOptions.compilerToolset, "gcc")


  def test_explict_tools_two(self):
    inOptions = getCmndLineOptions(
      ["--install-dir=/dev_env_base", "--common-tools=cmake,autotools", "--compiler-toolset=gcc,mpich"],
      True)
    self.assertEqual(inOptions.commonTools, "cmake,autotools")
    self.assertEqual(inOptions.compilerToolset, "gcc,mpich")


  def test_no_tools(self):
    inOptions = getCmndLineOptions(
      ["--install-dir=/dev_env_base", "--common-tools=", "--compiler-toolset="],
      True)
    self.assertEqual(inOptions.commonTools, "")
    self.assertEqual(inOptions.compilerToolset, "")


  def test_do_all(self):
    inOptions = getCmndLineOptions(
      ["--install-dir=/dev_env_base", "--do-all"],
      True)
    self.assertEqual(inOptions.doInitialSetup, True)
    self.assertEqual(inOptions.doDownload, True)
    self.assertEqual(inOptions.doInstall, True)
    self.assertEqual(inOptions.showFinalInstructions, True)
    self.assertEqual(inOptions.doAll, True)


#
# Test substituteStrings()
#


class test_substituteStrings(unittest.TestCase):


  def test_1(self):
    outputStr = substituteStrings(
      "var1=@VAR1@\n" \
      "var2=@VAR2@ + @VAR1@\n" \
      "var3=@VAR3@\n",
      [ ("@VAR1@", "val1"), ("@VAR2@", "val2"), ("@VAR3@", "val3") ] \
      )
    outputStr_expected = "var1=val1\nvar2=val2 + val1\nvar3=val3\n"
    self.assertEqual(outputStr, outputStr_expected)


#
# Main
#

if __name__ == '__main__':
  unittest.main()
