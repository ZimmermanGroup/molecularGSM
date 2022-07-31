# @HEADER
# ************************************************************************
#
#            TriBTS: Tribal Build, Integrate, and Test System
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
import re
import copy
import shutil
import unittest
import pprint

from FindCISupportDir import *
import CreateIssueTrackerFromCDashQuery as CIFCDQ
from GeneralScriptSupport import getCmndOutput, getScriptBaseDir
from CDashQueryAnalyzeReportUnitTestHelpers import *

g_testBaseDir = getScriptBaseDir()

g_pp = pprint.PrettyPrinter(indent=4)


# Base test directory in the build tree
g_baseTestDir="example_test_failure_github_issue"


#############################################################################
#
# System-level tests for CreateIssueTrackerFromCDashQuery.py through
# example_test_failure_github_issue.py
#
#############################################################################


def example_test_failure_github_issue_test_case(self, testCaseName, cmndLineArgsStr,
    cdashTestsJsonFile, expectedOutput, expectedIssue,
    expectedTestsWithIssueTrackersCsv,
  ):

  # Create dir to run in
  fullTestDir = os.path.join(g_baseTestDir, testCaseName)
  if os.path.exists(fullTestDir): shutil.rmtree(fullTestDir)
  os.makedirs(fullTestDir)
  os.chdir(fullTestDir)

  # Run example_test_failure_github_issue.py
  example_exec = os.path.join(testCiSupportDir,
    'example_test_failure_github_issue.py')
  cdashTestsJsonFileFullPath = os.path.join(testCiSupportDir, cdashTestsJsonFile)
  cmnd = example_exec + " " + cmndLineArgsStr + \
    " -i newGitHubIssue.md -t newTestsWithIssueTrackers.csv"
  os.environ['TRIBITS_DIR'] = tribitsDir
  os.environ['CREATE_ISSUE_TRACKER_FROM_CDASH_QUERY_FILE_FOR_UNIT_TESTING'] = \
    os.path.join(testCiSupportDir, cdashTestsJsonFile)
  (output, rtnCode) = getCmndOutput(cmnd, getStdErr=True,
    throwOnError=False, rtnCode=True)

  # Check outputs
  self.maxDiff = None
  self.assertEqual(output, expectedOutput)
  self.assertEqual(rtnCode, 0)
  with open("newGitHubIssue.md", 'r') as fileHandle:
    newGitHubIssueText = fileHandle.read()
  self.assertEqual(newGitHubIssueText, expectedIssue)
  with open("newTestsWithIssueTrackers.csv", 'r') as fileHandle:
    newTestsWithIssueTrackers = fileHandle.read()
  self.assertEqual(newTestsWithIssueTrackers, expectedTestsWithIssueTrackersCsv)


class test_example_test_failure_github_issue(unittest.TestCase):


  def test_t2_b2(self):
    example_test_failure_github_issue_test_case(self,
      "test_t2_b2", "-u dummy_url -s dummy_summary",
     g_example_t2_b2_test_data, g_example_t2_b2_output,
     g_example_t2_b2_github_issue, g_example_t2_b2_tests_with_issue_trackers_csv )


#
# Example with 2 tests and 2 builds (after getting unique set)
# 

g_example_t2_b2_test_data = \
  "CreateIssueTrackerFromCDashQuery/test_data_tests_2_builds_2.json"

g_example_t2_b2_output = r"""
***
*** Getting data to create a new issue tracker
***

Downloading full list of nonpassing tests from CDash URL:

   dummy_url

  Since the file exists, using cached data from file:
    """+g_testBaseDir+r"""/CreateIssueTrackerFromCDashQuery/test_data_tests_2_builds_2.json

Total number of nonpassing tests over all days = 4

Total number of unique nonpassing test/build pairs over all days = 3

Number of test names = 2

Number of build names = 2

Writing out new issue tracker text to 'newGitHubIssue.md'

Writing out list of test/biuld pairs for CSV file 'newTestsWithIssueTrackers.csv'
"""

g_example_t2_b2_github_issue = r"""
SUMMARY: dummy_summary 2018-10-14

## Description

As shown in [this query](dummy_url) (click "Show Matching Output" in upper right) the tests:

* `test_1`
* `test_2`

in the builds:

* `build_1`
* `build_2`

started failing on testing day 2018-10-14.


## Current Status on CDash

Run the [above query](dummy_url) adjusting the "Begin" and "End" dates to match today any other date range or just click "CURRENT" in the top bar to see results for the current testing day.
"""

g_example_t2_b2_tests_with_issue_trackers_csv = \
r"""site, buildName, testname, issue_tracker_url, issue_tracker
site_1, build_1, test_1, https://github.com/<group>/<repo>/issues/<newissueid>, #<newissueid>
site_2, build_2, test_1, https://github.com/<group>/<repo>/issues/<newissueid>, #<newissueid>
site_1, build_1, test_2, https://github.com/<group>/<repo>/issues/<newissueid>, #<newissueid>
"""


#
# Run the unit tests!
#

if __name__ == '__main__':

  # Clean out and re-recate the base test directory
  if os.path.exists(g_baseTestDir): shutil.rmtree(g_baseTestDir)
  os.mkdir(g_baseTestDir)

  unittest.main()
