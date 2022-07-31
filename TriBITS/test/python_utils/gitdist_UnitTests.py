#!/usr/bin/env python
# -*- coding: utf-8 -*-
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

#################################
# Unit testing code for gitdist #
#################################

import sys
import imp
import shutil

from unittest_helpers import *

pythonDir = os.path.abspath(GeneralScriptSupport.getScriptBaseDir())
utilsDir = pythonDir+"/utils"
tribitsDir = os.path.abspath(pythonDir+"/..")

sys.path = [pythonUtilsDir] + sys.path
from gitdist import *


#
# Utility functions for testing
#


gitdistPath = pythonUtilsDir+"/gitdist"
gitdistPathNoColor = gitdistPath+" --dist-no-color"
gitdistPathMock = gitdistPathNoColor+" --dist-use-git=mockgit --dist-no-opt"
mockGitPath = pythonUtilsDir+"/mockprogram.py"

unitTestDataDir = testPythonUtilsDir

tempMockProjectDir = "MockProjectDir"

testBaseDir = os.getcwd()


def getCmndOutputInMockProjectDir(cmnd):
  os.chdir(mockProjectDir)
  cmndOut = getCmndOutput(cmnd)
  os.chdir(testBaseDir)
  return cmndOut


def createAndMoveIntoTestDir(testDir):
  if os.path.exists(testDir): shutil.rmtree(testDir)
  os.mkdir(testDir)
  os.chdir(testDir)
  if not os.path.exists(tempMockProjectDir): os.mkdir(tempMockProjectDir)
  os.chdir(tempMockProjectDir)
  return os.path.join(testBaseDir, testDir, tempMockProjectDir)


class GitDistOptions:

  def __init__(self, useGit):
    self.useGit = useGit


#
# Unit tests for createTable
#


class test_createTable(unittest.TestCase):


  def setUp(self):
    self.gitdistMoveToBaseDir = os.environ.get("GITDIST_MOVE_TO_BASE_DIR", "")
    os.environ["GITDIST_MOVE_TO_BASE_DIR"] = ""
    self.gitdistUnitTestSttySize = os.environ.get(
      "GITDIST_UNIT_TEST_STTY_SIZE", ""
    )


  def tearDown(self):
    os.environ["GITDIST_MOVE_TO_BASE_DIR"] = self.gitdistMoveToBaseDir
    os.environ["GITDIST_UNIT_TEST_STTY_SIZE"] = self.gitdistUnitTestSttySize


  def test_full_table(self):
    tableData = [
      { "label" : "ID", "align" : "R",
        "fields" : ["0", "1", "2"] },
      { "label" : "Repo Dir", "align" : "L",
         "fields" : ["Base: BaseRepo", "ExtraRepo1", "Path/To/ExtraRepo2" ] },
      { "label":"Branch", "align":"L",
        "fields" : ["dummy", "master", "HEAD" ] },
      { "label" : "Tracking Branch", "align":"L",
        "fields" : ["", "origin/master", "" ] },
      { "label" : "C", "align":"R", "fields" : ["", "1", "" ] },
      { "label" : "M", "align":"R", "fields" : ["0", "2", "25" ] },
      { "label" : "?", "align":"R", "fields" : ["0", "0", "4" ] },
      ]
    table = createTable(tableData)
    #print(table)
    table_expected = \
      "-------------------------------------------------------------------\n" \
      "| ID | Repo Dir           | Branch | Tracking Branch | C | M  | ? |\n" \
      "|----|--------------------|--------|-----------------|---|----|---|\n" \
      "|  0 | Base: BaseRepo     | dummy  |                 |   |  0 | 0 |\n" \
      "|  1 | ExtraRepo1         | master | origin/master   | 1 |  2 | 0 |\n" \
      "|  2 | Path/To/ExtraRepo2 | HEAD   |                 |   | 25 | 4 |\n" \
      "-------------------------------------------------------------------\n"
    self.assertEqual(table, table_expected)


  def test_full_table_utf8(self):
    if sys.version_info < (3,):
      print("Test disabled for Python 2.")
    else:
      tableData = [
        { "label" : "ID", "align" : "R",
          "fields" : ["0", "1", "2"] },
        { "label" : "Repo Dir", "align" : "L",
           "fields" : ["Base: BaseRepo", "ExtraRepo1", "Path/To/ExtraRepo2" ] },
        { "label":"Branch", "align":"L",
          "fields" : ["dummy", "master", "HEAD" ] },
        { "label" : "Tracking Branch", "align":"L",
          "fields" : ["", "origin/master", "" ] },
        { "label" : "C", "align":"R", "fields" : ["", "1", "" ] },
        { "label" : "M", "align":"R", "fields" : ["0", "2", "25" ] },
        { "label" : "?", "align":"R", "fields" : ["0", "0", "4" ] },
        ]
      table = createTable(tableData, True)
      #print(table)
      table_expected = \
        "┌────┬────────────────────┬────────┬─────────────────┬───┬────┬───┐\n" \
        "│ ID │ Repo Dir           │ Branch │ Tracking Branch │ C │ M  │ ? │\n" \
        "┝━━━━┿━━━━━━━━━━━━━━━━━━━━┿━━━━━━━━┿━━━━━━━━━━━━━━━━━┿━━━┿━━━━┿━━━┥\n" \
        "│  0 │ Base: BaseRepo     │ dummy  │                 │   │  0 │ 0 │\n" \
        "│  1 │ ExtraRepo1         │ master │ origin/master   │ 1 │  2 │ 0 │\n" \
        "│  2 │ Path/To/ExtraRepo2 │ HEAD   │                 │   │ 25 │ 4 │\n" \
        "└────┴────────────────────┴────────┴─────────────────┴───┴────┴───┘\n"
      self.assertEqual(table, table_expected)


  def test_no_rows(self):
    tableData = [
      { "label" : "ID", "align" : "R",
        "fields" : [] },
      { "label" : "Repo Dir", "align" : "L",
         "fields" : [] },
      { "label":"Branch", "align":"L",
        "fields" : [] },
      { "label" : "Tracking Branch", "align":"L",
        "fields" : [] },
      { "label" : "C", "align":"R", "fields" : [] },
      { "label" : "M", "align":"R", "fields" : [] },
      { "label" : "?", "align":"R", "fields" : [] },
      ]
    table = createTable(tableData)
    #print(table)
    table_expected = \
      "--------------------------------------------------------\n" \
      "| ID | Repo Dir | Branch | Tracking Branch | C | M | ? |\n" \
      "|----|----------|--------|-----------------|---|---|---|\n" \
      "--------------------------------------------------------\n"
    self.assertEqual(table, table_expected)


  def test_no_rows_utf8(self):
    if sys.version_info < (3,):
      print("Test disabled for Python 2.")
    else:
      tableData = [
        { "label" : "ID", "align" : "R",
          "fields" : [] },
        { "label" : "Repo Dir", "align" : "L",
           "fields" : [] },
        { "label":"Branch", "align":"L",
          "fields" : [] },
        { "label" : "Tracking Branch", "align":"L",
          "fields" : [] },
        { "label" : "C", "align":"R", "fields" : [] },
        { "label" : "M", "align":"R", "fields" : [] },
        { "label" : "?", "align":"R", "fields" : [] },
        ]
      table = createTable(tableData, True)
      #print(table)
      table_expected = \
        "┌────┬──────────┬────────┬─────────────────┬───┬───┬───┐\n" \
        "│ ID │ Repo Dir │ Branch │ Tracking Branch │ C │ M │ ? │\n" \
        "┝━━━━┿━━━━━━━━━━┿━━━━━━━━┿━━━━━━━━━━━━━━━━━┿━━━┿━━━┿━━━┥\n" \
        "└────┴──────────┴────────┴─────────────────┴───┴───┴───┘\n"
      self.assertEqual(table, table_expected)


  def test_one_row(self):
    tableData = [
      { "label" : "ID", "align" : "R",
        "fields" : ["0"] },
      { "label" : "Repo Dir", "align" : "L",
         "fields" : ["Base: BaseRepo"] },
      { "label":"Branch", "align":"L",
        "fields" : ["dummy"] },
      { "label" : "Tracking Branch", "align":"L",
        "fields" : ["origin/master"] },
      { "label" : "C", "align":"R", "fields" : ["24"] },
      { "label" : "M", "align":"R", "fields" : ["25"] },
      { "label" : "?", "align":"R", "fields" : ["4"] },
      ]
    table = createTable(tableData)
    #print(table)
    table_expected = \
      "----------------------------------------------------------------\n" \
      "| ID | Repo Dir       | Branch | Tracking Branch | C  | M  | ? |\n" \
      "|----|----------------|--------|-----------------|----|----|---|\n" \
      "|  0 | Base: BaseRepo | dummy  | origin/master   | 24 | 25 | 4 |\n" \
      "----------------------------------------------------------------\n"
    self.assertEqual(table, table_expected)


  def test_one_row_utf8(self):
    if sys.version_info < (3,):
      print("Test disabled for Python 2.")
    else:
      tableData = [
        { "label" : "ID", "align" : "R",
          "fields" : ["0"] },
        { "label" : "Repo Dir", "align" : "L",
           "fields" : ["Base: BaseRepo"] },
        { "label":"Branch", "align":"L",
          "fields" : ["dummy"] },
        { "label" : "Tracking Branch", "align":"L",
          "fields" : ["origin/master"] },
        { "label" : "C", "align":"R", "fields" : ["24"] },
        { "label" : "M", "align":"R", "fields" : ["25"] },
        { "label" : "?", "align":"R", "fields" : ["4"] },
        ]
      table = createTable(tableData, True)
      #print(table)
      table_expected = \
        "┌────┬────────────────┬────────┬─────────────────┬────┬────┬───┐\n" \
        "│ ID │ Repo Dir       │ Branch │ Tracking Branch │ C  │ M  │ ? │\n" \
        "┝━━━━┿━━━━━━━━━━━━━━━━┿━━━━━━━━┿━━━━━━━━━━━━━━━━━┿━━━━┿━━━━┿━━━┥\n" \
        "│  0 │ Base: BaseRepo │ dummy  │ origin/master   │ 24 │ 25 │ 4 │\n" \
        "└────┴────────────────┴────────┴─────────────────┴────┴────┴───┘\n"
      self.assertEqual(table, table_expected)


  def test_row_mismatch(self):
    tableData = [
      { "label" : "ID", "align" : "R",
        "fields" : ["0", "1"] },
      { "label" : "Repo Dir", "align" : "L",
         "fields" : ["Base: BaseRepo"] },
      ]
    #createTable(tableData)
    self.assertRaises(Exception, createTable, tableData)
  

  def create_stty_size_table(self):
    tableData = [
      { "label" : "ID", "align" : "R", "fields" : ["0", "1", "2"] },
      { "label" : "Repo Dir", "align" : "L",
        "fields" : [
          "MockProjectDir (Base)",
          "ExtraRepo1",
          "really/long/path/to/ExtraRepo2"
        ]
      },
      { "label" : "Branch", "align" : "L",
        "fields" : [
          "medium_branch",
          "short_br",
          "really_long_branch_name"
        ]
      },
      { "label" : "Tracking Branch", "align" : "L",
        "fields" : [
          "medium_remote/medium_branch",
          "origin/short_br",
          "long_remote_name/really_long_branch_name"
        ]
      },
      { "label" : "C", "align" : "R", "fields" : ["", "", ""] },
      { "label" : "M", "align" : "R", "fields" : ["", "", ""] },
      { "label" : "?", "align" : "R", "fields" : ["", "", ""] }
    ]
    return tableData


  def test_stty_size_larger_than_needed(self):
    os.environ["GITDIST_UNIT_TEST_STTY_SIZE"] = "60 140"
    table = createTable(self.create_stty_size_table())
    #print(table)
    table_expected = \
      "------------------------------------------------------------------------------------------------------------------------\n" \
      "| ID | Repo Dir                       | Branch                  | Tracking Branch                          | C | M | ? |\n" \
      "|----|--------------------------------|-------------------------|------------------------------------------|---|---|---|\n" \
      "|  0 | MockProjectDir (Base)          | medium_branch           | medium_remote/medium_branch              |   |   |   |\n" \
      "|  1 | ExtraRepo1                     | short_br                | origin/short_br                          |   |   |   |\n" \
      "|  2 | really/long/path/to/ExtraRepo2 | really_long_branch_name | long_remote_name/really_long_branch_name |   |   |   |\n" \
      "------------------------------------------------------------------------------------------------------------------------\n"
    self.assertEqual(table, table_expected)


  def test_stty_size_exactly_right(self):
    os.environ["GITDIST_UNIT_TEST_STTY_SIZE"] = "60 120"
    table = createTable(self.create_stty_size_table())
    #print(table)
    table_expected = \
      "------------------------------------------------------------------------------------------------------------------------\n" \
      "| ID | Repo Dir                       | Branch                  | Tracking Branch                          | C | M | ? |\n" \
      "|----|--------------------------------|-------------------------|------------------------------------------|---|---|---|\n" \
      "|  0 | MockProjectDir (Base)          | medium_branch           | medium_remote/medium_branch              |   |   |   |\n" \
      "|  1 | ExtraRepo1                     | short_br                | origin/short_br                          |   |   |   |\n" \
      "|  2 | really/long/path/to/ExtraRepo2 | really_long_branch_name | long_remote_name/really_long_branch_name |   |   |   |\n" \
      "------------------------------------------------------------------------------------------------------------------------\n"
    self.assertEqual(table, table_expected)


  def test_stty_size_slightly_too_small(self):
    os.environ["GITDIST_UNIT_TEST_STTY_SIZE"] = "60 106"
    table = createTable(self.create_stty_size_table())
    #print(table)
    table_expected = \
      "----------------------------------------------------------------------------------------------------------\n" \
      "| ID | Repo Dir                  | Branch              | Tracking Branch                     | C | M | ? |\n" \
      "|----|---------------------------|---------------------|-------------------------------------|---|---|---|\n" \
      "|  0 | MockProjectDir (Base)     | medium_branch       | medium_remote/medium_branch         |   |   |   |\n" \
      "|  1 | ExtraRepo1                | short_br            | origin/short_br                     |   |   |   |\n" \
      "|  2 | really/long.../ExtraRepo2 | really_l...nch_name | long_remote_name...long_branch_name |   |   |   |\n" \
      "----------------------------------------------------------------------------------------------------------\n"
    self.assertEqual(table, table_expected)


  def test_stty_size_significantly_too_small(self):
    os.environ["GITDIST_UNIT_TEST_STTY_SIZE"] = "60 70"
    table = createTable(self.create_stty_size_table())
    #print(table)
    table_expected = \
      "----------------------------------------------------------------------\n" \
      "| ID | Repo Dir      | Branch     | Tracking Branch      | C | M | ? |\n" \
      "|----|---------------|------------|----------------------|---|---|---|\n" \
      "|  0 | MockP...Base) | medi...nch | medium_re...m_branch |   |   |   |\n" \
      "|  1 | ExtraRepo1    | short_br   | origin/short_br      |   |   |   |\n" \
      "|  2 | reall...Repo2 | real...ame | long_remo...nch_name |   |   |   |\n" \
      "----------------------------------------------------------------------\n"
    self.assertEqual(table, table_expected)


  def test_stty_size_too_small_for_heading(self):
    os.environ["GITDIST_UNIT_TEST_STTY_SIZE"] = "60 50"
    table = createTable(self.create_stty_size_table())
    #print(table)
    table_expected = \
      "------------------------------------------------------------------------------------------------------------------------\n" \
      "| ID | Repo Dir                       | Branch                  | Tracking Branch                          | C | M | ? |\n" \
      "|----|--------------------------------|-------------------------|------------------------------------------|---|---|---|\n" \
      "|  0 | MockProjectDir (Base)          | medium_branch           | medium_remote/medium_branch              |   |   |   |\n" \
      "|  1 | ExtraRepo1                     | short_br                | origin/short_br                          |   |   |   |\n" \
      "|  2 | really/long/path/to/ExtraRepo2 | really_long_branch_name | long_remote_name/really_long_branch_name |   |   |   |\n" \
      "------------------------------------------------------------------------------------------------------------------------\n"
    self.assertEqual(table, table_expected)


#
# Unit tests for createMarkdownTable
#


class test_createMarkdownTable(unittest.TestCase):


  def create_table_data(self):
    tableData = [
      {"label": "Repository", "align": "L",
       "fields": ["MockProjectDir", "ExtraRepo1", "ExtraRepo2"]},
      {"label": "SHA1", "align": "C",
       "fields": ["e2dc488", "f671414", "50bbf3e"]},
      {"label": "Commit Date", "align": "L",
       "fields": ["2019-10-23 10:16:07", "2019-10-22 11:18:47",
                  "2019-10-17 16:32:15"]},
      {"label": "Author", "align": "L",
       "fields": ["user@domain.com", "wile.e.coyote@acme.com",
                  "someone@somwhere.com"]},
      {"label": "Summary", "align": "L",
       "fields": ["Merge Pull Request #1234 from user/repo/branch",
                  "Fixed a Bug", "Did Some Work"]}
      ]
    return tableData


  def test_full_markdown_table(self):
    tableData = self.create_table_data()
    table = createMarkdownTable(tableData)
    #print(table)
    table_expected = \
      "| Repository     | SHA1    | Commit Date         | Author                 | Summary                                        |\n" \
      "|:-------------- |:-------:|:------------------- |:---------------------- |:---------------------------------------------- |\n" \
      "| MockProjectDir | e2dc488 | 2019-10-23 10:16:07 | user@domain.com        | Merge Pull Request #1234 from user/repo/branch |\n" \
      "| ExtraRepo1     | f671414 | 2019-10-22 11:18:47 | wile.e.coyote@acme.com | Fixed a Bug                                    |\n" \
      "| ExtraRepo2     | 50bbf3e | 2019-10-17 16:32:15 | someone@somwhere.com   | Did Some Work                                  |"
    self.assertEqual(table, table_expected)


  def test_short_markdown_table(self):
    tableData = self.create_table_data()[0:2]
    table = createMarkdownTable(tableData)
    #print(table)
    table_expected = \
      "| Repository     | SHA1    |\n" \
      "|:-------------- |:-------:|\n" \
      "| MockProjectDir | e2dc488 |\n" \
      "| ExtraRepo1     | f671414 |\n" \
      "| ExtraRepo2     | 50bbf3e |"
    self.assertEqual(table, table_expected)


  def test_no_rows(self):
    tableData = self.create_table_data()
    for entry in tableData:
        entry["fields"][:] = []
    table = createMarkdownTable(tableData)
    #print(table)
    table_expected = \
      "| Repository | SHA1 | Commit Date | Author | Summary |\n" \
      "|:---------- |:----:|:----------- |:------ |:------- |"
    self.assertEqual(table, table_expected)


  def test_one_row(self):
    tableData = self.create_table_data()
    for entry in tableData:
        entry["fields"][:] = [entry["fields"][0]]
    table = createMarkdownTable(tableData)
    #print(table)
    table_expected = \
      "| Repository     | SHA1    | Commit Date         | Author          | Summary                                        |\n" \
      "|:-------------- |:-------:|:------------------- |:--------------- |:---------------------------------------------- |\n" \
      "| MockProjectDir | e2dc488 | 2019-10-23 10:16:07 | user@domain.com | Merge Pull Request #1234 from user/repo/branch |"
    self.assertEqual(table, table_expected)


  def test_row_mismatch(self):
    tableData = self.create_table_data()
    tableData[0]["fields"][:] = []
    self.assertRaises(Exception, createMarkdownTable, tableData)


#
# Unit tests for functions in gitdist
#


class test_gitdist_getRepoStats(unittest.TestCase):


  def setUp(self):
    self.gitdistMoveToBaseDir = os.environ.get("GITDIST_MOVE_TO_BASE_DIR", "")
    os.environ["GITDIST_MOVE_TO_BASE_DIR"] = ""


  def tearDown(self):
    os.environ["GITDIST_MOVE_TO_BASE_DIR"] = self.gitdistMoveToBaseDir


  def test_no_change(self):
    try:
      testDir = createAndMoveIntoTestDir("gitdist_getRepoStats_no_change")
      open(".mockprogram_inout.txt", "w").write(
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: local_branch\n" \
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: origin_repo/remote_branch\n" \
        "MOCK_PROGRAM_INPUT: shortlog -s HEAD ^origin_repo/remote_branch\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: \n" \
        "MOCK_PROGRAM_INPUT: status --porcelain\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: \n" \
        )
      options = GitDistOptions(mockGitPath)
      repoStats = getRepoStats(options)
      repoStats_expected = "{branch='local_branch'," \
        " trackingBranch='origin_repo/remote_branch', numCommits='0'," \
        " numModified='0', numUntracked='0'}" 
      self.assertEqual(str(repoStats), repoStats_expected)
    finally:
      os.chdir(testBaseDir)


  def test_all_changed_no_tracking_branch(self):
    try:
      testDir = createAndMoveIntoTestDir(
        "gitdist_getRepoStats_all_changed_no_tracking_branch")
      open(".mockprogram_inout.txt", "w").write(
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: local_branch\n" \
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
        "MOCK_PROGRAM_RETURN: 55\n" \
        "MOCK_PROGRAM_OUTPUT: error: blah blahh blah\n" \
        "MOCK_PROGRAM_INPUT: status --porcelain\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: M  file1\n" \
        " T file2\n" \
        " D file3\n" \
        "?? file4\n" \
        "?? file5\n" \
        )
      options = GitDistOptions(mockGitPath)
      repoStats = getRepoStats(options)
      repoStats_expected = "{branch='local_branch'," \
        " trackingBranch='', numCommits=''," \
        " numModified='3', numUntracked='2'}" 
      self.assertEqual(str(repoStats), repoStats_expected)
    finally:
      os.chdir(testBaseDir)


  def test_modified_and_staged_no_tracking_branch(self):
    try:
      testDir = createAndMoveIntoTestDir(
        "gitdist_getRepoStats_all_changed_no_tracking_branch")
      open(".mockprogram_inout.txt", "w").write(
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: local_branch\n" \
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
        "MOCK_PROGRAM_RETURN: 55\n" \
        "MOCK_PROGRAM_OUTPUT: error: blah blahh blah\n" \
        "MOCK_PROGRAM_INPUT: status --porcelain\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: M  file1\n" \
        "MM file1b\n" \
        " T file2\n" \
        "MT file2b\n" \
        " D file3\n" \
        "MD file3\n" \
        "?? file4\n" \
        "?? file5\n" \
        "?? file5b\n" \
        " A file6\n" \
        "A  file6b\n" \
        " U file7\n" \
        "U  file7b\n" \
        "R  file8\n" \
        )
      options = GitDistOptions(mockGitPath)
      repoStats = getRepoStats(options)
      repoStats_expected = "{branch='local_branch'," \
        " trackingBranch='', numCommits=''," \
        " numModified='11', numUntracked='3'}" 
      self.assertEqual(str(repoStats), repoStats_expected)
    finally:
      os.chdir(testBaseDir)


  def test_all_changed_detached_head(self):
    try:
      testDir = createAndMoveIntoTestDir("gitdist_getRepoStats_all_changed_detached_head")
      open(".mockprogram_inout.txt", "w").write(
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: HEAD\n" \
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
        "MOCK_PROGRAM_RETURN: 128\n" \
        "MOCK_PROGRAM_OUTPUT: fatal: blah blahh blah\n" \
        "MOCK_PROGRAM_INPUT: status --porcelain\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: M  file1\n" \
        " M file2\n" \
        "?? file3\n" \
        "?? file4\n" \
        "?? file5\n" \
        )
      options = GitDistOptions(mockGitPath)
      repoStats = getRepoStats(options)
      repoStats_expected = "{branch='HEAD'," \
        " trackingBranch='', numCommits=''," \
        " numModified='2', numUntracked='3'}" 
      self.assertEqual(str(repoStats), repoStats_expected)
    finally:
      os.chdir(testBaseDir)


  def test_all_ambiguous_head(self):
    try:
      testDir = createAndMoveIntoTestDir("gitdist_getRepoStats_all_changed_detached_head")
      open(".mockprogram_inout.txt", "w").write(
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: warning: refname 'HEAD' is ambiguous.\n" \
        "error: refname 'HEAD' is ambiguous\n" \
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: remoterepo/trackingbranch\n" \
        "MOCK_PROGRAM_INPUT: shortlog -s HEAD ^remoterepo/trackingbranch\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: 7\n" \
        "MOCK_PROGRAM_INPUT: status --porcelain\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: M  file1\n" \
        " M file2\n" \
        "?? file3\n" \
        "?? file4\n" \
        "?? file5\n" \
        )
      options = GitDistOptions(mockGitPath)
      repoStats = getRepoStats(options)
      repoStats_expected = "{branch='<AMBIGUOUS-HEAD>'," \
        " trackingBranch='remoterepo/trackingbranch', numCommits='7'," \
        " numModified='2', numUntracked='3'}" 
      self.assertEqual(str(repoStats), repoStats_expected)
    finally:
      os.chdir(testBaseDir)
    # NOTE: Above is a very strange test case.  It is what happens when
    # someone creates a tag called 'HEAD' using the command 'git tag HEAD'
    # (which was an accident obviously).  But amazingly, 'git rev-parse
    # --abbrev HEAD' still returns 0 but returns no name!  See TriBITS #100
    # for details.


  def test_all_changed_1_author(self):
    try:
      testDir = createAndMoveIntoTestDir("gitdist_getRepoStats_all_changed_1_author")
      open(".mockprogram_inout.txt", "w").write(
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: local_branch\n" \
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: origin_repo/remote_branch\n" \
        "MOCK_PROGRAM_INPUT: shortlog -s HEAD ^origin_repo/remote_branch\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: 1\tsome author\n" \
        "MOCK_PROGRAM_INPUT: status --porcelain\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: M  file1\n" \
        " M file2\n" \
        "?? file3\n" \
        "?? file4\n" \
        "?? file5\n" \
        )
      options = GitDistOptions(mockGitPath)
      repoStats = getRepoStats(options)
      repoStats_expected = "{branch='local_branch'," \
        " trackingBranch='origin_repo/remote_branch', numCommits='1'," \
        " numModified='2', numUntracked='3'}" 
      self.assertEqual(str(repoStats), repoStats_expected)
    finally:
      os.chdir(testBaseDir)


  def test_all_changed_3_authors(self):
    try:
      testDir = createAndMoveIntoTestDir("gitdist_getRepoStats_all_changed_3_authors")
      open(".mockprogram_inout.txt", "w").write(
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: local_branch\n" \
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: origin_repo/remote_branch\n" \
        "MOCK_PROGRAM_INPUT: shortlog -s HEAD ^origin_repo/remote_branch\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: 1 some author1\n" \
        "2 some author2\n" \
        "3 some author2\n" \
        "MOCK_PROGRAM_INPUT: status --porcelain\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: M  file1\n" \
        " M file2\n" \
        "?? file3\n" \
        "?? file4\n" \
        "?? file5\n" \
        )
      options = GitDistOptions(mockGitPath)
      repoStats = getRepoStats(options)
      repoStats_expected = "{branch='local_branch'," \
        " trackingBranch='origin_repo/remote_branch', numCommits='6'," \
        " numModified='2', numUntracked='3'}" 
      self.assertEqual(str(repoStats), repoStats_expected)
    finally:
      os.chdir(testBaseDir)


repoVersionFile_withSummary_1 = """*** Base Git Repo: MockTrilinos
sha1_1 [Mon Sep 23 11:34:59 2013 -0400] <author_1@ornl.gov>
First summary message
*** Git Repo: extraTrilinosRepo
sha1_2 [Fri Aug 30 09:55:07 2013 -0400] <author_2@ornl.gov>
Second summary message
*** Git Repo: extraRepoOnePackage
sha1_3 [Thu Dec 1 23:34:06 2011 -0500] <author_3@ornl.gov>
Third summary message
"""

repoVersionFile_withoutSummary_1 = """*** Base Git Repo: MockTrilinos
sha1_1 [Mon Sep 23 11:34:59 2013 -0400] <author_1@ornl.gov>
*** Git Repo: extraRepoTwoPackages
sha1_2 [Fri Aug 30 09:55:07 2013 -0400] <author_2@ornl.gov>
*** Git Repo: extraRepoOnePackageThreeSubpackages
sha1_3 [Thu Dec 1 23:34:06 2011 -0500] <author_3@ornl.gov>
"""


def writeGitMockProgram_base_3_2_1_repo1_22_0_2_repo2_0_0_0():

  open(".mockprogram_inout.txt", "w").write(
    "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: local_branch0\n" \
    "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: origin_repo0/remote_branch0\n" \
    "MOCK_PROGRAM_INPUT: shortlog -s HEAD ^origin_repo0/remote_branch0\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: 3 some author\n" \
    "MOCK_PROGRAM_INPUT: status --porcelain\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: M  file1\n" \
    " M file2\n" \
    "?? file2\n" \
    "MOCK_PROGRAM_INPUT: status\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: On branch local_branch0\n" \
    "Your branch is ahead of 'origin_repo0/remote_branch0' by 3 commits.\n" \
    )

  open("ExtraRepo1/.mockprogram_inout.txt", "w").write(
    "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: local_branch1\n" \
    "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: origin_repo1/remote_branch1\n" \
    "MOCK_PROGRAM_INPUT: shortlog -s HEAD ^origin_repo1/remote_branch1\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: 22 some author\n" \
    "MOCK_PROGRAM_INPUT: status --porcelain\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: ?? file1\n" \
    "MOCK_PROGRAM_INPUT: status\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: On branch local_branch1\n" \
    "Your branch is ahead of 'origin_repo1/remote_branch1' by 22 commits.\n" \
    )

  open("ExtraRepo2/.mockprogram_inout.txt", "w").write(
    "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: local_branch2\n" \
    "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: origin_repo2/remote_branch2\n" \
    "MOCK_PROGRAM_INPUT: shortlog -s HEAD ^origin_repo2/remote_branch2\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: \n" \
    "MOCK_PROGRAM_INPUT: status --porcelain\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: \n" \
    )


def writeGitMockProgram_dist_repo_versions_table():

  with open(".mockprogram_inout.txt", "w") as f:
    f.write(
      "MOCK_PROGRAM_INPUT: rev-parse --short HEAD\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: e2dc488\n" \
      "MOCK_PROGRAM_INPUT: log -1 --pretty=format:%cd --date=format:%G-%m-%d %H:%M:%S\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: 2019-10-23 10:16:07\n" \
      "MOCK_PROGRAM_INPUT: log -1 --pretty=format:%ae\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: user@domain.com\n" \
      "MOCK_PROGRAM_INPUT: log -1 --pretty=format:%s\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: Merge Pull Request #1234 from user/repo/branch\n" \
    )

  with open("ExtraRepo1/.mockprogram_inout.txt", "w") as f:
    f.write(
      "MOCK_PROGRAM_INPUT: rev-parse --short HEAD\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: f671414\n" \
      "MOCK_PROGRAM_INPUT: log -1 --pretty=format:%cd --date=format:%G-%m-%d %H:%M:%S\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: 2019-10-22 11:18:47\n" \
      "MOCK_PROGRAM_INPUT: log -1 --pretty=format:%ae\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: wile.e.coyote@acme.com\n" \
      "MOCK_PROGRAM_INPUT: log -1 --pretty=format:%s\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: Fixed a Bug\n" \
    )

  with open("ExtraRepo2/.mockprogram_inout.txt", "w") as f:
    f.write(
      "MOCK_PROGRAM_INPUT: rev-parse --short HEAD\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: 50bbf3e\n" \
      "MOCK_PROGRAM_INPUT: log -1 --pretty=format:%cd --date=format:%G-%m-%d %H:%M:%S\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: 2019-10-17 16:32:15\n" \
      "MOCK_PROGRAM_INPUT: log -1 --pretty=format:%ae\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: someone@somwhere.com\n" \
      "MOCK_PROGRAM_INPUT: log -1 --pretty=format:%s\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: Did Some Work\n" \
    )


def writeGitMockProgram_dist_repo_versions_table_1_change_base():

  with open(".mockprogram_inout.txt", "w") as f:
    f.write(
      "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: local_branch0\n" \
      "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: origin_repo0/remote_branch0\n" \
      "MOCK_PROGRAM_INPUT: shortlog -s HEAD ^origin_repo0/remote_branch0\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: 3 some author\n" \
      "MOCK_PROGRAM_INPUT: status --porcelain\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: M  file1\n" \
      "MOCK_PROGRAM_INPUT: rev-parse --short HEAD\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: e2dc488\n" \
      "MOCK_PROGRAM_INPUT: log -1 --pretty=format:%cd --date=format:%G-%m-%d %H:%M:%S\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: 2019-10-23 10:16:07\n" \
      "MOCK_PROGRAM_INPUT: log -1 --pretty=format:%ae\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: user@domain.com\n" \
      "MOCK_PROGRAM_INPUT: log -1 --pretty=format:%s\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: Merge Pull Request #1234 from user/repo/branch\n" \
    )

  with open("ExtraRepo1/.mockprogram_inout.txt", "w") as f:
    f.write(
      "MOCK_PROGRAM_INPUT: rev-parse --short HEAD\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: f671414\n" \
      "MOCK_PROGRAM_INPUT: log -1 --pretty=format:%cd --date=format:%G-%m-%d %H:%M:%S\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: 2019-10-22 11:18:47\n" \
      "MOCK_PROGRAM_INPUT: log -1 --pretty=format:%ae\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: wile.e.coyote@acme.com\n" \
      "MOCK_PROGRAM_INPUT: log -1 --pretty=format:%s\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: Fixed a Bug\n" \
    )

  with open("ExtraRepo2/.mockprogram_inout.txt", "w") as f:
    f.write(
      "MOCK_PROGRAM_INPUT: rev-parse --short HEAD\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: 50bbf3e\n" \
      "MOCK_PROGRAM_INPUT: log -1 --pretty=format:%cd --date=format:%G-%m-%d %H:%M:%S\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: 2019-10-17 16:32:15\n" \
      "MOCK_PROGRAM_INPUT: log -1 --pretty=format:%ae\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: someone@somwhere.com\n" \
      "MOCK_PROGRAM_INPUT: log -1 --pretty=format:%s\n" \
      "MOCK_PROGRAM_RETURN: 0\n" \
      "MOCK_PROGRAM_OUTPUT: Did Some Work\n" \
    )


def writeGitMockProgram_base_3_2_1_repo1_0_0_0_repo2_4_0_2():

  open(".mockprogram_inout.txt", "w").write(
    "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: local_branch0\n" \
    "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: origin_repo0/remote_branch0\n" \
    "MOCK_PROGRAM_INPUT: shortlog -s HEAD ^origin_repo0/remote_branch0\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: 3 some author\n" \
    "MOCK_PROGRAM_INPUT: status --porcelain\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: M  file1\n" \
    " M file2\n" \
    "?? file3\n" \
    "MOCK_PROGRAM_INPUT: status\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: On branch local_branch0\n" \
    "Your branch is ahead of 'origin_repo0/remote_branch0' by 3 commits.\n" \
    )

  open("ExtraRepo1/.mockprogram_inout.txt", "w").write(
    "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: local_branch1\n" \
    "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: origin_repo1/remote_branch1\n" \
    "MOCK_PROGRAM_INPUT: shortlog -s HEAD ^origin_repo1/remote_branch1\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: \n" \
    "MOCK_PROGRAM_INPUT: status --porcelain\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: \n" \
    )

  open("ExtraRepo2/.mockprogram_inout.txt", "w").write(
    "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: local_branch2\n" \
    "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: origin_repo2/remote_branch2\n" \
    "MOCK_PROGRAM_INPUT: shortlog -s HEAD ^origin_repo2/remote_branch2\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: 3 some author\n" \
    "1 some other author\n" \
    "MOCK_PROGRAM_INPUT: status --porcelain\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: ??  file1\n" \
    "?? file3\n" \
    "MOCK_PROGRAM_INPUT: status\n" \
    "MOCK_PROGRAM_RETURN: 0\n" \
    "MOCK_PROGRAM_OUTPUT: On branch local_branch2\n" \
    "Your branch is ahead of 'origin_repo2/remote_branch2' by 4 commits.\n" \
    )


class test_gitdist_getRepoVersionDictFromRepoVersionFileString(unittest.TestCase):


  def setUp(self):
    self.gitdistMoveToBaseDir = os.environ.get("GITDIST_MOVE_TO_BASE_DIR", "")
    os.environ["GITDIST_MOVE_TO_BASE_DIR"] = ""


  def tearDown(self):
    os.environ["GITDIST_MOVE_TO_BASE_DIR"] = self.gitdistMoveToBaseDir


  def test_repoVersionFile_withSummary_1(self):
    repoVersionDict = \
      getRepoVersionDictFromRepoVersionFileString(repoVersionFile_withSummary_1)
    expectedDict = {
      'MockTrilinos': 'sha1_1',
      'extraTrilinosRepo': 'sha1_2',
      'extraRepoOnePackage': 'sha1_3'
      }
    #print("repoVersionDict =\n" + str(repoVersionDict))
    self.assertEqual(repoVersionDict, expectedDict)


  def test_repoVersionFile_withoutSummary_1(self):
    repoVersionDict = \
      getRepoVersionDictFromRepoVersionFileString(repoVersionFile_withoutSummary_1)
    expectedDict = {
      'MockTrilinos': 'sha1_1',
      'extraRepoTwoPackages': 'sha1_2',
      'extraRepoOnePackageThreeSubpackages': 'sha1_3'
      }
    #print("repoVersionDict =\n" + str(repoVersionDict))
    self.assertEqual(repoVersionDict, expectedDict)


# ToDo: Add unit tests for requoteCmndLineArgsIntoArray!


#
# Test entire script gitdist
#


def assertContainsGitdistHelpHeader(testObj, cmndOut):
  cmndOutList = cmndOut.splitlines()
  cmndOutFirstLine = cmndOutList[0]
  cmndOutFirstLineAfterComma = cmndOutFirstLine.split(s(":"))[1].strip() 
  cmndOutFirstLineAfterComma_expected = s("gitdist [gitdist arguments] <raw-git-command> [git arguments]")
  testObj.assertEqual(cmndOutFirstLineAfterComma, cmndOutFirstLineAfterComma_expected)


def assertContainsAllGitdistHelpSections(testObj, cmndOut):
  testObj.assertEqual(
    GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^OVERVIEW:$"), "OVERVIEW:\n")
  testObj.assertEqual(
    GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^REPO SELECTION AND SETUP:$"), "REPO SELECTION AND SETUP:\n")
  testObj.assertEqual(
    GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^SUMMARY OF REPO STATUS:$"), "SUMMARY OF REPO STATUS:\n")
  testObj.assertEqual(
    GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^REPO VERSION FILES:$"), "REPO VERSION FILES:\n")
  testObj.assertEqual(
    GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^USEFUL ALIASES:$"), "USEFUL ALIASES:\n")
  testObj.assertEqual(
    GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^USAGE TIPS:$"), "USAGE TIPS:\n")
  testObj.assertEqual(
    GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^SCRIPT DEPENDENCIES:$"), "SCRIPT DEPENDENCIES:\n")
  testObj.assertEqual(
    GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^DEFAULT BRANCH SPECIFICATION:$"), "DEFAULT BRANCH SPECIFICATION:\n")
  testObj.assertEqual(
    GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^MOVE TO BASE DIRECTORY:$"), "MOVE TO BASE DIRECTORY:\n")
  testObj.assertEqual(
    GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^REPO VERSION TABLE:$"), "REPO VERSION TABLE:\n")


class test_gitdist(unittest.TestCase):


  def setUp(self):
    self.gitdistMoveToBaseDir = os.environ.get("GITDIST_MOVE_TO_BASE_DIR", "")
    os.environ["GITDIST_MOVE_TO_BASE_DIR"] = ""
    self.gitdistUnitTestSttySize = os.environ.get(
      "GITDIST_UNIT_TEST_STTY_SIZE", ""
    )
    os.environ["GITDIST_UNIT_TEST_STTY_SIZE"] = "60 120"


  def tearDown(self):
    os.environ["GITDIST_MOVE_TO_BASE_DIR"] = self.gitdistMoveToBaseDir
    os.environ["GITDIST_UNIT_TEST_STTY_SIZE"] = self.gitdistUnitTestSttySize


  def test_default(self):
    (cmndOut, errOut) = getCmndOutput(gitdistPathNoColor, rtnCode=True)
    cmndOut_expected = "Must specify git command. See 'git --help' for options.\n"
    self.assertEqual(s(cmndOut), s(cmndOut_expected))
    self.assertEqual(errOut, 1)


  # Make sure the default --help shows the section "OVERVIEW"
  def test_help(self):
    cmndOut = getCmndOutput(gitdistPath+" --help")
    assertContainsGitdistHelpHeader(self, cmndOut)
    self.assertEqual(
      GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^OVERVIEW:$"), "")
    self.assertEqual(
      GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^REPO SELECTION AND SETUP:$"), "")


  # Make sure --dist-help= does not print OVERVIEW section
  def test_dist_help_none_help(self):
    cmndOut = getCmndOutput(gitdistPath+" --dist-help= --help")
    assertContainsGitdistHelpHeader(self, cmndOut)
    self.assertEqual(
      GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^OVERVIEW:$"), "")
    self.assertEqual(
      GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^Options:$"), "Options:\n")


  # --dist-help=aliases --help
  def test_dist_help_aliases_help(self):
    cmndOut = getCmndOutput(gitdistPath+" --dist-help=aliases --help")
    assertContainsGitdistHelpHeader(self, cmndOut)
    self.assertEqual(
      GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^USEFUL ALIASES:$"), "USEFUL ALIASES:\n")
    self.assertEqual(
      GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^REPO SELECTION AND SETUP:$"), "")


  # Make sure --dist-help=all prints all the topic headers
  def test_dist_help_all_help(self):
    cmndOut = getCmndOutput(gitdistPath+" --dist-help=all --help")
    assertContainsGitdistHelpHeader(self, cmndOut)
    assertContainsAllGitdistHelpSections(self, cmndOut)


  # Test that --dist-help --help prints nice error message
  def test_dist_help_help(self):
    cmndOut = getCmndOutput(gitdistPath+" --dist-help --help")
    cmndOut_expected = "gitdist: error: option --dist-help: invalid choice: '--help' (choose from '', 'overview', 'repo-selection-and-setup', 'dist-repo-status', 'repo-versions', 'dist-repo-versions-table', 'aliases', 'default-branch', 'move-to-base-dir', 'usage-tips', 'script-dependencies', 'all')\n"
    self.assertEqual(s(cmndOut), s(cmndOut_expected))


  # Test --dist-helps=invalid-pick picked up as invalid value.
  def test_dist_help_invalid_pick_help(self):
    cmndOut = getCmndOutput(gitdistPath+" --dist-help=invalid-pick --help")
    assertContainsGitdistHelpHeader(self, cmndOut)
    errorToFind = "gitdist: error: option --dist-help: invalid choice: 'invalid-pick' (choose from '', 'overview', 'repo-selection-and-setup', 'dist-repo-status', 'repo-versions', 'dist-repo-versions-table', 'aliases', 'default-branch', 'move-to-base-dir', 'usage-tips', 'script-dependencies', 'all')"
    self.assertEqual(
      GeneralScriptSupport.extractLinesMatchingSubstr(cmndOut,errorToFind), errorToFind+"\n")


  # Test --dist-help (show error string)
  def test_dist_help(self):
    (cmndOut, errOut) = getCmndOutput(gitdistPath+" --dist-help", rtnCode=True)
    if sys.version_info < (3,):
      anOrOne = "an"
    else:
      anOrOne = "1"
    self.assertEqual(
      s(cmndOut), s("gitdist: error: --dist-help option requires "+anOrOne+" argument\n"))
    self.assertEqual(errOut, 2)


  # Test --dist-help= (show no-op string)
  def test_dist_help_none(self):
    (cmndOut, errOut) = getCmndOutput(gitdistPathNoColor+" --dist-help=", rtnCode=True)
    self.assertEqual(
      s(cmndOut), s("Must specify git command. See 'git --help' for options.\n"))
    self.assertEqual(errOut, 1)


  # Test --dist-help=overview
  def test_dist_help_overview(self):
    cmndOut = getCmndOutput(gitdistPath+" --dist-help=overview")
    self.assertEqual(
      GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^OVERVIEW:$"), "OVERVIEW:\n")
    self.assertEqual(
      GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^Options:$"), "")


  # Test --dist-help=usage-tips
  def test_dist_help_usage_tips(self):
    cmndOut = getCmndOutput(gitdistPath+" --dist-help=usage-tips")
    self.assertEqual(
      GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^USAGE TIPS:$"), "USAGE TIPS:\n")
    self.assertEqual(
      GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^Options:$"), "")


  # Test --dist-help=all
  def test_dist_help_all(self):
    cmndOut = getCmndOutput(gitdistPath+" --dist-help=all")
    assertContainsAllGitdistHelpSections(self, cmndOut)
    self.assertEqual(
      GeneralScriptSupport.extractLinesMatchingRegex(cmndOut,"^Options:$"), "")


  def test_noEgGit(self):
    (cmndOut, errOut) = getCmndOutput(gitdistPathNoColor+" --dist-use-git= log",
      rtnCode=True)
    cmndOut_expected = "Can't find git, please set --dist-use-git\n"
    self.assertEqual(s(cmndOut), s(cmndOut_expected))
    self.assertEqual(errOut, 1)


  def test_log_args(self):
    cmndOut = getCmndOutputInMockProjectDir(gitdistPathMock+" log HEAD -1")
    cmndOut_expected = \
      "\n*** Base Git Repo: MockTrilinos\n" \
      "['mockgit', 'log', 'HEAD', '-1']\n\n"
    self.assertEqual(s(cmndOut), s(cmndOut_expected))


  def test_dot_gitdist(self):
    os.chdir(testBaseDir)
    try:

      # Create a mock git meta-project

      testDir = createAndMoveIntoTestDir("gitdist_dot_gitdist")

      os.mkdir("ExtraRepo1")
      os.makedirs("Path/To/ExtraRepo2")
      os.mkdir("ExtraRepo3")

      # Make sure .gitdist.default is found and read correctly
      open(".gitdist.default", "w").write(
        ".\n" \
        "ExtraRepo1\n" \
        "Path/To/ExtraRepo2\n" \
        "MissingExtraRep\n" \
        "ExtraRepo3\n"
        )
      cmndOut = GeneralScriptSupport.getCmndOutput(gitdistPathMock+" status",
        workingDir=testDir)
      cmndOut_expected = \
        "\n*** Base Git Repo: MockProjectDir\n" \
        "['mockgit', 'status']\n\n" \
        "*** Git Repo: ExtraRepo1\n" \
        "['mockgit', 'status']\n\n" \
        "*** Git Repo: Path/To/ExtraRepo2\n" \
        "['mockgit', 'status']\n\n" \
        "*** Git Repo: ExtraRepo3\n" \
        "['mockgit', 'status']\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))
      # NOTE: Above ensures that all of the paths are read correctly and that
      # missing paths (MissingExtraRepo) are ignored.

      # Make sure that .gitdist overrides .gitdist.default
      open(".gitdist", "w").write(
        ".\n" \
        "ExtraRepo1\n" \
        "\n" \
        "   \n" \
        "ExtraRepo3\n" \
        "\n"
        )
      cmndOut = GeneralScriptSupport.getCmndOutput(gitdistPathMock+" status",
        workingDir=testDir)
      cmndOut_expected = \
        "\n*** Base Git Repo: MockProjectDir\n" \
        "['mockgit', 'status']\n\n" \
        "*** Git Repo: ExtraRepo1\n" \
        "['mockgit', 'status']\n\n" \
        "*** Git Repo: ExtraRepo3\n" \
        "['mockgit', 'status']\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

      # Make sure that --dist-repos overrides all files
      cmndOut = GeneralScriptSupport.getCmndOutput(
        gitdistPathMock+" --dist-repos=.,ExtraRepo1,Path/To/ExtraRepo2 status",
        workingDir=testDir)
      cmndOut_expected = \
        "\n*** Base Git Repo: MockProjectDir\n" \
        "['mockgit', 'status']\n\n" \
        "*** Git Repo: ExtraRepo1\n" \
        "['mockgit', 'status']\n\n" \
        "*** Git Repo: Path/To/ExtraRepo2\n" \
        "['mockgit', 'status']\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

    finally:
      os.chdir(testBaseDir)
    

  def test_log_args_extra_repo_1(self):
    cmndOut = getCmndOutputInMockProjectDir(
      gitdistPathMock+" --dist-repos=.,extraTrilinosRepo log HEAD -1")
    cmndOut_expected = \
      "\n*** Base Git Repo: MockTrilinos\n" \
      "['mockgit', 'log', 'HEAD', '-1']\n\n" \
      "*** Git Repo: extraTrilinosRepo\n" \
      "['mockgit', 'log', 'HEAD', '-1']\n\n"
    self.assertEqual(s(cmndOut), s(cmndOut_expected))


  def test_log_args_extra_repo_2_not_first(self):
    cmndOut = getCmndOutputInMockProjectDir(
      gitdistPathMock+\
        " --dist-repos=.,extraTrilinosRepo,extraRepoOnePackage "+\
        " --dist-not-repos=extraTrilinosRepo "+\
        " log HEAD -1"
      )
    cmndOut_expected = \
      "\n*** Base Git Repo: MockTrilinos\n" \
      "['mockgit', 'log', 'HEAD', '-1']\n\n" \
      "*** Git Repo: extraRepoOnePackage\n" \
      "['mockgit', 'log', 'HEAD', '-1']\n\n"
    self.assertEqual(s(cmndOut), s(cmndOut_expected))


  def test_log_args_extra_repo_2_not_second(self):
    cmndOut = getCmndOutputInMockProjectDir(
      gitdistPathMock+\
        " --dist-repos=.,extraTrilinosRepo,extraRepoOnePackage "+\
        " --dist-not-repos=extraTrilinosRepo "+\
        " log HEAD -1"
      )
    cmndOut_expected = \
      "\n*** Base Git Repo: MockTrilinos\n" \
      "['mockgit', 'log', 'HEAD', '-1']\n\n" \
      "*** Git Repo: extraRepoOnePackage\n" \
      "['mockgit', 'log', 'HEAD', '-1']\n\n"
    self.assertEqual(s(cmndOut), s(cmndOut_expected))


  def test_log_args_extra_repo_1_not_base(self):
    cmndOut = getCmndOutputInMockProjectDir(
      gitdistPathMock+\
        " --dist-repos=.,extraTrilinosRepo "+\
        " --dist-not-repos=. "+\
        " log HEAD -1"
      )
    cmndOut_expected = \
      "\n*** Git Repo: extraTrilinosRepo\n" \
      "['mockgit', 'log', 'HEAD', '-1']\n\n"
    self.assertEqual(s(cmndOut), s(cmndOut_expected))


  def test_dist_mod_only_1_change_base(self):
    os.chdir(testBaseDir)
    try:

      # Create a mock git meta-project

      testDir = createAndMoveIntoTestDir("gitdist_dist_mod_only_1_change_base")

      os.mkdir("ExtraRepo1")
      os.mkdir("ExtraRepo2")

      open(".mockprogram_inout.txt", "w").write(
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: local_branch0\n" \
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: origin_repo0/remote_branch0\n" \
        "MOCK_PROGRAM_INPUT: shortlog -s HEAD ^origin_repo0/remote_branch0\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: 3 some author\n" \
        "MOCK_PROGRAM_INPUT: status --porcelain\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: M  file1\n" \
        "MOCK_PROGRAM_INPUT: status\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: On branch local_branch0\n" \
        "Your branch is ahead of 'origin_repo0/remote_branch0' by 3 commits.\n" \
        )

      open("ExtraRepo1/.mockprogram_inout.txt", "w").write(
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: local_branch1\n" \
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: origin_repo1/remote_branch1\n" \
        "MOCK_PROGRAM_INPUT: shortlog -s HEAD ^origin_repo1/remote_branch1\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: \n" \
        "MOCK_PROGRAM_INPUT: status --porcelain\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: \n" \
        )

      open("ExtraRepo2/.mockprogram_inout.txt", "w").write(
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: local_branch2\n" \
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: origin_repo2/remote_branch2\n" \
        "MOCK_PROGRAM_INPUT: shortlog -s HEAD ^origin_repo2/remote_branch2\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: \n" \
        "MOCK_PROGRAM_INPUT: status --porcelain\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: \n" \
        )

      cmndOut = GeneralScriptSupport.getCmndOutput(
        gitdistPath + " --dist-no-color --dist-use-git="+mockGitPath \
          +" --dist-mod-only --dist-repos=.,ExtraRepo1,ExtraRepo2 status",
        workingDir=testDir)
      cmndOut_expected = \
        "\n*** Base Git Repo: MockProjectDir\n" \
        "On branch local_branch0\n" \
        "Your branch is ahead of 'origin_repo0/remote_branch0' by 3 commits.\n\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

    finally:
      os.chdir(testBaseDir)


  def test_dist_mod_only_1_change_extrarepo1(self):
    os.chdir(testBaseDir)
    try:

      # Create a mock git meta-project

      testDir = createAndMoveIntoTestDir("gitdist_dist_mod_only_1_change_extrarepo1")

      os.mkdir("ExtraRepo1")
      os.mkdir("ExtraRepo2")

      open(".mockprogram_inout.txt", "w").write(
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: local_branch0\n" \
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: origin_repo0/remote_branch0\n" \
        "MOCK_PROGRAM_INPUT: shortlog -s HEAD ^origin_repo0/remote_branch0\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: \n" \
        "MOCK_PROGRAM_INPUT: status --porcelain\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: \n" \
        )

      open("ExtraRepo1/.mockprogram_inout.txt", "w").write(
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: local_branch1\n" \
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: origin_repo1/remote_branch1\n" \
        "MOCK_PROGRAM_INPUT: shortlog -s HEAD ^origin_repo1/remote_branch1\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: 1 some author\n" \
        "MOCK_PROGRAM_INPUT: status --porcelain\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: \n" \
        "MOCK_PROGRAM_INPUT: status\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: On branch local_branch1\n" \
        "Your branch is ahead of 'origin_repo1/remote_branch1' by 1 commits.\n" \
        )

      open("ExtraRepo2/.mockprogram_inout.txt", "w").write(
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: local_branch2\n" \
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: origin_repo2/remote_branch2\n" \
        "MOCK_PROGRAM_INPUT: shortlog -s HEAD ^origin_repo2/remote_branch2\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: \n" \
        "MOCK_PROGRAM_INPUT: status --porcelain\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: \n" \
        )

      cmndOut = GeneralScriptSupport.getCmndOutput(
        gitdistPath + " --dist-no-color --dist-use-git="+mockGitPath \
          +" --dist-mod-only --dist-repos=.,ExtraRepo1,ExtraRepo2 status",
        workingDir=testDir)
      cmndOut_expected = \
        "\n*** Git Repo: ExtraRepo1\nOn branch local_branch1\n" \
        "Your branch is ahead of 'origin_repo1/remote_branch1' by 1 commits.\n\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

    finally:
      os.chdir(testBaseDir)


  def test_dist_mod_only_1_extrarepo1_not_tracking_branch(self):
    os.chdir(testBaseDir)
    try:

      # Create a mock git meta-project

      testDir = createAndMoveIntoTestDir("dist_mod_only_1_extrarepo1_not_tracking_branch")

      os.mkdir("ExtraRepo1")

      open(".mockprogram_inout.txt", "w").write(
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: local_branch0\n" \
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: origin_repo0/remote_branch0\n" \
        "MOCK_PROGRAM_INPUT: shortlog -s HEAD ^origin_repo0/remote_branch0\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: 3 some author\n" \
        "MOCK_PROGRAM_INPUT: status --porcelain\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: M  file1\n" \
        "MOCK_PROGRAM_INPUT: status\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: On branch local_branch0\n" \
        "Your branch is ahead of 'origin_repo0/remote_branch0' by 3 commits.\n" \
        )

      open("ExtraRepo1/.mockprogram_inout.txt", "w").write(
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: local_branch1\n" \
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
        "MOCK_PROGRAM_RETURN: 128\n" \
        "MOCK_PROGRAM_OUTPUT: error: No upstream branch found for ''\n" \
        "MOCK_PROGRAM_INPUT: status --porcelain\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: \n" \
        )

      cmndOut = GeneralScriptSupport.getCmndOutput(
        gitdistPath + " --dist-no-color --dist-use-git="+mockGitPath \
          +" --dist-mod-only --dist-repos=.,ExtraRepo1,ExtraRepo2 status",
        workingDir=testDir)
      cmndOut_expected = \
        "\n*** Base Git Repo: MockProjectDir\n" \
        "On branch local_branch0\n" \
        "Your branch is ahead of 'origin_repo0/remote_branch0' by 3 commits.\n\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

    finally:
      os.chdir(testBaseDir)


  def test_dist_mod_only_1_extrarepo1_not_tracking_branch_with_mods(self):
    os.chdir(testBaseDir)
    try:

      # Create a mock git meta-project

      testDir = createAndMoveIntoTestDir("dist_mod_only_1_extrarepo1_not_tracking_branch_with_mods")

      os.mkdir("ExtraRepo1")

      open(".mockprogram_inout.txt", "w").write(
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: local_branch0\n" \
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: origin_repo0/remote_branch0\n" \
        "MOCK_PROGRAM_INPUT: shortlog -s HEAD ^origin_repo0/remote_branch0\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: \n" \
        "MOCK_PROGRAM_INPUT: status --porcelain\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: \n" \
        )

      open("ExtraRepo1/.mockprogram_inout.txt", "w").write(
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref HEAD\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: local_branch1\n" \
        "MOCK_PROGRAM_INPUT: rev-parse --abbrev-ref --symbolic-full-name @{u}\n" \
        "MOCK_PROGRAM_RETURN: 128\n" \
        "MOCK_PROGRAM_OUTPUT: error: No upstream branch found for ''\n" \
        "MOCK_PROGRAM_INPUT: status --porcelain\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: M  file1\n" \
        "MOCK_PROGRAM_INPUT: status\n" \
        "MOCK_PROGRAM_RETURN: 0\n" \
        "MOCK_PROGRAM_OUTPUT: On branch local_branch1\n" \
        "Your branch is ahead of 'origin_repo1/remote_branch1' by 1 commits.\n" \
        )

      # Make sure that --dist-repos overrides all files
      cmndOut = GeneralScriptSupport.getCmndOutput(
        gitdistPath + " --dist-no-color --dist-use-git="+mockGitPath \
          +" --dist-mod-only --dist-repos=.,ExtraRepo1,ExtraRepo2 status",
        workingDir=testDir)
      cmndOut_expected = \
        "\n*** Git Repo: ExtraRepo1\n" \
        "On branch local_branch1\n" \
        "Your branch is ahead of 'origin_repo1/remote_branch1' by 1 commits.\n\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

    finally:
      os.chdir(testBaseDir)


  def test_log_version_file(self):
    cmndOut = getCmndOutputInMockProjectDir(
      gitdistPathMock+ \
      " --dist-version-file="+unitTestDataDir+"/versionFile_withSummary_1.txt"+\
      " log _VERSION_ --some -other args")
    cmndOut_expected = \
      "\n*** Base Git Repo: MockTrilinos\n" \
      "['mockgit', 'log', 'sha1_1', '--some', '-other', 'args']\n\n"
    self.assertEqual(s(cmndOut), s(cmndOut_expected))


  def test_log_version_file_extra_repo_1(self):
    cmndOut = getCmndOutputInMockProjectDir(
      gitdistPathMock+ \
      " --dist-version-file="+unitTestDataDir+"/versionFile_withSummary_1.txt"+ \
      " --dist-repos=.,extraTrilinosRepo"+ \
      " log _VERSION_")
    cmndOut_expected = \
      "\n*** Base Git Repo: MockTrilinos\n" \
      "['mockgit', 'log', 'sha1_1']\n" \
      "\n*** Git Repo: extraTrilinosRepo\n['mockgit', 'log', 'sha1_2']\n\n"
    self.assertEqual(s(cmndOut), s(cmndOut_expected))


  def test_log_version_file_extra_repo_2(self):
    cmndOut = getCmndOutputInMockProjectDir(
      gitdistPathMock+ \
      " --dist-version-file="+unitTestDataDir+"/versionFile_withSummary_1.txt"+ \
      " --dist-repos=.,extraRepoOnePackage,extraTrilinosRepo"+ \
      " log _VERSION_")
    cmndOut_expected = \
      "\n*** Base Git Repo: MockTrilinos\n" \
      "['mockgit', 'log', 'sha1_1']\n" \
      "\n*** Git Repo: extraRepoOnePackage\n['mockgit', 'log', 'sha1_3']\n" \
      "\n*** Git Repo: extraTrilinosRepo\n['mockgit', 'log', 'sha1_2']\n\n"
    self.assertEqual(s(cmndOut), s(cmndOut_expected))


  def test_log_HEAD_version_file_extra_repo_1(self):
    cmndOut = getCmndOutputInMockProjectDir(
      gitdistPathMock+ \
      " --dist-version-file="+unitTestDataDir+"/versionFile_withSummary_1.txt"+ \
      " --dist-repos=.,extraTrilinosRepo"+ \
      " log HEAD ^_VERSION_")
    cmndOut_expected = \
      "\n*** Base Git Repo: MockTrilinos\n" \
      "['mockgit', 'log', 'HEAD', '^sha1_1']\n" \
      "\n*** Git Repo: extraTrilinosRepo\n['mockgit', 'log', 'HEAD', '^sha1_2']\n\n"
    self.assertEqual(s(cmndOut), s(cmndOut_expected))


  def test_version_file_invalid_extra_repo(self):
    cmndOut = getCmndOutputInMockProjectDir(
      gitdistPathMock+ \
      " --dist-version-file="+unitTestDataDir+"/versionFile_withSummary_1.txt"+ \
      " --dist-repos=.,extraRepoTwoPackages"+ \
      " log _VERSION_")
    cmndOut_expected = \
      "\n*** Base Git Repo: MockTrilinos\n['mockgit', 'log', 'sha1_1']\n" \
      "\n*** Git Repo: extraRepoTwoPackages\nRepo 'extraRepoTwoPackages' is not in the list of repos ['.', 'extraRepoOnePackage', 'extraTrilinosRepo'] read in from the version file.\n"
    self.assertEqual(s(cmndOut), s(cmndOut_expected))


  def test_log_not_version_file_2(self):
    cmndOut = getCmndOutputInMockProjectDir(
      gitdistPathMock+ \
      " --dist-version-file="+unitTestDataDir+"/versionFile_withSummary_1.txt"+ \
      " --dist-version-file2="+unitTestDataDir+"/versionFile_withSummary_1_2.txt"+ \
      " log _VERSION_ ^_VERSION2_")
    cmndOut_expected = \
      "\n*** Base Git Repo: MockTrilinos\n" \
      "['mockgit', 'log', 'sha1_1', '^sha1_1_2']\n\n"
    self.assertEqual(s(cmndOut), s(cmndOut_expected))


  def test_log_not_version_file_2_extra_repo_1(self):
    cmndOut = getCmndOutputInMockProjectDir(
      gitdistPathMock+ \
      " --dist-version-file="+unitTestDataDir+"/versionFile_withSummary_1.txt"+ \
      " --dist-version-file2="+unitTestDataDir+"/versionFile_withSummary_1_2.txt"+ \
      " --dist-repos=.,extraTrilinosRepo"+ \
      " log _VERSION_ ^_VERSION2_")
    cmndOut_expected = \
      "\n*** Base Git Repo: MockTrilinos\n" \
      "['mockgit', 'log', 'sha1_1', '^sha1_1_2']\n" \
      "\n*** Git Repo: extraTrilinosRepo\n['mockgit', 'log', 'sha1_2', '^sha1_2_2']\n\n"
    self.assertEqual(s(cmndOut), s(cmndOut_expected))


  def test_log_since_until_version_file_2_extra_repo_1(self):
    cmndOut = getCmndOutputInMockProjectDir(
      gitdistPathMock+ \
      " --dist-version-file="+unitTestDataDir+"/versionFile_withSummary_1.txt"+ \
      " --dist-version-file2="+unitTestDataDir+"/versionFile_withSummary_1_2.txt"+ \
      " --dist-repos=.,extraTrilinosRepo"+ \
      " log _VERSION2_.._VERSION_")
    cmndOut_expected = \
      "\n*** Base Git Repo: MockTrilinos\n" \
      "['mockgit', 'log', 'sha1_1_2..sha1_1']\n" \
      "\n*** Git Repo: extraTrilinosRepo\n['mockgit', 'log', 'sha1_2_2..sha1_2']\n\n"
    self.assertEqual(s(cmndOut), s(cmndOut_expected))
  # The above test ensures that it repalces the SHA1s for in the same cmndline args


  def test_dist_repo_status_all(self):
    os.chdir(testBaseDir)
    try:

      # Create a mock git meta-project

      testDir = createAndMoveIntoTestDir("gitdist_dist_repo_status_all")

      os.mkdir("ExtraRepo1")
      os.mkdir("ExtraRepo2")

      writeGitMockProgram_base_3_2_1_repo1_22_0_2_repo2_0_0_0()

      cmndOut = GeneralScriptSupport.getCmndOutput(
        gitdistPath + " --dist-no-color --dist-use-git="+mockGitPath \
          +" --dist-repos=.,ExtraRepo1,ExtraRepo2 dist-repo-status",
        workingDir=testDir)
      #print(cmndOut)
      cmndOut_expected = \
        "-----------------------------------------------------------------------------------------\n" \
        "| ID | Repo Dir              | Branch        | Tracking Branch             | C  | M | ? |\n" \
        "|----|-----------------------|---------------|-----------------------------|----|---|---|\n" \
        "|  0 | MockProjectDir (Base) | local_branch0 | origin_repo0/remote_branch0 |  3 | 2 | 1 |\n" \
        "|  1 | ExtraRepo1            | local_branch1 | origin_repo1/remote_branch1 | 22 |   | 1 |\n" \
        "|  2 | ExtraRepo2            | local_branch2 | origin_repo2/remote_branch2 |    |   |   |\n" \
        "-----------------------------------------------------------------------------------------\n" \
        "\n" \
        "(tip: to see a legend, pass in --dist-legend.)\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

    finally:
      os.chdir(testBaseDir)


  def test_dist_repo_versions_table(self):
    os.chdir(testBaseDir)
    try:

      # Create a mock git meta-project

      testDir = createAndMoveIntoTestDir("gitdist_dist_repo_versions_table")

      os.mkdir("ExtraRepo1")
      os.mkdir("ExtraRepo2")

      writeGitMockProgram_dist_repo_versions_table()

      cmndOut = GeneralScriptSupport.getCmndOutput(
        gitdistPath + " --dist-use-git=" + mockGitPath \
          + " --dist-repos=.,ExtraRepo1,ExtraRepo2 dist-repo-versions-table",
        workingDir=testDir)
      #print(cmndOut.decode("ascii"))
      cmndOut_expected = \
        "| Repository     | SHA1    | Commit Date         | Author                 | Summary                                        |\n" \
        "|:-------------- |:-------:|:------------------- |:---------------------- |:---------------------------------------------- |\n" \
        "| MockProjectDir | e2dc488 | 2019-10-23 10:16:07 | user@domain.com        | Merge Pull Request #1234 from user/repo/branch |\n" \
        "| ExtraRepo1     | f671414 | 2019-10-22 11:18:47 | wile.e.coyote@acme.com | Fixed a Bug                                    |\n" \
        "| ExtraRepo2     | 50bbf3e | 2019-10-17 16:32:15 | someone@somwhere.com   | Did Some Work                                  |\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

    finally:
      os.chdir(testBaseDir)


  def test_dist_repo_versions_table_1_change_base(self):
    os.chdir(testBaseDir)
    try:

      # Create a mock git meta-project

      testDir = createAndMoveIntoTestDir(
        "gitdist_dist_repo_versions_table_1_change_base")

      os.mkdir("ExtraRepo1")
      os.mkdir("ExtraRepo2")

      writeGitMockProgram_dist_repo_versions_table_1_change_base()

      cmndOut = GeneralScriptSupport.getCmndOutput(
        gitdistPath + " --dist-use-git=" + mockGitPath \
          + " --dist-repos=.,ExtraRepo1,ExtraRepo2 dist-repo-versions-table"
          + " --dist-mod-only",
        workingDir=testDir)
      #print(cmndOut.decode("ascii"))
      cmndOut_expected = \
        "| Repository     | SHA1    | Commit Date         | Author          | Summary                                        |\n" \
        "|:-------------- |:-------:|:------------------- |:--------------- |:---------------------------------------------- |\n" \
        "| MockProjectDir | e2dc488 | 2019-10-23 10:16:07 | user@domain.com | Merge Pull Request #1234 from user/repo/branch |\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

    finally:
      os.chdir(testBaseDir)


  def test_dist_repo_versions_short_table(self):
    os.chdir(testBaseDir)
    try:

      # Create a mock git meta-project

      testDir = createAndMoveIntoTestDir("gitdist_dist_repo_versions_short_table")

      os.mkdir("ExtraRepo1")
      os.mkdir("ExtraRepo2")

      writeGitMockProgram_dist_repo_versions_table()

      cmndOut = GeneralScriptSupport.getCmndOutput(
        gitdistPath + " --dist-use-git=" + mockGitPath \
          + " --dist-repos=.,ExtraRepo1,ExtraRepo2 dist-repo-versions-table" \
          + " --dist-short",
        workingDir=testDir)
      #print(cmndOut.decode("ascii"))
      cmndOut_expected = \
        "| Repository     | SHA1    |\n" \
        "|:-------------- |:-------:|\n" \
        "| MockProjectDir | e2dc488 |\n" \
        "| ExtraRepo1     | f671414 |\n" \
        "| ExtraRepo2     | 50bbf3e |\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

    finally:
      os.chdir(testBaseDir)


  def test_dist_repo_status_all_utf8(self):
    if sys.version_info < (3,):
      print("Test disabled for Python 2.")
    else:
      os.chdir(testBaseDir)
      try:

        # Create a mock git meta-project

        testDir = createAndMoveIntoTestDir("gitdist_dist_repo_status_all")

        os.mkdir("ExtraRepo1")
        os.mkdir("ExtraRepo2")

        writeGitMockProgram_base_3_2_1_repo1_22_0_2_repo2_0_0_0()

        cmndOut = GeneralScriptSupport.getCmndOutput(
          gitdistPath + " --dist-utf8-output --dist-no-color --dist-use-git=" \
            +mockGitPath \
            +" --dist-repos=.,ExtraRepo1,ExtraRepo2 dist-repo-status",
          workingDir=testDir)
        #print(cmndOut)
        cmndOut_expected = \
          "┌────┬───────────────────────┬───────────────┬─────────────────────────────┬────┬───┬───┐\n" \
          "│ ID │ Repo Dir              │ Branch        │ Tracking Branch             │ C  │ M │ ? │\n" \
          "┝━━━━┿━━━━━━━━━━━━━━━━━━━━━━━┿━━━━━━━━━━━━━━━┿━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┿━━━━┿━━━┿━━━┥\n" \
          "│  0 │ MockProjectDir (Base) │ local_branch0 │ origin_repo0/remote_branch0 │  3 │ 2 │ 1 │\n" \
          "│  1 │ ExtraRepo1            │ local_branch1 │ origin_repo1/remote_branch1 │ 22 │   │ 1 │\n" \
          "│  2 │ ExtraRepo2            │ local_branch2 │ origin_repo2/remote_branch2 │    │   │   │\n" \
          "└────┴───────────────────────┴───────────────┴─────────────────────────────┴────┴───┴───┘\n" \
          "\n" \
          "(tip: to see a legend, pass in --dist-legend.)\n"
        self.assertEqual(s(cmndOut), s(cmndOut_expected))

      finally:
        os.chdir(testBaseDir)


  def test_dist_repo_status_mod_only_first(self):
    os.chdir(testBaseDir)
    try:

      # Create a mock git meta-project

      testDir = createAndMoveIntoTestDir("gitdist_dist_repo_status_mod_only_first")

      os.mkdir("ExtraRepo1")
      os.mkdir("ExtraRepo2")

      writeGitMockProgram_base_3_2_1_repo1_22_0_2_repo2_0_0_0()

      cmndOut = GeneralScriptSupport.getCmndOutput(
        gitdistPath + " --dist-no-color --dist-use-git="+mockGitPath \
          +" --dist-repos=.,ExtraRepo1,ExtraRepo2 --dist-mod-only dist-repo-status",
        workingDir=testDir)
      #print(cmndOut)
      cmndOut_expected = \
        "-----------------------------------------------------------------------------------------\n" \
        "| ID | Repo Dir              | Branch        | Tracking Branch             | C  | M | ? |\n" \
        "|----|-----------------------|---------------|-----------------------------|----|---|---|\n" \
        "|  0 | MockProjectDir (Base) | local_branch0 | origin_repo0/remote_branch0 |  3 | 2 | 1 |\n" \
        "|  1 | ExtraRepo1            | local_branch1 | origin_repo1/remote_branch1 | 22 |   | 1 |\n" \
        "-----------------------------------------------------------------------------------------\n" \
        "\n" \
        "(tip: to see a legend, pass in --dist-legend.)\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

    finally:
      os.chdir(testBaseDir)


  def test_dist_repo_status_mod_only_first_utf8(self):
    if sys.version_info < (3,):
      print("Test disabled for Python 2.")
    else:
      os.chdir(testBaseDir)
      try:

        # Create a mock git meta-project

        testDir = createAndMoveIntoTestDir("gitdist_dist_repo_status_mod_only_first")

        os.mkdir("ExtraRepo1")
        os.mkdir("ExtraRepo2")

        writeGitMockProgram_base_3_2_1_repo1_22_0_2_repo2_0_0_0()

        cmndOut = GeneralScriptSupport.getCmndOutput(
          gitdistPath + " --dist-utf8-output --dist-no-color --dist-use-git=" \
            +mockGitPath \
            +" --dist-repos=.,ExtraRepo1,ExtraRepo2 --dist-mod-only dist-repo-status",
          workingDir=testDir)
        #print(cmndOut)
        cmndOut_expected = \
          "┌────┬───────────────────────┬───────────────┬─────────────────────────────┬────┬───┬───┐\n" \
          "│ ID │ Repo Dir              │ Branch        │ Tracking Branch             │ C  │ M │ ? │\n" \
          "┝━━━━┿━━━━━━━━━━━━━━━━━━━━━━━┿━━━━━━━━━━━━━━━┿━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┿━━━━┿━━━┿━━━┥\n" \
          "│  0 │ MockProjectDir (Base) │ local_branch0 │ origin_repo0/remote_branch0 │  3 │ 2 │ 1 │\n" \
          "│  1 │ ExtraRepo1            │ local_branch1 │ origin_repo1/remote_branch1 │ 22 │   │ 1 │\n" \
          "└────┴───────────────────────┴───────────────┴─────────────────────────────┴────┴───┴───┘\n" \
          "\n" \
          "(tip: to see a legend, pass in --dist-legend.)\n"
        self.assertEqual(s(cmndOut), s(cmndOut_expected))

      finally:
        os.chdir(testBaseDir)


  def test_dist_repo_status_mod_only_first_legend(self):
    os.chdir(testBaseDir)
    try:

      # Create a mock git meta-project

      testDir = createAndMoveIntoTestDir("gitdist_dist_repo_status_mod_only_first_legend")

      os.mkdir("ExtraRepo1")
      os.mkdir("ExtraRepo2")

      writeGitMockProgram_base_3_2_1_repo1_22_0_2_repo2_0_0_0()

      cmndOut = GeneralScriptSupport.getCmndOutput(
        gitdistPath + " --dist-no-color --dist-use-git="+mockGitPath \
          +" --dist-repos=.,ExtraRepo1,ExtraRepo2 --dist-mod-only" \
          +" --dist-legend dist-repo-status",
        workingDir=testDir)
      #print("+++++++++\n" + cmndOut + "+++++++\n")
      cmndOut_expected = \
        "-----------------------------------------------------------------------------------------\n" \
        "| ID | Repo Dir              | Branch        | Tracking Branch             | C  | M | ? |\n" \
        "|----|-----------------------|---------------|-----------------------------|----|---|---|\n" \
        "|  0 | MockProjectDir (Base) | local_branch0 | origin_repo0/remote_branch0 |  3 | 2 | 1 |\n" \
        "|  1 | ExtraRepo1            | local_branch1 | origin_repo1/remote_branch1 | 22 |   | 1 |\n" \
        "-----------------------------------------------------------------------------------------\n" \
        "\n" \
        "Legend:\n" \
        "* ID: Repository ID, zero based (order git commands are run)\n" \
        "* Repo Dir: Relative to base repo (base repo shown first with '(Base)')\n" \
        "* Branch: Current branch (or detached HEAD)\n" \
        "* Tracking Branch: Tracking branch (or empty if no tracking branch exists)\n" \
        "* C: Number local commits w.r.t. tracking branch (empty if zero or no TB)\n" \
        "* M: Number of tracked modified (uncommitted) files (empty if zero)\n" \
        "* ?: Number of untracked, non-ignored files (empty if zero)\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

    finally:
      os.chdir(testBaseDir)


  def test_dist_repo_status_mod_only_first_legend_utf8(self):
    if sys.version_info < (3,):
      print("Test disabled for Python 2.")
    else:
      os.chdir(testBaseDir)
      try:

        # Create a mock git meta-project

        testDir = createAndMoveIntoTestDir("gitdist_dist_repo_status_mod_only_first_legend")

        os.mkdir("ExtraRepo1")
        os.mkdir("ExtraRepo2")

        writeGitMockProgram_base_3_2_1_repo1_22_0_2_repo2_0_0_0()

        cmndOut = GeneralScriptSupport.getCmndOutput(
          gitdistPath + " --dist-utf8-output --dist-no-color --dist-use-git=" \
            +mockGitPath \
            +" --dist-repos=.,ExtraRepo1,ExtraRepo2 --dist-mod-only" \
            +" --dist-legend dist-repo-status",
          workingDir=testDir)
        #print("+++++++++\n" + cmndOut + "+++++++\n")
        cmndOut_expected = \
          "┌────┬───────────────────────┬───────────────┬─────────────────────────────┬────┬───┬───┐\n" \
          "│ ID │ Repo Dir              │ Branch        │ Tracking Branch             │ C  │ M │ ? │\n" \
          "┝━━━━┿━━━━━━━━━━━━━━━━━━━━━━━┿━━━━━━━━━━━━━━━┿━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┿━━━━┿━━━┿━━━┥\n" \
          "│  0 │ MockProjectDir (Base) │ local_branch0 │ origin_repo0/remote_branch0 │  3 │ 2 │ 1 │\n" \
          "│  1 │ ExtraRepo1            │ local_branch1 │ origin_repo1/remote_branch1 │ 22 │   │ 1 │\n" \
          "└────┴───────────────────────┴───────────────┴─────────────────────────────┴────┴───┴───┘\n" \
          "\n" \
          "Legend:\n" \
          "* ID: Repository ID, zero based (order git commands are run)\n" \
          "* Repo Dir: Relative to base repo (base repo shown first with '(Base)')\n" \
          "* Branch: Current branch (or detached HEAD)\n" \
          "* Tracking Branch: Tracking branch (or empty if no tracking branch exists)\n" \
          "* C: Number local commits w.r.t. tracking branch (empty if zero or no TB)\n" \
          "* M: Number of tracked modified (uncommitted) files (empty if zero)\n" \
          "* ?: Number of untracked, non-ignored files (empty if zero)\n\n"
        self.assertEqual(s(cmndOut), s(cmndOut_expected))

      finally:
        os.chdir(testBaseDir)


  def test_dist_repo_status_mod_only_first_last(self):
    os.chdir(testBaseDir)
    try:

      # Create a mock git meta-project

      testDir = createAndMoveIntoTestDir("gitdist_dist_repo_status_mod_only_first_last")

      os.mkdir("ExtraRepo1")
      os.mkdir("ExtraRepo2")

      writeGitMockProgram_base_3_2_1_repo1_0_0_0_repo2_4_0_2()

      cmndOut = GeneralScriptSupport.getCmndOutput(
        gitdistPath + " --dist-no-color --dist-use-git="+mockGitPath \
          +" --dist-repos=.,ExtraRepo1,ExtraRepo2 --dist-mod-only dist-repo-status",
        workingDir=testDir)
      #print(cmndOut)
      cmndOut_expected = \
        "----------------------------------------------------------------------------------------\n" \
        "| ID | Repo Dir              | Branch        | Tracking Branch             | C | M | ? |\n" \
        "|----|-----------------------|---------------|-----------------------------|---|---|---|\n" \
        "|  0 | MockProjectDir (Base) | local_branch0 | origin_repo0/remote_branch0 | 3 | 2 | 1 |\n" \
        "|  2 | ExtraRepo2            | local_branch2 | origin_repo2/remote_branch2 | 4 |   | 2 |\n" \
        "----------------------------------------------------------------------------------------\n" \
        "\n" \
        "(tip: to see a legend, pass in --dist-legend.)\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

    finally:
      os.chdir(testBaseDir)


  def test_dist_repo_status_mod_only_first_last_utf8(self):
    if sys.version_info < (3,):
      print("Test disabled for Python 2.")
    else:
      os.chdir(testBaseDir)
      try:

        # Create a mock git meta-project

        testDir = createAndMoveIntoTestDir("gitdist_dist_repo_status_mod_only_first_last")

        os.mkdir("ExtraRepo1")
        os.mkdir("ExtraRepo2")

        writeGitMockProgram_base_3_2_1_repo1_0_0_0_repo2_4_0_2()

        cmndOut = GeneralScriptSupport.getCmndOutput(
          gitdistPath + " --dist-utf8-output --dist-no-color --dist-use-git=" \
            +mockGitPath \
            +" --dist-repos=.,ExtraRepo1,ExtraRepo2 --dist-mod-only dist-repo-status",
          workingDir=testDir)
        #print(cmndOut)
        cmndOut_expected = \
          "┌────┬───────────────────────┬───────────────┬─────────────────────────────┬───┬───┬───┐\n" \
          "│ ID │ Repo Dir              │ Branch        │ Tracking Branch             │ C │ M │ ? │\n" \
          "┝━━━━┿━━━━━━━━━━━━━━━━━━━━━━━┿━━━━━━━━━━━━━━━┿━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┿━━━┿━━━┿━━━┥\n" \
          "│  0 │ MockProjectDir (Base) │ local_branch0 │ origin_repo0/remote_branch0 │ 3 │ 2 │ 1 │\n" \
          "│  2 │ ExtraRepo2            │ local_branch2 │ origin_repo2/remote_branch2 │ 4 │   │ 2 │\n" \
          "└────┴───────────────────────┴───────────────┴─────────────────────────────┴───┴───┴───┘\n" \
          "\n" \
          "(tip: to see a legend, pass in --dist-legend.)\n"
        self.assertEqual(s(cmndOut), s(cmndOut_expected))

      finally:
        os.chdir(testBaseDir)


  def test_dist_repo_status_extra_args_fail(self):
    os.chdir(testBaseDir)
    try:

      # Create a mock git meta-project

      testDir = createAndMoveIntoTestDir("dist_repo_status_extra_args_fail")

      (cmndOut, errOut) = getCmndOutput(
        gitdistPath + " --dist-no-color --dist-use-git="+mockGitPath \
          +" --dist-repos=.,ExtraRepo1,ExtraRepo2 --dist-mod-only" \
          +" --dist-legend dist-repo-status --name-status",
        rtnCode=True)
      #print(cmndOut)
      cmndOut_expected = \
        "Error, passing in extra git commands/args ='--name-status' with special command 'dist-repo-status' is not allowed!\n"
      self.assertEqual(cmndOut, s(cmndOut_expected))
      self.assertEqual(errOut, 1)

    finally:
      os.chdir(testBaseDir)


  def test_dist_repo_versions_table_extra_args_fail(self):
    os.chdir(testBaseDir)
    try:

      # Create a mock git meta-project

      testDir = createAndMoveIntoTestDir("dist_repo_versions_table_extra_args_fail")

      (cmndOut, errOut) = getCmndOutput(
        gitdistPath + " --dist-no-color --dist-use-git=" + mockGitPath \
          + " --dist-repos=.,ExtraRepo1,ExtraRepo2 dist-repo-versions-table" \
          + " --name-status",
        rtnCode=True)
      #print(cmndOut)
      cmndOut_expected = \
        "Error, passing in extra git commands/args ='--name-status' with special command 'dist-repo-versions-table' is not allowed!\n"
      self.assertEqual(cmndOut, s(cmndOut_expected))
      self.assertEqual(errOut, 1)

    finally:
      os.chdir(testBaseDir)


  def test_dist_default_branch(self):
    os.chdir(testBaseDir)
    try:

      # Create a mock git meta-project

      testDir = createAndMoveIntoTestDir("gitdist_default_branch")
      os.mkdir("ExtraRepo1")
      os.makedirs("Path/To/ExtraRepo2")
      os.mkdir("ExtraRepo3")

      # Make sure .gitdist.default is found and read correctly
      open(".gitdist.default", "w").write(
        ". master\n" \
        "ExtraRepo1 develop\n" \
        "Path/To/ExtraRepo2 app-devel\n" \
        "MissingExtraRepo\n" \
        "ExtraRepo3\n"
        )
      cmndOut = GeneralScriptSupport.getCmndOutput(
        gitdistPathMock+" checkout _DEFAULT_BRANCH_", workingDir=testDir)
      cmndOut_expected = \
        "\n*** Base Git Repo: MockProjectDir\n" \
        "['mockgit', 'checkout', 'master']\n\n" \
        "*** Git Repo: ExtraRepo1\n" \
        "['mockgit', 'checkout', 'develop']\n\n" \
        "*** Git Repo: Path/To/ExtraRepo2\n" \
        "['mockgit', 'checkout', 'app-devel']\n\n" \
        "*** Git Repo: ExtraRepo3\n" \
        "['mockgit', 'checkout', 'master']\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))
      # NOTE: Above ensures that all of the paths are read correctly and that
      # missing paths (MissingExtraRepo) are ignored.

      # Make sure that .gitdist overrides .gitdist.default
      open(".gitdist", "w").write(
        ". develop\n" \
        "ExtraRepo1 develop\n" \
        "ExtraRepo3 develop\n"
        )
      cmndOut = GeneralScriptSupport.getCmndOutput(
        gitdistPathMock+" checkout _DEFAULT_BRANCH_", workingDir=testDir)
      cmndOut_expected = \
        "\n*** Base Git Repo: MockProjectDir\n" \
        "['mockgit', 'checkout', 'develop']\n\n" \
        "*** Git Repo: ExtraRepo1\n" \
        "['mockgit', 'checkout', 'develop']\n\n" \
        "*** Git Repo: ExtraRepo3\n" \
        "['mockgit', 'checkout', 'develop']\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

      # Make sure that --dist-repos overrides all files
      cmndOut = GeneralScriptSupport.getCmndOutput(
        gitdistPathMock+" --dist-repos=.,ExtraRepo1,Path/To/ExtraRepo2 "+ \
        "checkout _DEFAULT_BRANCH_",
        workingDir=testDir)
      cmndOut_expected = \
        "\n*** Base Git Repo: MockProjectDir\n" \
        "['mockgit', 'checkout', 'master']\n\n" \
        "*** Git Repo: ExtraRepo1\n" \
        "['mockgit', 'checkout', 'master']\n\n" \
        "*** Git Repo: Path/To/ExtraRepo2\n" \
        "['mockgit', 'checkout', 'master']\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))
      
    finally:
      os.chdir(testBaseDir)


  def test_gitdist_move_to_base_dir_invalid_env_var(self):
    os.environ["GITDIST_MOVE_TO_BASE_DIR"] = "INVALID"
    cmndOut = getCmndOutput(gitdistPath+" status")
    cmndOut_expected = "Error, env var GITDIST_MOVE_TO_BASE_DIR='INVALID' is invalid!  Valid choices include empty '', IMMEDIATE_BASE, and EXTREME_BASE.\n"
    #print("cmndOut = ", cmndOut)
    #print("cmndOut_expected = ", cmndOut_expected)
    self.assertEqual(s(cmndOut), s(cmndOut_expected))


  def test_gitdist_move_to_base_dir(self):
    os.chdir(testBaseDir)
    try:

      # Create a mock git meta-project.
      testDir = createAndMoveIntoTestDir("gitdist_move_to_base_dir")
      os.makedirs("ExtraRepo/path/to/somewhere")
      open(".gitdist", "w").write(
        ".\n" \
        "ExtraRepo\n"
        )
      open("ExtraRepo/.gitdist", "w").write(
        ".\n"
        )
      os.chdir("ExtraRepo/path/to/somewhere")

      # Test with the default setting.
      os.environ["GITDIST_MOVE_TO_BASE_DIR"] = ""
      cmndOut = GeneralScriptSupport.getCmndOutput(gitdistPathMock+" status")
      cmndOut_expected = \
        "\n*** Base Git Repo: somewhere\n" \
        "['mockgit', 'status']\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

      # Test moving up the directory tree until we find a .gitdist file.
      os.environ["GITDIST_MOVE_TO_BASE_DIR"] = "IMMEDIATE_BASE"
      cmndOut = GeneralScriptSupport.getCmndOutput(gitdistPathMock+" status")
      cmndOut_expected = \
        "\n*** Base Git Repo: ExtraRepo\n" \
        "['mockgit', 'status']\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

      # Test moving up the directory tree until we find the outer-most .gitdist
      # file.
      os.environ["GITDIST_MOVE_TO_BASE_DIR"] = "EXTREME_BASE"
      cmndOut = GeneralScriptSupport.getCmndOutput(gitdistPathMock+" status")
      cmndOut_expected = \
        "\n*** Base Git Repo: MockProjectDir\n" \
        "['mockgit', 'status']\n\n" \
        "*** Git Repo: ExtraRepo\n" \
        "['mockgit', 'status']\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))
      
      # Rename the .gitdist files .gitdist.default, and try the tests again.
      os.rename("../../../.gitdist", "../../../.gitdist.default")
      os.rename("../../../../.gitdist", "../../../../.gitdist.default")

      # Test with the default setting.
      os.environ["GITDIST_MOVE_TO_BASE_DIR"] = ""
      cmndOut = GeneralScriptSupport.getCmndOutput(gitdistPathMock+" status")
      cmndOut_expected = \
        "\n*** Base Git Repo: somewhere\n" \
        "['mockgit', 'status']\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

      # Test moving up the directory tree until we find a .gitdist file.
      os.environ["GITDIST_MOVE_TO_BASE_DIR"] = "IMMEDIATE_BASE"
      cmndOut = GeneralScriptSupport.getCmndOutput(gitdistPathMock+" status")
      cmndOut_expected = \
        "\n*** Base Git Repo: ExtraRepo\n" \
        "['mockgit', 'status']\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

      # Test moving up the directory tree until we find the outer-most .gitdist
      # file.
      os.environ["GITDIST_MOVE_TO_BASE_DIR"] = "EXTREME_BASE"
      cmndOut = GeneralScriptSupport.getCmndOutput(gitdistPathMock+" status")
      cmndOut_expected = \
        "\n*** Base Git Repo: MockProjectDir\n" \
        "['mockgit', 'status']\n\n" \
        "*** Git Repo: ExtraRepo\n" \
        "['mockgit', 'status']\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))
      
    finally:
      os.chdir(testBaseDir)


  def test_gitdist_move_to_base_dir_no_dist_gitdist_file(self):
    os.chdir(testBaseDir)
    try:

      # Create a mock git meta-project but with no .gitdist[.default] files!
      testDir = createAndMoveIntoTestDir("gitdist_move_to_base_dir_no_dist_gitdist_file")
      os.makedirs(".git")
      os.makedirs("ExtraRepo/.git")
      os.makedirs("ExtraRepo/path/to/somewhere")
      os.chdir("ExtraRepo/path/to/somewhere")

      os.environ["GITDIST_MOVE_TO_BASE_DIR"] = ""
      cmndOut = GeneralScriptSupport.getCmndOutput(gitdistPathMock+" status")
      cmndOut_expected = \
        "\n*** Base Git Repo: somewhere\n" \
        "['mockgit', 'status']\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

      os.environ["GITDIST_MOVE_TO_BASE_DIR"] = "IMMEDIATE_BASE"
      cmndOut = GeneralScriptSupport.getCmndOutput(gitdistPathMock+" status")
      cmndOut_expected = \
        "\n*** Base Git Repo: somewhere\n" \
        "['mockgit', 'status']\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

      os.environ["GITDIST_MOVE_TO_BASE_DIR"] = "EXTREME_BASE"
      cmndOut = GeneralScriptSupport.getCmndOutput(gitdistPathMock+" status")
      cmndOut_expected = \
        "\n*** Base Git Repo: somewhere\n" \
        "['mockgit', 'status']\n\n"
      self.assertEqual(s(cmndOut), s(cmndOut_expected))

    finally:
      os.chdir(testBaseDir)


if __name__ == '__main__':
  unittest.main()
