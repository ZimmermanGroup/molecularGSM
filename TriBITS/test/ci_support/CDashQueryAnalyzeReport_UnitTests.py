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

import os
import sys
import copy
import shutil
import unittest
import pprint

from FindCISupportDir import *
from CDashQueryAnalyzeReport import *
from CDashQueryAnalyzeReportUnitTestHelpers import *
from Python2and3 import u, stru

g_testBaseDir = getScriptBaseDir()

tribitsBaseDir=os.path.abspath(g_testBaseDir+"/../../tribits")
mockProjectBaseDir=os.path.abspath(tribitsBaseDir+"/examples/MockTrilinos")

g_pp = pprint.PrettyPrinter(indent=2)


#
# Helper functions and classes
#


# Mock function object for getting data off of CDash as a stand-in for the
# function extractCDashApiQueryData().
class MockExtractCDashApiQueryDataFunctor(object):
  def __init__(self, cdashApiQueryUrl_expected, dataToReturn):
    self.cdashApiQueryUrl_expected = cdashApiQueryUrl_expected
    self.dataToReturn = dataToReturn
  def __call__(self, cdashApiQueryUrl):
    if cdashApiQueryUrl != self.cdashApiQueryUrl_expected:
      raise Exception(
        "Error, cdashApiQueryUrl='"+cdashApiQueryUrl+"' !="+\
        " cdashApiQueryUrl_expected='"+self.cdashApiQueryUrl_expected+"'!")
    return self.dataToReturn


# Helper script for creating test directories
def deleteThenCreateTestDir(testDir):
    outputCacheDir="test_getAndCacheCDashQueryDataOrReadFromCache_write_cache"
    if os.path.exists(testDir): shutil.rmtree(testDir)
    os.mkdir(testDir)


#############################################################################
#
# Test CDashQueryAnalyzeReport.convertInputDateArgToYYYYMMDD()
#
#############################################################################

class test_convertInputDateArgToYYYYMMDD(unittest.TestCase):

  def test_yesterday_before_testing_day_start(self):
    convertedDate = convertInputDateArgToYYYYMMDD("04:01", "yesterday",
      "2019-11-20T04:00:00 UTC")
    self.assertEqual(str(convertedDate), "2019-11-18 00:00:00")

  def test_yesterday_after_testing_day_start(self):
    convertedDate = convertInputDateArgToYYYYMMDD("04:01", "yesterday",
      "2019-11-20T04:02:00 UTC")
    self.assertEqual(str(convertedDate), "2019-11-19 00:00:00")

  def test_today_before_testing_day_start(self):
    convertedDate = convertInputDateArgToYYYYMMDD("04:01", "today",
      "2019-11-20T04:00:00 UTC")
    self.assertEqual(str(convertedDate), "2019-11-19 00:00:00")

  def test_today_after_testing_day_start(self):
    convertedDate = convertInputDateArgToYYYYMMDD("04:01", "today",
      "2019-11-20T04:02:00 UTC")
    self.assertEqual(str(convertedDate), "2019-11-20 00:00:00")

  def test_YYYYMMDD_before_testing_day_start(self):
    convertedDate = convertInputDateArgToYYYYMMDD("04:01", "2019-11-20",
      "2019-11-20T04:00:00 UTC")
    self.assertEqual(str(convertedDate), "2019-11-20 00:00:00")

  def test_YYYYMMDD_after_testing_day_start(self):
    convertedDate = convertInputDateArgToYYYYMMDD("04:01", "2019-11-20",
      "2019-11-20T04:02:00 UTC")
    self.assertEqual(str(convertedDate), "2019-11-20 00:00:00")

  # Above, in the last two tests, I am askig for the testing day 2019-11-20
  # and therefore it should not matter when I ask for that.  I should always
  # get results for that testing day!


#############################################################################
#
# Test CDashQueryAnalyzeReport.validateAndConvertYYYYMMDD()
#
#############################################################################

class test_validateAndConvertYYYYMMDD(unittest.TestCase):

  def test_pass1(self):
    yyyyymmdd = validateAndConvertYYYYMMDD("2015-12-21")
    self.assertEqual(str(yyyyymmdd), "2015-12-21 00:00:00")

  def test_pass2(self):
    yyyyymmdd = validateAndConvertYYYYMMDD("2015-12-01")
    self.assertEqual(str(yyyyymmdd), "2015-12-01 00:00:00")

  def test_pass3(self):
    yyyyymmdd = validateAndConvertYYYYMMDD("2015-12-1")
    self.assertEqual(str(yyyyymmdd), "2015-12-01 00:00:00")

  def test_pass4(self):
    yyyyymmdd = validateAndConvertYYYYMMDD("2015-01-1")
    self.assertEqual(str(yyyyymmdd), "2015-01-01 00:00:00")

  def test_pass4(self):
    yyyyymmdd = validateAndConvertYYYYMMDD("2015-1-9")
    self.assertEqual(str(yyyyymmdd), "2015-01-09 00:00:00")

  def test_fail_empty(self):
    self.assertRaises(ValueError, validateAndConvertYYYYMMDD,  "")

  def test_fail1(self):
    self.assertRaises(ValueError, validateAndConvertYYYYMMDD,  "201512-21")

  def test_fail1(self):
    #yyyyymmdd = validateAndConvertYYYYMMDD("201512-21")
    self.assertRaises(ValueError, validateAndConvertYYYYMMDD,  "201512-21")


#############################################################################
#
# Test CDashQueryAnalyzeReport.getFileNameStrFromText()
#
#############################################################################

class test_getFileNameStrFromText(unittest.TestCase):

  def test_simple(self):
    self.assertEqual(
      getFileNameStrFromText("This is something"), "This_is_something")

  def test_harder(self):
    self.assertEqual(
      getFileNameStrFromText("thi@ (something; other)"),
      "thi___something__other_")


#############################################################################
#
# Test CDashQueryAnalyzeReport.checkDictsAreSame()
#
#############################################################################

class test_checkDictsAreSame(unittest.TestCase):

  def test_same_dicts(self):
    dict_1 = { 'site':'site_ame', 'buildname':'build_name', 'data':'val1' }
    dict_2 = { 'site':'site_ame', 'buildname':'build_name', 'data':'val1' }
    expectedRtn = (True, None)
    self.assertEqual(checkDictsAreSame(dict_1, "dict_1", dict_2, "dict_2"), expectedRtn)

  def test_different_num_keys(self):
    dict_1 = { 'site':'site_ame', 'buildname':'build_name', 'data':'val1' }
    dict_2 = { 'site':'site_ame', 'buildname':'build_name' }
    expectedRtn = (False, "len(dict_1.keys())=3 != len(dict_2.keys())=2")
    self.assertEqual(checkDictsAreSame(dict_1, "dict_1", dict_2, "dict_2"), expectedRtn)

  def test_different_key_name_1(self):
    dict_1 = { 'site':'site_ame', 'buildname':'build_name', 'data':'val1' }
    dict_2 = { 'site':'site_ame', 'buildname':'build_name', 'data2':'val1' }
    expectedRtn = (False, "dict_1['data'] does not exist in dict_2")
    self.assertEqual(checkDictsAreSame(dict_1, "dict_1", dict_2, "dict_2"), expectedRtn)

  def test_different_key_name_2(self):
    dict_1 = { 'site':'site_ame', 'buildname':'build_name', 'data':'val1' }
    dict_2 = { 'site':'site_ame', 'buildName':'build_name', 'data':'val1' }
    expectedRtn = (False, "dict_1['buildname'] does not exist in dict_2")
    self.assertEqual(checkDictsAreSame(dict_1, "dict_1", dict_2, "dict_2"), expectedRtn)

  def test_different_key_value_1(self):
    dict_1 = { 'site':'site_ame', 'buildname':'build_name', 'data':'val1' }
    dict_2 = { 'site':'site_ame', 'buildname':'build_name', 'data':'val2' }
    expectedRtn = (False, "dict_1['data'] = 'val1' != dict_2['data'] = 'val2'")
    self.assertEqual(checkDictsAreSame(dict_1, "dict_1", dict_2, "dict_2"), expectedRtn)

  def test_different_key_value_2(self):
    dict_1 = { 'site':'site_name', 'buildname':'build_name', 'data':'val1' }
    dict_2 = { 'site':'site_name2', 'buildname':'build_name', 'data':'val1' }
    expectedRtn = (False, "dict_1['site'] = 'site_name' != dict_2['site'] = 'site_name2'")
    self.assertEqual(checkDictsAreSame(dict_1, "dict_1", dict_2, "dict_2"), expectedRtn)


#############################################################################
#
# Test CDashQueryAnalyzeReport.getCompressedFileNameIfTooLong()
#
#############################################################################

class test_getCompressedFileNameIfTooLong(unittest.TestCase):

  def test_short_filename(self):
    self.assertEqual(
      getCompressedFileNameIfTooLong("some_short_filename.txt"),
      "some_short_filename.txt")

  def test_too_long_filename(self):
    self.assertEqual(
      getCompressedFileNameIfTooLong(
        "2018-11-06-sierra-waterman-sparc-alltpls_waterman-gpu_cuda-9.2.88-gcc-7.2.0_openmpi-2.1.2_shared_opt-sparc-regression-ear99_aero_blottner-sphere_Ma5.0_laminar-isot_air-pg_imp-bdf1_nlin-newton_lin-pi_sccfv_1o_0000132-hex_np01_cgns_tpetra-bcrs_belos-fp-jacobi-HIST-30.json"),
      "ec424ddf0b79e61e539ff7a441019f0b928f88e9" )

  def test_too_long_filename_prefix_ext(self):
    self.assertEqual(
      getCompressedFileNameIfTooLong(
        "2018-11-06-sierra-waterman-sparc-alltpls_waterman-gpu_cuda-9.2.88-gcc-7.2.0_openmpi-2.1.2_shared_opt-sparc-regression-ear99_aero_blottner-sphere_Ma5.0_laminar-isot_air-pg_imp-bdf1_nlin-newton_lin-pi_sccfv_1o_0000132-hex_np01_cgns_tpetra-bcrs_belos-fp-jacobi-HIST-30.json",
        "2018-11-06-", "json"
        ),
      "2018-11-06-ec424ddf0b79e61e539ff7a441019f0b928f88e9.json" )


#############################################################################
#
# Test CDashQueryAnalyzeReport.getFilteredList()
#
#############################################################################

def isGreaterThan5(val): return val > 5

class test_getFilteredList(unittest.TestCase):

  def test_filter_list(self):
    origList = [ 4, 3, 6, 8, 10, 2, 12 ];
    self.assertEqual(getFilteredList(origList, isGreaterThan5), [6, 8, 10, 12])

  def test_filtered_list_empty(self):
    origList = [ 4, 3, 2 ];
    self.assertEqual(getFilteredList(origList, isGreaterThan5), [])

  def test_empty_list(self):
    origList = [];
    self.assertEqual(getFilteredList(origList, isGreaterThan5), [])


#############################################################################
#
# Test CDashQueryAnalyzeReport.splitListOnMatch()
#
#############################################################################

def isLessThan5(val): return val < 5

class test_splitListOnMatch(unittest.TestCase):

  def test_split_list(self):
    origList = [ 4, 3, 6, 8, 10, 2, 12 ];
    (lessThan5List, notLessThan5List) = splitListOnMatch(origList, isLessThan5) 
    self.assertEqual(lessThan5List, [ 4, 3, 2 ])
    self.assertEqual(notLessThan5List, [6, 8, 10, 12])

  def test_match_empty(self):
    origList = [ 6, 8, 10, 12 ];
    (lessThan5List, notLessThan5List) = splitListOnMatch(origList, isLessThan5) 
    self.assertEqual(lessThan5List, [])
    self.assertEqual(notLessThan5List, [6, 8, 10, 12])

  def test_nomatch_empty(self):
    origList = [ 4, 3, 2 ];
    (lessThan5List, notLessThan5List) = splitListOnMatch(origList, isLessThan5) 
    self.assertEqual(lessThan5List, [ 4, 3, 2 ])
    self.assertEqual(notLessThan5List, [])

  def test_empty(self):
    origList = [];
    (lessThan5List, notLessThan5List) = splitListOnMatch(origList, isLessThan5) 
    self.assertEqual(lessThan5List, [])
    self.assertEqual(notLessThan5List, [])


#############################################################################
#
# Test CDashQueryAnalyzeReport.foreachTransform()
#
#############################################################################

def sqrnum(num): return num*num

def dictnum(num): return { 'num':num }

def sqrdictnum(dict_inout):
  num = dict_inout['num']
  dict_inout['num'] = num*num
  return dict_inout

class test_foreachTransform(unittest.TestCase):

  def test_many_int(self):
    self.assertEqual(foreachTransform([1,2,3,4,5],sqrnum), [1,4,9,16,25])

  def test_1_int(self):
    self.assertEqual(foreachTransform([3],sqrnum), [9])

  def test_0_int(self):
    self.assertEqual(foreachTransform([],sqrnum), [])

  def test_many_dict(self):
    dm = dictnum
    self.assertEqual(
      foreachTransform([dm(1),dm(2),dm(3),dm(4)],sqrdictnum),
      [dm(1),dm(4),dm(9),dm(16)])


#############################################################################
#
# Test CDashQueryAnalyzeReport.NotMatchFunctor()
#
#############################################################################

def dummyMatch5(intVal):
  return (intVal == 5)

class test_NotMatchFunctor(unittest.TestCase):

  def test_dummyMatch5(self):
    self.assertEqual(dummyMatch5(4), False)
    self.assertEqual(dummyMatch5(5), True)
    self.assertEqual(dummyMatch5(6), False)

  def test_dummyMatch5(self):
    self.assertEqual(NotMatchFunctor(dummyMatch5)(4), True)
    self.assertEqual(NotMatchFunctor(dummyMatch5)(5), False)
    self.assertEqual(NotMatchFunctor(dummyMatch5)(6), True)


#############################################################################
#
# Test CDashQueryAnalyzeReport.removeElementsFromListGivenIndexes()
#
#############################################################################

class test_removeElementsFromListGivenIndexes(unittest.TestCase):

  def test_remove_none(self):
    list_orig = [0, 1, 2, 3, 4]
    idxToRemove = []
    list_expected = [0, 1, 2, 3, 4]
    self.assertEqual(removeElementsFromListGivenIndexes(list_orig,idxToRemove),
      list_expected )

  def test_remove_some1_ordered(self):
    list_orig = [0, 1, 2, 3, 4]
    idxToRemove = [0, 2, 3]
    list_expected = [1, 4] 
    self.assertEqual(removeElementsFromListGivenIndexes(list_orig,idxToRemove),
      list_expected )

  def test_remove_some1_unordered1(self):
    list_orig = [0, 1, 2, 3, 4]
    idxToRemove = [3, 0, 2]
    list_expected = [1, 4] 
    self.assertEqual(removeElementsFromListGivenIndexes(list_orig,idxToRemove),
      list_expected )

  def test_remove_some1_unordered2(self):
    list_orig = [0, 1, 2, 3, 4]
    idxToRemove = [3, 2, 0]
    list_expected = [1, 4] 
    self.assertEqual(removeElementsFromListGivenIndexes(list_orig,idxToRemove),
      list_expected )

  def test_remove_some2_ordered(self):
    list_orig = [0, 1, 2, 3, 4]
    idxToRemove = [0, 2, 4]
    list_expected = [1, 3]
    self.assertEqual(removeElementsFromListGivenIndexes(list_orig,idxToRemove),
      list_expected )

  def test_remove_some2_unordered1(self):
    list_orig = [0, 1, 2, 3, 4]
    idxToRemove = [2, 4, 0]
    list_expected = [1, 3]
    self.assertEqual(removeElementsFromListGivenIndexes(list_orig,idxToRemove),
      list_expected )

  def test_remove_some2_unordered2(self):
    list_orig = [0, 1, 2, 3, 4]
    idxToRemove = [4, 2, 0]
    list_expected = [1, 3]
    self.assertEqual(removeElementsFromListGivenIndexes(list_orig,idxToRemove),
      list_expected )


#############################################################################
#
# Test CDashQueryAnalyzeReport.readCsvFileIntoListOfDicts()
#
#############################################################################

class test_readCsvFileIntoListOfDicts(unittest.TestCase):

  def test_col_3_row_2_required_cols_pass(self):
    csvFileStr=\
        "col_0, col_1, col_2\n"+\
        "val_00, val_01, val_02\n"+\
        "val_10, val_11, val_12\n\n\n"  # Add extra blanks line for extra test!
    csvFileName = "readCsvFileIntoListOfDicts_col_3_row_2_expeced_cols_pass.csv"
    with open(csvFileName, 'w') as csvFileToWrite:
      csvFileToWrite.write(csvFileStr)
    listOfDicts = readCsvFileIntoListOfDicts(csvFileName, ['col_0', 'col_1', 'col_2'])
    listOfDicts_required = \
      [
        { 'col_0' : 'val_00', 'col_1' : 'val_01', 'col_2' : 'val_02' },
        { 'col_0' : 'val_10', 'col_1' : 'val_11', 'col_2' : 'val_12' },
        ]
    self.assertEqual(len(listOfDicts), 2)
    for i in range(len(listOfDicts_required)):
      self.assertEqual(listOfDicts[i], listOfDicts_required[i])

  def test_col_3_row_2_w_blanks_required_cols_pass(self):
    csvFileStr=\
        "col_0, col_1, col_2\n"+\
        "\n\n"+\
        "val_00, val_01, val_02\n"+\
        "\n"+\
        "val_10, val_11, val_12\n\n\n"
    csvFileName = "readCsvFileIntoListOfDicts_col_3_row_2_w_blanks_required_cols_pass.csv"
    with open(csvFileName, 'w') as csvFileToWrite:
      csvFileToWrite.write(csvFileStr)
    listOfDicts = readCsvFileIntoListOfDicts(csvFileName, ['col_0', 'col_1', 'col_2'])
    listOfDicts_required = \
      [
        { 'col_0' : 'val_00', 'col_1' : 'val_01', 'col_2' : 'val_02' },
        { 'col_0' : 'val_10', 'col_1' : 'val_11', 'col_2' : 'val_12' },
        ]
    self.assertEqual(len(listOfDicts), 2)
    for i in range(len(listOfDicts_required)):
      self.assertEqual(listOfDicts[i], listOfDicts_required[i])

  def test_col_3_row_2_required_cols_tuple_pass(self):
    csvFileStr=\
        "col_0, col_1, col_2\n"+\
        "val_00, val_01, val_02\n"+\
        "val_10, val_11, val_12\n"
    csvFileName = "readCsvFileIntoListOfDicts_test_col_3_row_2_required_cols_tuple_pass.csv"
    with open(csvFileName, 'w') as csvFileToWrite:
      csvFileToWrite.write(csvFileStr)
    listOfDicts = readCsvFileIntoListOfDicts(csvFileName, ('col_0', 'col_1', 'col_2'))
    listOfDicts_required = \
      [
        { 'col_0' : 'val_00', 'col_1' : 'val_01', 'col_2' : 'val_02' },
        { 'col_0' : 'val_10', 'col_1' : 'val_11', 'col_2' : 'val_12' },
        ]
    self.assertEqual(len(listOfDicts), 2)
    for i in range(len(listOfDicts_required)):
      self.assertEqual(listOfDicts[i], listOfDicts_required[i])

  def test_col_3_row_2_no_required_cols_pass(self):
    csvFileStr=\
        "col_0, col_1, col_2\n"+\
        "val_00, val_01, val_02\n"+\
        "val_10, val_11, val_12\n\n\n"  # Add extra blanks line for extra test!
    csvFileName = "readCsvFileIntoListOfDicts_col_3_row_2_no_required_cols_pass.csv"
    with open(csvFileName, 'w') as csvFileToWrite:
      csvFileToWrite.write(csvFileStr)
    listOfDicts = readCsvFileIntoListOfDicts(csvFileName)
    listOfDicts_required = \
      [
        { 'col_0' : 'val_00', 'col_1' : 'val_01', 'col_2' : 'val_02' },
        { 'col_0' : 'val_10', 'col_1' : 'val_11', 'col_2' : 'val_12' },
        ]
    self.assertEqual(len(listOfDicts), 2)
    for i in range(len(listOfDicts_required)):
      self.assertEqual(listOfDicts[i], listOfDicts_required[i])

  def test_col_3_row_2_required_3_optional_2_col_no_opt_pass(self):
    csvFileStr=\
        "col_0, col_1, col_2\n"+\
        "val_00, val_01, val_02\n"+\
        "val_10, val_11, val_12\n\n\n"  # Add extra blanks line for extra test!
    csvFileName = "readCsvFileIntoListOfDicts_col_3_row_2_required_3_optional_2_col_no_opt_pass.csv"
    with open(csvFileName, 'w') as csvFileToWrite:
      csvFileToWrite.write(csvFileStr)
    listOfDicts = readCsvFileIntoListOfDicts(csvFileName,
      ['col_0', 'col_1', 'col_2'], ['opt_0', 'opt_1'])
    listOfDicts_required = \
      [
        { 'col_0' : 'val_00', 'col_1' : 'val_01', 'col_2' : 'val_02' },
        { 'col_0' : 'val_10', 'col_1' : 'val_11', 'col_2' : 'val_12' },
        ]
    self.assertEqual(len(listOfDicts), 2)
    for i in range(len(listOfDicts_required)):
      self.assertEqual(listOfDicts[i], listOfDicts_required[i])

  def test_col_3_row_2_required_3_optional_2_col_opt_1_pass(self):
    csvFileStr=\
        "col_0, col_1, col_2, opt_1\n"+\
        "val_00, val_01, val_02, val_03\n"+\
        "val_10, val_11, val_12, val_13\n"
    csvFileName = "readCsvFileIntoListOfDicts_col_3_row_2_required_3_optional_2_col_opt_1_pass.csv"
    with open(csvFileName, 'w') as csvFileToWrite:
      csvFileToWrite.write(csvFileStr)
    listOfDicts = readCsvFileIntoListOfDicts(csvFileName,
      ['col_0', 'col_1', 'col_2'], ['opt_0', 'opt_1'])
    listOfDicts_required = \
      [
        { 'col_0':'val_00', 'col_1':'val_01', 'col_2':'val_02', 'opt_1':'val_03' },
        { 'col_0':'val_10', 'col_1':'val_11', 'col_2':'val_12', 'opt_1':'val_13' },
        ]
    self.assertEqual(len(listOfDicts), 2)
    for i in range(len(listOfDicts_required)):
      self.assertEqual(listOfDicts[i], listOfDicts_required[i])

  def test_col_3_row_2_optional_only_pass(self):
    csvFileStr=\
        "col_0, col_1, col_2, opt_1\n"+\
        "val_00, val_01, val_02, val_03\n"+\
        "val_10, val_11, val_12, val_13\n"
    csvFileName = "readCsvFileIntoListOfDicts_col_3_row_2_optional_only_pass.csv"
    with open(csvFileName, 'w') as csvFileToWrite:
      csvFileToWrite.write(csvFileStr)
    listOfDicts = readCsvFileIntoListOfDicts(csvFileName,
      (), ['opt_0', 'opt_1', 'col_0', 'col_1', 'col_2'])
    listOfDicts_required = \
      [
        { 'col_0':'val_00', 'col_1':'val_01', 'col_2':'val_02', 'opt_1':'val_03' },
        { 'col_0':'val_10', 'col_1':'val_11', 'col_2':'val_12', 'opt_1':'val_13' },
        ]
    self.assertEqual(len(listOfDicts), 2)
    for i in range(len(listOfDicts_required)):
      self.assertEqual(listOfDicts[i], listOfDicts_required[i])

  def test_col_3_row_2_required_3_optional_2_col_opt_1_mixed_order_pass(self):
    csvFileStr=\
        "col_2, opt_1, col_1, col_0\n"+\
        "val_00, val_01, val_02, val_03\n"+\
        "val_10, val_11, val_12, val_13\n"
    csvFileName = "readCsvFileIntoListOfDicts_col_3_row_2_required_3_optional_2_col_opt_1_mixed_order_pass.csv"
    with open(csvFileName, 'w') as csvFileToWrite:
      csvFileToWrite.write(csvFileStr)
    listOfDicts = readCsvFileIntoListOfDicts(csvFileName,
      ['col_0', 'col_1', 'col_2'], ['opt_0', 'opt_1'])
    listOfDicts_required = \
      [
        { 'col_2':'val_00', 'opt_1':'val_01', 'col_1':'val_02', 'col_0':'val_03' },
        { 'col_2':'val_10', 'opt_1':'val_11', 'col_1':'val_12', 'col_0':'val_13' },
        ]
    self.assertEqual(len(listOfDicts), 2)
    for i in range(len(listOfDicts_required)):
      self.assertEqual(listOfDicts[i], listOfDicts_required[i])

  def test_too_few_required_headers_fail(self):
    csvFileStr=\
        "wrong col, col_1, col_2\n"+\
        "val_00, val_01, val_02\n"
    csvFileName = "readCsvFileIntoListOfDicts_too_few_required_headers_fail.csv"
    with open(csvFileName, 'w') as csvFileToWrite:
      csvFileToWrite.write(csvFileStr)
    threwException = True
    try:
      listOfDicts = readCsvFileIntoListOfDicts(csvFileName, ['col_0', 'col_1'])
      threwException = False
    except Exception as errMsg:
      self.assertEqual( str(errMsg),
        "Error, for CSV file '"+csvFileName+"' the column header 'wrong col'"+\
        " is not in the set of required column headers '['col_0', 'col_1']'"+\
        " or optional column headers '[]'!"
        )
    if not threwException:
      self.assertFalse("ERROR: Did not throw an exception")

  def test_too_many_required_headers_fail(self):
    csvFileStr=\
        "col_0, col_1, col_2\n"+\
        "val_00, val_01, val_02\n"
    csvFileName = "readCsvFileIntoListOfDicts_too_many_required_headers_fail.csv"
    with open(csvFileName, 'w') as csvFileToWrite:
      csvFileToWrite.write(csvFileStr)
    threwException = True
    try:
      listOfDicts = readCsvFileIntoListOfDicts(csvFileName,
        ['col_0', 'col_1', 'col_2', 'col_3'])
      threwException = False
    except Exception as errMsg:
      self.assertEqual( str(errMsg),
        "Error, for CSV file '"+csvFileName+"' the required header"+\
        " 'col_3' is missing from the set of included column headers"+\
        " '['col_0', 'col_1', 'col_2']'!" )
    if not threwException:
      self.assertFalse("ERROR: Did not throw an exception")

  def test_wrong_required_col_0_fail(self):
    csvFileStr=\
        "wrong col, col_1, col_2\n"+\
        "val_00, val_01, val_02\n"
    csvFileName = "readCsvFileIntoListOfDicts_wrong_required_col_0_fail.csv"
    with open(csvFileName, 'w') as csvFileToWrite:
      csvFileToWrite.write(csvFileStr)
    #listOfDicts = readCsvFileIntoListOfDicts(csvFileName, ['col_0', 'col_1', 'col_2'])
    self.assertRaises(Exception, readCsvFileIntoListOfDicts,
      csvFileName, ['col_0', 'col_1', 'col_2'])

  def test_wrong_required_col_1_fail(self):
    csvFileStr=\
        "col_0, wrong col, col_2\n"+\
        "val_00, val_01, val_02\n"
    csvFileName = "readCsvFileIntoListOfDicts_wrong_required_col_1_fail.csv"
    with open(csvFileName, 'w') as csvFileToWrite:
      csvFileToWrite.write(csvFileStr)
    #listOfDicts = readCsvFileIntoListOfDicts(csvFileName, ['col_0', 'col_1', 'col_2'])
    self.assertRaises(Exception, readCsvFileIntoListOfDicts,
      csvFileName, ['col_0', 'col_1', 'col_2'])

  def test_col_3_row_2_bad_row_len_fail(self):
    csvFileStr=\
        "col_0, col_1, col_2\n"+\
        "val_00, val_01, val_02\n"+\
        "val_10, val_11, val_12, extra\n"
    csvFileName = "readCsvFileIntoListOfDicts_col_3_row_2_bad_row_len_fail.csv"
    with open(csvFileName, 'w') as csvFileToWrite:
      csvFileToWrite.write(csvFileStr)
    threwException = True
    try:
      listOfDicts = readCsvFileIntoListOfDicts(csvFileName)
      threwException = False
    except Exception as errMsg:
      self.assertEqual( str(errMsg),
        "Error, for CSV file '"+csvFileName+"' the data row 1"+\
        " ['val_10', 'val_11', 'val_12', 'extra'] has 4 entries"+\
        " which does not macth the number of column headers 3!" )
    if not threwException:
      self.assertFalse("ERROR: Did not throw an exception")

  def test_col_3_row_0_required_cols_pass(self):
    csvFileStr=\
        "col_0, col_1, col_2\n"
    csvFileName = "readCsvFileIntoListOfDicts_col_3_row_0_required_cols_pass.csv"
    with open(csvFileName, 'w') as csvFileToWrite:
      csvFileToWrite.write(csvFileStr)
    listOfDicts = readCsvFileIntoListOfDicts(csvFileName, ['col_0', 'col_1', 'col_2'])
    self.assertEqual(len(listOfDicts), 0)

  def test_empty_csv_file_with_required_fields_fail(self):
    csvFileStr=""
    csvFileName = "readCsvFileIntoListOfDicts_empty_csv_file_with_required_fields_fail.csv"
    with open(csvFileName, 'w') as csvFileToWrite:
      csvFileToWrite.write(csvFileStr)
    threwException = True
    try:
      listOfDicts = readCsvFileIntoListOfDicts(csvFileName, ['col_0'])
      threwException = False
    except Exception as errMsg:
      self.assertEqual( str(errMsg),
        "Error, CSV file '"+csvFileName+"' is empty which is not allowed!" )
    if not threwException:
      self.assertFalse("ERROR: Did not throw an exception")


#############################################################################
#
# Test CDashQueryAnalyzeReport.writeCsvFileStructureToStr()
#
#############################################################################

class test_writeCsvFileStructureToStr(unittest.TestCase):

  def test_rows_0(self):
    csvFileStruct = CsvFileStructure(
      ('field1', 'field2', 'field3', 'field4', ),
      ()
      )
    csvFileStr = writeCsvFileStructureToStr(csvFileStruct)
    csvFileStr_expected = \
      "field1, field2, field3, field4\n"
    self.assertEqual(csvFileStr, csvFileStr_expected)

  def test_rows_1(self):
    csvFileStruct = CsvFileStructure(
      ('field1', 'field2', 'field3', 'field4', ),
      [ ('dat11', 'dat12', '', '') ]
      )
    csvFileStr = writeCsvFileStructureToStr(csvFileStruct)
    csvFileStr_expected = \
      "field1, field2, field3, field4\n" + \
      "dat11, dat12, , \n"
    self.assertEqual(csvFileStr, csvFileStr_expected)

  def test_rows_3(self):
    csvFileStruct = CsvFileStructure(
      ('field1', 'field2', 'field3', 'field4', ),
      [ ('dat11', 'dat12', '', ''),
        ('', 'dat22', '', 'dat24'),
        ('dat31', '', '', 'dat44'),
        ]
      )
    csvFileStr = writeCsvFileStructureToStr(csvFileStruct)
    csvFileStr_expected = \
      "field1, field2, field3, field4\n"+\
      "dat11, dat12, , \n"+\
      ", dat22, , dat24\n"+\
      "dat31, , , dat44\n"
    self.assertEqual(csvFileStr, csvFileStr_expected)


#############################################################################
#
# Test CDashQueryAnalyzeReport.getExpectedBuildsListOfDictsfromCsvFile()
#
#############################################################################

class test_getExpectedBuildsListOfDictsfromCsvFile(unittest.TestCase):

  def test_getExpectedBuildsListOfDictsfromCsvFile(self):
    expectedBuildsCsvFileStr=\
        "group, site, buildname\n"+\
        "group1, site1, buildname1\n"+\
        "group1, site1, buildname2\n"+\
        "group2, site2, buildname2\n\n\n\n"
    csvFileName = "test_getExpectedBuildsListOfDictsfromCsvFile.csv"
    with open(csvFileName, 'w') as csvFileToWrite:
      csvFileToWrite.write(expectedBuildsCsvFileStr)
    expectedBuildsList = getExpectedBuildsListOfDictsfromCsvFile(csvFileName)
    expectedBuildsList_expected = \
      [
        { 'group' : 'group1', 'site' : 'site1', 'buildname' : 'buildname1' },
        { 'group' : 'group1', 'site' : 'site1', 'buildname' : 'buildname2' },
        { 'group' : 'group2', 'site' : 'site2', 'buildname' : 'buildname2' },
        ]
    self.assertEqual(len(expectedBuildsList), 3)
    for i in range(len(expectedBuildsList_expected)):
      self.assertEqual(expectedBuildsList[i], expectedBuildsList_expected[i])


#############################################################################
#
# Test CDashQueryAnalyzeReport.writeTestsListOfDictsToCsvFileStructure()
#
#############################################################################

class test_writeTestsLODToCsvFileStructure(unittest.TestCase):

  def test_tests_0(self):
    testsLOD = []
    csvFileStruct = writeTestsListOfDictsToCsvFileStructure(testsLOD)
    csvFileStruct_expected = CsvFileStructure(
      ('site', 'buildName', 'testname', 'issue_tracker_url', 'issue_tracker'),
      [] )
    self.assertEqual(csvFileStruct.headersList, csvFileStruct_expected.headersList)
    self.assertEqual(csvFileStruct.rowsList, csvFileStruct_expected.rowsList)

  def test_tests_1(self):
    testsLOD = [
      {'site':'site1', 'buildName':'build1', 'testname':'test1'},
      ]
    csvFileStruct = writeTestsListOfDictsToCsvFileStructure(testsLOD)
    csvFileStruct_expected = CsvFileStructure(
      ('site', 'buildName', 'testname', 'issue_tracker_url', 'issue_tracker'),
      [ ('site1', 'build1', 'test1', '', '' ),
        ]
      )
    self.assertEqual(csvFileStruct.headersList, csvFileStruct_expected.headersList)
    self.assertEqual(csvFileStruct.rowsList, csvFileStruct_expected.rowsList)

  def test_tests_3(self):
    testsLOD = [
      {'site':'site1', 'buildName':'build1', 'testname':'test1'},
      {'site':'site3', 'buildName':'build3', 'testname':'test3'},
      {'site':'site2', 'buildName':'build2', 'testname':'test2'},
      ]
    csvFileStruct = writeTestsListOfDictsToCsvFileStructure(testsLOD)
    csvFileStruct_expected = CsvFileStructure(
      ('site', 'buildName', 'testname', 'issue_tracker_url', 'issue_tracker'),
      [ ('site1', 'build1', 'test1', '', '' ),
        ('site3', 'build3', 'test3', '', '' ),
        ('site2', 'build2', 'test2', '', '' ),
        ]
      )
    self.assertEqual(csvFileStruct.headersList, csvFileStruct_expected.headersList)
    self.assertEqual(csvFileStruct.rowsList, csvFileStruct_expected.rowsList)


#############################################################################
#
# Test CDashQueryAnalyzeReport.getAndCacheCDashQueryDataOrReadFromCache()
#
#############################################################################


class test_getStandardTestsetTypeInfo(unittest.TestCase):

  def test_twoif(self):
    self.assertEqual(getStandardTestsetTypeInfo('twoif').testsetAcro, 'twoif')

  def test_twip(self):
    self.assertEqual(getStandardTestsetTypeInfo('twip').testsetAcro, 'twip')

  def test_testsetColor(self):
    tsti = getStandardTestsetTypeInfo('twif')
    self.assertEqual(tsti.testsetAcro, 'twif')
    self.assertEqual(tsti.testsetColor, cdashColorFailed())
    tsti = getStandardTestsetTypeInfo('twif', testsetColor=cdashColorMissing())
    self.assertEqual(tsti.testsetAcro, 'twif')
    self.assertEqual(tsti.testsetColor, cdashColorMissing())

  def test_invalid(self):
    threwExcept = True
    try:
      getStandardTestsetTypeInfo('invalid')
      threwExcept = False
    except Exception as errMsg:
      self.assertEqual( str(errMsg),
        "Error, testsetAcro = 'invalid' is not supported!" )
    if not threwExcept:
      self.assertFalse("ERROR: Did not throw an exception")



#############################################################################
#
# Test CDashQueryAnalyzeReport.getTestsetAcroFromTestDict()
#
#############################################################################


def tds(status, issue_tracker):
  td = { u('status') : u(status) }
  if issue_tracker:
    td.update( { u('issue_tracker') : u(issue_tracker) } )
  return td


class test_getTestsetAcroFromTestDict(unittest.TestCase):

  def run_test_case(self, status, issue_tracker, testsetAcro):
    self.assertEqual(getTestsetAcroFromTestDict(tds(status, issue_tracker)),
      testsetAcro )

  def test_pass_cases(self):
    self.run_test_case('Failed', None, 'twoif')
    self.run_test_case('Not Run', None, 'twoinr')
    self.run_test_case('Passed', '#1234', 'twip')
    self.run_test_case('Missing', '#1234', 'twim')
    self.run_test_case('Missing / Failed', '#1234', 'twim')
    self.run_test_case('Failed', '#1234', 'twif')
    self.run_test_case('Not Run', '#1234', 'twinr')

  def test_invalid(self):
    threwExcept = True
    try:
      getTestsetAcroFromTestDict(tds('Passed', None))  # Must have 'issue_tracker'!
      threwExcept = False
    except Exception as errMsg:
      self.assertEqual( str(errMsg),
      "Error, testDict = '{"+stru()+"'status': "+stru()+"'Passed'}' with fields"+\
        " status = 'Passed' and issue_tracker = 'None'"+\
        " is not a supported test-set type!" )
    if not threwExcept:
      self.assertFalse("ERROR: Did not throw an exception")


#############################################################################
#
# Test CDashQueryAnalyzeReport.getAndCacheCDashQueryDataOrReadFromCache()
#
#############################################################################

g_getAndCacheCDashQueryDataOrReadFromCache_data = {
  'keyname1' : "value1",
  'keyname2' : "value2",
   }

def dummyGetCDashData_for_getAndCacheCDashQueryDataOrReadFromCache(
  cdashQueryUrl_expected \
  ):
  if cdashQueryUrl_expected != "dummy-cdash-url":
    raise Exception("Error, cdashQueryUrl_expected != \'dummy-cdash-url\'")  
  return g_getAndCacheCDashQueryDataOrReadFromCache_data

class test_getAndCacheCDashQueryDataOrReadFromCache(unittest.TestCase):

  def test_getAndCacheCDashQueryDataOrReadFromCache_write_cache(self):
    outputCacheDir="test_getAndCacheCDashQueryDataOrReadFromCache_write_cache"
    outputCacheFile=outputCacheDir+"/cachedCDashQueryData.json"
    deleteThenCreateTestDir(outputCacheDir)
    mockExtractCDashApiQueryDataFunctor = MockExtractCDashApiQueryDataFunctor(
       "dummy-cdash-url", g_getAndCacheCDashQueryDataOrReadFromCache_data)
    cdashQueryData = getAndCacheCDashQueryDataOrReadFromCache(
      "dummy-cdash-url", outputCacheFile,
      useCachedCDashData=False,
      verbose=False,
      extractCDashApiQueryData_in=mockExtractCDashApiQueryDataFunctor
      )
    self.assertEqual(cdashQueryData, g_getAndCacheCDashQueryDataOrReadFromCache_data)
    with open(outputCacheFile, 'r') as inFile:
      cdashQueryData_cache = eval(inFile.read())
    self.assertEqual(cdashQueryData_cache, g_getAndCacheCDashQueryDataOrReadFromCache_data)

  def test_getAndCacheCDashQueryDataOrReadFromCache_read_cache(self):
    outputCacheDir="test_getAndCacheCDashQueryDataOrReadFromCache_read_cache"
    outputCacheFile=outputCacheDir+"/cachedCDashQueryData.json"
    deleteThenCreateTestDir(outputCacheDir)
    with open(outputCacheFile, 'w') as outFile:
      outFile.write(str(g_getAndCacheCDashQueryDataOrReadFromCache_data))
    cdashQueryData = getAndCacheCDashQueryDataOrReadFromCache(
      "dummy-cdash-url", outputCacheFile,
      useCachedCDashData=True,
      verbose=False,
      )
    self.assertEqual(cdashQueryData, g_getAndCacheCDashQueryDataOrReadFromCache_data)

  def test_getAndCacheCDashQueryDataOrReadFromCache_always_read_cache(self):
    outputCacheDir="test_getAndCacheCDashQueryDataOrReadFromCache_always_read_cache"
    outputCacheFile=outputCacheDir+"/cachedCDashQueryData.json"
    deleteThenCreateTestDir(outputCacheDir)
    with open(outputCacheFile, 'w') as outFile:
      outFile.write(str(g_getAndCacheCDashQueryDataOrReadFromCache_data))
    cdashQueryData = getAndCacheCDashQueryDataOrReadFromCache(
      "dummy-cdash-url", outputCacheFile,
      useCachedCDashData=True,
      verbose=False,
      )
    self.assertEqual(cdashQueryData, g_getAndCacheCDashQueryDataOrReadFromCache_data)


#############################################################################
#
# Test CDashQueryAnalyzeReport URL functions
#
#############################################################################

class test_CDashQueryAnalyzeReport_UrlFuncs(unittest.TestCase):

  def test_getCDashIndexQueryUrl(self):
    cdashIndexQueryUrl = getCDashIndexQueryUrl(
      "site.com/cdash", "project-name", "2015-12-21", "filtercount=1&morestuff" )
    cdashIndexQueryUrl_expected = \
      "site.com/cdash/api/v1/index.php?project=project-name&date=2015-12-21"+\
      "&filtercount=1&morestuff"
    self.assertEqual(cdashIndexQueryUrl, cdashIndexQueryUrl_expected)

  def test_getCDashIndexQueryUrl_no_date(self):
    cdashIndexQueryUrl = getCDashIndexQueryUrl(
      "site.com/cdash", "project-name", None, "filtercount=1&morestuff" )
    cdashIndexQueryUrl_expected = \
      "site.com/cdash/api/v1/index.php?project=project-name"+\
      "&filtercount=1&morestuff"
    self.assertEqual(cdashIndexQueryUrl, cdashIndexQueryUrl_expected)

  def test_getCDashIndexBrowserUrl(self):
    cdashIndexQueryUrl = getCDashIndexBrowserUrl(
      "site.com/cdash", "project-name", "2015-12-21", "filtercount=1&morestuff" )
    cdashIndexQueryUrl_expected = \
      "site.com/cdash/index.php?project=project-name&date=2015-12-21"+\
      "&filtercount=1&morestuff"
    self.assertEqual(cdashIndexQueryUrl, cdashIndexQueryUrl_expected)

  def test_getCDashIndexBrowserUrl_no_date(self):
    cdashIndexQueryUrl = getCDashIndexBrowserUrl(
      "site.com/cdash", "project-name", None, "filtercount=1&morestuff" )
    cdashIndexQueryUrl_expected = \
      "site.com/cdash/index.php?project=project-name"+\
      "&filtercount=1&morestuff"
    self.assertEqual(cdashIndexQueryUrl, cdashIndexQueryUrl_expected)

  def test_getCDashQueryTestsQueryUrl(self):
    cdashIndexQueryUrl = getCDashQueryTestsQueryUrl(
      "site.com/cdash", "project-name", "2015-12-21", "filtercount=1&morestuff" )
    cdashIndexQueryUrl_expected = \
      "site.com/cdash/api/v1/queryTests.php?project=project-name&date=2015-12-21"+\
      "&filtercount=1&morestuff"
    self.assertEqual(cdashIndexQueryUrl, cdashIndexQueryUrl_expected)

  def test_getCDashQueryTestsQueryUrl_project_name_space(self):
    cdashIndexQueryUrl = getCDashQueryTestsQueryUrl(
      "site.com/cdash", "project name", "2015-12-21", "filtercount=1&morestuff" )
    cdashIndexQueryUrl_expected = \
      "site.com/cdash/api/v1/queryTests.php?project=project%20name&date=2015-12-21"+\
      "&filtercount=1&morestuff"
    self.assertEqual(cdashIndexQueryUrl, cdashIndexQueryUrl_expected)

  def test_getCDashQueryTestsQueryUrl_no_date(self):
    cdashIndexQueryUrl = getCDashQueryTestsQueryUrl(
      "site.com/cdash", "project-name", None, "filtercount=1&morestuff" )
    cdashIndexQueryUrl_expected = \
      "site.com/cdash/api/v1/queryTests.php?project=project-name"+\
      "&filtercount=1&morestuff"
    self.assertEqual(cdashIndexQueryUrl, cdashIndexQueryUrl_expected)

  def test_getCDashQueryTestsBrowserUrl(self):
    cdashIndexQueryUrl = getCDashQueryTestsBrowserUrl(
      "site.com/cdash", "project-name", "2015-12-21", "filtercount=1&morestuff" )
    cdashIndexQueryUrl_expected = \
      "site.com/cdash/queryTests.php?project=project-name&date=2015-12-21"+\
      "&filtercount=1&morestuff"
    self.assertEqual(cdashIndexQueryUrl, cdashIndexQueryUrl_expected)

  def test_getCDashQueryTestsBrowserUrl_no_date(self):
    cdashIndexQueryUrl = getCDashQueryTestsBrowserUrl(
      "site.com/cdash", "project-name", None, "filtercount=1&morestuff" )
    cdashIndexQueryUrl_expected = \
      "site.com/cdash/queryTests.php?project=project-name"+\
      "&filtercount=1&morestuff"
    self.assertEqual(cdashIndexQueryUrl, cdashIndexQueryUrl_expected)


#############################################################################
#
# Test CDashQueryAnalyzeReport.extendCDashIndexBuildDict()
#
#############################################################################

# This summary build has just the minimal required fields
g_singleBuildPassesExtended = {
  'group':'groupName',
  'site':'siteName',
  'buildname':"buildName",
  'update': {'errors':0},
  'configure':{'error': 0},
  'compilation':{'error':0},
  'test': {'fail':0, 'notrun':0},
  'extra-stuff':'stuff',
  }

# Single build with extra stuff
g_singleBuildPassesRaw = {
  'site':'siteName',
  'buildname':"buildName",
  'update': {'errors':0},
  'configure':{'error': 0},
  'compilation':{'error':0},
  'test': {'fail':0, 'notrun':0},
  'extra-stuff':'stuff',
  }

class test_extendCDashIndexBuildDict(unittest.TestCase):

  def test_extendCDashIndexBuildDict_full(self):
    buildSummary = extendCDashIndexBuildDict(g_singleBuildPassesRaw, "groupName")
    self.assertEqual(buildSummary, g_singleBuildPassesExtended)

  def test_extendCDashIndexBuildDict_missing_update(self):
    fullCDashIndexBuild_in = copy.deepcopy(g_singleBuildPassesRaw)
    del fullCDashIndexBuild_in['update']
    buildSummary = extendCDashIndexBuildDict(fullCDashIndexBuild_in, "groupName")
    buildSummary_expected = copy.deepcopy(g_singleBuildPassesExtended)
    del buildSummary_expected['update']
    self.assertEqual(buildSummary, buildSummary_expected)

  def test_extendCDashIndexBuildDict_missing_configure(self):
    fullCDashIndexBuild_in = copy.deepcopy(g_singleBuildPassesRaw)
    del fullCDashIndexBuild_in['configure']
    buildSummary = extendCDashIndexBuildDict(fullCDashIndexBuild_in, "groupName")
    buildSummary_expected = copy.deepcopy(g_singleBuildPassesExtended)
    del buildSummary_expected['configure']
    self.assertEqual(buildSummary, buildSummary_expected)


#############################################################################
#
# Test CDashQueryAnalyzeReport.flattenCDashIndexBuildsToListOfDicts()
#
#############################################################################

# This file was taken from an actual CDash query and then modified a little to
# make for better testing.
g_fullCDashIndexBuildsJson = \
  eval(open(g_testBaseDir+'/cdash_index_query_data.json', 'r').read())
#print("\ng_fullCDashIndexBuildsJson:")
#g_pp.pprint(g_fullCDashIndexBuildsJson)

# This file was manually created from the above file to match what the reduced
# builds should be.
g_flattenedCDashIndexBuilds_expected = \
  eval(open(g_testBaseDir+'/cdash_index_query_data.flattened.json', 'r').read())
#print("\ng_flattenedCDashIndexBuilds_expected:")
#g_pp.pprint(g_flattenedCDashIndexBuilds_expected)

class test_flattenCDashIndexBuildsToListOfDicts(unittest.TestCase):

  def test_flattenCDashIndexBuildsToListOfDicts(self):
    flattendedCDashIndexBuilds = \
      flattenCDashIndexBuildsToListOfDicts(g_fullCDashIndexBuildsJson)
    #pp.pprint(flattendedCDashIndexBuilds)
    self.assertEqual(
      len(flattendedCDashIndexBuilds), len(g_flattenedCDashIndexBuilds_expected))
    for i in range(0, len(flattendedCDashIndexBuilds)):
      self.assertEqual(flattendedCDashIndexBuilds[i],
        g_flattenedCDashIndexBuilds_expected[i])


#############################################################################
#
# Test CDashQueryAnalyzeReport.flattenCDashQueryTestsToListOfDicts()
#
#############################################################################

# This file was taken from an actual CDash query and then modified a little to
# make for better testing.
g_fullCDashQueryTestsJson = \
  eval(open(g_testBaseDir+'/cdash_query_tests_data.json', 'r').read())
#print("g_fullCDashQueryTestsJson:")
#g_pp.pprint(g_fullCDashQueryTestsJson)

# This file was manually created from the above file to match what the reduced
# builds should be.
g_testsListOfDicts_expected = \
  eval(open(g_testBaseDir+'/cdash_query_tests_data.flattened.json', 'r').read())
#print("g_testsListOfDicts_expected:")
#g_pp.pprint(g_testsListOfDicts_expected)

class test_flattenCDashQueryTestsToListOfDicts(unittest.TestCase):

  def test_flattenCDashQueryTestsToListOfDicts(self):
    testsListOfDicts = \
      flattenCDashQueryTestsToListOfDicts(g_fullCDashQueryTestsJson)
    #pp.pprint(testsListOfDicts)
    self.assertEqual(
      len(testsListOfDicts), len(g_testsListOfDicts_expected))
    for i in range(0, len(testsListOfDicts)):
      self.assertEqual(testsListOfDicts[i], g_testsListOfDicts_expected[i])


#############################################################################
#
# Test CDashQueryAnalyzeReport.createLookupDictForListOfDicts()
#
#############################################################################

g_buildsListForExpectedBuilds = [
  { 'group':'group1', 'site':'site1', 'buildname':'build1', 'data':'val1' },
  { 'group':'group1', 'site':'site1', 'buildname':'build2', 'data':'val2' },
  { 'group':'group1', 'site':'site2', 'buildname':'build3', 'data':'val3' },
  { 'group':'group2', 'site':'site1', 'buildname':'build1', 'data':'val4' },
  { 'group':'group2', 'site':'site3', 'buildname':'build4', 'data':'val5' },
  ]

g_buildLookupDictForExpectedBuilds = {
  'group1' : {
    'site1' : {
      'build1':{
        'dict':{'group':'group1','site':'site1','buildname':'build1','data':'val1'},
        'idx':0 },
      'build2':{
        'dict':{'group':'group1','site':'site1','buildname':'build2','data':'val2'},
        'idx':1 },
      },
    'site2' : {
      'build3':{
        'dict':{'group':'group1','site':'site2','buildname':'build3','data':'val3'},
        'idx':2 },
      },
    },
  'group2' : {
    'site1' : {
      'build1':{
        'dict':{'group':'group2','site':'site1','buildname':'build1','data':'val4'},
        'idx':3 },
      },
    'site3' : {
      'build4':{
        'dict':{'group':'group2','site':'site3','buildname':'build4','data':'val5'},
        'idx':4 },
      },
    },
  }


class test_createLookupDictForListOfDicts(unittest.TestCase):

  def test_unique_dicts(self):
    buildLookupDict = createLookupDictForListOfDicts(
      g_buildsListForExpectedBuilds,
      ['group', 'site', 'buildname'] )
    #print("\nbuildLookupDict:")
    #g_pp.pprint(buildLookupDict)
    #print("\ng_buildLookupDictForExpectedBuilds:")
    #g_pp.pprint(g_buildLookupDictForExpectedBuilds)
    self.assertEqual(buildLookupDict, g_buildLookupDictForExpectedBuilds)

  def test_duplicate_dicts_error(self):
    listOfDicts = copy.deepcopy(g_buildsListForExpectedBuilds)
    origDictEle = g_buildsListForExpectedBuilds[0]
    newDictEle = copy.deepcopy(g_buildsListForExpectedBuilds[0])
    newDictEle['data'] = 'new_data_val1'
    listOfDicts.append(newDictEle)
    try:
      buildLookupDict = createLookupDictForListOfDicts(
        listOfDicts, ['group', 'site', 'buildname'] )
      self.assertEqual("Did not throw exception!", "no it did not!")
    except Exception as errMsg:
      self.assertEqual( str(errMsg),
        "Error, The element\n\n"+\
        "    listOfDicts[5] =\n\n"+\
        "      "+sorted_dict_str(newDictEle)+"\n\n"+\
        "  has duplicate values for the list of keys\n\n"+\
        "    ['group', 'site', 'buildname']\n\n"+\
        "  with the element already added\n\n"+\
        "    listOfDicts[0] =\n\n"+\
        "      "+sorted_dict_str(origDictEle)+"\n\n"+\
        "  and differs by at least the key/value pair\n\n"+\
        "    listOfDicts[5]['data'] = 'new_data_val1' != listOfDicts[0]['data'] = 'val1'" )

  def test_exact_duplicate_dicts_with_removal(self):
    listOfDicts = copy.deepcopy(g_buildsListForExpectedBuilds)
    origDictEle = g_buildsListForExpectedBuilds[0]
    newDictEle = copy.deepcopy(g_buildsListForExpectedBuilds[2])
    listOfDicts.insert(3, newDictEle)
    newDictEle = copy.deepcopy(g_buildsListForExpectedBuilds[0])
    listOfDicts.insert(1, newDictEle)
    newDictEle = copy.deepcopy(g_buildsListForExpectedBuilds[0])
    listOfDicts.insert(2, newDictEle)
    newDictEle = copy.deepcopy(g_buildsListForExpectedBuilds[4])
    listOfDicts.append(newDictEle)
    buildLookupDict = createLookupDictForListOfDicts(
      listOfDicts, ['group', 'site', 'buildname'], removeExactDuplicateElements=True )
    self.assertEqual(buildLookupDict, g_buildLookupDictForExpectedBuilds)


#############################################################################
#
# Test CDashQueryAnalyzeReport.lookupDictGivenLookupDict()
#
#############################################################################

def gsb(groupName, siteName, buildName):
  return {'group':groupName, 'site':siteName, 'buildname':buildName}

def luDictData(groupName, siteName, buildName, buildLookupDict):
  dictFound = lookupDictGivenLookupDict(buildLookupDict,
    ['group', 'site', 'buildname'], [groupName, siteName, buildName] )
  if not dictFound : return None
  return dictFound.get('data')

def luDictIdxData(groupName, siteName, buildName, buildLookupDict):
  (dictFound, idxFound) = lookupDictGivenLookupDict(buildLookupDict,
    ('group', 'site', 'buildname'), (groupName, siteName, buildName),
    alsoReturnIdx=True )
  if not dictFound : return (None, None)
  return (idxFound, dictFound.get('data'))
     
class test_lookupDictGivenLookupDict(unittest.TestCase):

  def test_bad_list_len(self):
    lud = createLookupDictForListOfDicts(g_buildsListForExpectedBuilds,
      ['group', 'site', 'buildname'] )
    try:
      rtn = lookupDictGivenLookupDict(lud, ['group', 'site', 'buildname'],
      ['group1', 'site1'])
      self.assertFalse("Error, did not throw!")
    except Exception as errMsg:
      self.assertEqual( str(errMsg),
        "Error, len(listOfKeys)=3 != len(listOfValues)=2 where"+\
        " listOfKeys=['group', 'site', 'buildname'] and"+\
        " listOfValues=['group1', 'site1']!" )

  def test_dict_only(self):
    lud = createLookupDictForListOfDicts(g_buildsListForExpectedBuilds,
      ['group', 'site', 'buildname'] )
    self.assertEqual(luDictData('group1','site1','build1',lud), 'val1')
    self.assertEqual(luDictData('group1','site1','build2',lud), 'val2')
    self.assertEqual(luDictData('group1','site2','build3',lud), 'val3')
    self.assertEqual(luDictData('group2','site1','build1',lud), 'val4')
    self.assertEqual(luDictData('group2','site3','build4',lud), 'val5')
    self.assertEqual(luDictData('group2','site3','build1',lud), None)
    self.assertEqual(luDictData('group2','site4','build1',lud), None)
    self.assertEqual(luDictData('group3','site1','build1',lud), None)

  def test_dict_and_idx(self):
    lud = createLookupDictForListOfDicts(g_buildsListForExpectedBuilds,
      ['group', 'site', 'buildname'] )
    self.assertEqual(luDictIdxData('group1','site1','build1', lud), (0,'val1'))
    self.assertEqual(luDictIdxData('group1','site1','build2', lud), (1,'val2'))
    self.assertEqual(luDictIdxData('group1','site2','build3', lud), (2,'val3'))
    self.assertEqual(luDictIdxData('group2','site1','build1', lud), (3,'val4'))
    self.assertEqual(luDictIdxData('group2','site3','build4', lud), (4,'val5'))
    self.assertEqual(luDictIdxData('group2','site3','build1', lud), (None, None))
    self.assertEqual(luDictIdxData('group2','site4','build1', lud), (None, None))
    self.assertEqual(luDictIdxData('group3','site1','build1', lud), (None, None))


#############################################################################
#
# Test CDashQueryAnalyzeReport.SearchableListOfDicts
#
#############################################################################

g_buildsListForExpectedBuildsUniqSiteBuildName = [
  { 'group':'group1', 'site':'site1', 'buildname':'build1', 'data':'val1' },
  { 'group':'group1', 'site':'site1', 'buildname':'build2', 'data':'val2' },
  { 'group':'group1', 'site':'site2', 'buildname':'build3', 'data':'val3' },
  { 'group':'group2', 'site':'site3', 'buildname':'build4', 'data':'val5' },
  ]

def slodLuData(slod, group, site, buildName):
  dictFound = slod.lookupDictGivenKeyValueDict(gsb(group, site, buildName))
  if not dictFound : return None
  return dictFound.get('data')

def slodLuIdxData(slod, group, site, buildName):
  (dictFound, idxFound) = slod.lookupDictGivenKeyValueDict(
    gsb(group, site, buildName), alsoReturnIdx=True)
  if not dictFound : return (None, None)
  return (idxFound, dictFound.get('data'))

def tsb(site, buildName):
  return {'site':site, 'buildName':buildName}

def slodmLuData(slodm, site, buildName):
  dictFound = slodm.lookupDictGivenKeyValueDict(tsb(site, buildName))
  if not dictFound : return None
  return dictFound.get('data')

def slodmLuIdxData(slodm, site, buildName):
  (dictFound, idxFound) = slodm.lookupDictGivenKeyValueDict(tsb(site, buildName),
    alsoReturnIdx=True)
  if not dictFound : return (None, None)
  return (idxFound, dictFound.get('data'))

class test_SearchableListOfDicts(unittest.TestCase):

  def test_basic(self):
    listOfKeys = ['group', 'site', 'buildname'] 
    slod = SearchableListOfDicts(g_buildsListForExpectedBuilds, listOfKeys)
    self.assertEqual(slod.getListOfDicts(), g_buildsListForExpectedBuilds)
    self.assertEqual(slod.getListOfKeys(), listOfKeys)
    self.assertEqual(slod.getKeyMapList(), None)
    self.assertEqual(len(slod), len(g_buildsListForExpectedBuilds))
    self.assertEqual(slod[0], g_buildsListForExpectedBuilds[0])
    self.assertEqual(slod[3], g_buildsListForExpectedBuilds[3])
    self.assertEqual(
      slod.lookupDictGivenKeyValuesList(('group1','site2','build3'))['data'], 'val3')
    (dictR,idxR)=slod.lookupDictGivenKeyValuesList(('group1','site2','build3'), True)
    self.assertEqual((dictR['data'],idxR), ('val3',2))
    self.assertEqual(
      slod.lookupDictGivenKeyValuesList(('group2','site4','build1')), None)
    (dictR,idxR)=slod.lookupDictGivenKeyValuesList(('group2','site4','build1'), True)
    self.assertEqual((idxR,dictR), (None, None))
    self.assertEqual(slodLuData(slod, 'group1','site1','build1'), 'val1')
    self.assertEqual(slodLuData(slod, 'group1','site2','build3'), 'val3')
    self.assertEqual(slodLuData(slod, 'group2','site4','build1'), None)
    self.assertEqual(slodLuIdxData(slod, 'group1','site1','build1'), (0,'val1'))
    self.assertEqual(slodLuIdxData(slod, 'group1','site2','build3'), (2,'val3'))
    self.assertEqual(slodLuIdxData(slod, 'group2','site4','build1'), (None, None))

  def test_with_key_map(self):
    listOfKeys = ['site', 'buildname'] 
    keyMapList = ['site', 'buildName'] 
    slodm = SearchableListOfDicts(g_buildsListForExpectedBuildsUniqSiteBuildName,
      listOfKeys, removeExactDuplicateElements=False, keyMapList=keyMapList)
    self.assertEqual(slodm.getListOfDicts(),
      g_buildsListForExpectedBuildsUniqSiteBuildName)
    self.assertEqual(slodm.getListOfKeys(), listOfKeys)
    self.assertEqual(slodm.getKeyMapList(), keyMapList)
    self.assertEqual(slodmLuData(slodm, 'site1', 'build1'), 'val1')
    self.assertEqual(slodmLuData(slodm, 'site2', 'build3'), 'val3')
    self.assertEqual(slodmLuData(slodm, 'site4', 'build1'), None)
    self.assertEqual(slodmLuIdxData(slodm, 'site1', 'build1'), (0,'val1'))
    self.assertEqual(slodmLuIdxData(slodm, 'site2', 'build3'), (2,'val3'))
    self.assertEqual(slodmLuIdxData(slodm, 'site4','build1'), (None, None))
    self.assertEqual(
      slodm.lookupDictGivenKeyValuesList(('site2','build3'))['data'], 'val3')
    (dictR,idxR)=slodm.lookupDictGivenKeyValuesList(('site2','build3'), True)
    self.assertEqual((dictR['data'],idxR), ('val3',2))
    self.assertEqual(
      slodm.lookupDictGivenKeyValuesList(('site4','build1')), None)
    (dictR,idxR)=slodm.lookupDictGivenKeyValuesList(('site4','build1'), True)
    self.assertEqual((idxR,dictR), (None, None))

  def test_exact_duplicate_ele_with_removal(self):
    listOfDicts = copy.deepcopy(g_buildsListForExpectedBuilds)
    newDictEle = copy.deepcopy(g_buildsListForExpectedBuilds[2])
    listOfDicts.insert(3, newDictEle)
    newDictEle = copy.deepcopy(g_buildsListForExpectedBuilds[0])
    listOfDicts.insert(1, newDictEle)
    newDictEle = copy.deepcopy(g_buildsListForExpectedBuilds[0])
    listOfDicts.insert(2, newDictEle)
    newDictEle = copy.deepcopy(g_buildsListForExpectedBuilds[4])
    listOfDicts.append(newDictEle)
    #print("\nlistOfDicts:")
    #g_pp.pprint(listOfDicts)
    listOfKeys = ['group', 'site', 'buildname'] 
    slod = SearchableListOfDicts(listOfDicts, listOfKeys,
      removeExactDuplicateElements=True)
    #print("\nslod.getListOfDicts():")
    #g_pp.pprint(slod.getListOfDicts())
    #print("\ng_buildsListForExpectedBuilds:")
    #g_pp.pprint(g_buildsListForExpectedBuilds)
    self.assertEqual(slod.getListOfDicts(), g_buildsListForExpectedBuilds)
    self.assertEqual(listOfDicts, g_buildsListForExpectedBuilds)
    self.assertEqual(len(slod), len(g_buildsListForExpectedBuilds))
    self.assertEqual(slod[0], g_buildsListForExpectedBuilds[0])
    self.assertEqual(slod[3], g_buildsListForExpectedBuilds[3])
    self.assertEqual(slodLuData(slod, 'group1','site1','build1'), 'val1')
    self.assertEqual(slodLuData(slod, 'group1','site2','build3'), 'val3')
    self.assertEqual(slodLuData(slod, 'group2','site4','build1'), None)

  def test_iterator(self):
    slod = SearchableListOfDicts(g_buildsListForExpectedBuilds,
      ['group', 'site', 'buildname'])
    i = 0
    for dictEle in slod:
      self.assertEqual(dictEle, g_buildsListForExpectedBuilds[i])
      i += 1  

  def test_in(self):
    slod = SearchableListOfDicts(g_buildsListForExpectedBuilds,
      ['group', 'site', 'buildname'])
    self.assertEqual(g_buildsListForExpectedBuilds[0] in slod, True)
    self.assertEqual(g_buildsListForExpectedBuilds[2] in slod, True)
    dummyDict = copy.deepcopy(g_buildsListForExpectedBuilds[0])
    dummyDict['data'] = 'different_val'
    self.assertEqual(dummyDict in slod, False)

  def test_indirect_update(self):
    origListOfDicts = copy.deepcopy(g_buildsListForExpectedBuilds)
    slod = SearchableListOfDicts( origListOfDicts, ['group', 'site', 'buildname'])
    buildDict = slod.lookupDictGivenKeyValuesList(['group1','site2','build3'])
    self.assertEqual(buildDict['data'], "val3")
    buildDict['data'] = "new_data"
    self.assertEqual(origListOfDicts[2]['data'], "new_data")


#############################################################################
#
# Test CDashQueryAnalyzeReport.getMissingExpectedBuildsList()
#
#############################################################################

def gsb_pass(group, site, build):
  buildDict = {
    'group':group,
    'site':site,
    'buildname':build,
    'update': {'errors':0},
    'configure':{'error': 0},
    'compilation':{'error':0, 'time':'20m10s'},
    'test': {'pass':1, 'fail':0, 'notrun':0},
    'extra-stuff':'stuff',
    'data':'val1' }
  return buildDict

g_buildsListForMissingExpectedBuilds = [
  gsb_pass('group1', 'site1', 'build1'),
  gsb_pass('group1', 'site1', 'build2'),
  gsb_pass('group1', 'site2', 'build3'),
  gsb_pass('group2', 'site1', 'build1'),
  gsb_pass('group2', 'site3', 'build4'),
  ]

g_expectedBuildsList = [
  gsb('group1', 'site2', 'build3'),
  gsb('group2', 'site3', 'build4'),
  gsb('group2', 'site3', 'build8'),   # This is always a missing expected build
  ]

class test_getMissingExpectedBuildsList(unittest.TestCase):

  def test_missing_2_no_conf_no_build_1_notests_1(self):
    listOfBuilds = copy.deepcopy(g_buildsListForMissingExpectedBuilds)
    #print("\nlistOfBuilds:"); g_pp.pprint(listOfBuilds)
    slob = createSearchableListOfBuilds(listOfBuilds)
    # Remove test results from one of the builds
    del slob.lookupDictGivenKeyValuesList(('group1','site2','build3'))['test']
    #print("\nlistOfBuilds:"); g_pp.pprint(listOfBuilds)
    # Remove build results from one of the builds
    buildDict = slob.lookupDictGivenKeyValuesList(('group2','site3','build4'))
    del buildDict['configure']
    del buildDict['compilation']
    #print("\nlistOfBuilds:"); g_pp.pprint(listOfBuilds)
    #
    missingExpectedBuildsList = getMissingExpectedBuildsList(slob, g_expectedBuildsList)
    #print("\nmissingExpectedBuildsList:"); g_pp.pprint(missingExpectedBuildsList)
    self.assertEqual(len(missingExpectedBuildsList), 3)
    #
    expectedBuildDict = gsb('group1', 'site2', 'build3')
    expectedBuildDict.update({'status':"Missing tests"})
    self.assertEqual(missingExpectedBuildsList[0], expectedBuildDict)
    #
    expectedBuildDict = gsb('group2', 'site3', 'build4')
    expectedBuildDict.update({'status':"Missing configure, build"})
    self.assertEqual(missingExpectedBuildsList[1], expectedBuildDict)
    #
    expectedBuildDict = gsb('group2', 'site3', 'build8')
    expectedBuildDict.update({'status':"Missing ALL"})
    self.assertEqual(missingExpectedBuildsList[2], expectedBuildDict)

  def test_compilation_time_zero_1_zero_tests_1(self):
    listOfBuilds = copy.deepcopy(g_buildsListForMissingExpectedBuilds)
    slob = createSearchableListOfBuilds(listOfBuilds)
    # Set test results to 0 passing tests and ensure that this not considered missing
    slob.lookupDictGivenKeyValuesList(('group1','site2','build3')).update(
      {'test':{'pass':0}})
    # Set compilation time to '0s' and remove test results which is a sign
    # that the build did not happen! (NOTE: This is a heuristic since it is
    # possible to have a zero build time and still have build results
    # submitted to CDash.  However, it is unlikely that the build time is zero
    # and just happens to be missing test results.  In that case, it is likely
    # that the job that was doing the build died and never submitted build
    # results in the first place.)
    buildDict = slob.lookupDictGivenKeyValuesList(('group2','site3','build4'))
    buildDict.update({'compilation':{'time':'0s'}})
    del buildDict['test']
    #
    missingExpectedBuildsList = getMissingExpectedBuildsList(slob, g_expectedBuildsList)
    #print("\nmissingExpectedBuildsList:"); g_pp.pprint(missingExpectedBuildsList)
    self.assertEqual(len(missingExpectedBuildsList), 2)
    #
    expectedBuildDict = gsb('group2', 'site3', 'build4')
    expectedBuildDict.update({'status':"Missing build, tests"})
    self.assertEqual(missingExpectedBuildsList[0], expectedBuildDict)
    #
    expectedBuildDict = gsb('group2', 'site3', 'build8')
    expectedBuildDict.update({'status':"Missing ALL"})
    self.assertEqual(missingExpectedBuildsList[1], expectedBuildDict)


#############################################################################
#
# Test CDashQueryAnalyzeReport.downloadBuildsOffCDashAndFlatten()
#
#############################################################################

class test_downloadBuildsOffCDashAndFlatten(unittest.TestCase):

  def test_allBuilds(self):
    # Define dummy CDash filter data
    cdashUrl = "site.come/cdash"
    projectName = "projectName"
    date = "YYYY-MM-DD"
    buildFilters = "build&filters"
    cdashIndexBuildsQueryUrl = \
      getCDashIndexQueryUrl(cdashUrl,  projectName, date, buildFilters)
    # Define mock object to return the data
    mockExtractCDashApiQueryDataFunctor = MockExtractCDashApiQueryDataFunctor(
       cdashIndexBuildsQueryUrl, g_fullCDashIndexBuildsJson )
    # Get the mock data off of CDash
    flattendedCDashIndexBuilds = downloadBuildsOffCDashAndFlatten(
      cdashIndexBuildsQueryUrl,
      fullCDashIndexBuildsJsonCacheFile=None,
      useCachedCDashData=False,
      verbose=False,
      extractCDashApiQueryData_in=mockExtractCDashApiQueryDataFunctor )
    # Assert the data returned is correct
    #g_pp.pprint(flattendedCDashIndexBuilds)
    self.assertEqual(
      len(flattendedCDashIndexBuilds), len(g_flattenedCDashIndexBuilds_expected))
    for i in range(0, len(flattendedCDashIndexBuilds)):
      self.assertEqual(flattendedCDashIndexBuilds[i], g_flattenedCDashIndexBuilds_expected[i])


#############################################################################
#
# Test CDashQueryAnalyzeReport.downloadTestsOffCDashQueryTestsAndFlatten()
#
#############################################################################

class test_downloadTestsOffCDashQueryTestsAndFlatten(unittest.TestCase):

  def test_all_tests(self):
    # Define dummy CDash filter data
    cdashUrl = "site.come/cdash"
    projectName = "projectName"
    date = "YYYY-MM-DD"
    nonpassingTestsFilters = "tests&filters"
    # cdash/api/v1/queryTests.php URL
    nonpassingTestsQueryUrl = getCDashQueryTestsQueryUrl(
      cdashUrl, projectName, date, nonpassingTestsFilters)
    # Define mock object to return the data
    mockExtractCDashApiQueryDataFunctor = MockExtractCDashApiQueryDataFunctor(
       nonpassingTestsQueryUrl, g_fullCDashQueryTestsJson )
    # Get the mock data off of CDash
    testsListOfDicts = downloadTestsOffCDashQueryTestsAndFlatten(
      nonpassingTestsQueryUrl,
      fullCDashQueryTestsJsonCacheFile=None,
      useCachedCDashData=False,
      verbose=False,
      extractCDashApiQueryData_in=mockExtractCDashApiQueryDataFunctor )
    # Assert the data returned is correct
    #g_pp.pprint(testsListOfDicts)
    self.assertEqual(
      len(testsListOfDicts), len(g_testsListOfDicts_expected))
    for i in range(0, len(testsListOfDicts)):
      self.assertEqual(testsListOfDicts[i], g_testsListOfDicts_expected[i])


#############################################################################
#
# Test CDashQueryAnalyzeReport.MatchDictKeysValuesFunctor
#
#############################################################################

def sbtiturlit(site, buildName, testname, it_url, it):
  return { 'site':site, 'buildName':buildName, 'testname':testname,
    'issue_tracker_url':it_url, 'issue_tracker':it }

g_testsWtihIssueTrackersList = [
  sbtiturlit('site1', 'build1', 'test1', 'url1', '#1111'),
  sbtiturlit('site1', 'build1', 'test2', 'url2', '#1112'),
  sbtiturlit('site2', 'build2', 'test1', 'url3', '#1113'),
  sbtiturlit('site2', 'build1', 'test5', 'url5', '#1114'),
  ]

def sbt(site, buildName, testname):
  return { 'site':site, 'buildName':buildName, 'testname':testname }

class test_MatchDictKeysValuesFunctor(unittest.TestCase):

  def test_1(self):
    testsWithIssueTrackerSLOD = createSearchableListOfTests(g_testsWtihIssueTrackersList)
    matchFunctor = MatchDictKeysValuesFunctor(testsWithIssueTrackerSLOD)
    self.assertEqual(matchFunctor(sbt('site1','build1','test1')), True)
    self.assertEqual(matchFunctor(sbt('site2','build2','test1')), True)
    self.assertEqual(matchFunctor(sbt('site2','build2','test7')), False)
    self.assertEqual(matchFunctor(sbt('site1','build2','test3')), False)
    self.assertEqual(matchFunctor(sbt('site2','build1','test5')), True)


#############################################################################
#
# Test CDashQueryAnalyzeReport.AddIssueTrackerInfoToTestDictFunctor
#
#############################################################################

def aitf_itf(aitf, site, buildName, testname):
  testDictTransformed = aitf(sbt(site, buildName, testname))
  return [
    testDictTransformed['issue_tracker_url'],
    testDictTransformed['issue_tracker'] 
    ]

class test_AddIssueTrackerInfoToTestDictFunctor(unittest.TestCase):

  def test_demo(self):
    # My initial test dict like gotten directly from CDash with no issue
    # tracker info
    initailTestDict = { 'site':'site1', 'buildName':'build1', 'testname':'test1',
     'other_data':'great' }
    # Create a functor that can add matching issue tracker info to a test dict
    # that may not have issue tracker info.
    testsWithIssueTrackerSLOD = createSearchableListOfTests(g_testsWtihIssueTrackersList)
    addIssueTrackerInfoFunctor = \
      AddIssueTrackerInfoToTestDictFunctor(testsWithIssueTrackerSLOD)
    # Use the functor to add the matching issue tracker info
    addIssueTrackerInfoFunctor(initailTestDict)
    # Check to make sure it added correct issue tracker data
    self.assertEqual(
      initailTestDict,
      { 'site':'site1', 'buildName':'build1', 'testname':'test1',
      'other_data':'great', 'issue_tracker_url':'url1', 'issue_tracker':'#1111' }
      )

  def test_pass(self):
    testsWithIssueTrackerSLOD = createSearchableListOfTests(g_testsWtihIssueTrackersList)
    aitf = AddIssueTrackerInfoToTestDictFunctor(testsWithIssueTrackerSLOD)
    self.assertEqual(aitf_itf(aitf, 'site1','build1','test1'), ['url1','#1111'])
    self.assertEqual(aitf_itf(aitf, 'site2','build2','test1'), ['url3','#1113'])

  def test_add_empty(self):
    testsWithIssueTrackerSLOD = createSearchableListOfTests(g_testsWtihIssueTrackersList)
    aitf = AddIssueTrackerInfoToTestDictFunctor(testsWithIssueTrackerSLOD)
    self.assertEqual(aitf_itf(aitf, 'site2','build2','test9'), ['',''])

  def test_missing_error(self):
    testsWithIssueTrackerSLOD = createSearchableListOfTests(g_testsWtihIssueTrackersList)
    aitf = AddIssueTrackerInfoToTestDictFunctor(testsWithIssueTrackerSLOD, False)
    dict_inout=sbt('site2','build2','test9')
    try:
      aitf(dict_inout)
      self.assertEqual("Error, did not thorw exception", "No it did not!")
    except Exception as errMsg:
      self.assertEqual(str(errMsg),
        "Error, testDict_inout="+str(dict_inout)+\
        " does not have an assigned issue tracker!" )


#############################################################################
#
# Test CDashQueryAnalyzeReport.doTestsWithIssueTrackersMatchExpectedBuilds()
#
#############################################################################

def gsb(group, site, buildname):
  return {'group':group,'site':site,'buildname':buildname }

g_expectedBuildsLOD = [
  gsb('group1', 'site1', 'build1'),
  gsb('group1', 'site1', 'build2'),
  gsb('group2', 'site2', 'build2'),
  gsb('group2', 'site1', 'build3'),
  ]
  # NOTE: 'site' and 'buildname' have to be unique for this part of the code!

g_testsWtihIssueTrackersLOD = [
  sbtiturlit('site1', 'build3', 'test1', 'url1', '#1111'),
  sbtiturlit('site1', 'build1', 'test2', 'url2', '#1112'),
  sbtiturlit('site2', 'build2', 'test1', 'url3', '#1113'),
  sbtiturlit('site2', 'build2', 'test5', 'url5', '#1114'),
  ]

class test_doTestsWithIssueTrackersMatchExpectedBuilds(unittest.TestCase):

  def test_all_match(self):
    testToExpectedBuildsSLOD = \
      createTestToBuildSearchableListOfDicts(g_expectedBuildsLOD)
    self.assertEqual(
      doTestsWithIssueTrackersMatchExpectedBuilds(
        g_testsWtihIssueTrackersLOD, testToExpectedBuildsSLOD),
      (True, "")
      )

  def test_nomatch_1(self):
    testsWtihIssueTrackersLOD = copy.deepcopy(g_testsWtihIssueTrackersLOD)
    testToExpectedBuildsSLOD = \
      createTestToBuildSearchableListOfDicts(g_expectedBuildsLOD)
    testsWtihIssueTrackersLOD[1]['buildName'] = 'build8'
    (matches, errMsg) = doTestsWithIssueTrackersMatchExpectedBuilds(
      testsWtihIssueTrackersLOD, testToExpectedBuildsSLOD)
    self.assertEqual(matches, False)
    self.assertEqual(errMsg,
      "Error: The following tests with issue trackers did not match 'site' and"+\
      " 'buildName' in one of the expected builds:\n"+\
      "  {'site'='site1', 'buildName'=build8', 'testname'=test2'}\n" )

  def test_nomatch_2(self):
    testsWtihIssueTrackersLOD = copy.deepcopy(g_testsWtihIssueTrackersLOD)
    testToExpectedBuildsSLOD = \
      createTestToBuildSearchableListOfDicts(g_expectedBuildsLOD)
    testsWtihIssueTrackersLOD[1]['buildName'] = 'build8'
    testsWtihIssueTrackersLOD[3]['site'] = 'site3'
    (matches, errMsg) = doTestsWithIssueTrackersMatchExpectedBuilds(
      testsWtihIssueTrackersLOD, testToExpectedBuildsSLOD)
    self.assertEqual(matches, False)
    self.assertEqual(errMsg,
      "Error: The following tests with issue trackers did not match 'site' and"+\
      " 'buildName' in one of the expected builds:\n"+\
      "  {'site'='site1', 'buildName'=build8', 'testname'=test2'}\n"+\
      "  {'site'='site3', 'buildName'=build2', 'testname'=test5'}\n" )


#############################################################################
#
# Test CDashQueryAnalyzeReport.dateFromBuildStartTime()
#
#############################################################################

class test_dateFromBuildStartTime(unittest.TestCase):

  def test_1(self):
    self.assertEqual(
      dateFromBuildStartTime(u('2001-01-01T05:54:03 UTC')), u('2001-01-01') )


#############################################################################
#
# Test CDashQueryAnalyzeReport.getUniqueSortedTestsHistoryListOfDicts()
#
#############################################################################

g_testDictFailed = {
  u('buildName'): u('build_name'),
  u('buildSummaryLink'): u('buildSummary.php?buildid=<buildid>'),
  u('buildstarttime'): u('2001-01-01T05:54:03 UTC'),
  u('details'): u('Completed (Failed)\n'),
  u('nprocs'): 4,
  u('prettyProcTime'): u('40s 400ms'),
  u('prettyTime'): u('10s 100ms'),
  u('procTime'): 40.4,
  u('site'): u('site_name'),
  u('siteLink'): u('viewSite.php?siteid=<site_id>'),
  u('status'): u('Failed'),
  u('statusclass'): u('error'),
  u('testDetailsLink'): u('testDetails.php?test=<testid>&build=<buildid>'),
  u('testname'): u('test_name'),
  u('time'): 10.1,
  u('issue_tracker'): u('#1234'),
  u('issue_tracker_url'): u('some.com/site/issue/1234')
  }

def getTestHistoryLOD5(statusListOrderedByDate,
  time = "05:54:03",
  timezoneStr = "UTC",
  ):
  testHistoryListLOD = []
  for i in range(5): testHistoryListLOD.append(copy.deepcopy(g_testDictFailed))
  testHistoryListLOD[1]['buildstarttime'] = '2001-01-01T'+time+' '+timezoneStr
  testHistoryListLOD[1]['status'] = statusListOrderedByDate[0]
  testHistoryListLOD[0]['buildstarttime'] = '2000-12-31T'+time+' '+timezoneStr
  testHistoryListLOD[0]['status'] = statusListOrderedByDate[1]
  testHistoryListLOD[4]['buildstarttime'] = '2000-12-30T'+time+' '+timezoneStr
  testHistoryListLOD[4]['status'] = statusListOrderedByDate[2]
  testHistoryListLOD[3]['buildstarttime'] = '2000-12-29T'+time+' '+timezoneStr
  testHistoryListLOD[3]['status'] = statusListOrderedByDate[3]
  testHistoryListLOD[2]['buildstarttime'] = '2000-12-28T'+time+' '+timezoneStr
  testHistoryListLOD[2]['status'] = statusListOrderedByDate[4]
  return testHistoryListLOD
  # NOTE: Above, we make them unsorted so that we can test the sort done
  # inside of AddTestHistoryToTestDictFuctor.  Also, the tests require the
  # exact ordering of this list do don't change it!

def getSortedTestHistoryLOD5(statusListOrderedByDate):
  sortedTestHistoryLOD = getTestHistoryLOD5(statusListOrderedByDate)
  sortedTestHistoryLOD.sort(reverse=True, key=DictSortFunctor(['buildstarttime']))
  return sortedTestHistoryLOD

class test_getUniqueSortedTestsHistoryLOD(unittest.TestCase):

  def test_empty_entries(self):
    testHistLOD = []
    uniTestHistLOD = \
      getUniqueSortedTestsHistoryListOfDicts(testHistLOD)
    self.assertEqual(len(uniTestHistLOD), 0)

  def test_one_entries(self):
    testHistLOD = [
      getSortedTestHistoryLOD5(['Passed','Passed','Failed','Passed','Failed'])[2]
      ]
    uniTestHistLOD = \
      getUniqueSortedTestsHistoryListOfDicts(testHistLOD)
    self.assertEqual(uniTestHistLOD[0]['buildstarttime'],'2000-12-30T05:54:03 UTC')

  def test_unique_entries(self):
    testHistLOD = \
      getSortedTestHistoryLOD5(['Passed','Passed','Failed','Passed','Failed'])
    uniTestHistLOD = \
      getUniqueSortedTestsHistoryListOfDicts(testHistLOD)
    self.assertEqual(uniTestHistLOD[0]['buildstarttime'],'2001-01-01T05:54:03 UTC')
    self.assertEqual(uniTestHistLOD[1]['buildstarttime'],'2000-12-31T05:54:03 UTC')
    self.assertEqual(uniTestHistLOD[2]['buildstarttime'],'2000-12-30T05:54:03 UTC')
    self.assertEqual(uniTestHistLOD[3]['buildstarttime'],'2000-12-29T05:54:03 UTC')
    self.assertEqual(uniTestHistLOD[4]['buildstarttime'],'2000-12-28T05:54:03 UTC')

  def test_duplicate_entries(self):
    testHistLOD = \
      getSortedTestHistoryLOD5(['Passed','Passed','Failed','Passed','Failed'])
    testHistLOD.insert(4, testHistLOD[4])
    testHistLOD.insert(2, testHistLOD[2])
    testHistLOD.insert(1, testHistLOD[0])
    uniTestHistLOD = \
      getUniqueSortedTestsHistoryListOfDicts(testHistLOD)
    self.assertEqual(uniTestHistLOD[0]['buildstarttime'],'2001-01-01T05:54:03 UTC')
    self.assertEqual(uniTestHistLOD[1]['buildstarttime'],'2000-12-31T05:54:03 UTC')
    self.assertEqual(uniTestHistLOD[2]['buildstarttime'],'2000-12-30T05:54:03 UTC')
    self.assertEqual(uniTestHistLOD[3]['buildstarttime'],'2000-12-29T05:54:03 UTC')
    self.assertEqual(uniTestHistLOD[4]['buildstarttime'],'2000-12-28T05:54:03 UTC')


#############################################################################
#
# Test CDashQueryAnalyzeReport.sortTestHistoryGetStatistics()
#
#############################################################################

class test_sortTestHistoryGetStatistics(unittest.TestCase):

  def test_all_passed(self):
    testHistoryLOD = getTestHistoryLOD5(['Passed','Passed','Passed','Passed','Passed'])
    testHistoryLOD.append(testHistoryLOD[2])  # Include a duplicate
    currentTestDate = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    (sortedTestHistoryLOD, testHistoryStats, testStatus) = \
      sortTestHistoryGetStatistics(testHistoryLOD, currentTestDate,
         testingDayStartTimeUtc, daysOfHistory)
    # Test the sorting
    self.assertEqual(len(sortedTestHistoryLOD), 5)
    self.assertEqual(sortedTestHistoryLOD[0]['buildstarttime'],'2001-01-01T05:54:03 UTC')
    self.assertEqual(sortedTestHistoryLOD[1]['buildstarttime'],'2000-12-31T05:54:03 UTC')
    self.assertEqual(sortedTestHistoryLOD[2]['buildstarttime'],'2000-12-30T05:54:03 UTC')
    self.assertEqual(sortedTestHistoryLOD[3]['buildstarttime'],'2000-12-29T05:54:03 UTC')
    self.assertEqual(sortedTestHistoryLOD[4]['buildstarttime'],'2000-12-28T05:54:03 UTC')
    self.assertEqual(testHistoryStats['pass_last_x_days'], 5)
    self.assertEqual(testHistoryStats['nopass_last_x_days'], 0)
    self.assertEqual(testHistoryStats['missing_last_x_days'], 0)
    self.assertEqual(testHistoryStats['consec_pass_days'], 5)
    self.assertEqual(testHistoryStats['consec_nopass_days'], 0)
    self.assertEqual(testHistoryStats['consec_missing_days'], 0)
    self.assertEqual(testHistoryStats['previous_nopass_date'], 'None')
    self.assertEqual(testStatus, 'Passed')
  # NOTE: The above test checks that the history gets sorted correctly.  We
  # don't need to do that for the remaining tests.

  def test_pass_3_but_nopass_2(self):
    testHistoryLOD = getTestHistoryLOD5(['Passed','Passed','Passed','Failed','Failed'])
    currentTestDate = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    (sortedTestHistoryLOD, testHistoryStats, testStatus) = \
      sortTestHistoryGetStatistics(testHistoryLOD, currentTestDate,
         testingDayStartTimeUtc, daysOfHistory)
    self.assertEqual(sortedTestHistoryLOD[0]['buildstarttime'],'2001-01-01T05:54:03 UTC')
    self.assertEqual(testHistoryStats['pass_last_x_days'], 3)
    self.assertEqual(testHistoryStats['nopass_last_x_days'], 2)
    self.assertEqual(testHistoryStats['missing_last_x_days'], 0)
    self.assertEqual(testHistoryStats['consec_pass_days'], 3)
    self.assertEqual(testHistoryStats['consec_nopass_days'], 0)
    self.assertEqual(testHistoryStats['consec_missing_days'], 0)
    self.assertEqual(testHistoryStats['previous_nopass_date'], '2000-12-29')
    self.assertEqual(testStatus, 'Passed')

  def test_pass_2_but_nopass_3(self):
    testHistoryLOD = getTestHistoryLOD5(['Passed','Passed','Failed','Passed','Failed'])
    currentTestDate = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    (sortedTestHistoryLOD, testHistoryStats, testStatus) = \
      sortTestHistoryGetStatistics(testHistoryLOD, currentTestDate,
         testingDayStartTimeUtc, daysOfHistory)
    self.assertEqual(sortedTestHistoryLOD[0]['buildstarttime'],'2001-01-01T05:54:03 UTC')
    self.assertEqual(testHistoryStats['pass_last_x_days'], 3)
    self.assertEqual(testHistoryStats['nopass_last_x_days'], 2)
    self.assertEqual(testHistoryStats['missing_last_x_days'], 0)
    self.assertEqual(testHistoryStats['consec_pass_days'], 2)
    self.assertEqual(testHistoryStats['consec_nopass_days'], 0)
    self.assertEqual(testHistoryStats['consec_missing_days'], 0)
    self.assertEqual(testHistoryStats['previous_nopass_date'], '2000-12-30')
    self.assertEqual(testStatus, 'Passed')

  def test_pass_1_but_nopass_2_missing_1(self):
    testHistoryLOD = \
      getSortedTestHistoryLOD5(['Passed','DELETED','Failed','Passed','Failed'])
    del testHistoryLOD[1]
    currentTestDate = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    (sortedTestHistoryLOD, testHistoryStats, testStatus) = \
      sortTestHistoryGetStatistics(testHistoryLOD, currentTestDate,
         testingDayStartTimeUtc, daysOfHistory)
    self.assertEqual(sortedTestHistoryLOD[0]['buildstarttime'],'2001-01-01T05:54:03 UTC')
    self.assertEqual(testHistoryStats['pass_last_x_days'], 2)
    self.assertEqual(testHistoryStats['nopass_last_x_days'], 2)
    self.assertEqual(testHistoryStats['missing_last_x_days'], 1)
    self.assertEqual(testHistoryStats['consec_pass_days'], 1)
    self.assertEqual(testHistoryStats['consec_nopass_days'], 0)
    self.assertEqual(testHistoryStats['consec_missing_days'], 0)
    self.assertEqual(testHistoryStats['previous_nopass_date'], '2000-12-30')
    self.assertEqual(testStatus, 'Passed')

  def test_all_failed(self):
    testHistoryLOD = getTestHistoryLOD5(['Failed','Failed','Failed','Failed','Failed'])
    currentTestDate = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    (sortedTestHistoryLOD, testHistoryStats, testStatus) = \
      sortTestHistoryGetStatistics(testHistoryLOD, currentTestDate,
         testingDayStartTimeUtc, daysOfHistory)
    self.assertEqual(sortedTestHistoryLOD[0]['buildstarttime'],'2001-01-01T05:54:03 UTC')
    self.assertEqual(testHistoryStats['pass_last_x_days'], 0)
    self.assertEqual(testHistoryStats['nopass_last_x_days'], 5)
    self.assertEqual(testHistoryStats['missing_last_x_days'], 0)
    self.assertEqual(testHistoryStats['consec_pass_days'], 0)
    self.assertEqual(testHistoryStats['consec_nopass_days'], 5)
    self.assertEqual(testHistoryStats['consec_missing_days'], 0)
    self.assertEqual(testHistoryStats['previous_nopass_date'], '2000-12-31')
    self.assertEqual(testStatus, 'Failed')

  def test_failed_3_passed_2(self):
    testHistoryLOD = getTestHistoryLOD5(['Failed','Not Run','Passed','Passed','Failed'])
    currentTestDate = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    (sortedTestHistoryLOD, testHistoryStats, testStatus) = \
      sortTestHistoryGetStatistics(testHistoryLOD, currentTestDate,
         testingDayStartTimeUtc, daysOfHistory)
    self.assertEqual(sortedTestHistoryLOD[0]['buildstarttime'],'2001-01-01T05:54:03 UTC')
    self.assertEqual(testHistoryStats['pass_last_x_days'], 2)
    self.assertEqual(testHistoryStats['nopass_last_x_days'], 3)
    self.assertEqual(testHistoryStats['missing_last_x_days'], 0)
    self.assertEqual(testHistoryStats['consec_pass_days'], 0)
    self.assertEqual(testHistoryStats['consec_nopass_days'], 2)
    self.assertEqual(testHistoryStats['consec_missing_days'], 0)
    self.assertEqual(testHistoryStats['previous_nopass_date'], '2000-12-31')
    self.assertEqual(testStatus, 'Failed')

  def test_failed_2_passed_1_missing_2(self):
    testHistoryLOD = \
      getSortedTestHistoryLOD5(['Failed','DELETED','Passed','DELETED','Failed'])
    del testHistoryLOD[3]
    del testHistoryLOD[1]
    currentTestDate = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    (sortedTestHistoryLOD, testHistoryStats, testStatus) = \
      sortTestHistoryGetStatistics(testHistoryLOD, currentTestDate,
         testingDayStartTimeUtc, daysOfHistory)
    # Test the sorting
    self.assertEqual(sortedTestHistoryLOD[0]['buildstarttime'],'2001-01-01T05:54:03 UTC')
    self.assertEqual(testHistoryStats['pass_last_x_days'], 1)
    self.assertEqual(testHistoryStats['nopass_last_x_days'], 2)
    self.assertEqual(testHistoryStats['missing_last_x_days'], 2)
    self.assertEqual(testHistoryStats['consec_pass_days'], 0)
    self.assertEqual(testHistoryStats['consec_nopass_days'], 1)
    self.assertEqual(testHistoryStats['consec_missing_days'], 0)
    self.assertEqual(testHistoryStats['previous_nopass_date'], '2000-12-28')
    self.assertEqual(testStatus, 'Failed')

  def test_all_notrun(self):
    testHistoryLOD = getTestHistoryLOD5(
      ['Not Run','Not Run','Not Run','Not Run','Not Run'])
    currentTestDate = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    (sortedTestHistoryLOD, testHistoryStats, testStatus) = \
      sortTestHistoryGetStatistics(testHistoryLOD, currentTestDate,
         testingDayStartTimeUtc, daysOfHistory)
    self.assertEqual(sortedTestHistoryLOD[0]['buildstarttime'],'2001-01-01T05:54:03 UTC')
    self.assertEqual(testHistoryStats['pass_last_x_days'], 0)
    self.assertEqual(testHistoryStats['nopass_last_x_days'], 5)
    self.assertEqual(testHistoryStats['missing_last_x_days'], 0)
    self.assertEqual(testHistoryStats['consec_pass_days'], 0)
    self.assertEqual(testHistoryStats['consec_nopass_days'], 5)
    self.assertEqual(testHistoryStats['consec_missing_days'], 0)
    self.assertEqual(testHistoryStats['previous_nopass_date'], '2000-12-31')
    self.assertEqual(testStatus, 'Not Run')

  def test_notrun_2_passed_2(self):
    testHistoryLOD = getTestHistoryLOD5(['Not Run','Not Run','Passed','Failed','Passed'])
    currentTestDate = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    (sortedTestHistoryLOD, testHistoryStats, testStatus) = \
      sortTestHistoryGetStatistics(testHistoryLOD, currentTestDate,
         testingDayStartTimeUtc, daysOfHistory)
    self.assertEqual(sortedTestHistoryLOD[0]['buildstarttime'],'2001-01-01T05:54:03 UTC')
    self.assertEqual(testHistoryStats['pass_last_x_days'], 2)
    self.assertEqual(testHistoryStats['nopass_last_x_days'], 3)
    self.assertEqual(testHistoryStats['missing_last_x_days'], 0)
    self.assertEqual(testHistoryStats['consec_pass_days'], 0)
    self.assertEqual(testHistoryStats['consec_nopass_days'], 2)
    self.assertEqual(testHistoryStats['consec_missing_days'], 0)
    self.assertEqual(testHistoryStats['previous_nopass_date'], '2000-12-31')
    self.assertEqual(testStatus, 'Not Run')

  def test_notrun_2_passed_1_missing_2(self):
    testHistoryLOD = \
      getSortedTestHistoryLOD5(['Not Run','DELETED','Passed','Failed','DELETED'])
    del testHistoryLOD[4]
    del testHistoryLOD[1]
    currentTestDate = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    (sortedTestHistoryLOD, testHistoryStats, testStatus) = \
      sortTestHistoryGetStatistics(testHistoryLOD, currentTestDate,
         testingDayStartTimeUtc, daysOfHistory)
    self.assertEqual(sortedTestHistoryLOD[0]['buildstarttime'],'2001-01-01T05:54:03 UTC')
    self.assertEqual(testHistoryStats['pass_last_x_days'], 1)
    self.assertEqual(testHistoryStats['nopass_last_x_days'], 2)
    self.assertEqual(testHistoryStats['missing_last_x_days'], 2)
    self.assertEqual(testHistoryStats['consec_pass_days'], 0)
    self.assertEqual(testHistoryStats['consec_nopass_days'], 1)
    self.assertEqual(testHistoryStats['consec_missing_days'], 0)
    self.assertEqual(testHistoryStats['previous_nopass_date'], '2000-12-29')
    self.assertEqual(testStatus, 'Not Run')

  def test_all_missing(self):
    testHistoryLOD = []
    currentTestDate = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    (sortedTestHistoryLOD, testHistoryStats, testStatus) = \
      sortTestHistoryGetStatistics(testHistoryLOD, currentTestDate,
         testingDayStartTimeUtc, daysOfHistory)
    self.assertEqual(sortedTestHistoryLOD, [])
    self.assertEqual(testHistoryStats['pass_last_x_days'], 0)
    self.assertEqual(testHistoryStats['nopass_last_x_days'], 0)
    self.assertEqual(testHistoryStats['missing_last_x_days'], 5)
    self.assertEqual(testHistoryStats['consec_pass_days'], 0)
    self.assertEqual(testHistoryStats['consec_nopass_days'], 0)
    self.assertEqual(testHistoryStats['consec_missing_days'], 5)
    self.assertEqual(testHistoryStats['previous_nopass_date'], 'None')
    self.assertEqual(testStatus, 'Missing')

  def test_all_missing_1_nopass_4(self):
    testHistoryLOD = \
      getSortedTestHistoryLOD5(['DELETED','Failed','Failed','Failed','Failed'])
    del testHistoryLOD[0]
    currentTestDate = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    (sortedTestHistoryLOD, testHistoryStats, testStatus) = \
      sortTestHistoryGetStatistics(testHistoryLOD, currentTestDate,
         testingDayStartTimeUtc, daysOfHistory)
    self.assertEqual(sortedTestHistoryLOD[0]['buildstarttime'],'2000-12-31T05:54:03 UTC')
    self.assertEqual(testHistoryStats['pass_last_x_days'], 0)
    self.assertEqual(testHistoryStats['nopass_last_x_days'], 4)
    self.assertEqual(testHistoryStats['missing_last_x_days'], 1)
    self.assertEqual(testHistoryStats['consec_pass_days'], 0)
    self.assertEqual(testHistoryStats['consec_nopass_days'], 0)
    self.assertEqual(testHistoryStats['consec_missing_days'], 1)
    self.assertEqual(testHistoryStats['previous_nopass_date'], '2000-12-31')
    self.assertEqual(testStatus, 'Missing')

  def test_all_missing_3_pass_1_nopass__1(self):
    testHistoryLOD = \
      getSortedTestHistoryLOD5(['DELETED','DELETED','Failed','DELETED','Passed'])
    del testHistoryLOD[3]
    del testHistoryLOD[1]
    del testHistoryLOD[0]
    currentTestDate = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    (sortedTestHistoryLOD, testHistoryStats, testStatus) = \
      sortTestHistoryGetStatistics(testHistoryLOD, currentTestDate,
         testingDayStartTimeUtc, daysOfHistory)
    self.assertEqual(sortedTestHistoryLOD[0]['buildstarttime'],'2000-12-30T05:54:03 UTC')
    self.assertEqual(testHistoryStats['pass_last_x_days'], 1)
    self.assertEqual(testHistoryStats['nopass_last_x_days'], 1)
    self.assertEqual(testHistoryStats['missing_last_x_days'], 3)
    self.assertEqual(testHistoryStats['consec_pass_days'], 0)
    self.assertEqual(testHistoryStats['consec_nopass_days'], 0)
    self.assertEqual(testHistoryStats['consec_missing_days'], 2)
    self.assertEqual(testHistoryStats['previous_nopass_date'], '2000-12-30')
    self.assertEqual(testStatus, 'Missing')

  def test_mdt_allpass_same_month(self):
    testHistoryLOD = getTestHistoryLOD5(
      ['Passed','Passed','Passed','Passed','Passed'], "18:44:29", "MDT")
    currentTestDate = "2001-01-02"
    testingDayStartTimeUtc = "18:00"
    daysOfHistory = 5
    (sortedTestHistoryLOD, testHistoryStats, testStatus) = \
      sortTestHistoryGetStatistics(testHistoryLOD, currentTestDate,
         testingDayStartTimeUtc, daysOfHistory)
    # Test the sorting
    self.assertEqual(sortedTestHistoryLOD[0]['buildstarttime'],'2001-01-01T18:44:29 MDT')
    self.assertEqual(sortedTestHistoryLOD[1]['buildstarttime'],'2000-12-31T18:44:29 MDT')
    self.assertEqual(sortedTestHistoryLOD[2]['buildstarttime'],'2000-12-30T18:44:29 MDT')
    self.assertEqual(sortedTestHistoryLOD[3]['buildstarttime'],'2000-12-29T18:44:29 MDT')
    self.assertEqual(sortedTestHistoryLOD[4]['buildstarttime'],'2000-12-28T18:44:29 MDT')
    self.assertEqual(testHistoryStats['pass_last_x_days'], 5)
    self.assertEqual(testHistoryStats['nopass_last_x_days'], 0)
    self.assertEqual(testHistoryStats['missing_last_x_days'], 0)
    self.assertEqual(testHistoryStats['consec_pass_days'], 5)
    self.assertEqual(testHistoryStats['consec_nopass_days'], 0)
    self.assertEqual(testHistoryStats['consec_missing_days'], 0)
    self.assertEqual(testHistoryStats['previous_nopass_date'], 'None')
    self.assertEqual(testStatus, 'Passed')
  # NOTE: The above test checks the logic where the raw 'buildstarttime' field
  # is in MDT and has the raw "YYYY-MM-DD" in the previous day but is actually
  # the current testing day according to the CDash project testing day start
  # time.  Here, the testing day start time is 18:00 MDT which is 02:00 UTC
  # the next calendar day.  Therefore '2001-01-01T18:44:29 MDT' is actually
  # '2001-01-02T02:44:29 UTC' with the calendar date '2001-01-02' which
  # matches the CDash testing day '2001-01-02'.

  def test_mdt_allpass_next_month(self):
    testHistoryLOD = getTestHistoryLOD5(
      ['Passed','Passed','Passed','Passed','Passed'], "18:44:29", "MDT")
    del testHistoryLOD[1]  # This is the most recent!
    #print("testHistoryLOD:")
    #g_pp.pprint(testHistoryLOD)
    currentTestDate = "2001-01-01"
    testingDayStartTimeUtc = "18:00"
    daysOfHistory = 4
    (sortedTestHistoryLOD, testHistoryStats, testStatus) = \
      sortTestHistoryGetStatistics(testHistoryLOD, currentTestDate,
         testingDayStartTimeUtc, daysOfHistory)
    # Test the sorting
    self.assertEqual(sortedTestHistoryLOD[0]['buildstarttime'],'2000-12-31T18:44:29 MDT')
    self.assertEqual(sortedTestHistoryLOD[1]['buildstarttime'],'2000-12-30T18:44:29 MDT')
    self.assertEqual(sortedTestHistoryLOD[2]['buildstarttime'],'2000-12-29T18:44:29 MDT')
    self.assertEqual(sortedTestHistoryLOD[3]['buildstarttime'],'2000-12-28T18:44:29 MDT')
    self.assertEqual(testHistoryStats['pass_last_x_days'], 4)
    self.assertEqual(testHistoryStats['nopass_last_x_days'], 0)
    self.assertEqual(testHistoryStats['missing_last_x_days'], 0)
    self.assertEqual(testHistoryStats['consec_pass_days'], 4)
    self.assertEqual(testHistoryStats['consec_nopass_days'], 0)
    self.assertEqual(testHistoryStats['consec_missing_days'], 0)
    self.assertEqual(testHistoryStats['previous_nopass_date'], 'None')
    self.assertEqual(testStatus, 'Passed')
  # NOTE: The above test ensures that the datetime logic will shift to the
  # next month for the testing day.

  def test_mdt_missing_and_failed(self):
    testHistoryLOD = getTestHistoryLOD5(
      ['DELETED','DELETED','Passed','Failed','Passed'], "18:44:29", "MDT")
    del testHistoryLOD[1] # Remove two most recent days
    del testHistoryLOD[0]
    #print("testHistoryLOD:")
    #g_pp.pprint(testHistoryLOD)
    currentTestDate = "2001-01-02"
    testingDayStartTimeUtc = "18:00"
    daysOfHistory = 5
    (sortedTestHistoryLOD, testHistoryStats, testStatus) = \
      sortTestHistoryGetStatistics(testHistoryLOD, currentTestDate,
         testingDayStartTimeUtc, daysOfHistory)
    # Test the sorting
    self.assertEqual(sortedTestHistoryLOD[0]['buildstarttime'],'2000-12-30T18:44:29 MDT')
    self.assertEqual(sortedTestHistoryLOD[1]['buildstarttime'],'2000-12-29T18:44:29 MDT')
    self.assertEqual(sortedTestHistoryLOD[2]['buildstarttime'],'2000-12-28T18:44:29 MDT')
    self.assertEqual(testHistoryStats['pass_last_x_days'], 2)
    self.assertEqual(testHistoryStats['nopass_last_x_days'], 1)
    self.assertEqual(testHistoryStats['missing_last_x_days'], 2)
    self.assertEqual(testHistoryStats['consec_pass_days'], 0)
    self.assertEqual(testHistoryStats['consec_nopass_days'], 0)
    self.assertEqual(testHistoryStats['consec_missing_days'], 2)
    self.assertEqual(testHistoryStats['previous_nopass_date'], '2000-12-30') # Shifted!
    self.assertEqual(testStatus, 'Missing')

  def test_empty_1_pass_2_failed_2(self):
    testHistoryLOD = getTestHistoryLOD5(['ToBeRemoved','Passed','Failed','Passed','Failed'])
    testDictWithStatusToBeRemoved = \
      next((item for item in testHistoryLOD if item['status']=='ToBeRemoved'), None)
    del testDictWithStatusToBeRemoved['status']
    currentTestDate = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    (sortedTestHistoryLOD, testHistoryStats, testStatus) = \
      sortTestHistoryGetStatistics(testHistoryLOD, currentTestDate,
         testingDayStartTimeUtc, daysOfHistory)
    self.assertEqual(sortedTestHistoryLOD[0]['buildstarttime'],'2001-01-01T05:54:03 UTC')
    self.assertEqual(testHistoryStats['pass_last_x_days'], 2)
    self.assertEqual(testHistoryStats['nopass_last_x_days'], 3)
    self.assertEqual(testHistoryStats['missing_last_x_days'], 0)
    self.assertEqual(testHistoryStats['consec_pass_days'], 0)
    self.assertEqual(testHistoryStats['consec_nopass_days'], 1)
    self.assertEqual(testHistoryStats['consec_missing_days'], 0)
    self.assertEqual(testHistoryStats['previous_nopass_date'], '2000-12-30')
    self.assertEqual(testStatus, 'Not Run')


#############################################################################
#
# Test CDashQueryAnalyzeReport.checkCDashTestDictsAreSame()
#
#############################################################################

g_cdashTestDict = {
  u('site') : u('site1'),
  u('buildName') : u('build1'),
  u('testname') : u('test1'),
  u('testDetailsLink') : u('testDetails.php?test=58569474&build=4143620'),
  u('time') : 0.22,
  u('otherData') : u('dataValue'),
  } 

class test_extractTestIdAndBuildIdFromTestDetailsLink(unittest.TestCase):

  def test_old_cdash(self):
    self.assertEqual(
      extractTestIdAndBuildIdFromTestDetailsLink(
        u('testDetails.php?test=58569474&build=4143620') ),
      ("58569474", "4143620")
      )

  def test_new_cdash(self):
    self.assertEqual(
      extractTestIdAndBuildIdFromTestDetailsLink(u('test/14614128')),
      ("14614128", "")
      )


class test_checkCDashTestDictsAreSame(unittest.TestCase):

  def test_exact_same(self):
    testDict_1 = copy.deepcopy(g_cdashTestDict)
    testDict_2 = copy.deepcopy(g_cdashTestDict)
    expectedRtn = (True, None)
    self.assertEqual(
      checkCDashTestDictsAreSame(testDict_1, "testDict_1", testDict_2, "testDict_2"),
      expectedRtn )

  def test_same_except_for_testid(self):
    testDict_1 = copy.deepcopy(g_cdashTestDict)
    testDict_2 = copy.deepcopy(g_cdashTestDict)
    testDict_2['testDetailsLink'] = u('testDetails.php?test=58569475&build=4143620')
    expectedRtn = (True, None)
    self.assertEqual(
      checkCDashTestDictsAreSame(testDict_1, "testDict_1", testDict_2, "testDict_2"),
      expectedRtn )

  def test_same_status_and_details(self):
    testDict_1 = copy.deepcopy(g_cdashTestDict)
    testDict_2 = copy.deepcopy(g_cdashTestDict)
    testDict_2['testDetailsLink'] = u('testDetails.php?test=58569475&build=4143620')
    testDict_2['time'] = 0.23
    testDict_2['prettyTime'] = "0.23"
    testDict_2['procTime'] = 4.0
    testDict_2['prettyProcTime'] = "4.0"
    testDict_2['matchingoutput'] = "some different output"
    expectedRtn = (True, None)
    self.assertEqual(
      checkCDashTestDictsAreSame(testDict_1, "testDict_1", testDict_2, "testDict_2"),
      expectedRtn )

  def test_different_testid_and_buildid(self):
    testDict_1 = copy.deepcopy(g_cdashTestDict)
    testDict_2 = copy.deepcopy(g_cdashTestDict)
    testDict_2['testDetailsLink'] = u('testDetails.php?test=58569475&build=4143621')
    expectedRtn = (False,
      "testDict_1['testDetailsLink'] = 'testDetails.php?test=58569474&build=4143620' !="+\
      " testDict_2['testDetailsLink'] = 'testDetails.php?test=58569475&build=4143621'")
    self.assertEqual(
      checkCDashTestDictsAreSame(testDict_1, "testDict_1", testDict_2, "testDict_2"),
      expectedRtn )

  def test_different_other_key_value_pair(self):
    testDict_1 = copy.deepcopy(g_cdashTestDict)
    testDict_2 = copy.deepcopy(g_cdashTestDict)
    testDict_2['otherData'] = u('dataValueDiff')
    expectedRtn = (False,
      "testDict_1['otherData'] = 'dataValue' != testDict_2['otherData'] = 'dataValueDiff'")
    self.assertEqual(
      checkCDashTestDictsAreSame(testDict_1, "testDict_1", testDict_2, "testDict_2"),
      expectedRtn )


#############################################################################
#
# Test CDashQueryAnalyzeReport.getTestHistoryCacheFileName()
#
#############################################################################

class test_getTestHistoryCacheFileName(unittest.TestCase):

  def test_normal(self):
    cacheFileName = getTestHistoryCacheFileName('YYYY-MM-DD', 'site_name',
      'build_name', 'test_name', 7)
    cacheFileName_expected = \
      "YYYY-MM-DD-site_name-build_name-test_name-HIST-7.json"
    self.assertEqual(cacheFileName, cacheFileName_expected)

  def test_test_name_with_slash(self):
    cacheFileName = getTestHistoryCacheFileName('YYYY-MM-DD', 'site_name',
      'build_name', 'base_test_name/test_name', 7)
    cacheFileName_expected = \
      "YYYY-MM-DD-site_name-build_name-base_test_name_test_name-HIST-7.json"
    self.assertEqual(cacheFileName, cacheFileName_expected)


#############################################################################
#
# Test CDashQueryAnalyzeReport.AddTestHistoryToTestDictFunctor
#
#############################################################################


class test_AddTestHistoryToTestDictFunctor(unittest.TestCase):


  # Base test case for a non-passing test with test dict info already from
  # CDash.
  def test_nonpassingTest_downloadFromCDash(self):

    # Deep copy the test dict so we don't modify the original
    testDict = copy.deepcopy(g_testDictFailed)

    # Target test date
    testHistoryQueryUrl = \
      u('site.com/cdash/api/v1/queryTests.php?project=projectName&begin=2000-12-28&end=2001-01-01&filtercombine=and&filtercombine=&filtercount=3&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=testname&compare2=61&value2=test_name&field3=site&compare3=61&value3=site_name')

    # Create a subdir for the created cache file
    testCacheOutputDir = \
      os.getcwd()+"/AddTestHistoryToTestDictFunctor/test_nonpassingTest_downloadFromCDash"
    if os.path.exists(testCacheOutputDir): shutil.rmtree(testCacheOutputDir)
    os.makedirs(testCacheOutputDir)

    # Create dummy test history
    testHistoryLOD = getTestHistoryLOD5(
      [
        'Failed',
        'Failed',
        'Passed',
        'Passed',
        'Not Run',
        ]
      )

    # Construct arguments
    cdashUrl = "site.com/cdash"
    projectName = "projectName"
    date = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    useCachedCDashData = False
    alwaysUseCacheFileIfExists = False
    verbose = False
    printDetails = False
    requireMatchTestTopTestHistory = True
    mockExtractCDashApiQueryDataFunctor = MockExtractCDashApiQueryDataFunctor(
      testHistoryQueryUrl, {'builds':testHistoryLOD})

    # Construct the functor
    addTestHistoryFunctor = AddTestHistoryToTestDictFunctor(
      cdashUrl, projectName, date, testingDayStartTimeUtc, daysOfHistory,
      testCacheOutputDir, useCachedCDashData, alwaysUseCacheFileIfExists,
      verbose, printDetails, requireMatchTestTopTestHistory,
      mockExtractCDashApiQueryDataFunctor,
      )

    # Apply the functor to add the test history to the test dict
    addTestHistoryFunctor(testDict)

    testHistoryBrowserUrl = u('site.com/cdash/queryTests.php?project=projectName&begin=2000-12-28&end=2001-01-01&filtercombine=and&filtercombine=&filtercount=3&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=testname&compare2=61&value2=test_name&field3=site&compare3=61&value3=site_name')

    # Checkt the set fields out output
    self.assertEqual(testDict['site'], 'site_name')
    self.assertEqual(testDict['buildName'], 'build_name')
    self.assertEqual(testDict['buildName_url'],
      u('site.com/cdash/index.php?project=projectName&begin=2000-12-28&end=2001-01-01&filtercombine=and&filtercombine=&filtercount=2&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=site&compare2=61&value2=site_name')
      )
    self.assertEqual(testDict['testname'], 'test_name')
    self.assertEqual(testDict['testname_url'], u('site.com/cdash/testDetails.php?test=<testid>&build=<buildid>'))
    self.assertEqual(testDict['status'], 'Failed')
    self.assertEqual(testDict['details'], 'Completed (Failed)\n')
    self.assertEqual(testDict['status_url'], u('site.com/cdash/testDetails.php?test=<testid>&build=<buildid>'))
    self.assertEqual(testDict['status_color'], 'red')
    self.assertEqual(testDict['test_history_num_days'], 5)
    self.assertEqual(testDict['test_history_query_url'], testHistoryQueryUrl)
    self.assertEqual(testDict['test_history_browser_url'], testHistoryBrowserUrl)
    self.assertEqual(
      testDict['test_history_list'][0]['buildstarttime'], '2001-01-01T05:54:03 UTC')
    self.assertEqual(
      testDict['test_history_list'][1]['buildstarttime'], '2000-12-31T05:54:03 UTC')
    self.assertEqual(
      testDict['test_history_list'][2]['buildstarttime'], '2000-12-30T05:54:03 UTC')
    self.assertEqual(
      testDict['test_history_list'][3]['buildstarttime'], '2000-12-29T05:54:03 UTC')
    self.assertEqual(
      testDict['test_history_list'][4]['buildstarttime'], '2000-12-28T05:54:03 UTC')
    self.assertEqual(testDict['pass_last_x_days'], 2)
    self.assertEqual(testDict['pass_last_x_days_url'], testHistoryBrowserUrl)
    self.assertEqual(testDict['pass_last_x_days_color'], 'green')
    self.assertEqual(testDict['nopass_last_x_days'], 3)
    self.assertEqual(testDict['nopass_last_x_days_url'], testHistoryBrowserUrl)
    self.assertEqual(testDict['nopass_last_x_days_color'], 'red')
    self.assertEqual(testDict['missing_last_x_days'], 0)
    self.assertEqual(testDict['missing_last_x_days_url'], testHistoryBrowserUrl)
    self.assertEqual(testDict['consec_pass_days'], 0)
    self.assertEqual(testDict['consec_pass_days_url'], testHistoryBrowserUrl)
    self.assertEqual(testDict['consec_pass_days_color'], 'green')
    self.assertEqual(testDict['consec_nopass_days'], 2)
    self.assertEqual(testDict['consec_nopass_days_url'], testHistoryBrowserUrl)
    self.assertEqual(testDict['consec_nopass_days_color'], 'red')
    self.assertEqual(testDict['consec_missing_days'], 0)
    self.assertEqual(testDict['consec_missing_days_url'], testHistoryBrowserUrl)
    self.assertEqual(testDict['consec_missing_days_color'], 'gray')
    self.assertEqual(testDict['previous_nopass_date'], '2000-12-31')
    self.assertEqual(testDict['issue_tracker'], '#1234')
    self.assertEqual(testDict['issue_tracker_url'], 'some.com/site/issue/1234')

    # Check for the existence of the created Cache file
    cacheFile = \
      testCacheOutputDir+"/2001-01-01-site_name-build_name-test_name-HIST-5.json"
    self.assertEqual(os.path.exists(testCacheOutputDir), True)
    # ToDo: Check the contents of the cache file!


  # Base test case for a non-passing test with test dict info already from
  # CDash but there the data is in MDT and there is a shift in the calendar
  # date when converted to UTC.
  def test_mdt_nonpassingTest_downloadFromCDash(self):

    # Deep copy the test dict so we don't modify the original
    testDict = copy.deepcopy(g_testDictFailed)
    testDict['buildstarttime'] = '2001-01-01T18:44:29 MDT'

    # Target test date
    testHistoryQueryUrl = \
      u('site.com/cdash/api/v1/queryTests.php?project=projectName&begin=2000-12-29&end=2001-01-02&filtercombine=and&filtercombine=&filtercount=3&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=testname&compare2=61&value2=test_name&field3=site&compare3=61&value3=site_name')

    # Create a subdir for the created cache file
    testCacheOutputDir = \
      os.getcwd()+"/AddTestHistoryToTestDictFunctor/test_nonpassingTest_downloadFromCDash"
    if os.path.exists(testCacheOutputDir): shutil.rmtree(testCacheOutputDir)
    os.makedirs(testCacheOutputDir)

    # Create dummy test history
    testHistoryLOD = getTestHistoryLOD5(
      [
        'Failed',
        'Failed',
        'Passed',
        'Passed',
        'Not Run',
        ],
      "18:44:29",
      "MDT",
      )

    # Construct arguments
    cdashUrl = "site.com/cdash"
    projectName = "projectName"
    date = "2001-01-02"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    useCachedCDashData = False
    alwaysUseCacheFileIfExists = False
    verbose = False
    printDetails = False
    requireMatchTestTopTestHistory = True
    mockExtractCDashApiQueryDataFunctor = MockExtractCDashApiQueryDataFunctor(
      testHistoryQueryUrl, {'builds':testHistoryLOD})

    # Construct the functor
    addTestHistoryFunctor = AddTestHistoryToTestDictFunctor(
      cdashUrl, projectName, date, testingDayStartTimeUtc, daysOfHistory,
      testCacheOutputDir, useCachedCDashData, alwaysUseCacheFileIfExists,
      verbose, printDetails, requireMatchTestTopTestHistory,
      mockExtractCDashApiQueryDataFunctor,
      )

    # Apply the functor to add the test history to the test dict
    addTestHistoryFunctor(testDict)

    testHistoryBrowserUrl = u('site.com/cdash/queryTests.php?project=projectName&begin=2000-12-29&end=2001-01-02&filtercombine=and&filtercombine=&filtercount=3&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=testname&compare2=61&value2=test_name&field3=site&compare3=61&value3=site_name')

    # Checkt the set fields out output
    self.assertEqual(testDict['site'], 'site_name')
    self.assertEqual(testDict['buildName'], 'build_name')
    self.assertEqual(testDict['buildName_url'],
      u('site.com/cdash/index.php?project=projectName&begin=2000-12-29&end=2001-01-02&filtercombine=and&filtercombine=&filtercount=2&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=site&compare2=61&value2=site_name')
      )
    self.assertEqual(testDict['testname'], 'test_name')
    self.assertEqual(testDict['testname_url'], u('site.com/cdash/testDetails.php?test=<testid>&build=<buildid>'))
    self.assertEqual(testDict['status'], 'Failed')
    self.assertEqual(testDict['details'], 'Completed (Failed)\n')
    self.assertEqual(testDict['status_url'], u('site.com/cdash/testDetails.php?test=<testid>&build=<buildid>'))
    self.assertEqual(testDict['status_color'], 'red')
    self.assertEqual(testDict['test_history_num_days'], 5)
    self.assertEqual(testDict['test_history_query_url'], testHistoryQueryUrl)
    self.assertEqual(testDict['test_history_browser_url'], testHistoryBrowserUrl)
    self.assertEqual(
      testDict['test_history_list'][0]['buildstarttime'], '2001-01-01T18:44:29 MDT')
    self.assertEqual(
      testDict['test_history_list'][1]['buildstarttime'], '2000-12-31T18:44:29 MDT')
    self.assertEqual(
      testDict['test_history_list'][2]['buildstarttime'], '2000-12-30T18:44:29 MDT')
    self.assertEqual(
      testDict['test_history_list'][3]['buildstarttime'], '2000-12-29T18:44:29 MDT')
    self.assertEqual(
      testDict['test_history_list'][4]['buildstarttime'], '2000-12-28T18:44:29 MDT')
    self.assertEqual(testDict['pass_last_x_days'], 2)
    self.assertEqual(testDict['pass_last_x_days_url'], testHistoryBrowserUrl)
    self.assertEqual(testDict['pass_last_x_days_color'], 'green')
    self.assertEqual(testDict['nopass_last_x_days'], 3)
    self.assertEqual(testDict['nopass_last_x_days_url'], testHistoryBrowserUrl)
    self.assertEqual(testDict['nopass_last_x_days_color'], 'red')
    self.assertEqual(testDict['missing_last_x_days'], 0)
    self.assertEqual(testDict['missing_last_x_days_url'], testHistoryBrowserUrl)
    self.assertEqual(testDict['consec_pass_days'], 0)
    self.assertEqual(testDict['consec_pass_days_url'], testHistoryBrowserUrl)
    self.assertEqual(testDict['consec_pass_days_color'], 'green')
    self.assertEqual(testDict['consec_nopass_days'], 2)
    self.assertEqual(testDict['consec_nopass_days_url'], testHistoryBrowserUrl)
    self.assertEqual(testDict['consec_nopass_days_color'], 'red')
    self.assertEqual(testDict['consec_missing_days'], 0)
    self.assertEqual(testDict['consec_missing_days_url'], testHistoryBrowserUrl)
    self.assertEqual(testDict['consec_missing_days_color'], 'gray')
    self.assertEqual(testDict['previous_nopass_date'], '2001-01-01') # In UTC!
    self.assertEqual(testDict['issue_tracker'], '#1234')
    self.assertEqual(testDict['issue_tracker_url'], 'some.com/site/issue/1234')

    # Check for the existence of the created Cache file
    cacheFile = \
      testCacheOutputDir+"/2001-01-01-site_name-build_name-test_name-HIST-5.json"
    self.assertEqual(os.path.exists(testCacheOutputDir), True)


  # Test the case where the testDict just has the minimal fields that come the
  # tests with issue trackers CSV file but the test actually did get run and
  # passed.  In this case, the AddTestHistoryToTestDictFunctor just fills in
  # the missing info.  Also, this test tests the case where the test history
  # is read from a cache file.
  def test_empty_test_passing(self):

    # Initial test dict as it would come from the tests with issue trackers
    # CSV file
    testDict = {
      u('site'): u('site_name'),
      u('buildName'): u('build_name'),
      u('testname'): u('test_name'),
      u('issue_tracker'): u('#1234'),
      u('issue_tracker_url'): u('some.com/site/issue/1234')
    }
    
    # Target test date
    testHistoryQueryUrl = \
      u('site.com/cdash/api/v1/queryTests.php?project=projectName&begin=2000-12-28&end=2001-01-01&filtercombine=and&filtercombine=&filtercount=3&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=testname&compare2=61&value2=test_name&field3=site&compare3=61&value3=site_name')

    # Create a subdir for the created cache file
    testCacheOutputDir = \
      os.getcwd()+"/AddTestHistoryToTestDictFunctor/test_empty_test_passing"
    if os.path.exists(testCacheOutputDir): shutil.rmtree(testCacheOutputDir)
    os.makedirs(testCacheOutputDir)

    # Create dummy test history and put it in a cache file
    testHistoryLOD = getTestHistoryLOD5(
      [
        'Passed',
        'Passed',
        'Failed',
        'Failed',
        'Failed',
        ]
      )
    testHistoryLOD[1]['details'] = u"Completed (Passed)\n"
    testHistoryJson = { 'builds' : testHistoryLOD }
    testHistorycacheFilePath = \
      testCacheOutputDir+"/2001-01-01-site_name-build_name-test_name-HIST-5.json"
    pprintPythonDataToFile(testHistoryJson, testHistorycacheFilePath)

    # Construct arguments
    cdashUrl = "site.com/cdash"
    projectName = "projectName"
    date = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    useCachedCDashData = True
    alwaysUseCacheFileIfExists = True
    verbose = False
    printDetails = False

    # Construct the functor
    addTestHistoryFunctor = AddTestHistoryToTestDictFunctor(
      cdashUrl, projectName, date, testingDayStartTimeUtc, daysOfHistory,
      testCacheOutputDir, useCachedCDashData, alwaysUseCacheFileIfExists,
      verbose, printDetails,
      )

    # Apply the functor to add the test history to the test dict.  This will
    # also fill in the missing data for the testDict.
    addTestHistoryFunctor(testDict)

    # Check the set fields out output
    self.assertEqual(testDict['site'], 'site_name')
    self.assertEqual(testDict['buildName'], 'build_name')
    self.assertEqual(testDict['buildName_url'],
      u('site.com/cdash/index.php?project=projectName&begin=2000-12-28&end=2001-01-01&filtercombine=and&filtercombine=&filtercount=2&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=site&compare2=61&value2=site_name')
      )
    self.assertEqual(testDict['testname'], 'test_name')
    self.assertEqual(testDict['testname_url'], u('site.com/cdash/testDetails.php?test=<testid>&build=<buildid>'))
    self.assertEqual(testDict['status'], 'Passed')
    self.assertEqual(testDict['details'], 'Completed (Passed)\n')
    self.assertEqual(testDict['status_url'], u('site.com/cdash/testDetails.php?test=<testid>&build=<buildid>'))
    self.assertEqual(testDict['test_history_num_days'], 5)
    self.assertEqual(testDict['test_history_query_url'], testHistoryQueryUrl)
    self.assertEqual(testDict['test_history_browser_url'], u('site.com/cdash/queryTests.php?project=projectName&begin=2000-12-28&end=2001-01-01&filtercombine=and&filtercombine=&filtercount=3&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=testname&compare2=61&value2=test_name&field3=site&compare3=61&value3=site_name')
      )
    self.assertEqual(
      testDict['test_history_list'][0]['buildstarttime'], '2001-01-01T05:54:03 UTC')
    self.assertEqual(
      testDict['test_history_list'][1]['buildstarttime'], '2000-12-31T05:54:03 UTC')
    self.assertEqual(
      testDict['test_history_list'][2]['buildstarttime'], '2000-12-30T05:54:03 UTC')
    self.assertEqual(
      testDict['test_history_list'][3]['buildstarttime'], '2000-12-29T05:54:03 UTC')
    self.assertEqual(
      testDict['test_history_list'][4]['buildstarttime'], '2000-12-28T05:54:03 UTC')
    self.assertEqual(testDict['nopass_last_x_days'], 3)
    self.assertEqual(testDict['nopass_last_x_days_url'],
       u('site.com/cdash/queryTests.php?project=projectName&begin=2000-12-28&end=2001-01-01&filtercombine=and&filtercombine=&filtercount=3&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=testname&compare2=61&value2=test_name&field3=site&compare3=61&value3=site_name') )
    self.assertEqual(testDict['previous_nopass_date'], '2000-12-30')
    #self.assertEqual(testDict['previous_nopass_date_url'], 'DUMMY NO MATCH')
    self.assertEqual(testDict['issue_tracker'], '#1234')
    self.assertEqual(testDict['issue_tracker_url'], 'some.com/site/issue/1234')


  # Test the case where the testDict just has the minimal fields that come the
  # tests with issue trackers CSV file and the tests did not actually run in
  # the current testing day.
  def test_empty_test_missing(self):

    # Initial test dict as it would come from the tests with issue trackers
    # CSV file
    testDict = {
      u('site'): u('site_name'),
      u('buildName'): u('build_name'),
      u('testname'): u('test_name'),
      u('issue_tracker'): u('#1234'),
      u('issue_tracker_url'): u('some.com/site/issue/1234')
    }
    
    # Target test date
    testHistoryQueryUrl = \
      u('site.com/cdash/api/v1/queryTests.php?project=projectName&begin=2000-12-28&end=2001-01-01&filtercombine=and&filtercombine=&filtercount=3&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=testname&compare2=61&value2=test_name&field3=site&compare3=61&value3=site_name')

    # Create a subdir for the created cache file
    testCacheOutputDir = \
      os.getcwd()+"/AddTestHistoryToTestDictFunctor/test_empty_test_missing"
    if os.path.exists(testCacheOutputDir): shutil.rmtree(testCacheOutputDir)
    os.makedirs(testCacheOutputDir)

    # Create dummy test history and put it in a cache file
    testHistoryLOD = getTestHistoryLOD5(
      [
        'will be removed',
        'will be removed',
        'Failed',
        'Failed',
        'Failed',
        ]
      )
    del testHistoryLOD[0]  # These should get read of two most recent days!
    del testHistoryLOD[0]
    testHistoryJson = { 'builds' : testHistoryLOD }
    testHistorycacheFilePath = \
      testCacheOutputDir+"/2001-01-01-site_name-build_name-test_name-HIST-5.json"
    pprintPythonDataToFile(testHistoryJson, testHistorycacheFilePath)

    # Construct arguments
    cdashUrl = "site.com/cdash"
    projectName = "projectName"
    date = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    useCachedCDashData = True
    alwaysUseCacheFileIfExists = True
    verbose = False
    printDetails = False

    # Construct the functor
    addTestHistoryFunctor = AddTestHistoryToTestDictFunctor(
      cdashUrl, projectName, date, testingDayStartTimeUtc, daysOfHistory,
      testCacheOutputDir, useCachedCDashData, alwaysUseCacheFileIfExists,
      verbose, printDetails,
      )

    # Apply the functor to add the test history to the test dict.  This will
    # also fill in some data but will mark the test as "Missing".
    addTestHistoryFunctor(testDict)

    # Check the set fields out output
    self.assertEqual(testDict['site'], 'site_name')
    self.assertEqual(testDict['buildName'], 'build_name')
    self.assertEqual(testDict['buildName_url'],
      u('site.com/cdash/index.php?project=projectName&begin=2000-12-28&end=2001-01-01&filtercombine=and&filtercombine=&filtercount=2&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=site&compare2=61&value2=site_name')
      )
    self.assertEqual(testDict['testname'], 'test_name')
    self.assertEqual(testDict.get('testname_url',None), None)
    self.assertEqual(testDict['status'], 'Missing')
    self.assertEqual(testDict['details'], 'Missing')
    self.assertEqual(testDict.get('status_url', None), None)
    self.assertEqual(testDict['test_history_num_days'], 5)
    self.assertEqual(testDict['test_history_query_url'], testHistoryQueryUrl)
    self.assertEqual(testDict['test_history_browser_url'], u('site.com/cdash/queryTests.php?project=projectName&begin=2000-12-28&end=2001-01-01&filtercombine=and&filtercombine=&filtercount=3&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=testname&compare2=61&value2=test_name&field3=site&compare3=61&value3=site_name')
      )
    self.assertEqual(len(testDict['test_history_list']), 3)
    self.assertEqual(
      testDict['test_history_list'][0]['buildstarttime'], '2000-12-30T05:54:03 UTC')
    self.assertEqual(
      testDict['test_history_list'][1]['buildstarttime'], '2000-12-29T05:54:03 UTC')
    self.assertEqual(
      testDict['test_history_list'][2]['buildstarttime'], '2000-12-28T05:54:03 UTC')
    self.assertEqual(testDict['nopass_last_x_days'], 3)
    self.assertEqual(testDict['nopass_last_x_days_url'],
       u('site.com/cdash/queryTests.php?project=projectName&begin=2000-12-28&end=2001-01-01&filtercombine=and&filtercombine=&filtercount=3&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=testname&compare2=61&value2=test_name&field3=site&compare3=61&value3=site_name') )
    self.assertEqual(testDict['previous_nopass_date'], '2000-12-30')
    #self.assertEqual(testDict['previous_nopass_date_url'], 'DUMMY NO MATCH')
    self.assertEqual(testDict['issue_tracker'], '#1234')
    self.assertEqual(testDict['issue_tracker_url'], 'some.com/site/issue/1234')


  # Test the case where the testDict just has the minimal fields that come
  # from the tests with issue trackers CSV file and the test did not actually
  # run in the current testing day.  Also, in this case, there is no recent
  # test history.
  def test_empty_test_missing_no_recent_history(self):

    # Initial test dict as it would come from the tests with issue trackers
    # CSV file
    testDict = {
      u('site'): u('site_name'),
      u('buildName'): u('build_name'),
      u('testname'): u('test_name'),
      u('issue_tracker'): u('#1234'),
      u('issue_tracker_url'): u('some.com/site/issue/1234')
    }
    
    # Target test date
    testHistoryQueryUrl = \
      u('site.com/cdash/api/v1/queryTests.php?project=projectName&begin=2000-12-28&end=2001-01-01&filtercombine=and&filtercombine=&filtercount=3&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=testname&compare2=61&value2=test_name&field3=site&compare3=61&value3=site_name')

    # Create a subdir for the created cache file
    testCacheOutputDir = \
      os.getcwd()+"/AddTestHistoryToTestDictFunctor/test_empty_test_missing_no_recent_history"
    if os.path.exists(testCacheOutputDir): shutil.rmtree(testCacheOutputDir)
    os.makedirs(testCacheOutputDir)

    # Create dummy test history with no recent tests
    testHistoryLOD = []
    testHistoryJson = { 'builds' : testHistoryLOD }
    testHistorycacheFilePath = \
      testCacheOutputDir+"/2001-01-01-site_name-build_name-test_name-HIST-5.json"
    pprintPythonDataToFile(testHistoryJson, testHistorycacheFilePath)

    # Construct arguments
    cdashUrl = "site.com/cdash"
    projectName = "projectName"
    date = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    useCachedCDashData = True
    alwaysUseCacheFileIfExists = True
    verbose = False
    printDetails = False

    # Construct the functor
    addTestHistoryFunctor = AddTestHistoryToTestDictFunctor(
      cdashUrl, projectName, date, testingDayStartTimeUtc, daysOfHistory,
      testCacheOutputDir, useCachedCDashData, alwaysUseCacheFileIfExists,
      verbose, printDetails,
      )

    # Apply the functor to add the test history to the test dict.  This will
    # also fill in some data but will mark the test as "Missing".
    addTestHistoryFunctor(testDict)

    # Check the set fields out output
    self.assertEqual(testDict['site'], 'site_name')
    self.assertEqual(testDict['buildName'], 'build_name')
    self.assertEqual(testDict['buildName_url'],
      u('site.com/cdash/index.php?project=projectName&begin=2000-12-28&end=2001-01-01&filtercombine=and&filtercombine=&filtercount=2&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=site&compare2=61&value2=site_name')
      )
    self.assertEqual(testDict['testname'], 'test_name')
    self.assertEqual(testDict.get('testname_url',None), None)
    self.assertEqual(testDict['status'], 'Missing')
    self.assertEqual(testDict['details'], 'Missing')
    self.assertEqual(testDict.get('status_url', None), None)
    self.assertEqual(testDict['test_history_num_days'], 5)
    self.assertEqual(testDict['test_history_query_url'], testHistoryQueryUrl)
    self.assertEqual(testDict['test_history_browser_url'], u('site.com/cdash/queryTests.php?project=projectName&begin=2000-12-28&end=2001-01-01&filtercombine=and&filtercombine=&filtercount=3&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=testname&compare2=61&value2=test_name&field3=site&compare3=61&value3=site_name')
      )
    self.assertEqual(len(testDict['test_history_list']), 0)
    self.assertEqual(testDict['nopass_last_x_days'], 0)
    self.assertEqual(testDict['nopass_last_x_days_url'],
       u('site.com/cdash/queryTests.php?project=projectName&begin=2000-12-28&end=2001-01-01&filtercombine=and&filtercombine=&filtercount=3&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=testname&compare2=61&value2=test_name&field3=site&compare3=61&value3=site_name') )
    self.assertEqual(testDict['previous_nopass_date'], 'None')
    #self.assertEqual(testDict['previous_nopass_date_url'], 'DUMMY NO MATCH')
    self.assertEqual(testDict['issue_tracker'], '#1234')
    self.assertEqual(testDict['issue_tracker_url'], 'some.com/site/issue/1234')


  # Check error message for case where the status of the test history is not
  # equal to the status of the test dict for a non-passsing test.  This can
  # happen when the history cdash/queryTests.php query for test history brings
  # in a test more recent than the top-level cdash/queryTests.php query for
  # non-passing tests.
  def test_mismatch_top_test_history_status(self):

    # Initial test dict as it would come from the tests with issue trackers
    # CSV file (could be passing or failing, we don't know)
    testDict = {
      u('site'): u('site_name'),
      u('buildName'): u('build_name'),
      u('testname'): u('test_name'),
      u('issue_tracker'): u('#1234'),
      u('issue_tracker_url'): u('some.com/site/issue/1234')
    }

    # Target test date
    testHistoryQueryUrl = \
      u('site.com/cdash/api/v1/queryTests.php?project=projectName&begin=2000-12-28&end=2001-01-01&filtercombine=and&filtercombine=&filtercount=3&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=testname&compare2=61&value2=test_name&field3=site&compare3=61&value3=site_name')

    # Create a subdir for the created cache file
    testCacheOutputDir = \
      os.getcwd()+"/AddTestHistoryToTestDictFunctor/test_mismatch_top_test_history_status"
    if os.path.exists(testCacheOutputDir): shutil.rmtree(testCacheOutputDir)
    os.makedirs(testCacheOutputDir)

    # Create dummy test history
    testHistoryLOD = getTestHistoryLOD5(
      [
        'Failed',  # Top test can't be not 'Passed' or missing!
        'Failed',
        'Passed',
        'Passed',
        'Not Run',
        ]
      )

    # Make it easy to find the top test dict below
    testHistoryLOD.sort(reverse=True, key=DictSortFunctor(['buildstarttime']))

    # Construct arguments
    cdashUrl = "site.com/cdash"
    projectName = "projectName"
    date = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    useCachedCDashData = False
    alwaysUseCacheFileIfExists = False
    verbose = False
    printDetails = False
    requireMatchTestTopTestHistory = True
    mockExtractCDashApiQueryDataFunctor = MockExtractCDashApiQueryDataFunctor(
      testHistoryQueryUrl, {'builds':testHistoryLOD})

    # Construct the functor
    addTestHistoryFunctor = AddTestHistoryToTestDictFunctor(
      cdashUrl, projectName, date, testingDayStartTimeUtc, daysOfHistory,
      testCacheOutputDir, useCachedCDashData, alwaysUseCacheFileIfExists,
      verbose, printDetails, requireMatchTestTopTestHistory,
      mockExtractCDashApiQueryDataFunctor,
      )

    # Apply the functor to add the test history to the test dict
    try:
      addTestHistoryFunctor(testDict)
      self.assertTrue(False)
    except Exception as errMsg:
      self.assertEqual( str(errMsg),
        "Error, test testDict['status'] = 'None' != "+\
        "top test history testStatus = 'Failed'"+\
        " where:\n\n"+\
        "   testDict = "+sorted_dict_str(testDict)+"\n\n"+\
        "   top test history dict = "+sorted_dict_str(testHistoryLOD[0])+"\n\n" )


  # Check the case where a tracked test with a minimal testDict that is not
  # failing in the list of global nonpassing tests (and is therefore
  # considered missing) actually has test history for the current testing day
  # which will typically be 'Failed'.  This is to support the use case where
  # random system failures get sorted out of the global list of nonpassing
  # tests but tracked test may be failing in the current.
  def test_mismatch_top_test_history_status_allowed(self):

    # Initial test dict as it would come from the tests with issue trackers
    # CSV file (was not presenet in global nonpassing list of tests due to
    # matching extra filtering criterial).
    testDict = {
      u('site'): u('site_name'),
      u('buildName'): u('build_name'),
      u('testname'): u('test_name'),
      u('issue_tracker'): u('#1234'),
      u('issue_tracker_url'): u('some.com/site/issue/1234')
    }

    # Target test date
    testHistoryQueryUrl = \
      u('site.com/cdash/api/v1/queryTests.php?project=projectName&begin=2000-12-28&end=2001-01-01&filtercombine=and&filtercombine=&filtercount=3&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=testname&compare2=61&value2=test_name&field3=site&compare3=61&value3=site_name')

    # Create a subdir for the created cache file
    testCacheOutputDir = \
      os.getcwd()+"/AddTestHistoryToTestDictFunctor/test_mismatch_top_test_history_status"
    if os.path.exists(testCacheOutputDir): shutil.rmtree(testCacheOutputDir)
    os.makedirs(testCacheOutputDir)

    # Create dummy test history
    testHistoryLOD = getTestHistoryLOD5(
      [
        'Failed',  # This test got filtered out of the global list of nonpassing tests 
        'Failed',
        'Passed',
        'Passed',
        'Not Run',
        ]
      )

    # Make it easy to find the top test dict below
    testHistoryLOD.sort(reverse=True, key=DictSortFunctor(['buildstarttime']))

    # Construct arguments
    cdashUrl = "site.com/cdash"
    projectName = "projectName"
    date = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    useCachedCDashData = False
    alwaysUseCacheFileIfExists = False
    verbose = False
    printDetails = False
    requireMatchTestTopTestHistory = False
    mockExtractCDashApiQueryDataFunctor = MockExtractCDashApiQueryDataFunctor(
      testHistoryQueryUrl, {'builds':testHistoryLOD})

    # Construct the functor
    addTestHistoryFunctor = AddTestHistoryToTestDictFunctor(
      cdashUrl, projectName, date, testingDayStartTimeUtc, daysOfHistory,
      testCacheOutputDir, useCachedCDashData, alwaysUseCacheFileIfExists,
      verbose, printDetails, requireMatchTestTopTestHistory,
      mockExtractCDashApiQueryDataFunctor,
      )

    # Apply the functor to add the test history to the test dict.  This will
    # also fill in the missing data for the testDict.
    addTestHistoryFunctor(testDict)

    # Check the set fields out output
    self.assertEqual(testDict['site'], 'site_name')
    self.assertEqual(testDict['buildName'], 'build_name')
    self.assertEqual(testDict['testname'], 'test_name')
    self.assertEqual(testDict['testname_url'], u('site.com/cdash/testDetails.php?test=<testid>&build=<buildid>'))
    self.assertEqual(testDict['status'], 'Missing / Failed')
    self.assertEqual(testDict['details'], 'Completed (Failed)\n')
    self.assertEqual(testDict['status_url'], u('site.com/cdash/testDetails.php?test=<testid>&build=<buildid>'))
    self.assertEqual(testDict['test_history_num_days'], 5)
    self.assertEqual(testDict['test_history_query_url'], testHistoryQueryUrl)
    self.assertEqual(
      testDict['test_history_list'][0]['buildstarttime'], '2001-01-01T05:54:03 UTC')
    self.assertEqual(
      testDict['test_history_list'][1]['buildstarttime'], '2000-12-31T05:54:03 UTC')
    self.assertEqual(
      testDict['test_history_list'][2]['buildstarttime'], '2000-12-30T05:54:03 UTC')
    self.assertEqual(
      testDict['test_history_list'][3]['buildstarttime'], '2000-12-29T05:54:03 UTC')
    self.assertEqual(
      testDict['test_history_list'][4]['buildstarttime'], '2000-12-28T05:54:03 UTC')
    self.assertEqual(testDict['nopass_last_x_days'], 3)
    self.assertEqual(testDict['issue_tracker'], '#1234')
    self.assertEqual(testDict['issue_tracker_url'], 'some.com/site/issue/1234')


  # Check error message for case where the 'buildstarttime' of the test
  # history is not equal to the 'buildstarttime' of the test dict for a
  # non-passsing test.  This can happen when the history cdash/queryTests.php
  # query for test history brings in a test more recent than the top-level
  # cdash/queryTests.php query for non-passing tests.
  def test_mismatch_top_test_history_buildstarttime(self):

    # Deep copy the test dict so we don't modify the original
    testDict = copy.deepcopy(g_testDictFailed)

    # Target test date
    testHistoryQueryUrl = \
      u('site.com/cdash/api/v1/queryTests.php?project=projectName&begin=2000-12-28&end=2001-01-01&filtercombine=and&filtercombine=&filtercount=3&showfilters=1&filtercombine=and&field1=buildname&compare1=61&value1=build_name&field2=testname&compare2=61&value2=test_name&field3=site&compare3=61&value3=site_name')

    # Create a subdir for the created cache file
    testCacheOutputDir = \
      os.getcwd()+"/AddTestHistoryToTestDictFunctor/test_mismatch_top_test_history_buildstarttime"
    if os.path.exists(testCacheOutputDir): shutil.rmtree(testCacheOutputDir)
    os.makedirs(testCacheOutputDir)

    # Create dummy test history
    testHistoryLOD = getTestHistoryLOD5(
      [
        'Failed',  # Top test can't be not 'Passed' or missing!
        'Failed',
        'Passed',
        'Passed',
        'Not Run',
        ]
      )

    # Make it easy to find the top test dict below
    testHistoryLOD.sort(reverse=True, key=DictSortFunctor(['buildstarttime']))

    # Add a more recent test that also fails!
    moreRecentTestDict = copy.deepcopy(g_testDictFailed)
    moreRecentTestDict[u('buildstarttime')] = u('2001-01-01T08:00:00 UTC')
    testHistoryLOD.insert(0, moreRecentTestDict)

    # Construct arguments
    cdashUrl = "site.com/cdash"
    projectName = "projectName"
    date = "2001-01-01"
    testingDayStartTimeUtc = "00:00"
    daysOfHistory = 5
    useCachedCDashData = False
    alwaysUseCacheFileIfExists = False
    verbose = False
    printDetails = False
    requireMatchTestTopTestHistory = True
    mockExtractCDashApiQueryDataFunctor = MockExtractCDashApiQueryDataFunctor(
      testHistoryQueryUrl, {'builds':testHistoryLOD})

    # Construct the functor
    addTestHistoryFunctor = AddTestHistoryToTestDictFunctor(
      cdashUrl, projectName, date, testingDayStartTimeUtc, daysOfHistory,
      testCacheOutputDir, useCachedCDashData, alwaysUseCacheFileIfExists,
      verbose, printDetails, requireMatchTestTopTestHistory, 
      mockExtractCDashApiQueryDataFunctor,
      )

    # Apply the functor to add the test history to the test dict
#    addTestHistoryFunctor(testDict)
    try:
      addTestHistoryFunctor(testDict)
      self.assertTrue(False)
    except Exception as errMsg:
      None
      self.assertEqual( str(errMsg),
        "Error, testDict['buildstarttime'] = '2001-01-01T05:54:03 UTC' != "+\
        "top test history 'buildstarttime' = '2001-01-01T08:00:00 UTC'"+\
        " where:\n\n"+\
        "   testDict = "+sorted_dict_str(testDict)+"\n\n"+\
        "   top test history dict = "+sorted_dict_str(testHistoryLOD[0])+"\n\n" )


#############################################################################
#
# Test CDashQueryAnalyzeReport.addCDashTestingDayFunctor
#
#############################################################################


class test_addCDashTestingDayFunctor(unittest.TestCase):

  def test_1(self):
    testDict = { u('testname'):u('test1') }
    addCDashTestingDayFunctor = AddCDashTestingDayFunctor("YYYY-MM-DD")
    testDict=addCDashTestingDayFunctor(testDict)
    testDict_expected = { u('testname'):u('test1'), u('cdash_testing_day'):u('YYYY-MM-DD')}
    self.assertEqual(testDict, testDict_expected)


#############################################################################
#
# Test CDashQueryAnalyzeReport.buildHasConfigureFailures()
#
#############################################################################

class test_buildHasConfigureFailures(unittest.TestCase):

  def test_has_no_configure_failures(self):
    buildDict = copy.deepcopy(g_singleBuildPassesExtended)
    self.assertEqual(buildHasConfigureFailures(buildDict), False)

  def test_has_configure_failures(self):
    buildDict = copy.deepcopy(g_singleBuildPassesExtended)
    buildDict['configure']['error'] = 1
    self.assertEqual(buildHasConfigureFailures(buildDict), True)

  def test_has_no_configure_results(self):
    buildDict = copy.deepcopy(g_singleBuildPassesExtended)
    del buildDict['configure']
    self.assertEqual(buildHasConfigureFailures(buildDict), False)


#############################################################################
#
# Test CDashQueryAnalyzeReport.buildHasBuildFailures()
#
#############################################################################

class test_buildHasBuildFailures(unittest.TestCase):

  def test_has_no_build_failures(self):
    buildDict = copy.deepcopy(g_singleBuildPassesExtended)
    self.assertEqual(buildHasBuildFailures(buildDict), False)

  def test_has_build_failures(self):
    buildDict = copy.deepcopy(g_singleBuildPassesExtended)
    buildDict['compilation']['error'] = 1
    self.assertEqual(buildHasBuildFailures(buildDict), True)

  def test_has_no_build_results(self):
    buildDict = copy.deepcopy(g_singleBuildPassesExtended)
    del buildDict['compilation']
    self.assertEqual(buildHasBuildFailures(buildDict), False)


#############################################################################
#
# Test CDashQueryAnalyzeReport.isTestPassed(), isTestFailed() and
# isTestNotRun()
#
#############################################################################

def testDictStatus(status):
  testDict = copy.deepcopy(g_testDictFailed)
  testDict['status'] = status
  return testDict

class test_isTestPassed(unittest.TestCase):

  def test_passed(self):
    self.assertEqual(isTestPassed(testDictStatus('Passed')), True)

  def test_failed(self):
    self.assertEqual(isTestPassed(testDictStatus('Failed')), False)

  def test_notrun(self):
    self.assertEqual(isTestPassed(testDictStatus('Not Run')), False)

  def test_missing(self):
    self.assertEqual(isTestPassed(testDictStatus('Missing')), False)
    self.assertEqual(isTestPassed(testDictStatus('Missing / Failed')), False)

class test_isTestFailed(unittest.TestCase):

  def test_passed(self):
    self.assertEqual(isTestFailed(testDictStatus('Passed')), False)

  def test_failed(self):
    self.assertEqual(isTestFailed(testDictStatus('Failed')), True)

  def test_notrun(self):
    self.assertEqual(isTestFailed(testDictStatus('Not Run')), False)

  def test_missing(self):
    self.assertEqual(isTestFailed(testDictStatus('Missing')), False)
    self.assertEqual(isTestFailed(testDictStatus('Missing / Failed')), False)

class test_isTestNotRun(unittest.TestCase):

  def test_passed(self):
    self.assertEqual(isTestNotRun(testDictStatus('Passed')), False)

  def test_failed(self):
    self.assertEqual(isTestNotRun(testDictStatus('Failed')), False)

  def test_notrun(self):
    self.assertEqual(isTestNotRun(testDictStatus('Not Run')), True)

  def test_missing(self):
    self.assertEqual(isTestNotRun(testDictStatus('Missing')), False)
    self.assertEqual(isTestNotRun(testDictStatus('Missing / Failed')), False)

class test_isTestMissing(unittest.TestCase):

  def test_passed(self):
    self.assertEqual(isTestMissing(testDictStatus('Passed')), False)

  def test_failed(self):
    self.assertEqual(isTestMissing(testDictStatus('Failed')), False)

  def test_notrun(self):
    self.assertEqual(isTestMissing(testDictStatus('Not Run')), False)

  def test_missing(self):
    self.assertEqual(isTestMissing(testDictStatus('Missing')), True)
    self.assertEqual(isTestMissing(testDictStatus('Missing / Failed')), True)



#############################################################################
#
# Test CDashQueryAnalyzeReport.sortAndLimitListOfDicts()
#
#############################################################################

def createDictForTest(data1, data2, data3):
  return { 'key1':data1, 'key2':data2, 'key3':data3 }

def createDictForTestWithUrl(data1, data2, data3):
  return {
    'key1':data1[0], 'key1_url':data1[1],
    'key2':data2[0], 'key2_url':data2[1],
    'key3':data3[0], 'key3_url':data3[1],
    }
 
class test_sortAndLimitListOfDicts(unittest.TestCase):
  
  def test_no_sort_no_limit(self):
    cd = createDictForTest
    origList = [
      cd("c1_a", 3, "c2_b"),
      cd("c1_a", 1, "c2_c"),
      cd("c1_b", 2, "c2_a"),
      ]
    resultList = sortAndLimitListOfDicts(origList)
    resultList_expected = origList
    self.assertEqual(resultList, resultList_expected)
  
  def test_multicol_sort_no_limit(self):
    cd = createDictForTest
    origList = [
      cd("c1_a", 3, "c2_b"),
      cd("c1_b", 2, "c2_a"),
      cd("c1_a", 1, "c2_c"),
      ]
    resultList = sortAndLimitListOfDicts(origList,  ['key1', 'key2'])
    resultList_expected = [
      cd("c1_a", 1, "c2_c"),
      cd("c1_a", 3, "c2_b"),
      cd("c1_b", 2, "c2_a"),
      ]
    self.assertEqual(resultList, resultList_expected)
  
  def test_multicol_sort_limit_2(self):
    cd = createDictForTest
    origList = [
      cd("c1_a", 3, "c2_b"),
      cd("c1_b", 2, "c2_a"),
      cd("c1_a", 1, "c2_c"),
      ]
    resultList = sortAndLimitListOfDicts(origList,  ['key1', 'key2'], 2)
    resultList_expected = [
      cd("c1_a", 1, "c2_c"),
      cd("c1_a", 3, "c2_b"),
      ]
    self.assertEqual(resultList, resultList_expected)
  
  def test_multicol_sort_limit_3(self):
    cd = createDictForTest
    origList = [
      cd("c1_a", 3, "c2_b"),
      cd("c1_b", 2, "c2_a"),
      cd("c1_a", 1, "c2_c"),
      ]
    resultList = sortAndLimitListOfDicts(origList,  ['key1', 'key2'], 3)
    resultList_expected = [
      cd("c1_a", 1, "c2_c"),
      cd("c1_a", 3, "c2_b"),
      cd("c1_b", 2, "c2_a"),
      ]
    self.assertEqual(resultList, resultList_expected)
  
  def test_multicol_sort_limit_4(self):
    cd = createDictForTest
    origList = [
      cd("c1_a", 3, "c2_b"),
      cd("c1_b", 2, "c2_a"),
      cd("c1_a", 1, "c2_c"),
      ]
    resultList = sortAndLimitListOfDicts(origList,  ['key1', 'key2'], 4)
    resultList_expected = [
      cd("c1_a", 1, "c2_c"),
      cd("c1_a", 3, "c2_b"),
      cd("c1_b", 2, "c2_a"),
      ]
    self.assertEqual(resultList, resultList_expected)
  
  def test_multicol_sort_limit_0(self):
    cd = createDictForTest
    origList = [
      cd("c1_a", 3, "c2_b"),
      cd("c1_b", 2, "c2_a"),
      cd("c1_a", 1, "c2_c"),
      ]
    resultList = sortAndLimitListOfDicts(origList,  ['key1', 'key2'], 0)
    resultList_expected = []
    self.assertEqual(resultList, resultList_expected)
  
  def test_no_sort_limit_2(self):
    cd = createDictForTest
    origList = [
      cd("c1_a", 3, "c2_b"),
      cd("c1_a", 1, "c2_c"),
      cd("c1_b", 2, "c2_a"),
      ]
    resultList = sortAndLimitListOfDicts(origList, None, 2)
    resultList_expected = [
      cd("c1_a", 3, "c2_b"),
      cd("c1_a", 1, "c2_c"),
      ]
    self.assertEqual(resultList, resultList_expected)
  
  def test_sort_key2_no_limit(self):
    cd = createDictForTest
    origList = [
      cd("c1_a", 3, "c2_b"),
      cd("c1_a", 1, "c2_c"),
      cd("c1_b", 2, "c2_a"),
      ]
    resultList = sortAndLimitListOfDicts(origList, ['key2'])
    resultList_expected = [
      cd("c1_a", 1, "c2_c"),
      cd("c1_b", 2, "c2_a"),
      cd("c1_a", 3, "c2_b"),
      ]
    self.assertEqual(resultList, resultList_expected)


#############################################################################
#
# Test CDashQueryAnalyzeReport.colorHtmlText()
#
#############################################################################
 
class test_colorHtmlText(unittest.TestCase):

  def test_none(self):
    self.assertEqual(colorHtmlText("some text", None), "some text")

  def test_empty(self):
    self.assertEqual(colorHtmlText("some text", ""), "some text")

  def test_red(self):
    self.assertEqual(colorHtmlText("some text", "red"),
      "<font color=\"red\">some text</font>")

  def test_green(self):
    self.assertEqual(colorHtmlText("some text", "green"),
      "<font color=\"green\">some text</font>")

  def test_gray(self):
    self.assertEqual(colorHtmlText("some text", "gray"),
      "<font color=\"gray\">some text</font>")

  def test_orange(self):
    self.assertEqual(colorHtmlText("some text", "orange"),
      "<font color=\"orange\">some text</font>")

  def test_invalid(self):
    try:
      coloredText = colorHtmlText("some text", "badcolor")
      self.assertTrue(False)   # Should not get here!
    except Exception as errMsg:
      self.assertEqual(str(errMsg),
        "Error, color='badcolor' is invalid.  Only 'red', 'green',"+\
        " 'gray' and 'orange' are supported!" )


#############################################################################
#
# Test CDashQueryAnalyzeReport.addHtmlSoftWordBreaks()
#
#############################################################################
 
class test_addHtmlSoftWordBreaks(unittest.TestCase):

  def test_1(self):
    self.assertEqual(addHtmlSoftWordBreaks("some_long_name"),
      "some_&shy;long_&shy;name")



#############################################################################
#
# Test CDashQueryAnalyzeReport.getFullCDashHtmlReportPageStr()
#
#############################################################################


class test_getFullCDashHtmlReportPageStr(unittest.TestCase):

  def setUp(self):
    cdashReportData = CDashReportData()
    cdashReportData.htmlEmailBodyTop = "body top\n"
    cdashReportData.htmlEmailBodyBottom = "body bottom\n"
    self.cdashReportData = cdashReportData

  def test_defaults(self):
    reportHtml = getFullCDashHtmlReportPageStr(self.cdashReportData)
    reportHtml_expected = \
"""<html>

<body>

body top

body bottom

</body>

</html>
"""
    self.assertEqual(reportHtml, reportHtml_expected)

  def test_with_title(self):
    reportHtml = getFullCDashHtmlReportPageStr(self.cdashReportData,
      pageTitle="page title")
    reportHtml_expected = \
"""<html>

<body>

<h2>page title</h2>

body top

body bottom

</body>

</html>
"""
    self.assertEqual(reportHtml, reportHtml_expected)

  def test_with_title_and_style(self):
    reportHtml = getFullCDashHtmlReportPageStr(self.cdashReportData,
      pageTitle="page title", pageStyle="<style>my style</style>\n")
    reportHtml_expected = \
"""<html>

<head>
<style>my style</style>
</head>

<body>

<h2>page title</h2>

body top

body bottom

</body>

</html>
"""
    self.assertEqual(reportHtml, reportHtml_expected)

  def test_with_title_and_style_and_details(self):
    reportHtml = getFullCDashHtmlReportPageStr(self.cdashReportData,
      pageTitle="page title", pageStyle="<style>my style</style>\n",
      detailsBlockSummary="these are the details")
    reportHtml_expected = \
"""<html>

<head>
<style>my style</style>
</head>

<body>

<h2>page title</h2>

body top

<details>

<summary><b>these are the details:</b> (click to expand)</b></summary>

body bottom

</details>

</body>

</html>
"""
    self.assertEqual(reportHtml, reportHtml_expected)


#############################################################################
#
# Test CDashQueryAnalyzeReport.createHtmlTableStr()
#
#############################################################################
 
class test_createHtmlTableStr(unittest.TestCase):
  
  # Check that the contents are put in the right place, the correct alignment,
  # correct handling of non-string data, etc.
  def test_3x3_table_correct_contents(self):
    tcd = TableColumnData
    trd = createDictForTest
    colDataList = [
      tcd("Data 3", 'key3'),
      tcd("Data 1", 'key1'),
      tcd("Data 2", 'key2', "right"),  # Alignment and non-string dat3
      ]
    rowDataList = [
      trd("r1d1", 1, "r1d3"),
      trd("r2d1", 2, "r2d3"),
      trd("r3d1", 3, "r3d3"),
      ]
    htmlTable = createHtmlTableStr("My great data", colDataList, rowDataList,
      htmlStyle="<style>my_style</style>",  # Test custom table style
      #htmlStyle=None,       # Uncomment to view this style
      #htmlTableStyle="",    # Uncomment to view this style
      )
    #print(htmlTable)
    #with open("test_3x2_table.html", 'w') as outFile: outFile.write(htmlTable)
    # NOTE: Above, uncomment the htmlStyle=None, ... line and the print and
    # file write commands to view the formatted table in a browser to see if
    # this gets the data right and you like the default table style.
    htmlTable_expected = \
r"""<style>my_style</style>
<h3>My great data</h3>
<table style="width:100%" boarder="1">

<tr>
<th>Data 3</th>
<th>Data 1</th>
<th>Data 2</th>
</tr>

<tr>
<td align="left">r1d3</td>
<td align="left">r1d1</td>
<td align="right">1</td>
</tr>

<tr>
<td align="left">r2d3</td>
<td align="left">r2d1</td>
<td align="right">2</td>
</tr>

<tr>
<td align="left">r3d3</td>
<td align="left">r3d1</td>
<td align="right">3</td>
</tr>

</table>

"""
    self.assertEqual(htmlTable, htmlTable_expected)

  # Check the correct default table style is set
  def test_1x1_table_correct_style(self):
    tcd = TableColumnData
    colDataList = [  tcd("Data 1", 'key1') ]
    rowDataList = [ {'key1':'data1'} ]
    htmlTable = createHtmlTableStr("My great data", colDataList, rowDataList, htmlTableStyle="")
    #print(htmlTable)
    #with open("test_1x1_table_style.html", 'w') as outFile: outFile.write(htmlTable)
    # NOTE: Above, uncomment the print and file write to view the formatted
    # table in a browser to see if this gets the data right and you like the
    # default table style.
    htmlTable_expected = \
r"""<style>table, th, td {
  padding: 5px;
  border: 1px solid black;
  border-collapse: collapse;
}
tr:nth-child(even) {background-color: #eee;}
tr:nth-child(odd) {background-color: #fff;}
</style>
<h3>My great data</h3>
<table >

<tr>
<th>Data 1</th>
</tr>

<tr>
<td align="left">data1</td>
</tr>

</table>

"""
    self.assertEqual(htmlTable, htmlTable_expected)

  # Check that a bad column dict key name throws
  def test_1x1_bad_key_fail(self):
    tcd = TableColumnData
    colDataList = [  tcd("Data 1", 'badKey') ]
    rowDataList = [ {'key1':'data1'} ]
    try:
      htmlTable = createHtmlTableStr("Title", colDataList, rowDataList)
      self.assertEqual("Exception did not get throw!", "No it did not!")
    except Exception as errMsg:
      self.assertEqual(str(errMsg),
         "Error, column 0 dict key='badKey' row 0 entry is 'None' which is"+\
         " not allowed!\n\nRow dict = {'key1': 'data1'}")
  
  # Check that the contents are put in the right place, the correct alignment,
  # correct handling of non-string data, etc.
  def test_3x3_table_with_url_correct_contents(self):
    tcd = TableColumnData
    trdu = createDictForTestWithUrl
    colDataList = [
      tcd("Data 3", 'key3'),
      tcd("Data 1", 'key1'),
      tcd("Data 2", 'key2', "right"),  # Alignment and non-string dat3
      ]
    rowDataList = [
      trdu(["r1d1","some.com/r1d1"], [1,"some.com/r1d2"], ["r1_d3","some.com/r1d3"]),
      trdu(["r2d1","some.com/r2d1"], [2,"some.com/r2d2"], ["r2_d3","some.com/r2d3"]),
      trdu(["r3d1","some.com/r3d1"], [3,"some.com/r3d2"], ["r3_d3","some.com/r3d3"]),
      ] # NOTE: Above, using '_' we test adding soft line breaks '_&shy;'
    # Add some color
    rowDataList[0]['key1_color'] = 'red'
    rowDataList[2]['key2_color'] = 'green'
    # Create the table
    htmlTable = createHtmlTableStr("My great data", colDataList, rowDataList,
      htmlStyle="",         # No style!
      #htmlStyle=None,       # Uncomment to view this style
      htmlTableStyle="",    # Uncomment to view this style
      )
    #print(htmlTable)
    #with open("test_3x3_table_with_url_correct_contents.html", 'w') as outFile:
    #  outFile.write(htmlTable)
    # NOTE: Above, uncomment the htmlStyle=None, ... line and the print and
    # file write commands to view the formatted table in a browser to see if
    # this gets the data right and you like the default table style.
    htmlTable_expected = \
r"""<h3>My great data</h3>
<table >

<tr>
<th>Data 3</th>
<th>Data 1</th>
<th>Data 2</th>
</tr>

<tr>
<td align="left"><a href="some.com/r1d3">r1_&shy;d3</a></td>
<td align="left"><a href="some.com/r1d1"><font color="red">r1d1</font></a></td>
<td align="right"><a href="some.com/r1d2">1</a></td>
</tr>

<tr>
<td align="left"><a href="some.com/r2d3">r2_&shy;d3</a></td>
<td align="left"><a href="some.com/r2d1">r2d1</a></td>
<td align="right"><a href="some.com/r2d2">2</a></td>
</tr>

<tr>
<td align="left"><a href="some.com/r3d3">r3_&shy;d3</a></td>
<td align="left"><a href="some.com/r3d1">r3d1</a></td>
<td align="right"><a href="some.com/r3d2"><font color="green">3</font></a></td>
</tr>

</table>

"""
    self.assertEqual(htmlTable, htmlTable_expected)
      

#############################################################################
#
# Test CDashQueryAnalyzeReport.createCDashDataSummaryHtmlTableStr()
#
#############################################################################


def missingExpectedBuildsRow(group, site, buildName, missingStatus):
  return { 'group':group, 'site':site, 'buildname':buildName,
    'status':missingStatus }

class test_getCDashDataSummaryHtmlTableTitleStr(unittest.TestCase):

  def test_no_limitRowsToDisplay(self):
    self.assertEqual(
      getCDashDataSummaryHtmlTableTitleStr("data name", "dac", 30),
      "data name: dac=30" )

  def test_limitRowsToDisplay(self):
    self.assertEqual(
      getCDashDataSummaryHtmlTableTitleStr("data name", "dac", 30, 15),
      "data name (limited to 15): dac=30" )

class test_DictSortFunctor(unittest.TestCase):

  def test_call(self):
    meb = missingExpectedBuildsRow
    row = meb("group1", "site1", "build2", "Build exists but not tests")
    sortKeyFunctor = DictSortFunctor(['group', 'site', 'buildname'])
    sortKey = sortKeyFunctor(row)
    self.assertEqual(sortKey, "group1-site1-build2")
    
  def test_sort(self):
    meb = missingExpectedBuildsRow
    rowDataList = [
      meb("group1", "site1", "build2", "Build exists but not tests"),
      meb("group1", "site1", "build1", "Build is missing"),
      ]
    sortKeyFunctor = DictSortFunctor(['group', 'site', 'buildname'])
    rowDataList.sort(key=sortKeyFunctor)
    rowDataList_expected = [
      meb("group1", "site1", "build1", "Build is missing"),
      meb("group1", "site1", "build2", "Build exists but not tests"),
      ]
    self.assertEqual(rowDataList, rowDataList_expected)
   
   
class test_createCDashDataSummaryHtmlTableStr(unittest.TestCase):

  def test_2x4_missing_expected_builds(self):
    tcd = TableColumnData
    meb = missingExpectedBuildsRow
    colDataList = [
      tcd("Group",'group'),
      tcd("Site", 'site'),
      tcd("Build Name", 'buildname'),
      tcd("Missing Status", 'status'),
      ]
    rowDataList = [
      meb("group1", "site1", "build2", "Build exists but not tests"),
      meb("group1", "site1", "build1", "Build is missing"),  # Should be listed first!
      ]
    rowDataListCopy = copy.deepcopy(rowDataList)  # Make sure a copy is sorted!
    htmlTable = createCDashDataSummaryHtmlTableStr(
      "Missing expected builds", "bme",
      colDataList, rowDataList,
      ['group', 'site', 'buildname'],
      #htmlStyle="my_style",  # Don't check default style
      #htmlStyle=None,       # Uncomment to view this style
      #htmlTableStyle="",    # Uncomment to view this style
      )
    #print(htmlTable)
    #with open("test_2x4_missing_expected_builds.html", 'w') as outFile: outFile.write(htmlTable)
    # NOTE: Above, uncomment the htmlStyle=None, ... line and the print and
    # file write commands to view the formatted table in a browser to see if
    # this gets the data right and you like the default table style.
    htmlTable_expected = \
r"""<style>table, th, td {
  padding: 5px;
  border: 1px solid black;
  border-collapse: collapse;
}
tr:nth-child(even) {background-color: #eee;}
tr:nth-child(odd) {background-color: #fff;}
</style>
<h3>Missing expected builds: bme=2</h3>
<table style="width:100%" boarder="1">

<tr>
<th>Group</th>
<th>Site</th>
<th>Build Name</th>
<th>Missing Status</th>
</tr>

<tr>
<td align="left">group1</td>
<td align="left">site1</td>
<td align="left">build1</td>
<td align="left">Build is missing</td>
</tr>

<tr>
<td align="left">group1</td>
<td align="left">site1</td>
<td align="left">build2</td>
<td align="left">Build exists but not tests</td>
</tr>

</table>

"""
    self.assertEqual(htmlTable, htmlTable_expected)
    self.assertEqual(rowDataList, rowDataListCopy)   # Make sure not sorting in place

# ToDo: Test without sorting

# ToDo: Test with limitRowsToDisplay > len(rowDataList)

# ToDo: Test with limitRowsToDisplay == len(rowDataList)

# ToDo: Test with limitRowsToDisplay < len(rowDataList)

# ToDo: Test with now rows and therefore now table printed


#############################################################################
#
# Test CDashQueryAnalyzeReport.createCDashTestHtmlTableStr()
#
#############################################################################

# ToDo: Add unit tests for createCDashTestHtmlTableStr()!


#############################################################################
#
# Test CDashQueryAnalyzeReport.binTestDictsByIssueTracker()
#
#############################################################################

def tdwi(site, buildname, testname, issueTrackerNum):
  testDict = {
    u('site'): u(site),
    u('buildName'): u(buildname),
    u('testname'): u(testname),
  }
  if issueTrackerNum:
    testDict.update(
       {
         u('issue_tracker'): u('#')+issueTrackerNum,
         u('issue_tracker_url'): u('some.com/site/issue/')+issueTrackerNum,
         }
       )
  return testDict


class test_binTestDictsByIssueTracker(unittest.TestCase):

  def test_empty(self):
    testsLOD =[]
    (tdbi, twoiLOD) = binTestDictsByIssueTracker(testsLOD)
    tdbi_expected = {}
    twoiLOD_expected = []
    self.assertEqual(tdbi, tdbi_expected)
    self.assertEqual(twoiLOD, twoiLOD_expected)

  def test_issues_1_noissues_1(self):
    testsLOD =[
      tdwi('site1', 'build1', 'test1', '1234'),
      tdwi('site2', 'build2', 'test2', '1234'),
      tdwi('site3', 'build3', 'test3', ''),
      ]
    (tdbi, twoiLOD) = binTestDictsByIssueTracker(testsLOD)
    # Check tdbi
    self.assertEqual(len(tdbi.keys()), 1)
    self.assertEqual(tdbi['#1234'][0],
      tdwi('site1', 'build1', 'test1', '1234'))
    self.assertEqual(tdbi['#1234'][1],
      tdwi('site2', 'build2', 'test2', '1234'))
    tdbi_expected = {
      u('#1234') : [
        tdwi('site1', 'build1', 'test1', '1234'),
        tdwi('site2', 'build2', 'test2', '1234'),
        ],
      }
    self.assertEqual(tdbi, tdbi_expected)
    # Check twoiLOD
    self.assertEqual(len(twoiLOD), 1)
    twoiLOD_expected = [
      tdwi('site3', 'build3', 'test3', ''),
      ]
    self.assertEqual(twoiLOD, twoiLOD_expected)

  def test_issues_3_noissues_2(self):
    testsLOD =[
      tdwi('site1', 'build1', 'test1', '1234'),
      tdwi('site1', 'build2', 'test1', ''),
      tdwi('site2', 'build3', 'test2', '1234'),
      tdwi('site3', 'build4', 'test3', '1236'),
      tdwi('site3', 'build5', 'test3', ''),
      tdwi('site4', 'build6', 'test1', '1235'),
      ]
    (tdbi, twoiLOD) = binTestDictsByIssueTracker(testsLOD)
    tdbi_expected = {
      '#1234' : [
        tdwi('site1', 'build1', 'test1', '1234'),
        tdwi('site2', 'build3', 'test2', '1234'),
        ],
      '#1235' : [
        tdwi('site4', 'build6', 'test1', '1235'),
        ],
      '#1236' : [
        tdwi('site3', 'build4', 'test3', '1236'),
        ],
      }
    twoiLOD_expected = [
      tdwi('site1', 'build2', 'test1', ''),
      tdwi('site3', 'build5', 'test3', ''),
      ]
    self.assertEqual(tdbi, tdbi_expected)
    self.assertEqual(twoiLOD, twoiLOD_expected)


#############################################################################
#
# Test CDashQueryAnalyzeReport.binTestDictsByTestsetAcro()
#
#############################################################################

def tdswi(testname, status, issueTrackerNum):
  testDict = {
    u('testname'): u(testname),
    u('status'): u(status),
  }
  if issueTrackerNum:
    testDict.update(
       {
         u('issue_tracker'): u('#')+issueTrackerNum,
         u('issue_tracker_url'): u('some.com/site/issue/')+issueTrackerNum,
         }
       )
  return testDict


class test_binTestDictsByTestsetAcro(unittest.TestCase):

  def test_empty(self):
    testsLOD =[]
    tbtsa = binTestDictsByTestsetAcro(testsLOD)
    tbtsa_expected = {}
    self.assertEqual(tbtsa, tbtsa_expected)

  def test_twoif(self):
    testsLOD =[
      tdswi('test1', 'Failed', '')
      ]
    tbtsa = binTestDictsByTestsetAcro(testsLOD)
    self.assertEqual(len(tbtsa.keys()), 1)
    self.assertEqual(tbtsa['twoif'],
      [
        tdswi('test1', 'Failed', ""),
        ],
       )

  def test_twoif_twip(self):
    testsLOD =[
      tdswi('test1', 'Failed', ''),
      tdswi('test2', 'Passed', '1234'),
      tdswi('test3', 'Failed', ''),
      ]
    tbtsa = binTestDictsByTestsetAcro(testsLOD)
    self.assertEqual(len(tbtsa.keys()), 2)
    self.assertEqual(tbtsa['twoif'],
      [
        tdswi('test1', 'Failed', ""),
        tdswi('test3', 'Failed', ''),
        ],
       )
    self.assertEqual(tbtsa['twip'],
      [
        tdswi('test2', 'Passed', '1234'),
        ],
       )

  def test_all(self):
    testsLOD =[
      tdswi('test1', 'Failed', '1234'),
      tdswi('test2', 'Failed', ''),
      tdswi('test3', 'Passed', '1235'),
      tdswi('test4', 'Failed', '1234'),
      tdswi('test5', 'Not Run', ''),
      tdswi('test6', 'Missing', '1235'),
      tdswi('test7', 'Not Run', '1236'),
      ]
    tbtsa = binTestDictsByTestsetAcro(testsLOD)
    self.assertEqual(len(tbtsa.keys()), 6)
    self.assertEqual(tbtsa['twoif'],
      [
        tdswi('test2', 'Failed', ''),
        ],
       )
    self.assertEqual(tbtsa['twoinr'],
      [
        tdswi('test5', 'Not Run', ''),
        ],
       )
    self.assertEqual(tbtsa['twip'],
      [
        tdswi('test3', 'Passed', '1235'),
        ],
       )
    self.assertEqual(tbtsa['twim'],
      [
        tdswi('test6', 'Missing', '1235'),
        ],
       )
    self.assertEqual(tbtsa['twif'],
      [
        tdswi('test1', 'Failed', '1234'),
        tdswi('test4', 'Failed', '1234'),
        ],
       )
    self.assertEqual(tbtsa['twinr'],
      [
        tdswi('test7', 'Not Run', '1236'),
        ],
       )


#############################################################################
#
# Test CDashQueryAnalyzeReport.getIssueTrackerFieldsAndAssertAllSame()
#
#############################################################################


def tdit(testname, issue_tracker_id, skipUrlField=False):
  td = { u('testname'):u(testname) }
  if issue_tracker_id:
    td.update( { u('issue_tracker') : u('#')+issue_tracker_id } )
    if not skipUrlField:
      td.update( {
        u('issue_tracker_url') :
          u('https://github.com/org/repo/issues/')+issue_tracker_id } )
  return td


class test_getIssueTrackerFieldsAndAssertAllSame(unittest.TestCase):


  def test_empty(self):
    testsLOD = []
    issueTracker = getIssueTrackerFieldsAndAssertAllSame(testsLOD)
    self.assertEqual(issueTracker, None)


  def test_all_matching(self):
    testsLOD = [
      tdit('test1', '1234'),
      tdit('test2', '1234'),
      tdit('test3', '1234'),
      ]
    issueTracker = getIssueTrackerFieldsAndAssertAllSame(testsLOD)
    self.assertEqual(issueTracker,
      (u('#1234'), u('https://github.com/org/repo/issues/1234')) )


  def test_missing_issue_tracker_field(self):
    testsLOD = [
      tdit('test1', '1234'),
      tdit('test2', None),
      tdit('test3', '1234'),
      ]
    threwExcept = True
    try:
      issueTracker = getIssueTrackerFieldsAndAssertAllSame(testsLOD)
      threwExcept = False
    except IssueTrackerFieldError as errMsg:
      self.assertEqual( str(errMsg),
        "Error, the test dict {"+stru()+"'testname': "+stru()+"'test2'} at index 1"+\
        " is missing the 'issue_tracker' field!" )
    if not threwExcept:
      self.assertFalse("ERROR: Did not throw an exception")


  def test_missing_issue_tracker_url_field(self):
    testsLOD = [
      tdit('test1', '1234'),
      tdit('test2', '1234', skipUrlField=True),
      tdit('test3', '1234'),
      ]
    threwExcept = True
    try:
      issueTracker = getIssueTrackerFieldsAndAssertAllSame(testsLOD)
      threwExcept = False
    except IssueTrackerFieldError as errMsg:
      self.assertEqual( str(errMsg),
        "Error, the test dict"+\
        " {"+stru()+"'issue_tracker': "+stru()+"'#1234', "+stru()+"'testname': "+stru()+"'test2'} at index 1"+\
        " is missing the 'issue_tracker_url' field!" )
    if not threwExcept:
      self.assertFalse("ERROR: Did not throw an exception")


  def test_inconsistent_issue_tracker_field(self):
    testsLOD = [
      tdit('test1', '1234'),
      tdit('test2', '1235'),
      tdit('test3', '1234'),
      ]
    threwExcept = True
    try:
      issueTracker = getIssueTrackerFieldsAndAssertAllSame(testsLOD)
      threwExcept = False
    except IssueTrackerFieldError as errMsg:
      self.assertEqual( str(errMsg),
        "Error, the test dict {"+stru()+"'issue_tracker': "+stru()+"'#1235', "+stru()+"'issue_tracker_url':"+\
        " "+stru()+"'https://github.com/org/repo/issues/1235', "+stru()+"'testname': "+stru()+"'test2'} at"+\
        " index 1 has a different 'issue_tracker' field '#1235' than the expected"+\
        " value of '#1234'!" )
    if not threwExcept:
      self.assertFalse("ERROR: Did not throw an exception")


  def test_inconsistent_issue_tracker_field(self):
    testsLOD = [
      tdit('test1', '1234'),
      tdit('test2', '1234'),
      tdit('test3', '1234'),
      ]
    testsLOD[2]['issue_tracker_url'] = u('https://github.com/org/repo/issues/1236')
    threwExcept = True
    try:
      issueTracker = getIssueTrackerFieldsAndAssertAllSame(testsLOD)
      threwExcept = False
    except IssueTrackerFieldError as errMsg:
      self.assertEqual( str(errMsg),
        "Error, the test dict {"+stru()+"'issue_tracker': "+stru()+"'#1234', "+stru()+"'issue_tracker_url':"+\
        " "+stru()+"'https://github.com/org/repo/issues/1236', "+stru()+"'testname': "+stru()+"'test3'} at"+\
        " index 2 has a different 'issue_tracker_url' field"+\
        " 'https://github.com/org/repo/issues/1236' than the expected value of"+\
        " 'https://github.com/org/repo/issues/1234'!" )
    if not threwExcept:
      self.assertFalse("ERROR: Did not throw an exception")


  #def test_inconsistent_issue_tracker_url_field(self):


#############################################################################
#
# Test CDashQueryAnalyzeReport.IssueTrackerTestsStatusReporter
#
#############################################################################


cdash_analyze_and_report_dir = g_testBaseDir+'/cdash_analyze_and_report'


g_twoif_10_twoinr2_twif_8_twinr_1_test_data_out = \
  eval(
    open(cdash_analyze_and_report_dir+'/twoif_10_twoinr2_twif_8_twinr_1/test_data.json',
    'r').read())


def makeTestPassing(testDict):
  testDict['status'] = u('Passed')
  testDict['status_color'] = cdashColorPassed()
  testDict['details'] = u('Completed (Passed)\n')


def makeTestMissing(testDict):
  setTestDictAsMissing(testDict)


def setIssueTrackerFields(testsLOD, issue_tracker, issue_tracker_url):
  for testDict in testsLOD:
    testDict['issue_tracker'] = issue_tracker
    testDict['issue_tracker_url'] = issue_tracker_url


class test_IssueTrackerTestsStatusReporter(unittest.TestCase):


  def test_empty(self):
    issueTrackerTestsStatusReporter = IssueTrackerTestsStatusReporter(verbose=False)
    testsLOD = []
    okayToCloseIssue = \
      issueTrackerTestsStatusReporter.reportIssueTrackerTestsStatus(testsLOD)
    self.assertEqual(okayToCloseIssue, True)
    reportHtml = issueTrackerTestsStatusReporter.getIssueTrackerTestsStatusReport()
    self.assertEqual(reportHtml, None)


  def test_non_matching_issue_tracker_field(self):
    allTestsLOD = copy.deepcopy(g_twoif_10_twoinr2_twif_8_twinr_1_test_data_out)
    issueTrackerTestsStatusReporter = IssueTrackerTestsStatusReporter(verbose=False)
    threwExcept = True
    try:
      testsLOD = allTestsLOD
      okayToCloseIssue = \
        issueTrackerTestsStatusReporter.reportIssueTrackerTestsStatus(testsLOD)
      threwExcept = False
    except IssueTrackerFieldError as errMsg:
      assertFindListOfStringsInString(self,
        [
          "Error, the test dict {",
          u("'buildName': "+stru()+"'Trilinos-atdm-cee-rhel6-clang-opt-serial'"),
          u("'testname': "+stru()+"'PanzerAdaptersIOSS_tIOSSConnManager2_MPI_2'"),
          "} at index 1 has a different 'issue_tracker' field '#3632'"+\
            " than the expected value of '#3640'!",
          ],
        str(errMsg),
        "errMsg",
        debugPrint=False,
        )
    if not threwExcept:
      self.assertFalse("ERROR: Did not throw an exception")


  def test_twif_8_twinr_1(self):
    testsLOD = copy.deepcopy(g_twoif_10_twoinr2_twif_8_twinr_1_test_data_out)
    setIssueTrackerFields(testsLOD, u('#1234'),
      u('https://github.com/trilinos/Trilinos/issues/1234') )
    issueTrackerTestsStatusReporter = IssueTrackerTestsStatusReporter(verbose=False)
    okayToCloseIssue = \
      issueTrackerTestsStatusReporter.reportIssueTrackerTestsStatus(testsLOD)
    self.assertEqual(okayToCloseIssue, False)
    summaryLineDataNumbersList_expected = \
      ['twif=8', 'twinr=1']
    self.assertEqual(
      issueTrackerTestsStatusReporter.cdashReportData.summaryLineDataNumbersList,
      summaryLineDataNumbersList_expected)
    issueTrckerTestsStatusReportHtml = \
      issueTrackerTestsStatusReporter.getIssueTrackerTestsStatusReport()
    # TODO: REMOVE THIS FILE WRITE!!!
    #with open("issueTrckerTestsStatusReport.html", 'w') as testsHtmlReportFile:
    #  testsHtmlReportFile.write(issueTrckerTestsStatusReportHtml)
    assertListOfRegexsFoundInListOfStrs(self,
      regexList=[
        '<h2>Test results for issue #1234 as of 2018-10-28</h2>',
        '<details>',
        '<summary><b>Detailed test results:</b> [(]click to expand[)]</b></summary>',
        "<h3><font color=.red.>Tests with issue trackers Failed: twif=8</font></h3>",
        "<td align=\"left\">cee-rhel6</td>",
        "<td align=\"left\"><a href=\"https://something.com/cdash/testDetails.php[?]test=57816429&build=4107319\">MueLu_&shy;UnitTestsBlockedEpetra_&shy;MPI_&shy;1</a></td>",
        "<td align=\"left\"><a href=\"https://something.com/cdash/testDetails.php[?]test=57816429&build=4107319\"><font color=\"red\">Failed</font></a></td>",
        "<td align=\"right\"><a href=\"https://github.com/trilinos/Trilinos/issues/1234\">#1234</a></td>",
        "<h3><font color=\"orange\">Tests with issue trackers Not Run: twinr=1</font></h3>",
        "<td align=\"left\">cee-rhel6</td>",
        "<td align=\"left\"><a href=\"https://something.com/cdash/testDetails.php[?]test=57816373&build=4107331\">Teko_&shy;ModALPreconditioner_&shy;MPI_&shy;1</a></td>",
        "<td align=\"left\"><a href=\"https://something.com/cdash/testDetails.php[?]test=57816373&build=4107331\"><font color=\"orange\">Not Run</font></a></td>",
        "<td align=\"left\">Required Files Missing</td>",
        "<td align=\"right\"><a href=\"https://github.com/trilinos/Trilinos/issues/1234\">#1234</a></td>",
        '</details>',
        ],
      stringsList=issueTrckerTestsStatusReportHtml.split('\n'),
      stringsListName="issueTrckerTestsStatusReportHtml",
      debugPrint=False
      )


  def test_twip_1_twif_5_twim_2_twinr_1(self):
    testsLOD = copy.deepcopy(g_twoif_10_twoinr2_twif_8_twinr_1_test_data_out)
    setIssueTrackerFields(testsLOD, u('#1234'),
      u('https://github.com/trilinos/Trilinos/issues/1234') )
    # Change a test from failing to passing
    testIdx = getIdxOfTestInTestLOD(testsLOD,
      'cee-rhel6', 'Trilinos-atdm-cee-rhel6-gnu-4.9.3-opt-serial',
      'PanzerAdaptersIOSS_tIOSSConnManager2_MPI_2')
    makeTestPassing(testsLOD[testIdx])
    # Change a from from faling to missing
    testIdx = getIdxOfTestInTestLOD(testsLOD,
      'cee-rhel6', 'Trilinos-atdm-cee-rhel6-intel-opt-serial',
      'PanzerAdaptersIOSS_tIOSSConnManager2_MPI_2')
    makeTestMissing(testsLOD[testIdx])
    # Change another a test from failing to missing
    testIdx = getIdxOfTestInTestLOD(testsLOD,
      'cee-rhel6', 'Trilinos-atdm-cee-rhel6-intel-opt-serial',
      'PanzerAdaptersIOSS_tIOSSConnManager3_MPI_3')
    makeTestMissing(testsLOD[testIdx])
    # Run the reporter
    issueTrackerTestsStatusReporter = IssueTrackerTestsStatusReporter(verbose=False)
    okayToCloseIssue = \
      issueTrackerTestsStatusReporter.reportIssueTrackerTestsStatus(testsLOD)
    self.assertEqual(okayToCloseIssue, False)
    # Check the basic collected
    summaryLineDataNumbersList_expected = \
      ['twip=1', 'twim=2', 'twif=5', 'twinr=1']
    self.assertEqual(
      issueTrackerTestsStatusReporter.cdashReportData.summaryLineDataNumbersList,
      summaryLineDataNumbersList_expected)
    htmlEmailBodyTop_expected = \
      '<font color="green">Tests with issue trackers Passed: twip=1</font><br>\n'+\
      '<font color="gray">Tests with issue trackers Missing: twim=2</font><br>\n'+\
      '<font color="red">Tests with issue trackers Failed: twif=5</font><br>\n'+\
      '<font color="orange">Tests with issue trackers Not Run: twinr=1</font><br>\n'
    self.assertEqual(
      issueTrackerTestsStatusReporter.cdashReportData.htmlEmailBodyTop,
      htmlEmailBodyTop_expected)
    assertListOfRegexsFoundInListOfStrs(self,
      regexList=[
        '<h3><font color="green">Tests with issue trackers Passed: twip=1</font></h3>',
        '<td align="left"><a href=".*">Trilinos-atdm-cee-rhel6-gnu-4.9.3-opt-serial</a></td>',
        '<td align="left"><a href=".*">PanzerAdaptersIOSS_&shy;tIOSSConnManager2_&shy;MPI_&shy;2</a></td>',
        '<td align="left"><a href=".*"><font color="green">Passed</font></a></td>',
        '<td align="left">Completed [(]Passed[)]</td>',

        '<h3><font color="gray">Tests with issue trackers Missing: twim=2</font></h3>',
        '<td align="left"><a href=".*">Trilinos-atdm-cee-rhel6-intel-opt-serial</a></td>',
        '<td align="left"><a href=".*">PanzerAdaptersIOSS_&shy;tIOSSConnManager2_&shy;MPI_&shy;2</a></td>',
        '<td align="left"><a href=".*"><font color="gray">Missing</font></a></td>',
        '<td align="left">Missing</td>',

        '<h3><font color="red">Tests with issue trackers Failed: twif=5</font></h3>',
        '<td align="left"><a href=".*">Trilinos-atdm-cee-rhel6-clang-opt-serial</a></td>',
        '<td align="left"><a href=".*">MueLu_&shy;UnitTestsBlockedEpetra_&shy;MPI_&shy;1</a></td>',
        '<td align="left"><a href=".*"><font color="red">Failed</font></a></td>',
        '<td align="left">Completed [(]Failed[)]</td>',

        '<h3><font color="orange">Tests with issue trackers Not Run: twinr=1</font></h3>',
        '<td align="left"><a href=".*">Trilinos-atdm-cee-rhel6-clang-opt-serial</a></td>',
        '<td align="left"><a href=".*">Teko_&shy;ModALPreconditioner_&shy;MPI_&shy;1</a></td>',
        '<td align="left"><a href=".*"><font color="orange">Not Run</font></a></td>',
        '<td align="left">Required Files Missing</td>',
        ],
      stringsList=issueTrackerTestsStatusReporter.testsetsReporter.\
        cdashReportData.htmlEmailBodyBottom.split('\n'),
      stringsListName="cdashReportData.htmlEmailBodyBottom",
      debugPrint=False
      )
    # Get the summary report
    issueTrckerTestsStatusReportHtml = \
      issueTrackerTestsStatusReporter.getIssueTrackerTestsStatusReport()
    # TODO: REMOVE THIS FILE WRITE!!!
    #with open("issueTrckerTestsStatusReport.html", 'w') as testsHtmlReportFile:
    #  testsHtmlReportFile.write(issueTrckerTestsStatusReportHtml)
    assertListOfRegexsFoundInListOfStrs(self,
      regexList=[
        '<h2>Test results for issue #1234 as of 2018-10-28</h2>',
        '<font color="green">Tests with issue trackers Passed: twip=1</font><br>',
        '<font color="gray">Tests with issue trackers Missing: twim=2</font><br>',
        '<font color="red">Tests with issue trackers Failed: twif=5</font><br>',
        '<font color="orange">Tests with issue trackers Not Run: twinr=1</font><br>',
        '<details>',
        '<summary><b>Detailed test results:</b> [(]click to expand[)]</b></summary>',
        '<h3><font color="green">Tests with issue trackers Passed: twip=1</font></h3>',
        '<h3><font color="gray">Tests with issue trackers Missing: twim=2</font></h3>',
        '<h3><font color="red">Tests with issue trackers Failed: twif=5</font></h3>',
        '<h3><font color="orange">Tests with issue trackers Not Run: twinr=1</font></h3>',
        '</details>',
        ],
      stringsList=issueTrckerTestsStatusReportHtml.split('\n'),
      stringsListName="issueTrckerTestsStatusReportHtml",
      debugPrint=False
      )

  # NOTE: The above tests for the class
  # CDashQueryAnalyzeReport.IssueTrackerTestsStatusReporter also tests the
  # classes CDashQueryAnalyzeReport.SingleTestsetReporter and
  # CDashQueryAnalyzeReport.TestsetsReporter.  I just did not want to
  # duplicate all of those large and complex tests for little added value.



#
# Run the unit tests!
#

if __name__ == '__main__':

  unittest.main()
