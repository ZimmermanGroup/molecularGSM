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

########################################
# Unit testing code for mockprogram.py #
########################################

from unittest_helpers import *

import imp
import shutil

#thisFilePath = __file__
#thisFileRealAbsBasePath = os.path.dirname(os.path.abspath(thisFilePath))
#tribitsDir = os.path.abspath(thisFileRealAbsBasePath+"/../../tribits")
#print("tribitsDir = '" + tribitsDir + "'")
#pythonUtilsDir = os.path.join(tribitsDir, 'python_utils')
#print("pythonUtilsDir = '" + pythonUtilsDir + "'")
#
#sys.path = [pythonUtilsDir] + sys.path
#
#import GeneralScriptSupport

mockProgramPath = pythonUtilsDir+"/mockprogram.py"

testBaseDir = os.getcwd()

def createAndMoveIntoTestDir(testDirName_in):
  testDirName = "mockprogram_"+testDirName_in
  if os.path.exists(testDirName): shutil.rmtree(testDirName)
  os.mkdir(testDirName)
  os.chdir(testDirName)
  return os.path.join(testBaseDir, testDirName)


class test_mockprogram(unittest.TestCase):


  def test_no_mockprogram_file(self):
    testDir = createAndMoveIntoTestDir("no_mockprogram_file")
    try:
      (output, errorCode) = GeneralScriptSupport.runSysCmndInterface(
        mockProgramPath+" some input", rtnOutput=True)
      expected_output = "Error: .mockprogram_inout.txt is missing!\n"
      self.assertEqual(output, expected_output)
      self.assertEqual(errorCode, 1)
    finally:
      os.chdir(testBaseDir)


  def test_empty_mockprogram_file(self):
    testDir = createAndMoveIntoTestDir("empty_mockprogram_file")
    try:
      with open('.mockprogram_inout.txt', 'w') as fileHandle:
        fileHandle.write("")
      (output, errorCode) = GeneralScriptSupport.runSysCmndInterface(
        mockProgramPath+" some input", rtnOutput=True)
      expected_output = "Error: .mockprogram_inout.txt has less than three lines:\n-------------\n-------------\n"
      self.assertEqual(output, expected_output)
      self.assertEqual(errorCode, 2)
    finally:
      os.chdir(testBaseDir)


  def test_missing_program_input(self):
    testDir = createAndMoveIntoTestDir("missing_program_input")
    try:
      with open('.mockprogram_inout.txt', 'w') as fileHandle:
        fileHandle.write(
          "MOCK_PROGRAM_INPUTS: some input\n" \
          "MOCK_PROGRAM_OUTPUT: some output\n" \
          "more output\n" \
          )
      (output, errorCode) = GeneralScriptSupport.runSysCmndInterface(
        mockProgramPath+" some input", rtnOutput=True)
      expected_output = "Error, first line = 'MOCK_PROGRAM_INPUTS: some input', does not match ^MOCK_PROGRAM_INPUT:\n"
      self.assertEqual(output, expected_output)
      self.assertEqual(errorCode, 3)
    finally:
      os.chdir(testBaseDir)


  def test_input_not_matching(self):
    testDir = createAndMoveIntoTestDir("input_not_matching")
    try:
      with open('.mockprogram_inout.txt', 'w') as fileHandle:
        fileHandle.write(
          "MOCK_PROGRAM_INPUT: some other input\n" \
          "MOCK_PROGRAM_RETURN: 0\n" \
          "MOCK_PROGRAM_OUTPUT: some output\n" \
          )
      (output, errorCode) = GeneralScriptSupport.runSysCmndInterface(
        mockProgramPath+" some input", rtnOutput=True)
      expected_output = "Error, input args='some input' does not match expected='some other input'\n"
      self.assertEqual(output, expected_output)
      self.assertEqual(errorCode, 4)
    finally:
      os.chdir(testBaseDir)


  def test_missing_program_return(self):
    testDir = createAndMoveIntoTestDir("missing_program_return")
    try:
      with open('.mockprogram_inout.txt', 'w') as fileHandle:
        fileHandle.write(
          "MOCK_PROGRAM_INPUT: some input\n" \
          "MOCK_PROGRAM_OUTPUT: some output\n" \
          "more output\n" \
          )
      (output, errorCode) = GeneralScriptSupport.runSysCmndInterface(
        mockProgramPath+" some input", rtnOutput=True)
      expected_output = "Error, second line = 'MOCK_PROGRAM_OUTPUT: some output', does not match ^MOCK_PROGRAM_RETURN:\n"
      self.assertEqual(output, expected_output)
      self.assertEqual(errorCode, 5)
    finally:
      os.chdir(testBaseDir)


  def test_missing_program_output(self):
    testDir = createAndMoveIntoTestDir("missing_program_output")
    try:
      with open('.mockprogram_inout.txt', 'w') as fileHandle:
        fileHandle.write(
          "MOCK_PROGRAM_INPUT: some input\n" \
          "MOCK_PROGRAM_RETURN: 0\n" \
          "more output\n" \
          )
      (output, errorCode) = GeneralScriptSupport.runSysCmndInterface(
        mockProgramPath+" some input", rtnOutput=True)
      expected_output = "Error, third line = 'more output', does not match ^MOCK_PROGRAM_OUTPUT:\n"
      self.assertEqual(output, expected_output)
      self.assertEqual(errorCode, 6)
    finally:
      os.chdir(testBaseDir)


  def test_call_1(self):
    testDir = createAndMoveIntoTestDir("call_1")
    try:
      with open('.mockprogram_inout.txt', 'w') as fileHandle:
        fileHandle.write(
          "MOCK_PROGRAM_INPUT: some input\n" \
          "MOCK_PROGRAM_RETURN: 11\n" \
          "MOCK_PROGRAM_OUTPUT: some output\n" \
          )
      (output, errorCode) = GeneralScriptSupport.runSysCmndInterface(
        mockProgramPath+" some input", rtnOutput=True)
      expected_output = "some output\n"
      self.assertEqual(output, expected_output)
      self.assertEqual(errorCode, 11)
      with open('.mockprogram_inout.txt', 'r') as fileHandle:
        remainingMockFileStr = fileHandle.read()
      self.assertEqual(remainingMockFileStr, "")
    finally:
      os.chdir(testBaseDir)


  def test_call_1_MOCKPROGRAM_INOUT_FILE_OVERRIDE(self):
    testDir = createAndMoveIntoTestDir("call_1")
    try:
      os.mkdir("subdir")
      with open('subdir/mockprogram_inout.txt', 'w') as fileHandle:
        fileHandle.write(
          "MOCK_PROGRAM_INPUT: some input\n" \
          "MOCK_PROGRAM_RETURN: 11\n" \
          "MOCK_PROGRAM_OUTPUT: some output\n" \
          )
      (output, errorCode) = GeneralScriptSupport.runSysCmndInterface(
        mockProgramPath+" some input", rtnOutput=True,
        extraEnv={"MOCKPROGRAM_INOUT_FILE_OVERRIDE":"subdir/mockprogram_inout.txt"} )
      expected_output = "some output\n"
      self.assertEqual(output, expected_output)
      self.assertEqual(errorCode, 11)
      with open("subdir/mockprogram_inout.txt", 'r') as fileHandle:
        remainingMockFileStr = fileHandle.read()
      self.assertEqual(remainingMockFileStr, "")
    finally:
      os.chdir(testBaseDir)


  def test_call_1_multiline_out(self):
    testDir = createAndMoveIntoTestDir("call_1_multiline_out")
    try:
      with open('.mockprogram_inout.txt', 'w') as fileHandle:
        fileHandle.write(
          "MOCK_PROGRAM_INPUT: some input\n" \
          "MOCK_PROGRAM_RETURN: 11\n" \
          "MOCK_PROGRAM_OUTPUT: some output\n" \
          "another line of output\n" \
          "last output line\n"
          )
      (output, errorCode) = GeneralScriptSupport.runSysCmndInterface(
        mockProgramPath+" some input", rtnOutput=True)
      expected_output = "some output\nanother line of output\nlast output line\n"
      self.assertEqual(output, expected_output)
      self.assertEqual(errorCode, 11)
      with open('.mockprogram_inout.txt', 'r') as fileHandle:
        remainingMockFileStr = fileHandle.read()
      self.assertEqual(remainingMockFileStr, "")
    finally:
      os.chdir(testBaseDir)


  def test_call_2(self):
    testDir = createAndMoveIntoTestDir("call_2")
    try:
      with open('.mockprogram_inout.txt', 'w') as fileHandle:
        fileHandle.write(
          "MOCK_PROGRAM_INPUT: some input 1\n" \
          "MOCK_PROGRAM_RETURN: 13\n" \
          "MOCK_PROGRAM_OUTPUT: some output 1\n" \
          "MOCK_PROGRAM_INPUT: some input 2\n" \
          "MOCK_PROGRAM_RETURN: 15\n" \
          "MOCK_PROGRAM_OUTPUT: some output 2\n" \
          )
      (output, errorCode) = GeneralScriptSupport.runSysCmndInterface(
        mockProgramPath+" some input 1", rtnOutput=True)
      expected_output = "some output 1\n"
      self.assertEqual(output, expected_output)
      self.assertEqual(errorCode, 13)
      (output, errorCode) = GeneralScriptSupport.runSysCmndInterface(
        mockProgramPath+" some input 2", rtnOutput=True)
      expected_output = "some output 2\n"
      self.assertEqual(output, expected_output)
      self.assertEqual(errorCode, 15)
      with open('.mockprogram_inout.txt', 'r') as fileHandle:
        remainingMockFileStr = fileHandle.read()
      self.assertEqual(remainingMockFileStr, "")
    finally:
      os.chdir(testBaseDir)


  def test_call_2_MOCKPROGRAM_INOUT_FILE_OVERRIDE(self):
    testDir = createAndMoveIntoTestDir("call_2")
    try:
      os.mkdir("subdir")
      with open('subdir/mockprogram_inout.txt', 'w') as fileHandle:
        fileHandle.write(
          "MOCK_PROGRAM_INPUT: some input 1\n" \
          "MOCK_PROGRAM_RETURN: 13\n" \
          "MOCK_PROGRAM_OUTPUT: some output 1\n" \
          "MOCK_PROGRAM_INPUT: some input 2\n" \
          "MOCK_PROGRAM_RETURN: 15\n" \
          "MOCK_PROGRAM_OUTPUT: some output 2\n" \
          )
      (output, errorCode) = GeneralScriptSupport.runSysCmndInterface(
        mockProgramPath+" some input 1", rtnOutput=True,
        extraEnv={"MOCKPROGRAM_INOUT_FILE_OVERRIDE":"subdir/mockprogram_inout.txt"} )
      expected_output = "some output 1\n"
      self.assertEqual(output, expected_output)
      self.assertEqual(errorCode, 13)
      (output, errorCode) = GeneralScriptSupport.runSysCmndInterface(
        mockProgramPath+" some input 2", rtnOutput=True,
        extraEnv={"MOCKPROGRAM_INOUT_FILE_OVERRIDE":"subdir/mockprogram_inout.txt"} )
      expected_output = "some output 2\n"
      self.assertEqual(output, expected_output)
      self.assertEqual(errorCode, 15)
      with open('subdir/mockprogram_inout.txt', 'r') as fileHandle:
        remainingMockFileStr = fileHandle.read()
      self.assertEqual(remainingMockFileStr, "")
    finally:
      os.chdir(testBaseDir)


  def test_call_2_multiline_output_1(self):
    testDir = createAndMoveIntoTestDir("call_2_multiline_output_1")
    try:
      with open('.mockprogram_inout.txt', 'w') as fileHandle:
        fileHandle.write(
          "MOCK_PROGRAM_INPUT: some input 1\n" \
          "MOCK_PROGRAM_RETURN: 13\n" \
          "MOCK_PROGRAM_OUTPUT: some output 1\n" \
          "another line of output\n" \
          "last output line\n"
          "MOCK_PROGRAM_INPUT: some input 2\n" \
          "MOCK_PROGRAM_RETURN: 15\n" \
          "MOCK_PROGRAM_OUTPUT: some output 2\n" \
          )
      (output, errorCode) = GeneralScriptSupport.runSysCmndInterface(
        mockProgramPath+" some input 1", rtnOutput=True)
      expected_output = "some output 1\nanother line of output\nlast output line\n"
      self.assertEqual(output, expected_output)
      self.assertEqual(errorCode, 13)
      (output, errorCode) = GeneralScriptSupport.runSysCmndInterface(
        mockProgramPath+" some input 2", rtnOutput=True)
      expected_output = "some output 2\n"
      self.assertEqual(output, expected_output)
      self.assertEqual(errorCode, 15)
      with open('.mockprogram_inout.txt', 'r') as fileHandle:
        remainingMockFileStr = fileHandle.read()
      self.assertEqual(remainingMockFileStr, "")
    finally:
      os.chdir(testBaseDir)


if __name__ == '__main__':
  unittest.main()
