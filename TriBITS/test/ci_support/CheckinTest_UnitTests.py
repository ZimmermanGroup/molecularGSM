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
# Unit testing code for CheckinTest.py #
########################################

import os
import sys

from FindCISupportDir import *
from CheckinTest import *

import unittest

g_testBaseDir = getScriptBaseDir()

tribitsBaseDir=os.path.abspath(g_testBaseDir+"/../../tribits")
mockProjectBaseDir=os.path.abspath(tribitsBaseDir+"/examples/MockTrilinos")

#####################################################
#
# Testing helper code
#
#####################################################


class MockOptions:
  def __init__(self):
    self.projectName = "Trilinos"
    self.srcDir = mockProjectBaseDir
    self.tribitsDir = tribitsBaseDir 
    self.enableAllPackages = 'auto'
    self.extraReposFile = ""
    self.extraReposType = ""
    self.extraRepos = ""
    self.ignoreMissingExtraRepos = ""
    self.withCmake = "cmake"


def assertFileExists(testObject, filePath):
  testObject.assertEqual(os.path.isfile(filePath), True,
    "Error, the file '" + filePath + "' does not exist!")


def assertFileNotExists(testObject, filePath):
  testObject.assertEqual(os.path.isfile(filePath), False,
    "Error, the file '" + filePath + "' exists!")


def assertGrepFileForRegexStrList(testObject, testName, fileName, regexStrList, verbose):
  testObject.assertEqual(os.path.isfile(fileName), True,
    "Error, the file '" + fileName + "' does not exist!")
  for regexToFind in regexStrList.strip().split('\n'):
    if regexToFind == "": continue
    cmnd = "grep '" + regexToFind + "' " + fileName
    #print(cmnd)
    foundRegex = s(getCmndOutput(cmnd, True, False))
    msg = "\n" + testName + ": In '" + fileName + "' look for regex '" + \
          regexToFind + "' ..." + "'" + foundRegex + "': " 
    if foundRegex: msg += "PASSED"
    else: msg += "FAILED"
    if verbose:
      print(msg)
    testObject.assertNotEqual(foundRegex, "", msg)


def assertNotGrepFileForRegexStrList(testObject, testName, fileName, regexStrList, verbose):
  assert(os.path.isfile(fileName))
  for regexToFind in regexStrList.strip().split('\n'):
    if regexToFind == "": continue
    foundRegex = s(getCmndOutput("grep '" + regexToFind + "' " + fileName, True,
                                 False))
    if verbose or foundRegex:
      msg = "\n" + testName + ": In '" + fileName + "' assert not exist regex '" \
            + regexToFind + "' ... '" + foundRegex + "': "
      if foundRegex: msg += "FAILED"
      else: msg += "PASSED"
      print(msg)
    testObject.assertEqual(foundRegex, "")


#############################################################################
#
# Test trimLineToLen()
#
#############################################################################


class test_trimLineToLen(unittest.TestCase):

  def test_underNumChars(self):
    self.assertEqual(trimLineToLen("something", 10), "something")

  def test_equalNumChars(self):
    self.assertEqual(trimLineToLen("something", 9), "something")

  def test_over1NumChars(self):
    self.assertEqual(trimLineToLen("something", 8), "somethin..")

  def test_over2NumChars(self):
    self.assertEqual(trimLineToLen("something", 7), "somethi..")



#############################################################################
#
# Test formatMinutesStr()
#
#############################################################################


class test_formatMinutesStr(unittest.TestCase):

  def test_00(self):
    self.assertEqual(formatMinutesStr(0.000000), "0.00 min")

  def test_01(self):
    self.assertEqual(formatMinutesStr(1245.244678), "1245.24 min")

  def test_02(self):
    self.assertEqual(formatMinutesStr(1245.245678), "1245.25 min")

  def test_03(self):
    self.assertEqual(formatMinutesStr(1.245678), "1.25 min")

  def test_04(self):
    self.assertEqual(formatMinutesStr(0.202), "0.20 min")

  def test_05(self):
    self.assertEqual(formatMinutesStr(0.204), "0.20 min")

  def test_06(self):
    self.assertEqual(formatMinutesStr(0.2053333), "0.21 min")

  def test_07(self):
    self.assertEqual(formatMinutesStr(0.2943333), "0.29 min")

  def test_08(self):
    self.assertEqual(formatMinutesStr(0.2993333), "0.30 min")

  def test_09(self):
    self.assertEqual(formatMinutesStr(45.2993333), "45.30 min")

  def test_10(self):
    self.assertEqual(formatMinutesStr(45.2493333), "45.25 min")


#############################################################################
#
# Test getTimeInMinFromTotalTimeLine()
#
#############################################################################


class test_getTimeInMinFromTotalTimeLine(unittest.TestCase):

  def test_None(self):
    self.assertEqual(
      getTimeInMinFromTotalTimeLine(
         "MPI_DEBUG", None),
      -1.0)

  def test_Empty(self):
    self.assertEqual(
      getTimeInMinFromTotalTimeLine(
         "MPI_DEBUG", ""),
      -1.0)

  def test_00(self):
    self.assertEqual(
      getTimeInMinFromTotalTimeLine(
         "MPI_DEBUG", "Total time for MPI_DEBUG = 1.16723643541 min"),
      1.16723643541)


#############################################################################
#
# Test extractPackageEnablesFromChangeStatus()
#
#############################################################################


projectDepsXmlFileDefaultOverride=g_testBaseDir+"/TrilinosPackageDependencies.gold.xml"
projectDependenciesDefault = getProjectDependenciesFromXmlFile(projectDepsXmlFileDefaultOverride)


class test_extractPackageEnablesFromChangeStatus(unittest.TestCase):


  def test_enable_all_and_other_packages(self):

    updateOutputStr = """
M	CMakeLists.txt
M	cmake/TrilinosPackages.cmake
M	cmake/python/checkin-test.py
M	doc/Thyra/coding_guildlines/ThyraCodingGuideLines.tex
P	packages/thyra/dummy.blah
A	packages/teuchos/example/ExplicitInstantiation/four_files/CMakeLists.txt
"""

    options = MockOptions()
    enablePackagesList = []

    extractPackageEnablesFromChangeStatus(updateOutputStr, options, GitRepo(""),
      enablePackagesList, False, projectDependenciesDefault)

    self.assertEqual( options.enableAllPackages, 'on' )
    self.assertEqual( enablePackagesList, [u'TrilinosFramework', u'Teuchos'] )


  def test_some_packages(self):

    updateOutputStr = """
? packages/triutils/doc/html
M	cmake/python/checkin-test.py
M	cmake/python/dump-cdash-deps-xml-file.py
A	packages/stratimikos/src/dummy.C
P       packages/stratimikos/dummy.blah
M	packages/thyra/src/Thyra_ConfigDefs.hpp
D	packages/tpetra/FSeconds.f
"""

    options = MockOptions()
    enablePackagesList = []

    extractPackageEnablesFromChangeStatus(updateOutputStr, options, GitRepo(""),
      enablePackagesList, False, projectDependenciesDefault)

    self.assertEqual( options.enableAllPackages, 'auto' )
    self.assertEqual( enablePackagesList, [u'TrilinosFramework', u'Stratimikos', u'ThyraCoreLibs', u'Tpetra'] )


  def test_extra_repo(self):

    updateOutputStr = """
M	CMakeLists.txt
M	ExtraTrilinosPackages.cmake
M	stalix/README
"""
    # NOTE: Above, we ignore top-level changes in extra repos which would cause global rebuilds
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    projectDependenciesLocal = getProjectDependenciesFromXmlFile(projectDepsXmlFileOverride)

    options = MockOptions()
    enablePackagesList = []

    extractPackageEnablesFromChangeStatus(updateOutputStr, options,
      GitRepo("preCopyrightTrilinos"),
      enablePackagesList, False, projectDependenciesLocal)

    self.assertEqual( options.enableAllPackages, 'auto' )
    self.assertEqual( enablePackagesList, [u'Stalix'] )



#############################################################################
#
#         Test getLastCommitMessageStrFromRawCommitLogStr
#
#############################################################################


class test_getLastCommitMessageStrFromRawCommitLogStr(unittest.TestCase):


  def test_clean_commit(self):
    cleanCommitMsg_expected = \
"""Some Commit Message

Some commit body

Some other message
"""
    rawLogOutput = "Standard git header stuff\n\n"+cleanCommitMsg_expected
    (cleanCommitMsg, numBlankLines) = getLastCommitMessageStrFromRawCommitLogStr(rawLogOutput)
    self.assertEqual(numBlankLines, -1)
    self.assertEqual(cleanCommitMsg, cleanCommitMsg_expected)


  def test_dirty_commit_1(self):
    cleanCommitMsg_expected = \
"""Some Commit Message

Some commit body

Some other message
"""
    rawLogOutput = \
       "Standard git header stuff\n\n" \
       +cleanCommitMsg_expected+ \
       "\nBuild/Test Cases Summary\n"
    #print("\nrawLogOutput:\n----------------\n", rawLogOutput, "----------------\n")
    (cleanCommitMsg, numBlankLines) = getLastCommitMessageStrFromRawCommitLogStr(rawLogOutput)
    #print("\ncleanCommitMsg:\n----------------\n", cleanCommitMsg, "-----------------\n")
    self.assertEqual(numBlankLines, 1)
    self.assertEqual(cleanCommitMsg, cleanCommitMsg_expected)


  def test_dirty_commit_2(self):
    cleanCommitMsg_expected = \
"""Some Commit Message

Some commit body

Some other message
"""
    rawLogOutput = \
       "Standard git header stuff\n\n" \
       +cleanCommitMsg_expected+ \
       "\nBuild/Test Cases Summary\n"
    #print("\nrawLogOutput:\n----------------\n", rawLogOutput, "----------------\n")
    (cleanCommitMsg, numBlankLines) = getLastCommitMessageStrFromRawCommitLogStr(rawLogOutput)
    self.assertEqual(numBlankLines, 1)
    self.assertEqual(cleanCommitMsg, cleanCommitMsg_expected)
    #print("\ncleanCommitMsg:\n----------------\n", cleanCommitMsg, "-----------------\n")


  def test_invalid_commit(self):
    cleanCommitMsg_expected = \
"""Some Commit Message

Some commit body

Some other message"""
    rawLogOutput = \
       "Standard git header stuff\n\n" \
       +cleanCommitMsg_expected+ \
       "\nBuild/Test Cases Summary\n"
    #print("\nrawLogOutput:\n----------------\n", rawLogOutput, "----------------\n")
    #(cleanCommitMsg, numBlankLines) = getLastCommitMessageStrFromRawCommitLogStr(rawLogOutput)
    self.assertRaises(Exception, getLastCommitMessageStrFromRawCommitLogStr, rawLogOutput)


  def test_two_summary_blocks(self):
    cleanCommitMsg_expected = \
"""Some Commit Message

Some commit body

Some other message
"""
    rawLogOutput = \
       "Standard git header stuff\n\n" \
       +cleanCommitMsg_expected+ \
       "\nBuild/Test Cases Summary\n"
    (cleanCommitMsg, numBlankLines) = getLastCommitMessageStrFromRawCommitLogStr(rawLogOutput)
    self.assertEqual(numBlankLines, 1)
    self.assertEqual(cleanCommitMsg, cleanCommitMsg_expected)
    # Strip it again to make sure we can pull it off again and recover
    rawLogOutput = \
       "Standard git header stuff\n\n" \
       +cleanCommitMsg+ \
       "\nBuild/Test Cases Summary\n"
    (cleanCommitMsg, numBlankLines) = getLastCommitMessageStrFromRawCommitLogStr(rawLogOutput)
    self.assertEqual(numBlankLines, 1)
    self.assertEqual(cleanCommitMsg, cleanCommitMsg_expected)


################################################################################
#
# Test Project name matching.
#
################################################################################

class test_matchProjectName(unittest.TestCase):
  def test_good_match(self):
    line = 'SET(PROJECT_NAME TestProject)'
    match = matchProjectName(line)
    self.assertEqual(match, 'TestProject')
    
  def test_match_with_extra_spaces(self):
    line = '  set ( PROJECT_NAME   TestProject ) '
    match = matchProjectName(line)
    self.assertEqual(match, 'TestProject')

  def test_no_match_wrong_variable(self):
    line = 'SET(SOME_VAR TestProject)'
    match = matchProjectName(line)
    self.assertFalse(match)

  def test_match_with_comment_at_end(self):
    line = 'Set(PROJECT_NAME TestProject) # This is a comment'
    match = matchProjectName(line)
    self.assertEqual(match, 'TestProject')


#############################################################################
#
# Test CMake helpers
#
#############################################################################

class test_cmakeScopedDefine(unittest.TestCase):
  def test_simple(self):
    result = cmakeScopedDefine('ProjectName', 'SOME_FLAG:BOOL', 'ON')
    self.assertEqual(result, '-DProjectName_SOME_FLAG:BOOL=ON')


#############################################################################
#
# Test TribitsGetExtraReposForCheckinTest.cmake 
#
#############################################################################


#run_extrarepo_test_verbose = True
run_extrarepo_test_verbose = False


def run_extrarepo_test(testObject, testName, extraReposFile, expectedReposList, \
  extraCmakeVars=None, expectedErrOutput=None \
  ):
  extraReposPythonOutFile = os.getcwd()+"/"+testName+".py"
  global g_withCmake
  cmnd = "\""+g_withCmake+"\""+ \
    " -DPROJECT_SOURCE_DIR="+mockProjectBaseDir+ \
    " -DTRIBITS_BASE_DIR="+tribitsBaseDir
  if extraCmakeVars:
    cmnd += " "+extraCmakeVars
  cmnd += \
    " -DEXTRA_REPOS_FILE="+os.path.join(g_testBaseDir,extraReposFile)+ \
    " -DEXTRA_REPOS_PYTHON_OUT_FILE="+extraReposPythonOutFile+ \
    " -DUNITTEST_SKIP_FILTER_OR_ASSERT_EXTRA_REPOS=TRUE"+ \
    " -DTRIBITS_PROCESS_EXTRAREPOS_LISTS_DEBUG=TRUE"+ \
    " -P "+tribitsBaseDir+"/ci_support/TribitsGetExtraReposForCheckinTest.cmake"
  consoleOutFile = testName+".out"
  rtn = echoRunSysCmnd(cmnd, throwExcept=False, timeCmnd=True, outFile=consoleOutFile,
    verbose=run_extrarepo_test_verbose)
  consoleOutputStr = readStrFromFile(consoleOutFile)
  if run_extrarepo_test_verbose:
    print("\nrtn =", rtn)
    print("\n" + consoleOutFile + ":\n", consoleOutputStr)
  if rtn == 0:
    readReposListTxt = readStrFromFile(extraReposPythonOutFile)
    if run_extrarepo_test_verbose:
      print("\nreadReposListTxt:\n", readReposListTxt)
    readReposList = eval(readReposListTxt)
    if run_extrarepo_test_verbose:
      print("readReposList:\n", readReposList)
    testObject.assertEqual(readReposList, expectedReposList)
  else:
    if run_extrarepo_test_verbose:
      print("\nexpectedErrOutput =", expectedErrOutput)
    foundExpectedErrOutput = consoleOutputStr.find(expectedErrOutput)
    if foundExpectedErrOutput == -1:
      print("Error, failed to find:\n\n", expectedErrOutput)
      print("\n\nin the output:\n\n", consoleOutputStr)
    testObject.assertNotEqual(foundExpectedErrOutput, -1)


class test_TribitsGetExtraReposForCheckinTest(unittest.TestCase):

  def test_ExtraRepos1_implicit(self):
    run_extrarepo_test(
      self,
      "test_ExtraRepos1_implicit",
      "ExtraReposList_1.cmake",
      [
        {
          'NAME' : 'ExtraRepo1',
          'DIR' : 'ExtraRepo1',
          'REPOTYPE' : 'GIT',
          'REPOURL' : 'someurl.com:/git/data/SomeExtraRepo1',
          'HASPKGS' : 'HASPACKAGES',
          'PREPOST' : 'POST',
          'CATEGORY' : 'Continuous',
          },
        ],
      )

  def test_ExtraRepos1_explicit(self):
    run_extrarepo_test(
      self,
      "test_ExtraRepos1_explicit",
      "ExtraReposList_1.cmake",
      [
        {
          'NAME' : 'ExtraRepo1',
          'DIR' : 'ExtraRepo1',
          'REPOTYPE' : 'GIT',
          'REPOURL' : 'someurl.com:/git/data/SomeExtraRepo1',
          'HASPKGS' : 'HASPACKAGES',
          'PREPOST' : 'POST',
          'CATEGORY' : 'Continuous',
          },
        ],
      extraCmakeVars="-DENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Continuous"
      )

  def test_ExtraReposAll(self):
    run_extrarepo_test(
      self,
      "test_ExtraReposAll",
      "ExtraReposList.cmake",
      [
        {
          'NAME' : 'ExtraRepo1',
          'DIR' : 'ExtraRepo1',
          'REPOTYPE' : 'GIT',
          'REPOURL' : 'someurl.com:/ExtraRepo1',
          'HASPKGS' : 'HASPACKAGES',
          'PREPOST' : 'POST',
          'CATEGORY' : 'Continuous',
          },
        {
          'NAME' : 'ExtraRepo2',
          'DIR' : 'packages/SomePackage/Blah',
          'REPOTYPE' : 'GIT',
          'REPOURL' : 'someurl2.com:/ExtraRepo2',
          'HASPKGS' : 'NOPACKAGES',
          'PREPOST' : 'POST',
          'CATEGORY' : 'Nightly',
          },
        {
          'NAME' : 'ExtraRepo3',
          'DIR' : 'ExtraRepo3',
          'REPOTYPE' : 'HG',
          'REPOURL' : 'someurl3.com:/ExtraRepo3',
          'HASPKGS' : 'HASPACKAGES',
          'PREPOST' : 'POST',
          'CATEGORY' : 'Continuous',
          },
        {
          'NAME' : 'ExtraRepo4',
          'DIR' : 'ExtraRepo4',
          'REPOTYPE' : 'SVN',
          'REPOURL' : 'someurl4.com:/ExtraRepo4',
          'HASPKGS' : 'HASPACKAGES',
          'PREPOST' : 'POST',
          'CATEGORY' : 'Nightly',
          },
        ],
      extraCmakeVars="-DENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Nightly"
      )

  def test_ExtraReposContinuous(self):
    run_extrarepo_test(
      self,
      "test_ExtraReposContinuous",
      "ExtraReposList.cmake",
      [
        {
          'NAME' : 'ExtraRepo1',
          'DIR' : 'ExtraRepo1',
          'REPOTYPE' : 'GIT',
          'REPOURL' : 'someurl.com:/ExtraRepo1',
          'HASPKGS' : 'HASPACKAGES',
          'PREPOST' : 'POST',
          'CATEGORY' : 'Continuous',
          },
        {
          'NAME' : 'ExtraRepo3',
          'DIR' : 'ExtraRepo3',
          'REPOTYPE' : 'HG',
          'REPOURL' : 'someurl3.com:/ExtraRepo3',
          'HASPKGS' : 'HASPACKAGES',
          'PREPOST' : 'POST',
          'CATEGORY' : 'Continuous',
          },
        ],
      )

  def test_ExtraReposInvalidCategory(self):
    run_extrarepo_test(
      self,
      "test_ExtraReposInvalidCategory",
      "ExtraReposListInvalidCategory.cmake",
      [],
      )

  def test_ExtraReposInvalidType(self):
    run_extrarepo_test(
      self,
      "test_ExtraReposInvalidType",
      "ExtraReposListInvalidType.cmake",
      ["Will never get compared"],
      expectedErrOutput="Error, the repo type of 'InvalidType' for extra repo ExtraRepo1"
      )

  def test_ExtraReposEmptyList(self):
    run_extrarepo_test(
      self,
      "test_ExtraReposEmptyList",
      "ExtraReposListEmptyList.cmake",
      ["Will never get compared"],
      expectedErrOutput="Trilinos_EXTRAREPOS_DIR_VCTYPE_REPOURL_PACKSTAT_CATEGORY is not defined!",
      )

  def test_ExtraReposEmptyFile(self):
    run_extrarepo_test(
      self,
      "test_ExtraReposEmptyFile",
      "ExtraReposListEmptyFile.cmake",
      ["Will never get compared"],
      expectedErrOutput="Trilinos_EXTRAREPOS_DIR_VCTYPE_REPOURL_PACKSTAT_CATEGORY is not defined!",
      )


#############################################################################
#
# Test TribitsGitRepos
#
#############################################################################


def assertCompareGitRepoLists(testObject, gitRepoList, expectedGitRepoList):
  testObject.assertEqual(len(gitRepoList), len(expectedGitRepoList))
  for i in range(len(gitRepoList)):
    gitRepo = gitRepoList[i]
    expectedGitRepo = expectedGitRepoList[i]
    testObject.assertEqual(gitRepo.repoName, expectedGitRepo.repoName)
    testObject.assertEqual(gitRepo.repoDir, expectedGitRepo.repoDir)
    testObject.assertEqual(gitRepo.repoType, expectedGitRepo.repoType)
    testObject.assertEqual(gitRepo.repoHasPackages, expectedGitRepo.repoHasPackages)
    testObject.assertEqual(gitRepo.hasChanges, expectedGitRepo.hasChanges)


def assertCompareTribitGitRepos(testObject, tribitsGitRepo, expectedTribitsGitRepo):
  assertCompareGitRepoLists(testObject, tribitsGitRepo.gitRepoList(),
     expectedTribitsGitRepo.gitRepoList())
  testObject.assertEqual(tribitsGitRepo.tribitsExtraRepoNamesList(),
    expectedTribitsGitRepo.tribitsExtraRepoNamesList())


#test_TribitsGitRepos_verbose = True
test_TribitsGitRepos_verbose = False


def test_TribitsGitRepos_run_case(testObject, testName, inOptions, \
  expectPass, \
  expectedTribitsExtraRepoNamesList, expectedGitRepos, \
  consoleRegexMatches=None, consoleRegexNotMatches=None, \
  exceptionRegexMatches=None \
  ):
  inOptions.withCmake = g_withCmake
  currDir = os.getcwd()
  if os.path.exists(testName):
    runSysCmnd("rm -rf "+testName)
  os.mkdir(testName)
  os.chdir(testName)
  try:
    consoleOutputFile = "Console.out" 
    tribitsGitRepos = TribitsGitRepos()
    cmndPassed = False
    try:
      tribitsGitRepos.initFromCommandlineArguments(inOptions, \
        consoleOutputFile=consoleOutputFile, \
        verbose=test_TribitsGitRepos_verbose)
      cmndPassed = True
      # NOTE: the file consoleOutputFile still gets written, even if throw
    except Exception as e:
      #print("e =", e)
      if exceptionRegexMatches:
        eMsg = e.args[0]
        for exceptRegex in exceptionRegexMatches.split('\n'):
          matchResult = re.search(exceptRegex, eMsg)
          if not matchResult:
            print("Error, the regex expression '" + exceptRegex + "' was not" +
                  " found in the exception string '" + eMsg + "'!")
          testObject.assertNotEqual(matchResult, None)
    testObject.assertEqual(cmndPassed, expectPass)
    if cmndPassed:
      #print("\ntribitsGitRepos =", tribitsGitRepos)
      testObject.assertEqual(tribitsGitRepos.numTribitsExtraRepos(), len(expectedTribitsExtraRepoNamesList))
      expectedTribitsGitRepo = TribitsGitRepos().reset()
      expectedTribitsGitRepo._TribitsGitRepos__gitRepoList.extend(expectedGitRepos)
      testObject.assertEqual(tribitsGitRepos.tribitsExtraRepoNamesList(), expectedTribitsExtraRepoNamesList)
      expectedTribitsGitRepo._TribitsGitRepos__tribitsExtraRepoNamesList.extend(expectedTribitsExtraRepoNamesList)
      assertCompareTribitGitRepos(testObject, tribitsGitRepos, expectedTribitsGitRepo)
    if consoleRegexMatches:
      assertGrepFileForRegexStrList(testObject, testName, consoleOutputFile,
        consoleRegexMatches, test_TribitsGitRepos_verbose)
    if consoleRegexNotMatches:
      assertNotGrepFileForRegexStrList(testObject, testName, consoleOutputFile,
        consoleRegexNotMatches, test_TribitsGitRepos_verbose)
  finally:
    os.chdir(currDir)


class test_TribitsGitRepos(unittest.TestCase):

  def test_noExtraRepos(self):
    tribitsGitRepos = TribitsGitRepos()
    expectedTribitsGitRepo = TribitsGitRepos().reset()
    expectedTribitsGitRepo._TribitsGitRepos__gitRepoList.append(GitRepo("", "", "GIT", True))
    self.assertEqual(tribitsGitRepos.tribitsExtraRepoNamesList(), [])
    self.assertEqual(tribitsGitRepos.numTribitsExtraRepos(), 0)
    assertCompareTribitGitRepos(self, tribitsGitRepos, expectedTribitsGitRepo)

  def test_noExtraReposFile_extraRepos(self):
    testName = "test_noExtraReposFile_extraRepos"
    inOptions = MockOptions()
    inOptions.extraRepos = "preCopyrightTrilinos"
    inOptions.extraReposFile = ""
    expectedTribitsExtraRepoNamesList = ["preCopyrightTrilinos"]
    expectedGitRepos = [
      GitRepo("", "", "GIT", True),
      GitRepo('preCopyrightTrilinos', 'preCopyrightTrilinos', "GIT", True),
      ]
    consoleRegexMatches = None
    consoleRegexNotMatches = None
    test_TribitsGitRepos_run_case(self, testName, inOptions, True, \
      expectedTribitsExtraRepoNamesList, expectedGitRepos, \
      consoleRegexMatches, consoleRegexNotMatches)

  def test_ExtraRepos3_Continuous(self):
    testName = "test_ExtraRepos3_Continuous"
    inOptions = MockOptions()
    inOptions.extraReposFile = \
      g_testBaseDir+"/ExtraReposListExisting_3.cmake"
    inOptions.extraReposType = "Continuous"
    expectedTribitsExtraRepoNamesList = ["preCopyrightTrilinos"]
    expectedGitRepos = [
      GitRepo("", "", "GIT", True),
      GitRepo('preCopyrightTrilinos', 'preCopyrightTrilinos', "GIT", True),
      GitRepo('ExtraTeuchosRepo', 'packages/teuchos/extrastuff', 'GIT', False)
      ]
    consoleRegexMatches = \
      "Adding POST extra Continuous repository preCopyrightTrilinos\n"+\
      "Adding POST extra Continuous repository ExtraTeuchosRepo\n"
    consoleRegexNotMatches = \
      "Adding POST extra Nightly repository extraTrilinosRepo\n"
    test_TribitsGitRepos_run_case(self, testName, inOptions, True, \
      expectedTribitsExtraRepoNamesList, expectedGitRepos, \
      consoleRegexMatches, consoleRegexNotMatches)

  def test_ExtraRepos3_Nightly(self):
    testName = "test_ExtraRepos3_Nightly"
    inOptions = MockOptions()
    inOptions.extraReposFile = \
      g_testBaseDir+"/ExtraReposListExisting_3.cmake"
    inOptions.extraReposType = "Nightly"
    expectedPass = True
    expectedTribitsExtraRepoNamesList = ["preCopyrightTrilinos", "extraTrilinosRepo"]
    expectedGitRepos = [
      GitRepo("", "", "GIT", True),
      GitRepo('preCopyrightTrilinos', 'preCopyrightTrilinos', "GIT", True),
      GitRepo('extraTrilinosRepo', 'extraTrilinosRepo', "GIT", True),
      GitRepo('ExtraTeuchosRepo', 'packages/teuchos/extrastuff', 'GIT', False)
      ]
    consoleRegexMatches = \
      "Adding POST extra Continuous repository preCopyrightTrilinos\n"+\
      "Adding POST extra Continuous repository ExtraTeuchosRepo\n"+\
      "Adding POST extra Nightly repository extraTrilinosRepo\n"
    consoleRegexNotMatches = None
    test_TribitsGitRepos_run_case(self, testName, inOptions, expectedPass, \
      expectedTribitsExtraRepoNamesList, expectedGitRepos, \
      consoleRegexMatches, consoleRegexNotMatches)

  def test_ExtraReposExisting1Missing1_assert(self):
    testName = "test_ExtraReposExisting1Missing1_assert"
    inOptions = MockOptions()
    inOptions.extraReposFile = \
      g_testBaseDir+"/ExtraReposListExisting1Missing1.cmake"
    inOptions.extraReposType = "Nightly"
    expectedPass = False
    expectedTribitsExtraRepoNamesList = ["Will never be compared"]
    expectedGitRepos = ["Will never be compared."]
    consoleRegexMatches = \
      "ERROR! Skipping missing extra repo .MissingRepo. since\n"
    consoleRegexNotMatches = \
      "Adding POST extra Continuous repository MissingRepo\n"
    test_TribitsGitRepos_run_case(self, testName, inOptions, expectedPass, \
      expectedTribitsExtraRepoNamesList, expectedGitRepos, \
    consoleRegexMatches, consoleRegexNotMatches)

  def test_ExtraReposExisting1Missing1_ignore(self):
    testName = "test_ExtraReposExisting1Missing1_ignore"
    inOptions = MockOptions()
    inOptions.extraReposFile = \
      g_testBaseDir+"/ExtraReposListExisting1Missing1.cmake"
    inOptions.extraReposType = "Nightly"
    inOptions.ignoreMissingExtraRepos = True
    expectedPass = True
    expectedTribitsExtraRepoNamesList = ["preCopyrightTrilinos"]
    expectedGitRepos = [
      GitRepo("", "", "GIT", True),
      GitRepo('preCopyrightTrilinos', 'preCopyrightTrilinos', "GIT", True),
      ]
    consoleRegexMatches = \
      "NOTE: Ignoring missing extra repo .MissingRepo. as requested since\n"
    consoleRegexNotMatches = \
      "Adding POST extra Continuous repository MissingRepo"
    test_TribitsGitRepos_run_case(self, testName, inOptions, expectedPass, \
      expectedTribitsExtraRepoNamesList, expectedGitRepos, \
    consoleRegexMatches, consoleRegexNotMatches)

  def test_ExtraRepos3_listExtraReposNotListed1(self):
    testName = "test_ExtraRepos3_listExtraReposNotListed1"
    inOptions = MockOptions()
    inOptions.extraReposFile = \
      g_testBaseDir+"/ExtraReposListExisting_3.cmake"
    inOptions.extraRepos = "extraRepoNotInList"
    inOptions.extraReposType = "Nightly"
    expectedPass = False
    expectedTribitsExtraRepoNamesList = ["Will never be compared"]
    expectedGitRepos = ["Will never be compared"]
    consoleRegexMatches = \
      "ERROR! The list of extra repos passed in .extraRepoNotInList. is not\n"
    consoleRegexNotMatches = None
    test_TribitsGitRepos_run_case(self, testName, inOptions, expectedPass, \
      expectedTribitsExtraRepoNamesList, expectedGitRepos, \
      consoleRegexMatches, consoleRegexNotMatches)

  def test_ExtraRepos3_listExtraReposNotListed2(self):
    testName = "test_ExtraRepos3_listExtraReposNotListed2"
    inOptions = MockOptions()
    inOptions.extraReposFile = \
      g_testBaseDir+"/ExtraReposListExisting_3.cmake"
    inOptions.extraRepos = "preCopyrightTrilinos,extraRepoNotInList"
    inOptions.extraReposType = "Nightly"
    expectedPass = False
    expectedTribitsExtraRepoNamesList = ["Will never be compared"]
    expectedGitRepos = ["Will never be compared"]
    consoleRegexMatches = \
      "ERROR! The list of extra repos passed in\n"+\
     ".preCopyrightTrilinos.extraRepoNotInList. is not a subset and in the same\n"
    consoleRegexNotMatches = None
    test_TribitsGitRepos_run_case(self, testName, inOptions, expectedPass, \
      expectedTribitsExtraRepoNamesList, expectedGitRepos, \
      consoleRegexMatches, consoleRegexNotMatches)

  def test_ExtraRepos3_extraReposFullList_right_order(self):
    testName = "test_ExtraRepos3_extraReposFullList_right_order"
    inOptions = MockOptions()
    inOptions.extraReposFile = \
      g_testBaseDir+"/ExtraReposListExisting_3.cmake"
    inOptions.extraReposType = "Nightly"
    inOptions.extraRepos = "preCopyrightTrilinos,extraTrilinosRepo,ExtraTeuchosRepo"
    expectedPass = True
    expectedTribitsExtraRepoNamesList = ["preCopyrightTrilinos", "extraTrilinosRepo"]
    expectedGitRepos = [
      GitRepo("", "", "GIT", True),
      GitRepo('preCopyrightTrilinos', 'preCopyrightTrilinos', "GIT", True),
      GitRepo('extraTrilinosRepo', 'extraTrilinosRepo', "GIT", True),
      GitRepo('ExtraTeuchosRepo', 'packages/teuchos/extrastuff', 'GIT', False)
      ]
    consoleRegexMatches = None
    consoleRegexNotMatches = None
    test_TribitsGitRepos_run_case(self, testName, inOptions, expectedPass, \
      expectedTribitsExtraRepoNamesList, expectedGitRepos, \
      consoleRegexMatches, consoleRegexNotMatches)

  def test_ExtraRepos3_extraReposFullList_wrong_order(self):
    testName = "test_ExtraRepos3_extraReposFullList_wrong_order"
    inOptions = MockOptions()
    inOptions.extraReposFile = \
      g_testBaseDir+"/ExtraReposListExisting_3.cmake"
    inOptions.extraReposType = "Nightly"
    inOptions.extraRepos = "extraTrilinosRepo,preCopyrightTrilinos"
    expectedPass = False
    expectedTribitsExtraRepoNamesList = ["Will never be compared"]
    expectedGitRepos = ["Will never be compared"]
    consoleRegexMatches = \
      "ERROR! The list of extra repos passed in\n"+\
     ".extraTrilinosRepo;preCopyrightTrilinos. is not a subset and in the same\n"
    consoleRegexNotMatches = None
    test_TribitsGitRepos_run_case(self, testName, inOptions, expectedPass, \
      expectedTribitsExtraRepoNamesList, expectedGitRepos, \
      consoleRegexMatches, consoleRegexNotMatches)

  def test_ExtraRepos3_listExtraRepos1_first(self):
    testName = "test_ExtraRepos3_listExtraRepos1_first"
    inOptions = MockOptions()
    inOptions.extraReposFile = \
      g_testBaseDir+"/ExtraReposListExisting_3.cmake"
    inOptions.extraRepos = "preCopyrightTrilinos"
    inOptions.extraReposType = "Nightly"
    expectedPass = True
    expectedTribitsExtraRepoNamesList = ["preCopyrightTrilinos"]
    expectedGitRepos = [
      GitRepo("", "", "GIT", True),
      GitRepo('preCopyrightTrilinos', 'preCopyrightTrilinos', "GIT", True),
      ]
    consoleRegexMatches = None
    consoleRegexNotMatches = None
    test_TribitsGitRepos_run_case(self, testName, inOptions, expectedPass, \
      expectedTribitsExtraRepoNamesList, expectedGitRepos, \
      consoleRegexMatches, consoleRegexNotMatches)

  def test_ExtraRepos3_listExtraRepos1_middle(self):
    testName = "test_ExtraRepos3_listExtraRepos1_middle"
    inOptions = MockOptions()
    inOptions.extraReposFile = \
      g_testBaseDir+"/ExtraReposListExisting_3.cmake"
    inOptions.extraRepos = "extraTrilinosRepo"
    inOptions.extraReposType = "Nightly"
    expectedPass = True
    expectedTribitsExtraRepoNamesList = ["extraTrilinosRepo"]
    expectedGitRepos = [
      GitRepo("", "", "GIT", True),
      GitRepo('extraTrilinosRepo', 'extraTrilinosRepo', "GIT", True),
      ]
    consoleRegexMatches = None
    consoleRegexNotMatches = None
    test_TribitsGitRepos_run_case(self, testName, inOptions, expectedPass, \
      expectedTribitsExtraRepoNamesList, expectedGitRepos, \
      consoleRegexMatches, consoleRegexNotMatches)

  def test_ExtraRepos3_listExtraRepos1_last(self):
    testName = "test_ExtraRepos3_listExtraRepos1_last"
    inOptions = MockOptions()
    inOptions.extraReposFile = \
      g_testBaseDir+"/ExtraReposListExisting_3.cmake"
    inOptions.extraRepos = "ExtraTeuchosRepo"
    inOptions.extraReposType = "Nightly"
    expectedPass = True
    expectedTribitsExtraRepoNamesList = []
    expectedGitRepos = [
      GitRepo("", "", "GIT", True),
      GitRepo('ExtraTeuchosRepo', 'packages/teuchos/extrastuff', 'GIT', False)
      ]
    consoleRegexMatches = None
    consoleRegexNotMatches = None
    test_TribitsGitRepos_run_case(self, testName, inOptions, expectedPass, \
      expectedTribitsExtraRepoNamesList, expectedGitRepos, \
      consoleRegexMatches, consoleRegexNotMatches)

  def test_ExtraRepos3NoContinuous_noExtraRepos(self):
    testName = "test_ExtraRepos3NoContinuous_noExtraRepos"
    inOptions = MockOptions()
    inOptions.extraReposFile = \
      g_testBaseDir+"/ExtraReposList3NoContinuous.cmake"
    inOptions.extraRepos = ""
    inOptions.extraReposType = "Continuous"
    expectedPass = True
    expectedTribitsExtraRepoNamesList = []
    expectedGitRepos = [
      GitRepo("", "", "GIT", True),
      ]
    consoleRegexMatches = None
    consoleRegexNotMatches = None
    test_TribitsGitRepos_run_case(self, testName, inOptions, expectedPass, \
      expectedTribitsExtraRepoNamesList, expectedGitRepos, \
      consoleRegexMatches, consoleRegexNotMatches)

  def test_ExtraReposHasPackagesAndDeepDir(self):
    testName = "test_ExtraReposHasPackagesAndDeepDir"
    inOptions = MockOptions()
    inOptions.extraReposFile = \
      g_testBaseDir+"/ExtraReposListHasPackagesAndDeepDir.cmake"
    inOptions.extraRepos = ""
    inOptions.extraReposType = "Continuous"
    expectedPass = False
    expectedTribitsExtraRepoNamesList = [ "Will never be compared"]
    expectedGitRepos = ["Will never be compared"]
    consoleRegexMatches = None
    consoleRegexNotMatches = None
    exceptionRegexMatches = \
      "ERROR!  For extra repo 'ExtraTeuchosRepo', if repoHasPackages==True then repoDir must be same as repo name, not 'packages/teuchos/extrastuff'!\n"
    test_TribitsGitRepos_run_case(self, testName, inOptions, expectedPass, \
      expectedTribitsExtraRepoNamesList, expectedGitRepos, \
      consoleRegexMatches, consoleRegexNotMatches, exceptionRegexMatches)

  def test_ExtraReposNotGit(self):
    testName = "test_ExtraReposNotGit"
    inOptions = MockOptions()
    inOptions.extraReposFile = \
      g_testBaseDir+"/ExtraReposListNotGit.cmake"
    inOptions.extraRepos = ""
    inOptions.extraReposType = "Continuous"
    expectedPass = False
    expectedTribitsExtraRepoNamesList = [ "Will never be compared"]
    expectedGitRepos = ["Will never be compared"]
    consoleRegexMatches = None
    consoleRegexNotMatches = None
    exceptionRegexMatches = \
      "ERROR!  For extra repo 'ExtraTeuchosRepo', the repo type 'SVN' is not supported by the checkin-test.py script, only 'GIT'!\n"
    test_TribitsGitRepos_run_case(self, testName, inOptions, expectedPass, \
      expectedTribitsExtraRepoNamesList, expectedGitRepos, \
      consoleRegexMatches, consoleRegexNotMatches, exceptionRegexMatches)

  def test_ExraRepoListProjectDefault(self):
    testName = "test_ExraRepoListProjectDefault"
    inOptions = MockOptions()
    inOptions.extraReposFile = "project"
    inOptions.extraRepos = "preCopyrightTrilinos,extraTrilinosRepo"
    inOptions.extraReposType = "Nightly"
    expectedPass = True
    expectedTribitsExtraRepoNamesList = ["preCopyrightTrilinos", "extraTrilinosRepo"]
    expectedGitRepos = [
      GitRepo("", "", "GIT", True),
      GitRepo('preCopyrightTrilinos', 'preCopyrightTrilinos', "GIT", True),
      GitRepo('extraTrilinosRepo', 'extraTrilinosRepo', "GIT", True),
      ]
    consoleRegexMatches = \
      "Adding POST extra Continuous repository preCopyrightTrilinos\n"+\
      "Adding POST extra Nightly repository extraTrilinosRepo\n"
    consoleRegexNotMatches = None
    exceptionRegexMatches = None
    test_TribitsGitRepos_run_case(self, testName, inOptions, expectedPass, \
      expectedTribitsExtraRepoNamesList, expectedGitRepos, \
      consoleRegexMatches, consoleRegexNotMatches, exceptionRegexMatches)


#############################################################################
#
# Test RemoteRepoAndBranch
#
#############################################################################


def remoteRepoAndBranchIsSame(rrab1, rrab2):
  isSame = True
  errMsg = ""
  if rrab1.remoteRepo != rrab2.remoteRepo:
    isSame = False
    errMsg = "Error, rrab1.remoteRepo='"+rrab1.remoteRepo+"'" \
      +" != rrab2.remoteRepo='"+rrab2.remoteRepo+"'"
  if rrab1.remoteBranch != rrab2.remoteBranch:
    isSame = False
    errMsg = "Error, rrab1.remoteBranch='"+rrab1.remoteBranch+"'" \
      +" != rrab2.remoteBranch='"+rrab2.remoteBranch+"'"
  return (isSame, errMsg)


class test_RemoteRepoAndBranch(unittest.TestCase):

  def test_construct(self):
    remoteRepoAndBranch = RemoteRepoAndBranch("remote-repo", "remote-branch")
    self.assertEqual(remoteRepoAndBranch.remoteRepo, "remote-repo")
    self.assertEqual(remoteRepoAndBranch.remoteBranch, "remote-branch")

  def test_assertRemoteRepoAndBranchEqual_same(self):
    (isSame, errMsg) = remoteRepoAndBranchIsSame(
      RemoteRepoAndBranch("remote-repo", "remote-branch"),
      RemoteRepoAndBranch("remote-repo", "remote-branch") )
    self.assertEqual(isSame, True)
    self.assertEqual(errMsg, "")

  def test_assertRemoteRepoAndBranchEqual_diff_repo(self):
    (isSame, errMsg) = remoteRepoAndBranchIsSame(
      RemoteRepoAndBranch("remote-repo0", "remote-branch"),
      RemoteRepoAndBranch("remote-repo1", "remote-branch") )
    self.assertEqual(isSame, False)
    self.assertEqual(errMsg,
      "Error, rrab1.remoteRepo='remote-repo0' != rrab2.remoteRepo='remote-repo1'")

  def test_assertRemoteRepoAndBranchEqual_diff_branch(self):
    (isSame, errMsg) = remoteRepoAndBranchIsSame(
      RemoteRepoAndBranch("remote-repo", "remote-branch0"),
      RemoteRepoAndBranch("remote-repo", "remote-branch1") )
    self.assertEqual(isSame, False)
    self.assertEqual(errMsg,
      "Error, rrab1.remoteBranch='remote-branch0' != rrab2.remoteBranch='remote-branch1'")


#############################################################################
#
# Test RepoExtraRemotePulls
#
#############################################################################


def assertRemoteRepoAndBranchSame(testObj, rrab1, rrab2):
  (isSame, errMsg) = remoteRepoAndBranchIsSame(rrab1, rrab2)
  testObj.assertEqual(isSame, True, errMsg)


class test_RepoExtraRemotePulls(unittest.TestCase):

  def test_1(self):
    repoRemotePulls = RepoExtraRemotePulls(
      GitRepo("localrepo", "local/repo/dir", "GIT", False),
      [ RemoteRepoAndBranch("remote0","branch0"),
        RemoteRepoAndBranch("remote1","branch1") ]
      )
    self.assertEqual(repoRemotePulls.gitRepo.repoName, "localrepo")
    self.assertEqual(repoRemotePulls.gitRepo.repoDir, "local/repo/dir")
    self.assertEqual(len(repoRemotePulls.remoteRepoAndBranchList), 2)
    assertRemoteRepoAndBranchSame(self,
      repoRemotePulls.remoteRepoAndBranchList[0],
      RemoteRepoAndBranch("remote0", "branch0") )
    assertRemoteRepoAndBranchSame(self,
      repoRemotePulls.remoteRepoAndBranchList[1],
      RemoteRepoAndBranch("remote1", "branch1") )


#############################################################################
#
# Test getLocalRepoRemoteRepoAndBranchFromExtraPullArg
#
#############################################################################


class test_getLocalRepoRemoteRepoAndBranchFromExtraPullArg(unittest.TestCase):

  def test_remoterepo_remotebranch(self):
    self.assertEqual(
      getLocalRepoRemoteRepoAndBranchFromExtraPullArg("remoterepo:remotebranch"),
      ("", "remoterepo", "remotebranch", True) )

  def test_localrepo_remoterepo_remotebranch(self):
    self.assertEqual(
      getLocalRepoRemoteRepoAndBranchFromExtraPullArg("localrepo:remoterepo:remotebranch"),
      ("localrepo", "remoterepo", "remotebranch", False) )

  def test_empty_remoterepo_remotebranch(self):
    self.assertEqual(
      getLocalRepoRemoteRepoAndBranchFromExtraPullArg(":remoterepo:remotebranch"),
      ("", "remoterepo", "remotebranch", False) )
    # This is for the use case where the local repo is empty which matches the
    # base repo.

  def test_nocolon(self):
    self.assertRaises( ValueError,
        getLocalRepoRemoteRepoAndBranchFromExtraPullArg, "something")

  def test_four_colons(self):
    self.assertRaises( ValueError,
        getLocalRepoRemoteRepoAndBranchFromExtraPullArg, "a:b:c:d")

  def test_empty_remoterepo(self):
    self.assertRaises( ValueError,
        getLocalRepoRemoteRepoAndBranchFromExtraPullArg, "localrepo::remotebranch")

  def test_empty_remotebranch(self):
    self.assertRaises( ValueError,
        getLocalRepoRemoteRepoAndBranchFromExtraPullArg, "localrepo:remoterepo:")


#############################################################################
#
# Test parseExtraPullFromArgs
#
#############################################################################


class test_parseExtraPullFromArgs(unittest.TestCase):

  def setUp(self):
    self.gitRepoList = [
      GitRepo("", "", "GIT", False),
      GitRepo("repo0", "repo0_dir", "GIT", False),
      GitRepo("repo1", "repo1_dir", "GIT", False)
      ]

  def test_extra_pull_from_0_empty(self):
    repoExtraRemotePullsList = parseExtraPullFromArgs(self.gitRepoList, "")
    self.assertEqual(len(repoExtraRemotePullsList), 3)
    self.assertEqual(repoExtraRemotePullsList[0].gitRepo.repoName, "")
    self.assertEqual(repoExtraRemotePullsList[0].gitRepo.repoDir, "")
    self.assertEqual(repoExtraRemotePullsList[0].remoteRepoAndBranchList, [])
    self.assertEqual(repoExtraRemotePullsList[1].gitRepo.repoName, "repo0")
    self.assertEqual(repoExtraRemotePullsList[1].gitRepo.repoDir, "repo0_dir")
    self.assertEqual(repoExtraRemotePullsList[1].remoteRepoAndBranchList, [])
    self.assertEqual(repoExtraRemotePullsList[2].gitRepo.repoName, "repo1")
    self.assertEqual(repoExtraRemotePullsList[2].gitRepo.repoDir, "repo1_dir")
    self.assertEqual(repoExtraRemotePullsList[2].remoteRepoAndBranchList, [])

  def test_extra_pull_from_1_allrepos(self):
    repoExtraRemotePullsList = parseExtraPullFromArgs(self.gitRepoList,
      "remoterepo:remotebranch")
    self.assertEqual(len(repoExtraRemotePullsList), 3)
    self.assertEqual(repoExtraRemotePullsList[0].gitRepo.repoName, "")
    self.assertEqual(repoExtraRemotePullsList[0].gitRepo.repoDir, "")
    self.assertEqual(len(repoExtraRemotePullsList[0].remoteRepoAndBranchList), 1)
    assertRemoteRepoAndBranchSame(self,
      repoExtraRemotePullsList[0].remoteRepoAndBranchList[0],
      RemoteRepoAndBranch("remoterepo", "remotebranch") )
    self.assertEqual(repoExtraRemotePullsList[1].gitRepo.repoName, "repo0")
    self.assertEqual(repoExtraRemotePullsList[1].gitRepo.repoDir, "repo0_dir")
    self.assertEqual(len(repoExtraRemotePullsList[1].remoteRepoAndBranchList), 1)
    assertRemoteRepoAndBranchSame(self,
      repoExtraRemotePullsList[1].remoteRepoAndBranchList[0],
      RemoteRepoAndBranch("remoterepo", "remotebranch") )
    self.assertEqual(repoExtraRemotePullsList[2].gitRepo.repoName, "repo1")
    self.assertEqual(repoExtraRemotePullsList[2].gitRepo.repoDir, "repo1_dir")
    self.assertEqual(len(repoExtraRemotePullsList[2].remoteRepoAndBranchList), 1)
    assertRemoteRepoAndBranchSame(self,
      repoExtraRemotePullsList[2].remoteRepoAndBranchList[0],
      RemoteRepoAndBranch("remoterepo", "remotebranch") )
    # Above shows that "remoterepo:remotebranch" matches all repos to maintain
    # backward compatibility.

  def test_extra_pull_from_1_empty_localrepo(self):
    repoExtraRemotePullsList = parseExtraPullFromArgs(self.gitRepoList,
      ":remoterepo:remotebranch")
    self.assertEqual(len(repoExtraRemotePullsList), 3)
    self.assertEqual(repoExtraRemotePullsList[0].gitRepo.repoName, "")
    self.assertEqual(repoExtraRemotePullsList[0].gitRepo.repoDir, "")
    self.assertEqual(len(repoExtraRemotePullsList[0].remoteRepoAndBranchList), 1)
    assertRemoteRepoAndBranchSame(self,
      repoExtraRemotePullsList[0].remoteRepoAndBranchList[0],
      RemoteRepoAndBranch("remoterepo", "remotebranch") )
    self.assertEqual(repoExtraRemotePullsList[1].gitRepo.repoName, "repo0")
    self.assertEqual(repoExtraRemotePullsList[1].gitRepo.repoDir, "repo0_dir")
    self.assertEqual(len(repoExtraRemotePullsList[1].remoteRepoAndBranchList), 0)
    self.assertEqual(repoExtraRemotePullsList[2].gitRepo.repoName, "repo1")
    self.assertEqual(repoExtraRemotePullsList[2].gitRepo.repoDir, "repo1_dir")
    self.assertEqual(len(repoExtraRemotePullsList[2].remoteRepoAndBranchList), 0)
    # Above shows that ":remoterepo:remotebranch" matches just the base repo.

  def test_extra_pull_from_2_localrepo0(self):
    repoExtraRemotePullsList = parseExtraPullFromArgs(self.gitRepoList,
      "repo0:remoterepo:remotebranch")
    self.assertEqual(len(repoExtraRemotePullsList), 3)
    self.assertEqual(repoExtraRemotePullsList[0].gitRepo.repoName, "")
    self.assertEqual(repoExtraRemotePullsList[0].gitRepo.repoDir, "")
    self.assertEqual(len(repoExtraRemotePullsList[0].remoteRepoAndBranchList), 0)
    self.assertEqual(repoExtraRemotePullsList[1].gitRepo.repoName, "repo0")
    self.assertEqual(repoExtraRemotePullsList[1].gitRepo.repoDir, "repo0_dir")
    self.assertEqual(len(repoExtraRemotePullsList[1].remoteRepoAndBranchList), 1)
    assertRemoteRepoAndBranchSame(self,
      repoExtraRemotePullsList[1].remoteRepoAndBranchList[0],
      RemoteRepoAndBranch("remoterepo", "remotebranch") )
    self.assertEqual(repoExtraRemotePullsList[2].gitRepo.repoName, "repo1")
    self.assertEqual(repoExtraRemotePullsList[2].gitRepo.repoDir, "repo1_dir")
    self.assertEqual(len(repoExtraRemotePullsList[2].remoteRepoAndBranchList), 0)

  def test_extra_pull_from_3_localrepo0_allrepos(self):
    repoExtraRemotePullsList = parseExtraPullFromArgs(self.gitRepoList,
      "remoterepo1:remotebranch1,repo0:remoterepo2:remotebranch2")
    self.assertEqual(len(repoExtraRemotePullsList), 3)
    self.assertEqual(repoExtraRemotePullsList[0].gitRepo.repoName, "")
    self.assertEqual(repoExtraRemotePullsList[0].gitRepo.repoDir, "")
    self.assertEqual(len(repoExtraRemotePullsList[0].remoteRepoAndBranchList), 1)
    assertRemoteRepoAndBranchSame(self,
      repoExtraRemotePullsList[0].remoteRepoAndBranchList[0],
      RemoteRepoAndBranch("remoterepo1", "remotebranch1") )
    self.assertEqual(repoExtraRemotePullsList[1].gitRepo.repoDir, "repo0_dir")
    self.assertEqual(len(repoExtraRemotePullsList[1].remoteRepoAndBranchList), 2)
    assertRemoteRepoAndBranchSame(self,
      repoExtraRemotePullsList[1].remoteRepoAndBranchList[0],
      RemoteRepoAndBranch("remoterepo1", "remotebranch1") )
    assertRemoteRepoAndBranchSame(self,
      repoExtraRemotePullsList[1].remoteRepoAndBranchList[1],
      RemoteRepoAndBranch("remoterepo2", "remotebranch2") )
    self.assertEqual(repoExtraRemotePullsList[2].gitRepo.repoDir, "repo1_dir")
    self.assertEqual(len(repoExtraRemotePullsList[2].remoteRepoAndBranchList), 1)
    assertRemoteRepoAndBranchSame(self,
      repoExtraRemotePullsList[2].remoteRepoAndBranchList[0],
      RemoteRepoAndBranch("remoterepo1", "remotebranch1") )

  def test_extra_pull_from_4_baserepo_localrepo1(self):
    repoExtraRemotePullsList = parseExtraPullFromArgs(self.gitRepoList,
      ":remoterepo1:remotebranch1,repo1:remoterepo2:remotebranch2")
    self.assertEqual(len(repoExtraRemotePullsList), 3)
    self.assertEqual(repoExtraRemotePullsList[0].gitRepo.repoName, "")
    self.assertEqual(repoExtraRemotePullsList[0].gitRepo.repoDir, "")
    self.assertEqual(len(repoExtraRemotePullsList[0].remoteRepoAndBranchList), 1)
    assertRemoteRepoAndBranchSame(self,
      repoExtraRemotePullsList[0].remoteRepoAndBranchList[0],
      RemoteRepoAndBranch("remoterepo1", "remotebranch1") )
    self.assertEqual(repoExtraRemotePullsList[1].gitRepo.repoDir, "repo0_dir")
    self.assertEqual(len(repoExtraRemotePullsList[1].remoteRepoAndBranchList), 0)
    self.assertEqual(repoExtraRemotePullsList[2].gitRepo.repoDir, "repo1_dir")
    self.assertEqual(len(repoExtraRemotePullsList[2].remoteRepoAndBranchList), 1)
    assertRemoteRepoAndBranchSame(self,
      repoExtraRemotePullsList[2].remoteRepoAndBranchList[0],
      RemoteRepoAndBranch("remoterepo2", "remotebranch2") )

  def test_extra_pull_from_5_localrepo1_baserepo(self):
    repoExtraRemotePullsList = parseExtraPullFromArgs(self.gitRepoList,
      "repo1:remoterepo2:remotebranch2,:remoterepo1:remotebranch1")
    self.assertEqual(len(repoExtraRemotePullsList), 3)
    self.assertEqual(repoExtraRemotePullsList[0].gitRepo.repoName, "")
    self.assertEqual(repoExtraRemotePullsList[0].gitRepo.repoDir, "")
    self.assertEqual(len(repoExtraRemotePullsList[0].remoteRepoAndBranchList), 1)
    assertRemoteRepoAndBranchSame(self,
      repoExtraRemotePullsList[0].remoteRepoAndBranchList[0],
      RemoteRepoAndBranch("remoterepo1", "remotebranch1") )
    self.assertEqual(repoExtraRemotePullsList[1].gitRepo.repoDir, "repo0_dir")
    self.assertEqual(len(repoExtraRemotePullsList[1].remoteRepoAndBranchList), 0)
    self.assertEqual(repoExtraRemotePullsList[2].gitRepo.repoDir, "repo1_dir")
    self.assertEqual(len(repoExtraRemotePullsList[2].remoteRepoAndBranchList), 1)
    assertRemoteRepoAndBranchSame(self,
      repoExtraRemotePullsList[2].remoteRepoAndBranchList[0],
      RemoteRepoAndBranch("remoterepo2", "remotebranch2") )
    # Above test shows that the order that the pull are listed is not
    # significant between repos, only for the same repo.

  def test_extra_pull_from_6_localrepo0_2extrapulls(self):
    repoExtraRemotePullsList = parseExtraPullFromArgs(self.gitRepoList,
      "repo0:remoterepo1:remotebranch1,repo0:remoterepo0:remotebranch0")
    self.assertEqual(len(repoExtraRemotePullsList), 3)
    self.assertEqual(repoExtraRemotePullsList[0].gitRepo.repoName, "")
    self.assertEqual(repoExtraRemotePullsList[0].gitRepo.repoDir, "")
    self.assertEqual(len(repoExtraRemotePullsList[0].remoteRepoAndBranchList), 0)
    self.assertEqual(repoExtraRemotePullsList[1].gitRepo.repoDir, "repo0_dir")
    self.assertEqual(len(repoExtraRemotePullsList[1].remoteRepoAndBranchList), 2)
    assertRemoteRepoAndBranchSame(self,
      repoExtraRemotePullsList[1].remoteRepoAndBranchList[0],
      RemoteRepoAndBranch("remoterepo1", "remotebranch1") )
    assertRemoteRepoAndBranchSame(self,
      repoExtraRemotePullsList[1].remoteRepoAndBranchList[1],
      RemoteRepoAndBranch("remoterepo0", "remotebranch0") )
    self.assertEqual(repoExtraRemotePullsList[2].gitRepo.repoDir, "repo1_dir")
    self.assertEqual(len(repoExtraRemotePullsList[2].remoteRepoAndBranchList), 0)
    # Above test shows that the order that the pulls are listed is
    # significant for the same repo.


#############################################################################
#
# Test checkin-test.py script
#
#############################################################################


# Test Data

g_cmndinterceptsDumpDepsXMLFile = \
  "IT: .*cmake .+ -P .+/TribitsDumpDepsXmlScript.cmake; 0; 'dump XML file passed'\n" \

def cmndinterceptsGetRepoStatsPass(changedFile="", \
  branch = "currentbranch", trackingBranch="origin/trackingbranch", \
  numCommits = "4" \
  ):
  return \
    "IT: git rev-parse --abbrev-ref HEAD; 0; '"+branch+"'\n" \
    "IT: git rev-parse --abbrev-ref --symbolic-full-name @{u}; 0; '"+trackingBranch+"'\n" \
    "IT: git shortlog -s HEAD ."+trackingBranch+"; 0; '    "+numCommits+"  John Doe'\n" \
    "IT: git status --porcelain; 0; '"+changedFile+"'\n"

def cmndinterceptsGetRepoStatsNoTrackingBranchPass(changedFile="", \
  branch = "currentbranch" \
  ):
  return \
    "IT: git rev-parse --abbrev-ref HEAD; 0; '"+branch+"'\n" \
    "IT: git rev-parse --abbrev-ref --symbolic-full-name @{u}; 128; ''\n" \
    "IT: git status --porcelain; 0; '"+changedFile+"'\n"

g_cmndinterceptsPullOnlyPasses = \
  "IT: git pull; 0; 'pulled changes passes'\n"

g_cmndinterceptsPullOnlyFails = \
  "IT: git pull; 1; 'pull failed'\n"

g_cmndinterceptsPullOnlyNoUpdatesPasses = \
  "IT: git pull; 0; 'Already up-to-date.'\n"

g_cmndinterceptsStatusPullPasses = \
  cmndinterceptsGetRepoStatsPass()+ \
  g_cmndinterceptsPullOnlyPasses

g_cmndinterceptsDiffOnlyPasses = \
  "IT: git diff --name-status origin/trackingbranch; 0; 'M\tpackages/teuchos/CMakeLists.txt'\n"

g_cmndinterceptsDiffOnlyPassesPreCopyrightTrilinos = \
  "IT: git diff --name-status origin/trackingbranch; 0; 'M\tteko/CMakeLists.txt'\n"

g_cmndinterceptsDiffOnlyPassesExtraTrilinosRepo = \
  "IT: git diff --name-status origin/trackingbranch; 0; 'M\textrapack/src/ExtraPack_ConfigDefs.hpp'\n"

g_cmndinterceptsPullPasses = \
  g_cmndinterceptsStatusPullPasses \
  +g_cmndinterceptsDiffOnlyPasses

g_cmndinterceptsNoChangesPullPasses = \
  cmndinterceptsGetRepoStatsPass(numCommits="0") \
  +g_cmndinterceptsPullOnlyPasses

g_cmndinterceptsConfigPasses = \
  "IT: \./do-configure; 0; 'do-configure passed'\n"

g_cmndinterceptsConfigBuildPasses = \
  g_cmndinterceptsConfigPasses+ \
  "IT: make -j3; 0; 'make passed'\n"

g_cmndinterceptsConfigBuildTestPasses = \
  g_cmndinterceptsConfigBuildPasses+ \
  "IT: ctest -j5; 0; '100% tests passed, 0 tests failed out of 100'\n"

g_cmnginterceptsGitLogCmnds = \
  "IT: git cat-file -p HEAD; 0; 'This is the last commit message'\n" \
  "IT: git log --pretty=format:'%h' currentbranch \^origin/trackingbranch; 0; '54321'; '12345'\n"

g_cmndinterceptsFinalPullRebasePasses = \
  "IT: git pull && git rebase origin/trackingbranch; 0; 'final git pull and rebase passed'\n"

g_cmndinterceptsFinalPullRebaseFails = \
  "IT: git pull && git rebase origin/trackingbranch; 1; 'final git pull and rebase failed'\n"

g_cmndinterceptsAmendCommitPasses = \
  "IT: git commit --amend -F .*; 0; 'Amending the last commit passed'\n"

g_cmndinterceptsAmendCommitFails = \
  "IT: git commit --amend -F .*; 1; 'Amending the last commit failed'\n"

g_cmndinterceptsLogCommitsPasses = \
  "IT: git log --oneline currentbranch \^origin/trackingbranch; 0; '54321 Only one commit'\n"

g_cmndinterceptsPushOnlyPasses = \
  "IT: git push origin currentbranch:trackingbranch ; 0; 'push passes'\n"

g_cmndinterceptsPushOnlyFails = \
  "IT: git push origin currentbranch:trackingbranch; 1; 'push failed'\n"

g_cmndinterceptsFinalPushPasses = \
  g_cmndinterceptsFinalPullRebasePasses+\
  g_cmnginterceptsGitLogCmnds+ \
  g_cmndinterceptsAmendCommitPasses+ \
  g_cmndinterceptsLogCommitsPasses+ \
  "IT: git push origin currentbranch:trackingbranch; 0; 'push passes'\n"

g_cmndinterceptsFinalPushNoAppendTestResultsPasses = \
  "IT: git pull && git rebase origin/trackingbranch; 0; 'final git pull and rebase passed'\n" \
  +g_cmndinterceptsLogCommitsPasses \
  +g_cmndinterceptsPushOnlyPasses

g_cmndinterceptsFinalPushNoRebasePasses = \
  "IT: git pull; 0; 'final git pull only passed'\n" \
  +g_cmnginterceptsGitLogCmnds+ \
  "IT: git commit --amend -F .*; 0; 'Amending the last commit passed'\n" \
  +g_cmndinterceptsLogCommitsPasses \
  +g_cmndinterceptsPushOnlyPasses

g_cmndinterceptsSendBuildTestCaseEmail = \
  "IT: mailx -s .*; 0; 'Do not really sending build/test case email'\n"

g_cmndinterceptsSendFinalEmail = \
  "IT: mailx -s .*; 0; 'Do not really send email '\n"

g_cmndinterceptsExtraRepo1ThroughStatusPasses = \
  g_cmndinterceptsDumpDepsXMLFile \
  +cmndinterceptsGetRepoStatsPass() \
  +cmndinterceptsGetRepoStatsPass()

g_cmndinterceptsExtraRepo1ThroughStatusNoChangesPasses = \
  g_cmndinterceptsDumpDepsXMLFile \
  +cmndinterceptsGetRepoStatsPass(numCommits="0") \
  +cmndinterceptsGetRepoStatsPass(numCommits="0")

g_cmndinterceptsExtraRepo1DoAllThroughTest = \
  g_cmndinterceptsExtraRepo1ThroughStatusPasses \
  +g_cmndinterceptsPullOnlyPasses \
  +g_cmndinterceptsPullOnlyPasses \
  +g_cmndinterceptsDiffOnlyPasses \
  +g_cmndinterceptsDiffOnlyPassesPreCopyrightTrilinos \
  +g_cmndinterceptsConfigBuildTestPasses \
  +g_cmndinterceptsSendBuildTestCaseEmail

g_cmndinterceptsExtraRepo1TrilinosChangesDoAllThroughTest = \
  g_cmndinterceptsDumpDepsXMLFile \
  +cmndinterceptsGetRepoStatsPass() \
  +cmndinterceptsGetRepoStatsPass(numCommits="0") \
  +g_cmndinterceptsPullOnlyPasses \
  +g_cmndinterceptsPullOnlyPasses \
  +g_cmndinterceptsDiffOnlyPasses \
  +g_cmndinterceptsConfigBuildTestPasses \
  +g_cmndinterceptsSendBuildTestCaseEmail

g_cmndinterceptsExtraRepo1ExtraRepoChangesDoAllThroughTest = \
  g_cmndinterceptsDumpDepsXMLFile \
  +cmndinterceptsGetRepoStatsPass(numCommits="0") \
  +cmndinterceptsGetRepoStatsPass() \
  +g_cmndinterceptsPullOnlyPasses \
  +g_cmndinterceptsPullOnlyPasses \
  +g_cmndinterceptsDiffOnlyPassesPreCopyrightTrilinos \
  +g_cmndinterceptsConfigBuildTestPasses \
  +g_cmndinterceptsSendBuildTestCaseEmail

g_cmndinterceptsExtraRepo1NoChangesDoAllThroughTest = \
  g_cmndinterceptsDumpDepsXMLFile \
  +cmndinterceptsGetRepoStatsPass(numCommits="0") \
  +cmndinterceptsGetRepoStatsPass(numCommits="0") \
  +g_cmndinterceptsPullOnlyPasses \
  +g_cmndinterceptsPullOnlyPasses \
  +g_cmndinterceptsConfigBuildTestPasses \
  +g_cmndinterceptsSendBuildTestCaseEmail

g_cmndinterceptsExtraRepo1DoAllUpToPush = \
  g_cmndinterceptsExtraRepo1DoAllThroughTest \
  +g_cmndinterceptsFinalPullRebasePasses \
  +g_cmndinterceptsFinalPullRebasePasses \
  +g_cmnginterceptsGitLogCmnds \
  +g_cmndinterceptsAmendCommitPasses \
  +g_cmnginterceptsGitLogCmnds \
  +g_cmndinterceptsAmendCommitPasses 

g_cmndinterceptsExtraRepo1TrilinosChangesDoAllUpToPush = \
  g_cmndinterceptsExtraRepo1TrilinosChangesDoAllThroughTest \
  +g_cmndinterceptsFinalPullRebasePasses \
  +g_cmndinterceptsFinalPullRebasePasses \
  +g_cmnginterceptsGitLogCmnds \
  +g_cmndinterceptsAmendCommitPasses

g_cmndinterceptsExtraRepo1ExtraRepoChangesDoAllUpToPush = \
  g_cmndinterceptsExtraRepo1ExtraRepoChangesDoAllThroughTest \
  +g_cmndinterceptsFinalPullRebasePasses \
  +g_cmndinterceptsFinalPullRebasePasses \
  +g_cmnginterceptsGitLogCmnds \
  +g_cmndinterceptsAmendCommitPasses

g_cmndinterceptsExtraRepo1NoChangesDoAllUpToPush = \
  g_cmndinterceptsExtraRepo1NoChangesDoAllThroughTest \
  +g_cmndinterceptsFinalPullRebasePasses \
  +g_cmndinterceptsFinalPullRebasePasses \

g_expectedRegexUpdatePasses = \
  "Pull passed!\n" \

g_expectedRegexUpdateWithBuildCasePasses = \
  "Pull passed!\n" \
  "The pull passed!\n" \
  "Pull: Passed\n"

g_expectedRegexConfigPasses = \
  "Full package enable list:.*Teuchos.*\n" \
  "Configure passed!\n" \
  "The configure passed!\n" \
  "Configure: Passed\n" \

g_expectedRegexExplicitConfigPasses = \
  "Enabling only the explicitly specified packages .Teuchos.\n" \
  "Configure passed!\n" \
  "The configure passed!\n" \
  "Configure: Passed\n" \

g_expectedRegexBuildPasses = \
  "Build passed!\n" \
  "The build passed!\n" \
  "Build: Passed\n"

g_expectedRegexBuildFailed = \
  "Build failed returning 1!\n" \
  "The build FAILED!\n" \
  "Build: FAILED\n"

g_expectedRegexTestPasses = \
  "No tests failed!\n" \
  "testResultsLine = .100% tests passed, 0 tests failed out of 100.\n" \
  "passed: passed=100,notpassed=0\n" \
  "Test: Passed\n"

g_expectedRegexTestNotRun = \
  "The tests were never even run!\n" \
  "Test: FAILED\n"

g_expectedCommonOptionsSummary = \
  "Enabled Packages: Teuchos\n" \
  "Make Options: -j3\n" \
  "CTest Options: -j5\n"

g_verbose=True
g_verbose=False


#
# Test helper functions
#


g_checkin_test_tests_dir = "checkin_test_tests"




def create_checkin_test_case_dir(testName, verbose=False):
  baseDir = os.getcwd()
  testDirName = os.path.join(g_checkin_test_tests_dir, testName)
  createDir(g_checkin_test_tests_dir, verbose)
  createDir(testDirName, verbose)
  return testDirName


# Main unit test driver
def checkin_test_run_case(testObject, testName, optionsStr, cmndInterceptsStr, \
  expectPass, passRegexStrList, filePassRegexStrList=None, mustHaveCheckinTestOut=True, \
  failRegexStrList=None, fileFailRegexStrList=None, envVars=[], inPathGit=True, \
  grepForFinalPassFailStr=True, \
  logFileName=None, \
  printOutputFile=False
  ):

  verbose = g_verbose

  if grepForFinalPassFailStr:
    if expectPass:
      passRegexStrList += "REQUESTED ACTIONS: PASSED\n"
    else:
      passRegexStrList += "REQUESTED ACTIONS: FAILED\n"

  passRegexList = passRegexStrList.split('\n')

  if verbose: print("\npassRegexList =", passRegexList)

  # A) Create the test directory

  baseDir = os.getcwd()
  echoChDir(create_checkin_test_case_dir(testName, verbose), verbose)

  try:

    # B) Create the command to run the checkin-test.py script

    cmndArgs = [
      tribitsBaseDir + "/ci_support/checkin-test.py",
      "--with-cmake=\""+g_withCmake+"\"",
      "--project-name=Trilinos",
      "--src-dir="+mockProjectBaseDir,
      "--send-email-to=bogous@somwhere.com",
      "--project-configuration=%s" % os.path.join(g_testBaseDir,
        'CheckinTest_UnitTests_Config.py'),
      optionsStr,
      ]
    
    if logFileName:
      logFileNameExpected = logFileName
      cmndArgs.append("--log-file="+logFileName)
    else:
      logFileNameExpected = "checkin-test.out"

    cmnd = ' '.join(cmndArgs)
    
    # C) Set up the command intercept file

    baseCmndInterceptsStr = \
      "FT: .*checkin-test\.py.*\n" \
      "FT: .*cmake .*TribitsGetExtraReposForCheckinTest.cmake.*\n" \
      "FT: date\n" \
      "FT: rm [a-zA-Z0-9_/\.]+\n" \
      "FT: touch .*\n" \
      "FT: chmod .*\n" \
      "FT: hostname\n" \
      "FT: grep .*"+getTestOutputFileName()+"\n" \
      "FT: grep .*"+getEmailBodyFileName()+"\n" \
      "FT: grep .*REQUESTED ACTIONS\: PASSED.*\n"

    if inPathGit:
      baseCmndInterceptsStr += \
      "IT: git config --get user.email; 0; bogous@somwhere.com\n" \
      +"IT: which git; 0; /some/path/git\n"

    fullCmndInterceptsStr = baseCmndInterceptsStr + cmndInterceptsStr

    fullCmndInterceptsFileName = os.path.join(os.getcwd(), "cmndIntercepts.txt")
    writeStrToFile(fullCmndInterceptsFileName, fullCmndInterceptsStr)

    os.environ['GENERAL_SCRIPT_SUPPORT_CMND_INTERCEPTS_FILE'] = fullCmndInterceptsFileName

    os.environ['CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE'] = projectDepsXmlFileDefaultOverride
    
    # D) Run the checkin-test.py script with mock commands

    for envVar in envVars:
      (varName,varValue) = envVar.split("=")
      #print("varName="+varName)
      #print("varValue="+varValue)
      os.environ[varName] = varValue
      
    checkin_test_test_out = "checkin-test.test.out"

    rtnCode = echoRunSysCmnd(cmnd, timeCmnd=True, throwExcept=False,
      outFile=checkin_test_test_out, verbose=verbose)
    
    # E) Grep the main output file looking for specific strings

    if mustHaveCheckinTestOut:
      if logFileName:
        outputFileToGrep = logFileName
      else:
        outputFileToGrep = "checkin-test.out"
    else:
      outputFileToGrep = checkin_test_test_out

    if printOutputFile:
      print(readStrFromFile(outputFileToGrep))

    assertGrepFileForRegexStrList(testObject, testName, outputFileToGrep,
      passRegexStrList, verbose)

    if failRegexStrList:
      assertNotGrepFileForRegexStrList(testObject, testName, outputFileToGrep,
        failRegexStrList, verbose)

    # F) Grep a set of output files looking for given strings

    if filePassRegexStrList:
      for fileRegexGroup in filePassRegexStrList:
        (fileName, regexStrList) = fileRegexGroup
        assertGrepFileForRegexStrList(testObject, testName, fileName, regexStrList, verbose)

    if fileFailRegexStrList:
      for fileRegexGroup in fileFailRegexStrList:
        (fileName, regexStrList) = fileRegexGroup
        assertNotGrepFileForRegexStrList(testObject, testName, fileName, regexStrList, verbose)

    # G) Examine the final return code

    if expectPass:
      testObject.assertEqual(rtnCode, 0)
    else:
      testObject.assertNotEqual(rtnCode, 0)
    
  finally:
    # \H) Get back to the current directory and reset
    echoChDir(baseDir, verbose=verbose)
    os.environ['GENERAL_SCRIPT_SUPPORT_CMND_INTERCEPTS_FILE']=""


# Helper test case that is used as the initial case for other tests
def g_test_do_all_default_builds_mpi_debug_pass(testObject, testName):
  checkin_test_run_case(
    \
    testObject,
    \
    testName,
    \
    "--make-options=-j3 --ctest-options=-j5 --default-builds=MPI_DEBUG --do-all",
    \
    g_cmndinterceptsDumpDepsXMLFile \
    +g_cmndinterceptsPullPasses \
    +g_cmndinterceptsConfigBuildTestPasses \
    +g_cmndinterceptsSendBuildTestCaseEmail \
    +g_cmndinterceptsLogCommitsPasses \
    +g_cmndinterceptsSendFinalEmail \
    ,
    \
    True,
    \
    g_expectedRegexUpdateWithBuildCasePasses \
    +g_expectedRegexConfigPasses \
    +g_expectedRegexBuildPasses \
    +g_expectedRegexTestPasses \
    +"0) MPI_DEBUG: Will attempt to run!\n" \
    +"1) SERIAL_RELEASE: Will \*not\* attempt to run on request!\n" \
    +"0) MPI_DEBUG => passed: passed=100,notpassed=0\n" \
    +"1) SERIAL_RELEASE => Test case SERIAL_RELEASE was not run! => Does not affect push readiness!\n" \
    +g_expectedCommonOptionsSummary \
    +"=> A PUSH IS READY TO BE PERFORMED!\n" \
    +"^PASSED [(]READY TO PUSH[)]: Trilinos:\n" \
    ,
    \
    failRegexStrList = \
    "mailx .* trilinos-checkin-tests.*\n" \
    +"DID PUSH: Trilinos\n" \
    )


def checkin_test_configure_test(testObject, testName, optionsStr, filePassRegexStrList, \
  fileFailRegexStrList=[], modifiedFilesStr="", extraPassRegexStr="", doGitDiff=True \
  ):

  if modifiedFilesStr == "" :
    modifiedFilesStr = "M\tpackages/teuchos/CMakeLists.txt"
    modifiedFilesPorcelainStr = " M packages/teuchos/CMakeLists.txt"
  else:
    modifiedFilesStr = "M\t"+modifiedFilesStr
    modifiedFilesPorcelainStr = " M "+modifiedFilesStr

  if doGitDiff:
    gitDiffCmnd= \
      "IT: git diff --name-status origin/trackingbranch; 0; '"+modifiedFilesStr+"'\n"
  else:
    gitDiffCmnd=""

  checkin_test_run_case(
    \
    testObject,
    \
    testName,
    \
    " --allow-no-pull --configure --send-email-to= --skip-push-readiness-check" \
    +" " +optionsStr \
    ,
    \
    g_cmndinterceptsDumpDepsXMLFile \
    +cmndinterceptsGetRepoStatsPass(modifiedFilesPorcelainStr) \
    +gitDiffCmnd \
    +g_cmndinterceptsConfigPasses \
    ,
    \
    True,
    \
    "Configure passed!\n" \
    +"^NOT READY TO PUSH\n" \
    +extraPassRegexStr \
    ,
    filePassRegexStrList
    ,
    fileFailRegexStrList=fileFailRegexStrList
    )


def checkin_test_configure_enables_test(testObject, testName, optionsStr, regexListStr, \
  notRegexListStr="", modifiedFilesStr="", extraPassRegexStr="", doGitDiff=True \
  ):
  checkin_test_configure_test(
     testObject,
     testName,
     "--default-builds=MPI_DEBUG "+optionsStr,
     [("MPI_DEBUG/do-configure", regexListStr)],
     [("MPI_DEBUG/do-configure", notRegexListStr)],
     modifiedFilesStr,
     extraPassRegexStr,
     doGitDiff
     )


# Used as a helper in follow-up tests  
def  g_test_st_extra_builds_st_do_all_pass(testObject, testName):

  testBaseDir = create_checkin_test_case_dir(testName, g_verbose)

  writeStrToFile(testBaseDir+"/COMMON.config",
    "-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON\n" \
    +"-DBUILD_SHARED:BOOL=ON\n" \
    +"-DTPL_BLAS_LIBRARIES:PATH=/usr/local/libblas.a\n" \
    +"-DTPL_LAPACK_LIBRARIES:PATH=/usr/local/liblapack.a\n" \
    )

  writeStrToFile(testBaseDir+"/MPI_DEBUG_ST.config",
    "-DTPL_ENABLE_MPI:BOOL=ON\n" \
    +"-DTeuchos_ENABLE_SECONDARY_TESTED_CODE:BOOL=ON\n" \
    )

  modifiedFilesStr = ""

  checkin_test_run_case(
    \
    testObject,
    \
    testName,
    \
    "--make-options=-j3 --ctest-options=-j5" \
    +" --default-builds=MPI_DEBUG --st-extra-builds=MPI_DEBUG_ST" \
    +" --enable-packages=Phalanx" \
    +" --do-all" \
    ,
    \
    g_cmndinterceptsDumpDepsXMLFile \
    +g_cmndinterceptsPullPasses \
    +g_cmndinterceptsSendBuildTestCaseEmail \
    +g_cmndinterceptsConfigBuildTestPasses \
    +g_cmndinterceptsSendBuildTestCaseEmail \
    +g_cmndinterceptsLogCommitsPasses \
    +g_cmndinterceptsSendFinalEmail \
    ,
    \
    True,
    \
    "Phalanx of type ST is being excluded because it is not in the valid list of package types .PT.\n" \
    +"passed: Trilinos/MPI_DEBUG: skipped configure, build, test due to no enabled packages\n" \
    +"passed: Trilinos/MPI_DEBUG_ST: passed=100,notpassed=0\n" \
    +"0) MPI_DEBUG => Skipped configure, build, test due to no enabled packages! => Does not affect push readiness!\n" \
    +"2) MPI_DEBUG_ST => passed: passed=100,notpassed=0\n" \
    +"^PASSED [(]READY TO PUSH[)]\n" \
    )


#
# checkin_test unit tests
#


class test_checkin_test(unittest.TestCase):


  # A) Test basic passing use cases


  def test_help(self):
    testName = "help"
    checkin_test_run_case(
      self,
      testName,
      "--help",
      "", # No shell commands!
      True,
      "checkin-test.py \[OPTIONS\]\n" \
      "QUICKSTART\n" \
      "DETAILED DOCUMENTATION\n" \
      ".*--show-defaults.*\n" \
      ,
      mustHaveCheckinTestOut=False \
      ,
      grepForFinalPassFailStr=False \
      )
    # Help should not write the checkin-test.out file!
    self.assertEqual(
      os.path.exists(create_checkin_test_case_dir(testName, g_verbose)+"/checkin-test.out"),
      False)


  def test_help_debug_dump(self):
    testName = "help_debug_dump"
    checkin_test_run_case(
      self,
      testName,
      "--help",
      "", # No shell commands!
      True,
      "checkin-test.py \[OPTIONS\]\n" \
      +"thisFilePath\n" \
      +"thisFileRealAbsBasePath\n" \
      +"sys.path\n" \
      +"Loading project configuration from\n" \
      ,
      mustHaveCheckinTestOut=False,
      envVars=["TRIBITS_CHECKIN_TEST_DEBUG_DUMP=ON"] \
      ,
      grepForFinalPassFailStr=False \
      )


  def test_show_defaults(self):
    testName = "show_defaults"
    checkin_test_run_case(
      self,
      testName,
      "--show-defaults",
      "", # No shell commands!
      True,
      "Script: checkin-test.py\n" \
      +"\-\-use-makefiles\n" \
      +"\-\-send-build-case-email=always\n" \
      +"\-\-log-file=.checkin-test.out.\n" \
      ,
      mustHaveCheckinTestOut=False \
      ,
      grepForFinalPassFailStr=False \
      )
    # Help should not write the checkin-test.out file!
    self.assertEqual(
      os.path.exists(create_checkin_test_case_dir(testName, g_verbose)+"/checkin-test.out"),
      False)


  def test_local_defaults_override_project_defaults(self):
    
    testName = "local_defaults_override_project_defaults"

    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)

    writeStrToFile(testBaseDir+"/local-checkin-test-defaults.py",
      "defaults = [\n" \
      "  \"--send-email-to-on-push=dummy@nogood.com\",\n" \
      "  \"-j10\",\n" \
      "  \"--no-rebase\",\n" \
      "  \"--ctest-options=-E '(PackageA_Test1|PackageB_Test2)'\"\n" \
      "  ]\n"
      )

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      " --show-defaults" \
      ,
      "", # No shell commands!
      \
      True,
      \
      "\-\-send-email-to-on-push=.dummy@nogood.com.\n" \
      "\-j10\n" \
      "\-\-no-rebase\n" \
      "\-\-ctest-options=.-E ..PackageA_Test1.PackageB_Test2...\n" \
      ,
      mustHaveCheckinTestOut=False, grepForFinalPassFailStr=False,
      # Above, grep checkin-test.test.out since checkin-test.out is not
      # created when --show-defaults is passed in.
      )


  def test_command_args_override_local_defaults(self):
    
    testName = "command_args_override_local_defaults"

    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)

    writeStrToFile(testBaseDir+"/local-checkin-test-defaults.py",
      "defaults = [\n" \
      "  \"--send-email-to-on-push=dummy@nogood.com\",\n" \
      "  \"-j10\",\n" \
      "  \"--no-rebase\",\n" \
      "  \"--ctest-options=-E '(PackageA_Test1|PackageB_Test2)'\"\n" \
      "  ]\n"
      )

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      " --show-defaults" \
      " --send-email-to-on-push=nothing@good.gov" \
      " -j6 --rebase --ctest-options=\"-E '(Test5_|Test6_)'\""
      ,
      \
      "", # No shell commands!
      \
      True,
      \
      "\-\-send-email-to-on-push=.nothing@good.gov.\n" \
      "\-j6\n" \
      "\-\-rebase\n" \
      "\-\-ctest-options=.-E ..Test5_.Test6...\n" \
      ,
      mustHaveCheckinTestOut=False, grepForFinalPassFailStr=False,
      # Above, grep checkin-test.test.out since checkin-test.out is not
      # created when --show-defaults is passed in.
      )


  def test_do_all_push_pass(self):

    testName = "do_all_push_pass"
    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      "--make-options=-j3 --ctest-options=-j5" \
      +" --abort-gracefully-if-no-changes-pulled --abort-gracefully-if-no-changes-to-push" \
      +" --do-all --push" \
      +" --execute-on-ready-to-push=\"ssh -q godel /some/dir/some_command.sh &\"",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsFinalPushPasses \
      +g_cmndinterceptsSendFinalEmail \
      +"IT: ssh -q godel /some/dir/some_command.sh &; 0; 'extra command passed'\n" \
      ,
      \
      True,
      "[|] ID [|] Repo Dir            [|] Branch        [|] Tracking Branch       [|] C [|] M [|] [?] [|]\n" \
      "[|]  0 [|] MockTrilinos [(]Base[)] [|] currentbranch [|] origin/trackingbranch [|] 4 [|]   [|]   [|]\n" \
      "enable-packages=.. or --enable-all-packages=.auto. => git diffs w.r.t. tracking branch .will. be needed to look for changed files!\n" \
      "Need git diffs w.r.t. tracking branch so all repos must be on a branch and have a tracking branch!\n" \
      "'': Pulled changes from this repo!\n" \
      +"There where at least some changes pulled!\n" \
      +g_expectedRegexUpdateWithBuildCasePasses \
      +g_expectedRegexConfigPasses \
      +g_expectedRegexBuildPasses \
      +g_expectedRegexTestPasses \
      +"0) MPI_DEBUG => passed: passed=100,notpassed=0\n" \
      +"1) SERIAL_RELEASE => passed: passed=100,notpassed=0\n" \
      +g_expectedCommonOptionsSummary \
      +"=> A PUSH IS READY TO BE PERFORMED!\n" \
      +"^\*\*\* Commits for repo :\n" \
      +"^  54321 Only one commit\n" \
      +"mailx .* trilinos-checkin-tests.*\n" \
      +"^DID PUSH: Trilinos:\n" \
      +"Executing final command (ssh -q godel /some/dir/some_command.sh &) since a push is okay to be performed!\n" \
      +"Running: ssh -q godel /some/dir/some_command.sh &\n" \
      ,
      [
      (getInitialPullOutputFileName(""), "pulled changes passes\n"),
      (getModifiedFilesOutputFileName(""), "M\tpackages/teuchos/CMakeLists.txt\n"),
      (getFinalPullOutputFileName(""), "final git pull and rebase passed\n"),
      (getFinalCommitBodyFileName(""),
         getAutomatedStatusSummaryHeaderKeyStr()+"\n"
         +"Enabled Packages: Teuchos\n" \
         +"Enabled all Forward Packages\n" \
         ),
      ("MPI_DEBUG/do-configure.base",
       "\-DTPL_ENABLE_Pthread:BOOL=OFF\n"\
       +"\-DTPL_ENABLE_BinUtils:BOOL=OFF\n"\
       +"\-DTPL_ENABLE_MPI:BOOL=ON\n" \
       +"\-DTrilinos_ENABLE_TESTS:BOOL=ON\n" \
       +"\-DCMAKE_BUILD_TYPE:STRING=RELEASE\n" \
       +"\-DTrilinos_ENABLE_DEBUG:BOOL=ON\n" \
       +"\-DTrilinos_ENABLE_CHECKED_STL:BOOL=ON\n" \
       +"\-DTrilinos_ENABLE_EXPLICIT_INSTANTIATION:BOOL=ON\n"),
      ("MPI_DEBUG/do-configure",
       "\./do-configure.base\n" \
       +"\-DTrilinos_ENABLE_Teuchos:BOOL=ON\n" \
       +"\-DTrilinos_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=ON\n" \
       +"\-DTrilinos_ENABLE_ALL_FORWARD_DEP_PACKAGES:BOOL=ON\n"),
      ("SERIAL_RELEASE/do-configure.base",
       "\-DTPL_ENABLE_Pthread:BOOL=OFF\n"\
       +"\-DTPL_ENABLE_BinUtils:BOOL=OFF\n"\
       +"\-DTrilinos_ENABLE_TESTS:BOOL=ON\n" \
       +"\-DTPL_ENABLE_MPI:BOOL=OFF\n" \
       +"\-DCMAKE_BUILD_TYPE:STRING=RELEASE\n" \
       +"\-DTrilinos_ENABLE_DEBUG:BOOL=OFF\n" \
       +"\-DTrilinos_ENABLE_CHECKED_STL:BOOL=OFF\n" \
       +"\-DTrilinos_ENABLE_EXPLICIT_INSTANTIATION:BOOL=OFF\n"),
      ("SERIAL_RELEASE/do-configure",
       "\./do-configure.base\n" \
       +"\-DTrilinos_ENABLE_Teuchos:BOOL=ON\n" \
       +"\-DTrilinos_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=ON\n" \
       +"\-DTrilinos_ENABLE_ALL_FORWARD_DEP_PACKAGES:BOOL=ON\n"),
      ("commitFinalBody.out", 
        "Other local commits for this build/test group: 12345\n"),
      # ToDo: Add more files to check
      ]
      )

    # Make sure that the success files don't exist after a successful push
    assertFileNotExists(self, testBaseDir+"/pullInitial.success")
    mpiDebugDir=testBaseDir+"/MPI_DEBUG"
    assertFileNotExists(self, mpiDebugDir+"/configure.success")
    assertFileNotExists(self, mpiDebugDir+"/make.success")
    assertFileNotExists(self, mpiDebugDir+"/ctest.success")
    assertFileNotExists(self, mpiDebugDir+"/email.success")
    assertFileNotExists(self, mpiDebugDir+"/email.out")
    serialReleaseDir=testBaseDir+"/SERIAL_RELEASE"
    assertFileNotExists(self, serialReleaseDir+"/configure.success")
    assertFileNotExists(self, serialReleaseDir+"/make.success")
    assertFileNotExists(self, serialReleaseDir+"/ctest.success")
    assertFileNotExists(self, serialReleaseDir+"/email.success")
    assertFileNotExists(self, serialReleaseDir+"/email.out")

    # Make sure that the readiness check after the push reports the right
    # status.
    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      "",  # Just the default readiness check!
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      "Skipping getting list of modified files because pull failed!\n" \
      +"Not performing any build cases because no --configure, --build or --test was specified!\n" \
      +"0) MPI_DEBUG => Error, The build/test was never completed! (the file .MPI_DEBUG/email.out. does not exist.) => Not ready to push! (-1.00 min)\n" \
      +"1) SERIAL_RELEASE => Error, The build/test was never completed! (the file .SERIAL_RELEASE/email.out. does not exist.) => Not ready to push! (-1.00 min)\n" \
      +"^INITIAL PULL FAILED: Trilinos:\n"\
      +"REQUESTED ACTIONS: FAILED\n" \
      )

    # Make sure that the readiness check ignoring no --pull returns the right
    # status status.
    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      "--allow-no-pull",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      "Not performing pull since --allow-no-pull was passed in\n" \
      +"Not performing any build cases because no --configure, --build or --test was specified!\n" \
      +"0) MPI_DEBUG => Error, The build/test was never completed! (the file .MPI_DEBUG/email.out. does not exist.) => Not ready to push! (-1.00 min)\n" \
      +"1) SERIAL_RELEASE => Error, The build/test was never completed! (the file .SERIAL_RELEASE/email.out. does not exist.) => Not ready to push! (-1.00 min)\n" \
      +"^FAILED (NOT READY TO PUSH): Trilinos:\n"\
      +"REQUESTED ACTIONS: FAILED\n" \
      )


  def test_send_build_case_email_only_on_failure_do_all_push_pass(self):
    checkin_test_run_case(
      \
      self,
      \
      "test_send_build_case_email_only_on_failure_do_all_push_pass",
      \
      "--make-options=-j3 --ctest-options=-j5" \
      +" --send-build-case-email=only-on-failure --do-all --push" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsFinalPushPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      "Skipping sending build/test case email because everything passed and --send-build-case-email=only-on-failure was set\n" \
      +"0) MPI_DEBUG => passed: passed=100,notpassed=0\n" \
      +"1) SERIAL_RELEASE => passed: passed=100,notpassed=0\n" \
      +"mailx .* trilinos-checkin-tests.*\n" \
      +"^DID PUSH: Trilinos:\n" \
      ,logFileName="checkin-test.other.out"
      )


  # In this test, we test the behavior of the script where git on the path is
  # not found.
  def test_do_all_no_git_installed(self):
    checkin_test_run_case(
      \
      self,
      \
      "do_all_no_git_installed",
      \
      "--make-options=-j3 --ctest-options=-j5" \
      +" --default-builds=MPI_DEBUG" \
      +" --do-all --push" \
      ,
      \
      "IT: git config --get user.email; 0; bogous@somwhere.com\n" \
      +"IT: which git; 1; '/usr/bin/which: no git in (path1:path2:path3)'\n" \
      ,
      \
      False,
      \
      "Error, the .git. command is not in your path. ./usr/bin/which: no git in .path1:path2:path3.." \
      ,
      inPathGit=False \
      ,
      grepForFinalPassFailStr=False \
      )


  def test_do_all_default_builds_mpi_debug_pass(self):
    testName = "do_all_default_builds_mpi_debug_pass"
    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)
    g_test_do_all_default_builds_mpi_debug_pass(self, testName) 
    mpiDebugDir=testBaseDir+"/MPI_DEBUG"
    assertFileExists(self, mpiDebugDir+"/configure.success")
    assertFileExists(self, mpiDebugDir+"/make.success")
    assertFileExists(self, mpiDebugDir+"/ctest.success")
    assertFileExists(self, mpiDebugDir+"/email.success")
    assertFileExists(self, mpiDebugDir+"/email.out")


  def test_local_do_all_default_builds_mpi_debug_pass(self):
    checkin_test_run_case(
      \
      self,
      \
      "local_do_all_default_builds_mpi_debug_pass",
      \
      "--make-options=-j3 --ctest-options=-j5 --default-builds=MPI_DEBUG" \
      +" --extra-pull-from=someremoterepo:master --local-do-all" \
      +" --execute-on-ready-to-push=\"ssh -q godel /some/dir/some_command.sh &\"",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      g_expectedRegexConfigPasses \
      +g_expectedRegexBuildPasses \
      +g_expectedRegexTestPasses \
      +"0) MPI_DEBUG => passed: passed=100,notpassed=0\n" \
      +"1) SERIAL_RELEASE => Test case SERIAL_RELEASE was not run! => Does not affect push readiness!\n" \
      +g_expectedCommonOptionsSummary \
      +"A current successful pull does \*not\* exist => Not ready for final push!\n" \
      +"Explanation: In order to safely push, the local working directory needs\n" \
      +"A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      +"^PASSED [(]NOT READY TO PUSH[)]: Trilinos:\n" \
      +"Not executing final command (ssh -q godel /some/dir/some_command.sh &) since a push is not okay to be performed!\n" \
      )


  def test_local_do_all_detached_head_pass(self):
    checkin_test_run_case(
      \
      self,
      \
      "local_do_all_detached_head_pass",
      \
      "--make-options=-j3 --ctest-options=-j5 --default-builds=MPI_DEBUG" \
      +" --enable-all-packages=off --enable-packages=Teuchos --no-enable-fwd-packages" \
      +" --local-do-all" \
      ,
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsNoTrackingBranchPass(branch="HEAD") \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "enable-packages!=.. and --enable-all-packages=.off. => git diffs w.r.t. tracking branch .will not. be needed to look for changed files!\n" \
      +"No need for repos to be on a branch with a tracking branch!\n" \
      +"Skipping all pulls on request!\n" \
      +g_expectedRegexConfigPasses \
      +g_expectedRegexBuildPasses \
      +g_expectedRegexTestPasses \
      +"0) MPI_DEBUG => passed: passed=100,notpassed=0\n" \
      +"1) SERIAL_RELEASE => Test case SERIAL_RELEASE was not run! => Does not affect push readiness!\n" \
      +g_expectedCommonOptionsSummary \
      +"A current successful pull does \*not\* exist => Not ready for final push!\n" \
      +"A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      +"^PASSED [(]NOT READY TO PUSH[)]: Trilinos:\n" \
      )


  def test_do_all_default_builds_mpi_debug_test_fail_force_push_pass(self):
    checkin_test_run_case(
      \
      self,
      \
      "do_all_default_builds_mpi_debug_test_fail_force_push_pass",
      \
      "--make-options=-j3 --ctest-options=-j5 --default-builds=MPI_DEBUG" \
      " --do-all --force-push --push",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsConfigBuildPasses \
      +"IT: ctest -j5; 1; '80% tests passed, 20 tests failed out of 100'\n" \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsFinalPushPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,      \
      True,
      \
      g_expectedRegexUpdateWithBuildCasePasses \
      +g_expectedRegexConfigPasses \
      +g_expectedRegexBuildPasses \
      +"FAILED: ctest failed returning 1!\n" \
      +"testResultsLine = .80% tests passed, 20 tests failed out of 100.\n" \
      +"0) MPI_DEBUG => FAILED: passed=80,notpassed=20\n" \
      +"1) SERIAL_RELEASE => Test case SERIAL_RELEASE was not run! => Does not affect push readiness!\n" \
      +g_expectedCommonOptionsSummary \
      +"Test: FAILED\n" \
      +"=> A PUSH IS READY TO BE PERFORMED!\n" \
      +"\*\*\* WARNING: The acceptance criteria for doing a push has \*not\*\n" \
      +"\*\*\* been met, but a push is being forced anyway by --force-push!\n" \
      +"DID FORCED PUSH: Trilinos:\n" \
      +"REQUESTED ACTIONS: PASSED\n"
      )


  def test_do_all_default_builds_mpi_debug_then_wipe_clean_pull_pass(self):

    testName = "do_all_default_builds_mpi_debug_then_wipe_clean_pull_pass"

    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)

    # Do the build/test only first (ready to push)
    g_test_do_all_default_builds_mpi_debug_pass(self, testName)

    # Do the push after the fact
    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      "--make-options=-j3 --ctest-options=-j5 --default-builds=MPI_DEBUG" \
      +" --wipe-clean --pull" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +"FT: rm -rf MPI_DEBUG\n" \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "Running: rm -rf MPI_DEBUG\n" \
      +"0) MPI_DEBUG => No configure, build, or test for MPI_DEBUG was requested! => Not ready to push!\n" \
      +"=> A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      +"^PASSED [(]NOT READY TO PUSH[)]: Trilinos:\n"
      )

    assertFileExists(self, testBaseDir+"/pullInitial.success")



  def test_remove_existing_configure_files(self):

    testName = "remove_existing_configure_files"

    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)
    os.mkdir(testBaseDir+"/MPI_DEBUG")
    os.mkdir(testBaseDir+"/MPI_DEBUG/CMakeFiles")
    cmakeCacheFile = testBaseDir+"/MPI_DEBUG/CMakeCache.txt"
    runSysCmnd("touch "+cmakeCacheFile)
    cmakeFilesDir = testBaseDir+"/MPI_DEBUG/CMakeFiles"
    cmakeFilesDummyFile = cmakeFilesDir+"/dummy.txt"
    runSysCmnd("touch "+cmakeFilesDummyFile)

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      " --allow-no-pull --default-builds=MPI_DEBUG" \
      " --enable-packages=Teuchos --configure", \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsDiffOnlyPasses \
      +"FT: rm CMakeCache.txt\n" \
      +"FT: rm -rf CMakeFiles\n" \
      +g_cmndinterceptsConfigPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "Enabled Packages: Teuchos\n" \
      )

    self.assertEqual(os.path.exists(cmakeCacheFile), False)
    self.assertEqual(os.path.exists(cmakeFilesDir), False)
    self.assertEqual(os.path.exists(cmakeFilesDummyFile), False)


  def test_send_email_only_on_failure_do_all_push_pass(self):
    checkin_test_run_case(
      \
      self,
      \
      "send_email_only_on_failure_do_all_push_pass",
      \
      "--make-options=-j3 --ctest-options=-j5" \
      +" --abort-gracefully-if-no-changes-pulled --do-all --push" \
      +" --send-email-only-on-failure" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsFinalPushPasses \
      ,
      \
      True,
      \
      g_expectedRegexUpdateWithBuildCasePasses \
      +g_expectedRegexConfigPasses \
      +g_expectedRegexBuildPasses \
      +g_expectedRegexTestPasses \
      +g_expectedCommonOptionsSummary \
      +"MPI_DEBUG: Skipping sending build/test case email because it passed and --send-email-only-on-failure was set!\n" \
      +"SERIAL_RELEASE: Skipping sending build/test case email because it passed and --send-email-only-on-failure was set!\n" \
      +"Skipping sending final email because it passed and --send-email-only-on-failure was set!\n" \
      )


  def test_abort_gracefully_if_no_enables(self):
    checkin_test_run_case(
      \
      self,
      \
      "abort_gracefully_if_no_enables",
      \
      " --abort-gracefully-if-no-enables --do-all --push",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +g_cmndinterceptsPullOnlyPasses \
      ,
      \
      True,
      \
      "SKIPPED: MPI_DEBUG configure skipped because no packages are enabled!\n" \
      "SKIPPED: MPI_DEBUG build skipped because configure did not pass!\n" \
      "SKIPPED: MPI_DEBUG tests skipped because no packages are enabled!\n" \
      "SKIPPED: SERIAL_RELEASE configure skipped because no packages are enabled!\n" \
      "SKIPPED: SERIAL_RELEASE build skipped because configure did not pass!\n" \
      "SKIPPED: SERIAL_RELEASE tests skipped because no packages are enabled!\n" \
      +"subjectLine = .passed: Trilinos/MPI_DEBUG: skipped configure, build, test due to no enabled packages.\n" \
      +"subjectLine = .passed: Trilinos/SERIAL_RELEASE: skipped configure, build, test due to no enabled packages.\n" \
      +"0) MPI_DEBUG => Skipped configure, build, test due to no enabled packages! => Does not affect push readiness!\n" \
      +"1) SERIAL_RELEASE => Skipped configure, build, test due to no enabled packages! => Does not affect push readiness!\n" \
      +"MPI_DEBUG: Skipping sending build/test case email because there were no enables and --abort-gracefully-if-no-enables was set!\n"
      +"SERIAL_RELEASE: Skipping sending build/test case email because there were no enables and --abort-gracefully-if-no-enables was set!\n"
      +"There were no successful attempts to configure/build/test!\n" \
      +"Skipping sending final email because there were no enables and --abort-gracefully-if-no-enables was set!\n" \
      +"ABORTED DUE TO NO ENABLES: Trilinos:\n" \
      +"REQUESTED ACTIONS: PASSED\n" \
      )


  # ToDo: Add a test case where PT has no enables but ST does!


  def test_do_all_no_append_test_results_push_pass(self):
    testName = "do_all_no_append_test_results_push_pass"
    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)
    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      "--make-options=-j3 --ctest-options=-j5" \
      +" --do-all --no-append-test-results --push",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsFinalPushNoAppendTestResultsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      g_expectedRegexUpdateWithBuildCasePasses \
      +g_expectedRegexConfigPasses \
      +g_expectedRegexBuildPasses \
      +g_expectedRegexTestPasses \
      +"0) MPI_DEBUG => passed: passed=100,notpassed=0\n" \
      +"1) SERIAL_RELEASE => passed: passed=100,notpassed=0\n" \
      +g_expectedCommonOptionsSummary \
      +"Skipping appending test results on request (--no-append-test-results)!\n" \
      +"=> A PUSH IS READY TO BE PERFORMED!\n" \
      +"^DID PUSH: Trilinos:\n" \
      )


  def test_do_all_no_rebase_push_pass(self):
    checkin_test_run_case(
      \
      self,
      \
      "do_all_no_rebase_push_pass",
      \
      "--make-options=-j3 --ctest-options=-j5" \
      +" --do-all --no-rebase --push",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsFinalPushNoRebasePasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      g_expectedRegexUpdateWithBuildCasePasses \
      +g_expectedRegexConfigPasses \
      +g_expectedRegexBuildPasses \
      +g_expectedRegexTestPasses \
      +"0) MPI_DEBUG => passed: passed=100,notpassed=0\n" \
      +"1) SERIAL_RELEASE => passed: passed=100,notpassed=0\n" \
      +g_expectedCommonOptionsSummary \
      +"Skipping the final rebase on request! (see --no-rebase option)\n" \
      +"=> A PUSH IS READY TO BE PERFORMED!\n" \
      +"^DID PUSH: Trilinos:\n" \
      )


  def test_extra_repo_1_explicit_enable_configure_pass(self):

    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"

    testName = "extra_repo_1_explicit_enable_configure_pass"

    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)

    writeStrToFile(testBaseDir+"/MPI_DEBUG_ST.config",
      "-DTPL_ENABLE_MPI:BOOL=ON\n" \
      +"-DTeuchos_ENABLE_SECONDARY_TESTED_CODE:BOOL=ON\n" \
      )

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      "--extra-repos=preCopyrightTrilinos --allow-no-pull --without-default-builds" \
      " --extra-builds=MPI_DEBUG_ST --enable-packages=Stalix --configure", \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsDiffOnlyPassesPreCopyrightTrilinos \
      +g_cmndinterceptsConfigPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "[|] ID [|] Repo Dir             [|] Branch        [|] Tracking Branch       [|] C [|] M [|] [?] [|]\n" \
      "[|]  0 [|] MockTrilinos [(]Base[)]  [|] currentbranch [|] origin/trackingbranch [|] 4 [|]   [|]   [|]\n" \
      "[|]  1 [|] preCopyrightTrilinos [|] currentbranch [|] origin/trackingbranch [|] 4 [|]   [|]   [|]\n" \
      "-extra-repos=.preCopyrightTrilinos.\n" \
      +"Pulling in packages from POST extra repos: preCopyrightTrilinos ...\n" \
      +"projectDepsXmlFileOverride="+projectDepsXmlFileOverride+"\n" \
      +"Enabling only the explicitly specified packages .Stalix. ...\n" \
      +"Trilinos_EXTRA_REPOSITORIES:STRING=preCopyrightTrilinos\n" \
      +"Enabled Packages: Stalix\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride,
        "GITDIST_UNIT_TEST_STTY_SIZE=60 120" \
         ]
      )
    # NOTE: Above, we set GITDIST_UNIT_TEST_STTY_SIZE=120 so that
    # checkin-test.py will print the full table regardless what the terminal
    # size is in the env where this runs.


  def test_extra_repo_1_implicit_enable_configure_pass(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_implicit_enable_configure_pass",
      \
      "--extra-repos=preCopyrightTrilinos --allow-no-pull --default-builds=MPI_DEBUG --configure", \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsDiffOnlyPassesPreCopyrightTrilinos \
      +g_cmndinterceptsConfigPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "-extra-repos=.preCopyrightTrilinos.\n" \
      +"Pulling in packages from POST extra repos: preCopyrightTrilinos ...\n" \
      +"projectDepsXmlFileOverride="+projectDepsXmlFileOverride+"\n" \
      +"Modified file: .preCopyrightTrilinos/teko/CMakeLists.txt.\n" \
      +"  => Enabling .Teko.!\n" \
      +"Teko of type ST is being excluded because it is not in the valid list of package types .PT.\n" \
      +"Trilinos_EXTRA_REPOSITORIES:STRING=preCopyrightTrilinos\n" \
      +"Enabled Packages: Teuchos, Teko\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_1_do_all_push_pass(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_do_all_push_pass",
      \
      "--make-options=-j3 --ctest-options=-j5" \
      " --extra-repos=preCopyrightTrilinos --default-builds=MPI_DEBUG --do-all --push", \
      \
      g_cmndinterceptsExtraRepo1DoAllUpToPush \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsPushOnlyPasses \
      +g_cmndinterceptsPushOnlyPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "-extra-repos=.preCopyrightTrilinos.\n" \
      +"Pulling in packages from POST extra repos: preCopyrightTrilinos ...\n" \
      +"Enabling .Teko..\n" \
      +"Teko of type ST is being excluded because it is not in the valid list of package types .PT.\n" \
      +"'': Pulled changes from this repo!\n" \
      +".preCopyrightTrilinos.: Pulled changes from this repo!\n" \
      +"pullInitial.preCopyrightTrilinos.out\n" \
      +"Pull passed!\n"\
      +"All of the tests ran passed!\n" \
      +"pullFinal.preCopyrightTrilinos.out\n" \
      +"Final pull passed!\n" \
      +"commitFinalBody.preCopyrightTrilinos.out\n" \
      +"commitFinal.preCopyrightTrilinos.out\n" \
      +"push.preCopyrightTrilinos.out\n" \
      +"Push passed!\n" \
      +"Enabled Packages: Teuchos, Teko\n" \
      +"DID PUSH: Trilinos:\n" \
      +"REQUESTED ACTIONS: PASSED\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_pull_extra_pull_allrepos_pass(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_pull_extra_pull_pass",
      \
      "--extra-repos=preCopyrightTrilinos --pull --extra-pull-from=somemachine:someotherbranch", \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +"IT: git pull somemachine someotherbranch; 0; 'git extra pull passed'\n"
      +"IT: git pull somemachine someotherbranch; 0; 'git extra pull passed'\n"
      +cmndinterceptsGetRepoStatsPass(numCommits="1") \
      +cmndinterceptsGetRepoStatsPass(numCommits="3") \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsDiffOnlyPassesPreCopyrightTrilinos \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "pullInitial.out\n" \
      "pullInitial.preCopyrightTrilinos.out\n" \
      "pullInitialExtra.out\n" \
      "pullInitialExtra.preCopyrightTrilinos.out\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )
    # NOTE: In the above scenario, there are no local changes until the
    # --extra-pull-from pulls in commits.


  def test_extra_repo_pull_extra_pull_baserepo_pass(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_pull_extra_pull_pass",
      \
      "--extra-repos=preCopyrightTrilinos --pull --extra-pull-from=:somemachine:someotherbranch", \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +"IT: git pull somemachine someotherbranch; 0; 'git extra pull passed'\n"
      +cmndinterceptsGetRepoStatsPass(numCommits="1") \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "pullInitial.out\n" \
      "pullInitial.preCopyrightTrilinos.out\n" \
      "pullInitialExtra.out\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )
    # NOTE: In the above scenario, there are no local changes until the
    # --extra-pull-from pulls in commits to the base repo only!.


  def test_extra_repo_pull_extra_pull_repo0_pass(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_pull_extra_pull_pass",
      \
      "--extra-repos=preCopyrightTrilinos --pull --extra-pull-from=preCopyrightTrilinos:somemachine:someotherbranch", \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +"IT: git pull somemachine someotherbranch; 0; 'git extra pull passed'\n"
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +cmndinterceptsGetRepoStatsPass(numCommits="1") \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "pullInitial.out\n" \
      "pullInitial.preCopyrightTrilinos.out\n" \
      "3.c.0. Git Repo: preCopyrightTrilinos\n" \
      "pullInitialExtra.preCopyrightTrilinos.out\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )
    # NOTE: In the above scenario, there are no local changes until the
    # --extra-pull-from pulls in commits to the extra repo only!.


  def test_extra_repo_pull_extra_pull_allrepos_repo0_pass(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_pull_extra_pull_pass",
      \
      "--extra-repos=preCopyrightTrilinos --pull " \
        +" --extra-pull-from=remote0:remotebranch0,preCopyrightTrilinos:remote1:remotebranch1", \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +"IT: git pull remote0 remotebranch0; 0; 'git extra pull passed'\n"
      +"IT: git pull remote0 remotebranch0; 0; 'git extra pull passed'\n"
      +"IT: git pull remote1 remotebranch1; 0; 'git extra pull passed'\n"
      +cmndinterceptsGetRepoStatsPass(numCommits="2") \
      +cmndinterceptsGetRepoStatsPass(numCommits="1") \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "pullInitial.out\n" \
      "pullInitial.preCopyrightTrilinos.out\n" \
      "3.c.0. Git Repo: \n" \
      "3.c.1. Git Repo: preCopyrightTrilinos\n" \
      "pullInitialExtra.out\n" \
      "pullInitialExtra.preCopyrightTrilinos.out\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )
    # NOTE: In the above scenario, there are no local changes until the
    # --extra-pull-from pulls in commits to the extra repo only!.


  def test_extra_repo_pull_extra_pull_repo0_repo1_pass(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_pull_extra_pull_pass",
      \
      "--extra-repos=preCopyrightTrilinos,extraTrilinosRepo --pull " \
        +" --extra-pull-from=extraTrilinosRepo:remote0:remotebranch0,preCopyrightTrilinos:remote1:remotebranch1", \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +"IT: git pull remote1 remotebranch1; 0; 'git extra pull passed'\n"
      +"IT: git pull remote0 remotebranch0; 0; 'git extra pull passed'\n"
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +cmndinterceptsGetRepoStatsPass(numCommits="1") \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "pullInitial.out\n" \
      "pullInitial.preCopyrightTrilinos.out\n" \
      "pullInitial.extraTrilinosRepo.out\n" \
      "3.c.0. Git Repo: preCopyrightTrilinos\n" \
      "3.c.1. Git Repo: extraTrilinosRepo\n" \
      "pullInitialExtra.preCopyrightTrilinos.out\n" \
      "pullInitialExtra.extraTrilinosRepo.out\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )
    # NOTE: In the above scenario, there are no local changes until the
    # --extra-pull-from pulls in commits to the extra repo only!.


  def test_extra_repo_1_trilinos_changes_do_all_push_pass(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_trilinos_changes_do_all_push_pass",
      \
      "--make-options=-j3 --ctest-options=-j5" \
      " --extra-repos=preCopyrightTrilinos --default-builds=MPI_DEBUG --do-all --push", \
      \
      g_cmndinterceptsExtraRepo1TrilinosChangesDoAllUpToPush \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsPushOnlyPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "==> ..: Has modified files!\n" \
      +"==> .preCopyrightTrilinos.: Does .not. have any modified files!\n" \
      +"Skipping push to .preCopyrightTrilinos. because there are no commits!\n" \
      +"Push passed!\n" \
      +"DID PUSH: Trilinos:\n" \
      +"REQUESTED ACTIONS: PASSED\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_1_extra_repo_changes_do_all_push_pass(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_extra_repo_changes_do_all_push_pass",
      \
      "--make-options=-j3 --ctest-options=-j5" \
      " --enable-packages=Teuchos" \
      " --extra-repos=preCopyrightTrilinos --default-builds=MPI_DEBUG --do-all --push", \
      \
      g_cmndinterceptsExtraRepo1ExtraRepoChangesDoAllUpToPush \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsPushOnlyPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "==> ..: Does .not. have any modified files!\n" \
      +"==> .preCopyrightTrilinos.: Has modified files!\n" \
      +"Skipping push to .. because there are no commits!\n" \
      +"Push passed!\n" \
      +"DID PUSH: Trilinos:\n" \
      +"REQUESTED ACTIONS: PASSED\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_1_abort_gracefully_if_no_changes_pulled_no_updates_passes(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "test_extra_repo_1_abort_gracefully_if_no_changes_pulled_no_updates_passes",
      \
      "--extra-repos=preCopyrightTrilinos --abort-gracefully-if-no-changes-pulled --do-all --pull", \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      +g_cmndinterceptsDiffOnlyPassesPreCopyrightTrilinos \
      +g_cmndinterceptsLogCommitsPasses \
      ,
      \
      True,
      \
      "Pulling in packages from POST extra repos: preCopyrightTrilinos ...\n" \
      +"Did not pull any changes from this repo!\n" \
      +"No changes were pulled!\n" \
      +"Not performing any build cases because pull did not bring any [*]new[*] commits" \
        " and --abort-gracefully-if-no-changes-pulled was set!\n" \
      +"Skipping sending final email because there were no updates" \
          " and --abort-gracefully-if-no-changes-pulled was set!\n" \
      +"ABORTED DUE TO NO UPDATES\n" \
      +"REQUESTED ACTIONS: PASSED\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )
    # NOTE: The above test is the case where that are existing local commits
    # in the extra repo but no new commits are pulled so the test is aborted.


  def test_extra_repo_1_extra_pull_abort_gracefully_if_no_changes_pulled_no_updates_passes(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_extra_pull_abort_gracefully_if_no_changes_pulled_no_updates_passes",
      \
      "--extra-repos=preCopyrightTrilinos --abort-gracefully-if-no-changes-pulled" \
      +" --extra-pull-from=machine:master --do-all --pull" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      +g_cmndinterceptsDiffOnlyPassesPreCopyrightTrilinos \
      +g_cmndinterceptsLogCommitsPasses \
      ,
      \
      True,
      \
      "Pulling in packages from POST extra repos: preCopyrightTrilinos ...\n" \
      +"Did not pull any changes from this repo!\n" \
      +"No changes were pulled!\n" \
      +"Not performing any build cases because pull did not bring any [*]new[*] commits" \
        " and --abort-gracefully-if-no-changes-pulled was set!\n" \
      +"Skipping sending final email because there were no updates" \
          " and --abort-gracefully-if-no-changes-pulled was set!\n" \
      +"ABORTED DUE TO NO UPDATES\n" \
      +"REQUESTED ACTIONS: PASSED\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_1_extra_pull_abort_gracefully_if_no_changes_pulled_main_repo_update(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_extra_pull_abort_gracefully_if_no_changes_pulled_main_repo_update",
      \
      "--extra-repos=preCopyrightTrilinos --abort-gracefully-if-no-changes-pulled" \
      +" --extra-pull-from=machine:master --pull" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      +g_cmndinterceptsDiffOnlyPassesPreCopyrightTrilinos \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "Pulled changes from this repo!\n" \
      +"Did not pull any changes from this repo!\n" \
      +"There where at least some changes pulled!\n" \
      +"Pull passed!\n" \
      +"NOT READY TO PUSH\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_1_extra_pull_abort_gracefully_if_no_changes_pulled_extra_repo_update(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_extra_pull_abort_gracefully_if_no_changes_pulled_extra_repo_update",
      \
      "--extra-repos=preCopyrightTrilinos --abort-gracefully-if-no-changes-pulled" \
      +" --extra-pull-from=machine:master --pull" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      +g_cmndinterceptsDiffOnlyPassesPreCopyrightTrilinos \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "Pulled changes from this repo!\n" \
      +"Did not pull any changes from this repo!\n" \
      +"There where at least some changes pulled!\n" \
      +"Pull passed!\n" \
      +"NOT READY TO PUSH\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_1_extra_pull_abort_gracefully_if_no_changes_pulled_main_repo_extra_update(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_extra_pull_abort_gracefully_if_no_changes_pulled_main_repo_extra_update",
      \
      "--extra-repos=preCopyrightTrilinos --abort-gracefully-if-no-changes-pulled" \
      +" --extra-pull-from=machine:master --pull" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      +cmndinterceptsGetRepoStatsPass(numCommits="5") \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +g_cmndinterceptsDiffOnlyPassesPreCopyrightTrilinos \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "Pulled changes from this repo!\n" \
      +"Did not pull any changes from this repo!\n" \
      +"There where at least some changes pulled!\n" \
      +"Pull passed!\n" \
      +"NOT READY TO PUSH\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_1_extra_pull_abort_gracefully_if_no_changes_pulled_extra_repo_extra_update(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_extra_pull_abort_gracefully_if_no_changes_pulled_extra_repo_extra_update",
      \
      "--extra-repos=preCopyrightTrilinos --abort-gracefully-if-no-changes-pulled" \
      +" --extra-pull-from=machine:master --pull" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +cmndinterceptsGetRepoStatsPass(numCommits="1") \
      +g_cmndinterceptsDiffOnlyPassesPreCopyrightTrilinos \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "Pulled changes from this repo!\n" \
      +"Did not pull any changes from this repo!\n" \
      +"There where at least some changes pulled!\n" \
      +"Pull passed!\n" \
      +"NOT READY TO PUSH\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_1_abort_gracefully_if_no_changes_to_push_passes(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_abort_gracefully_if_no_changes_to_push_passes",
      \
      "--extra-repos=preCopyrightTrilinos --abort-gracefully-if-no-changes-to-push" \
        +" --do-all --pull", \
      \
      g_cmndinterceptsExtraRepo1ThroughStatusNoChangesPasses \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      +g_cmndinterceptsPullOnlyNoUpdatesPasses \
      ,
      \
      True,
      \
      "Pulling in packages from POST extra repos: preCopyrightTrilinos ...\n" \
      +"Did not pull any changes from this repo!\n" \
      +"No changes were pulled!\n" \
      +"Not performing any build cases because there are no local changes to push" \
        " and --abort-gracefully-if-no-changes-to-push!\n" \
      +"Skipping sending final email because there are no local changes to push" \
          " and --abort-gracefully-if-no-changes-to-push was set!\n" \
      +"ABORTED DUE TO NO CHANGES TO PUSH\n" \
      +"REQUESTED ACTIONS: PASSED\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_file_2_continuous_pull(self):

    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"

    testName = "test_extra_repo_file_2_continuous_pull"

    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)

    writeStrToFile(testBaseDir+"/MPI_DEBUG_ST.config",
      "-DTPL_ENABLE_MPI:BOOL=ON\n" \
      +"-DTeuchos_ENABLE_SECONDARY_TESTED_CODE:BOOL=ON\n" \
      )

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      " --extra-repos-file="+g_testBaseDir+"/ExtraReposListExisting_2.cmake" \
      " --extra-repos-type=Continuous" \
      " --extra-builds=MPI_DEBUG_ST --enable-packages=Stalix --pull", \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsDiffOnlyPassesPreCopyrightTrilinos \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "-extra-repos-file=.*ExtraReposListExisting_2.\n" \
      +"-extra-repos-type=.Continuous.\n" \
      +"Generate a Python datastructure containing TriBITS/git repos\n" \
      +"Selecting the set of .Continuous. extra repos .asserting all selected repos exist. [.][.][.]\n" \
      +"Adding POST extra Continuous repository preCopyrightTrilinos [.][.][.]\n" \
      +".NOT. adding POST extra Nightly repository extraTrilinosRepo\n" \
      +"Pulling in packages from POST extra repos: preCopyrightTrilinos [.][.][.]\n" \
      +"projectDepsXmlFileOverride="+projectDepsXmlFileOverride+"\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )
  # NOTE: The above test also greps the checkin-test.out log file to make sure
  # that the selection process for the repos is show, including what repos are
  # *NOT* selected.


  def test_extra_repo_file_2_nightly_pull(self):

    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.extraTrilinosRepo.gold.xml"

    testName = "test_extra_repo_file_2_nightly_pull"

    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)

    writeStrToFile(testBaseDir+"/MPI_DEBUG_ST.config",
      "-DTPL_ENABLE_MPI:BOOL=ON\n" \
      +"-DTeuchos_ENABLE_SECONDARY_TESTED_CODE:BOOL=ON\n" \
      )

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      " --extra-repos-file="+g_testBaseDir+"/ExtraReposListExisting_2.cmake" \
      " --extra-repos-type=Nightly" \
      " --extra-builds=MPI_DEBUG_ST --pull", \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +cmndinterceptsGetRepoStatsPass() \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsDiffOnlyPassesPreCopyrightTrilinos \
      +g_cmndinterceptsDiffOnlyPassesExtraTrilinosRepo \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "-extra-repos-file=.*ExtraReposListExisting_2.\n" \
      +"-extra-repos-type=.Nightly.\n" \
      +"Pulling in packages from POST extra repos: preCopyrightTrilinos,extraTrilinosRepo ...\n" \
      +"cmake .* -DTrilinos_EXTRA_REPOSITORIES=.preCopyrightTrilinos.extraTrilinosRepo. .* -P .*/TribitsDumpDepsXmlScript.cmake\n" \
      +"projectDepsXmlFileOverride="+projectDepsXmlFileOverride+"\n" \
      +"3.a.1) Git Repo: .preCopyrightTrilinos.\n" \
      +"3.a.2) Git Repo: .extraTrilinosRepo.\n" \
      +"Running in working directory: .*MockTrilinos/preCopyrightTrilinos ...\n" \
      +"Running in working directory: .*MockTrilinos/extraTrilinosRepo ...\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_file_3_continuous_pull_configure(self):

    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"

    testName = "test_extra_repo_file_3_continuous_pull_configure"

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      " --extra-repos-file="+g_testBaseDir+"/ExtraReposListExisting_3.cmake" \
      " --extra-repos-type=Continuous" \
      " --default-builds=MPI_DEBUG --pull --configure", \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +cmndinterceptsGetRepoStatsPass(numCommits="0") \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +"IT: git diff --name-status origin/trackingbranch; 0; 'M\tExtraTeuchosStuff.hpp'\n" \
      +g_cmndinterceptsConfigPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "-extra-repos-file=.*ExtraReposListExisting_3.\n" \
      +"-extra-repos-type=.Continuous.\n" \
      +"Pulling in packages from POST extra repos: preCopyrightTrilinos ...\n" \
      +"projectDepsXmlFileOverride="+projectDepsXmlFileOverride+"\n" \
      +"cmake .* -DTrilinos_EXTRA_REPOSITORIES=.preCopyrightTrilinos. .* -P .*/TribitsDumpDepsXmlScript.cmake\n" \
      +"Modified file: .packages/teuchos/extrastuff/ExtraTeuchosStuff.hpp.\n" \
      +"=> Enabling .Teuchos.!\n" \
      +"Full package enable list: .Teuchos.\n" \
      ,
      [
      ("MPI_DEBUG/do-configure",
       "\-DTrilinos_EXTRA_REPOSITORIES:STRING=preCopyrightTrilinos\n"+ \
       "\-DTrilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Continuous\n"+\
       "\-DTrilinos_EXTRAREPOS_FILE=.*/ExtraReposListExisting_3.cmake\n" \
       ), \
      ],
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_file_3_continuous_commits_but_no_diff_do_all_push(self):

    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.gold.xml"

    testName = "test_extra_repo_file_3_continuous_do_all_push"

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      " --extra-repos-file="+g_testBaseDir+"/ExtraReposListExisting_3.cmake" \
      " --extra-repos-type=Continuous" \
      " --extra-repos=ExtraTeuchosRepo" \
      " --make-options=-j3 --ctest-options=-j5" \
      " --default-builds=MPI_DEBUG --do-all --push", \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +"IT: git diff --name-status origin/trackingbranch; 0; ''\n" \
      +"IT: git diff --name-status origin/trackingbranch; 0; 'M\tExtraTeuchosStuff.hpp'\n" \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsFinalPullRebasePasses \
      +g_cmndinterceptsFinalPullRebasePasses \
      +g_cmnginterceptsGitLogCmnds \
      +g_cmndinterceptsAmendCommitPasses \
      +g_cmnginterceptsGitLogCmnds \
      +g_cmndinterceptsAmendCommitPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsPushOnlyPasses \
      +g_cmndinterceptsPushOnlyPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "-extra-repos-file=.*ExtraReposListExisting_3.\n" \
      +"-extra-repos-type=.Continuous.\n" \
      +"Modified file: .packages/teuchos/extrastuff/ExtraTeuchosStuff.hpp.\n" \
      +"=> Enabling .Teuchos.!\n" \
      +"Full package enable list: .Teuchos.\n" \
      +"push.ExtraTeuchosRepo.out\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )
    # NOTE: Above, the base repo has local commits but no modified files.
    # This is a rare situation but it does happen in real practice from time
    # to time.  Therefore, this is a valuable test case.

  def test_extra_repo_file_4_continuous_pull_configure(self):

    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preRepoOnePackage.preCopyrightTrilinos.gold.xml"

    testName = "test_extra_repo_file_4_continuous_pull_configure"

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      " --extra-repos-file="+g_testBaseDir+"/ExtraReposListExisting_4.cmake" \
      " --extra-repos-type=Continuous" \
      " --default-builds=MPI_DEBUG --pull --configure", \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +cmndinterceptsGetRepoStatsPass() \
      +cmndinterceptsGetRepoStatsPass() \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +"IT: git diff --name-status origin/trackingbranch; 0; ''\n" \
      +"IT: git diff --name-status origin/trackingbranch; 0; 'M\tpreRepoOnePackage.cpp'\n" \
      +"IT: git diff --name-status origin/trackingbranch; 0; ''\n" \
      +"IT: git diff --name-status origin/trackingbranch; 0; 'M\tExtraTeuchosStuff.hpp'\n" \
      +g_cmndinterceptsConfigPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "-extra-repos-file=.*ExtraReposListExisting_4.\n" \
      +"-extra-repos-type=.Continuous.\n" \
      +"Pulling in packages from PRE extra repos: preRepoOnePackage [.][.][.]\n" \
      +"Pulling in packages from POST extra repos: preCopyrightTrilinos [.][.][.]\n" \
      +"projectDepsXmlFileOverride="+projectDepsXmlFileOverride+"\n" \
      +"cmake .* -DTrilinos_PRE_REPOSITORIES=.preRepoOnePackage. -DTrilinos_EXTRA_REPOSITORIES=.preCopyrightTrilinos. .* -P .*/TribitsDumpDepsXmlScript.cmake\n" \
      +".preRepoOnePackage.: Has modified files\n" \
      +".ExtraTeuchosRepo.: Has modified files\n" \
      +"Modified file: .preRepoOnePackage/preRepoOnePackage.cpp.\n" \
      +"=> Enabling .preRepoOnePackage.\n" \
      +"Modified file: .packages/teuchos/extrastuff/ExtraTeuchosStuff.hpp.\n" \
      +"=> Enabling .Teuchos.\n" \
      +"Full package enable list: .preRepoOnePackage,Teuchos.\n" \
      ,
      [
      ("MPI_DEBUG/do-configure",
       "\-DTrilinos_EXTRA_REPOSITORIES:STRING=preCopyrightTrilinos\n"+ \
       "\-DTrilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Continuous\n"+\
       "\-DTrilinos_EXTRAREPOS_FILE=.*/ExtraReposListExisting_4.cmake\n" \
       ), \
      ],
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_file_project_nightly_nothing_fail(self):

    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"

    testName = "test_extra_repo_file_project_nightly_nothing_fail"

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      " --extra-repos-file=project" \
      " --extra-repos-type=Nightly" , \
      \
      "" \
      ,
      \
      False,
      \
      "ERROR! Skipping missing extra repo .Dakota. since\n" \
      "MockTrilinos/packages/TriKota/Dakota\n" \
      "Error, the command ..*cmake .*TribitsGetExtraReposForCheckinTest.cmake\n" \
      ,
      mustHaveCheckinTestOut=False \
      ,
      grepForFinalPassFailStr=False \
      )
    # NOTE: Above, this fails because Dakota listed as a Nightly is missing.
    # This aborts the script with the exception.


  def test_extra_repo_file_project_continuous_extra_repos_pull(self):

    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"

    testName = "test_extra_repo_file_project_continuous_extra_repos_pull"

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      " --extra-repos-file=project" \
      " --extra-repos-type=Continuous" \
      " --extra-repos=preCopyrightTrilinos" \
      " --pull", \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsDiffOnlyPassesPreCopyrightTrilinos \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "-extra-repos-file=.project.\n" \
      +"-extra-repos-type=.Continuous.\n" \
      +"Pulling in packages from POST extra repos: preCopyrightTrilinos ...\n" \
      +"projectDepsXmlFileOverride="+projectDepsXmlFileOverride+"\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_file_missing_assert_fail(self):

    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"

    testName = "test_extra_repo_file_missing_assert_fail"

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      " --extra-repos-file="+g_testBaseDir+"/ExtraReposListExisting1Missing1.cmake" \
      " --extra-repos-type=Continuous" , \
      \
      "" \
      ,
      \
      False,
      \
      "ERROR! Skipping missing extra repo .MissingRepo. since\n" \
      "Error, the command ..*cmake .*TribitsGetExtraReposForCheckinTest.cmake\n" \
      ,
      mustHaveCheckinTestOut=False \
      ,
      grepForFinalPassFailStr=False \
      )



  def test_extra_repo_file_missing_ignore_pull(self):

    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"

    testName = "test_extra_repo_file_missing_ignore_pull"

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      " --extra-repos-file="+g_testBaseDir+"/ExtraReposListExisting1Missing1.cmake" \
      " --extra-repos-type=Continuous --ignore-missing-extra-repos --pull" , \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsDiffOnlyPassesPreCopyrightTrilinos \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "NOTE: Ignoring missing extra repo .MissingRepo. as requested since\n" \
      "Pulling in packages from POST extra repos: preCopyrightTrilinos ...\n" \
      ,
      mustHaveCheckinTestOut=False
      )


#  def test_extra_repo_file_default_pull_configure(self):
#    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
#    checkin_test_run_case(
#      \
#      self,
#      \
#      "extra_repo_file_default_pull_configure",
#      \
#      "--extra-repos-file=default --pull --configure", \
#      \
#      "IT: .*cmake .+ -P .+/TribitsDumpDepsXmlScript.cmake; 0; 'dump XML file passed'\n" \
##      +g_cmndinterceptsDiffOnlyPasses \
#      ,
#      \
#      True,
#      \
#      "-extra-repos-file=.default.\n"+ \
#      "Dummy" \
#      ,
#      \
#      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
#      )


  def test_abort_gracefully_if_no_changes_pulled_status_fails(self):
    checkin_test_run_case(
      \
      self,
      \
      "abort_gracefully_if_no_changes_pulled_status_fails",
      \
      "--abort-gracefully-if-no-changes-pulled --do-all --pull" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass(changedFile=" M somefile") \
      +"IT: git status; 0; 'Git status returned changed but not updated'\n" \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      "ERROR: There are changed uncommitted files => cannot continue!\n" \
      +"Git status returned changed but not updated\n" \
      +"No changes were pulled!\n" \
      +"Skipping getting list of modified files because pull failed!\n" \
      +"Not running any build/test cases because the pull failed!\n" \
      +"  => A PUSH IS .NOT. READY TO BE PERFORMED!\n" \
      +"INITIAL PULL FAILED\n" \
      +"To find out more about this failure, grep the .checkin-test.out. log\n" \
      +"REQUESTED ACTIONS: FAILED\n" \
      )

    
  # B) Test package enable/disable logic


  # NOTE: The setting of built-in cmake cache variables in do-configure[.base]
  # files is tested in the unit test test_do_all_commit_push_pass(...)


  def test_read_config_files_mpi_debug(self):
    
    testName = "read_config_files"

    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)

    writeStrToFile(testBaseDir+"/COMMON.config",
      "-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON\n" \
      +"-DBUILD_SHARED:BOOL=ON\n" \
      +"-DTPL_BLAS_LIBRARIES:PATH=/usr/local/libblas.a\n" \
      +"-DTPL_LAPACK_LIBRARIES:PATH=/usr/local/liblapack.a\n" \
      )

    writeStrToFile(testBaseDir+"/MPI_DEBUG.config",
      "-DMPI_BASE_DIR:PATH=/usr/lib64/openmpi/1.2.7-gcc\n" \
      "-DMPI_CXX_COMPILER:PATHNAME=/usr/lib64/openmpi/1.2.7-gcc/mpicxx\n" \
      "-DMPI_C_COMPILER:PATHNAME=/usr/lib64/openmpi/1.2.7-gcc/mpicc\n" \
      "-DMPI_Fortran_COMPILER:PATHNAME=/usr/lib64/openmpi/1.2.7-gcc/mpif77\n" \
      )

    checkin_test_configure_test(
      \
      self,
      \
      testName,
      \
      "--default-builds=MPI_DEBUG",
      \
      [
      ("MPI_DEBUG/do-configure.base",
       "\-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON\n" \
       +"\-DBUILD_SHARED:BOOL=ON\n" \
       +"\-DTrilinos_TRIBITS_DIR:PATH=.*\n" \
       +"\-DTrilinos_TEST_CATEGORIES:STRING=BASIC\n" \
       +"\-DTrilinos_ENABLE_SECONDARY_TESTED_CODE:BOOL=OFF\n" \
       +"\-DTPL_BLAS_LIBRARIES:PATH=/usr/local/libblas.a\n" \
       +"\-DTPL_LAPACK_LIBRARIES:PATH=/usr/local/liblapack.a\n"
       +"\-DMPI_BASE_DIR:PATH=/usr/lib64/openmpi/1.2.7-gcc\n" \
       +"\-DMPI_CXX_COMPILER:PATHNAME=/usr/lib64/openmpi/1.2.7-gcc/mpicxx\n" \
       +"\-DMPI_C_COMPILER:PATHNAME=/usr/lib64/openmpi/1.2.7-gcc/mpicc\n" \
       +"\-DMPI_Fortran_COMPILER:PATHNAME=/usr/lib64/openmpi/1.2.7-gcc/mpif77\n" \
       ),
      ]
      )


  def test_set_test_categories(self):
 
    checkin_test_configure_test(
      \
      self,
      \
      "set_test_categories",
      \
      "--default-builds=MPI_DEBUG --test-categories=NIGHTLY",
      \
      [
      ("MPI_DEBUG/do-configure.base",
       "\-DTrilinos_TEST_CATEGORIES:STRING=NIGHTLY\n" \
       ),
      ]
      )


  def test_relative_src_dir(self):

    testName = "relative_src_dir"
    testDir = os.path.join(os.getcwd(), g_checkin_test_tests_dir, testName)
    relativePathToSrc = os.path.relpath(mockProjectBaseDir, testDir)
    #print "relativePathToSrc = " + relativePathToSrc
 
    checkin_test_configure_test(
      \
      self,
      \
      testName,
      \
      "--src-dir="+relativePathToSrc+" --default-builds=MPI_DEBUG",
      \
      [
      ("MPI_DEBUG/do-configure.base",
       mockProjectBaseDir \
       ),
      ],
      extraPassRegexStr = \
      "src-dir=."+relativePathToSrc+".\n"
      )


  def test_auto_enable(self):
    checkin_test_configure_enables_test(
      \
      self,
      \
      "auto_enable",
      \
      "", # Allow auto-enable of Teuchos!
      \
      "\-DTrilinos_ENABLE_Teuchos:BOOL=ON\n" \
      )


  def test_enable_packages(self):
    checkin_test_configure_enables_test(
      \
      self,
      \
      "enable_packages",
      \
      "--enable-packages=TrilinosFramework,RTOp,Thyra",
      \
      "\-DTrilinos_ENABLE_TrilinosFramework:BOOL=ON\n" \
      +"\-DTrilinos_ENABLE_RTOp:BOOL=ON\n" \
      +"\-DTrilinos_ENABLE_Thyra:BOOL=ON\n" \
      ,
      \
      "\-DTrilinos_ENABLE_Teuchos:BOOL=ON\n" \
      )
    # Above, the --enable-packages option turns off the check of the modified
    # files and set the enables manually.


  def test_enable_extra_packages(self):
    checkin_test_configure_enables_test(
      \
      self,
      \
      "enable_extra_packages",
      \
      "--enable-extra-packages=RTOp,Thyra",
      \
      "\-DTrilinos_ENABLE_Teuchos:BOOL=ON\n" \
      +"\-DTrilinos_ENABLE_RTOp:BOOL=ON\n" \
      +"\-DTrilinos_ENABLE_Thyra:BOOL=ON\n" \
      ,
      extraPassRegexStr=\
      "Enabling extra explicitly specified packages .RTOp,Thyra.\n" \
      "Final package enable list: \[Teuchos,RTOp,Thyra\]\n" \
      "Full package enable list: \[Teuchos,RTOp,Thyra\]\n" \
      )
    # Above, the --enable-extra-packages option leave on the check of the modified
    # files ahd just appends the set of enabled packages


  def test_disable_packages(self):
    checkin_test_configure_enables_test(
      \
      self,
      \
      "disable_packages",
      \
      "--disable-packages=Tpetra,Thyra",
      \
      "\-DTrilinos_ENABLE_Teuchos:BOOL=ON\n" \
      +"\-DTrilinos_ENABLE_Tpetra:BOOL=OFF\n" \
      +"\-DTrilinos_ENABLE_Thyra:BOOL=OFF\n" \
      )
    # Above: --disable-packages does not turn off auto-enable and therefore
    # Teuchos is picked up.


  def test_enable_disable_packages(self):
    checkin_test_configure_enables_test(
      \
      self,
      \
      "enable_disable_packages",
      \
      "--enable-packages=TrilinosFramework,RTOp,Thyra,Tpetra" \
      +" --disable-packages=Tpetra,Stratimikos",
      \
      "\-DTrilinos_ENABLE_TrilinosFramework:BOOL=ON\n" \
      +"\-DTrilinos_ENABLE_RTOp:BOOL=ON\n" \
      +"\-DTrilinos_ENABLE_Thyra:BOOL=ON\n" \
      +"\-DTrilinos_ENABLE_Tpetra:BOOL=OFF\n" \
      +"\-DTrilinos_ENABLE_Stratimikos:BOOL=OFF\n" \
      ,
      \
      "\-DTrilinos_ENABLE_Teuchos:BOOL=ON\n" \
      +"\-DTrilinos_ENABLE_Tpetra:BOOL=ON\n" \
      )
    # Above, Teuchos should not be enabled because --enable-packages should
    # result in the modified file in Teuchos to be ignored.  The enable for
    # Tpetra should not be on because it should be removed from the enable
    # list.


  def test_no_enable_fwd_packages(self):
    checkin_test_configure_enables_test(
      self,
      "no_enable_fwd_packages",
      "--no-enable-fwd-packages",
      "\-DTrilinos_ENABLE_ALL_FORWARD_DEP_PACKAGES:BOOL=OFF\n" \
      )


  def test_enable_all_packages_auto_implicit(self):
    checkin_test_configure_enables_test(
      self,
      "enable_all_packages_auto",
      "", # --enable-all-packages=auto
      "\-DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON\n" \
      +"\-DTrilinos_ENABLE_TrilinosFramework:BOOL=ON\n",
      modifiedFilesStr="cmake/utils/AppendSet.cmake",
      )


  def test_enable_all_packages_auto(self):
    checkin_test_configure_enables_test(
      self,
      "enable_all_packages_auto",
      "--enable-all-packages=auto",
      "\-DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON\n",
      modifiedFilesStr="CMakeLists.txt", # Will not trigger TrilinosFramework!
      extraPassRegexStr="Modified file: .CMakeLists.txt.\n"\
      +"Enabling all Trilinos packages!\n",
      )


  def test_enable_all_packages_on(self):
    checkin_test_configure_enables_test(
      self,
      "enable_all_packages_on",
      "--enable-all-packages=on",
      "\-DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON\n",
      modifiedFilesStr = "dummy.txt", # Will not trigger any enables!
      extraPassRegexStr=\
        "enable-all-packages=on => git diffs w.r.t. tracking branch .will not. be needed to look for changed files!\n" \
        "No need for repos to be on a branch with a tracking branch!\n" \
        +"Enabling all packages on request since --enable-all-packages=on\n"\
        +"Skipping detection of changed packages since --enable-all-packages=on\n"\
        +"cmakePkgOptions: ..-DTrilinos_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=ON., .-DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON., .-DTrilinos_ENABLE_ALL_FORWARD_DEP_PACKAGES:BOOL=ON..\n"\
        ,
      doGitDiff=False \
      )


  def test_enable_all_packages_on_enable_extra_packages(self):
    checkin_test_configure_enables_test(
      self,
      "enable_all_packages_on_enable_extra_packages",
      "--enable-all-packages=on --enable-extra-packages=RTOp",
      "\-DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON\n",
      modifiedFilesStr = "dummy.txt", # Will not trigger any enables!
      extraPassRegexStr=\
        "enable-all-packages=on => git diffs w.r.t. tracking branch .will not. be needed to look for changed files!\n" \
        "No need for repos to be on a branch with a tracking branch!\n" \
        +"Enabling all packages on request since --enable-all-packages=on\n"\
        +"Skipping detection of changed packages since --enable-all-packages=on\n"\
        +"Full package enable list: \[\]\n" \
        +"cmakePkgOptions: ..-DTrilinos_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=ON., .-DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON., .-DTrilinos_ENABLE_ALL_FORWARD_DEP_PACKAGES:BOOL=ON..\n"\
        ,
      doGitDiff=False \
      )


  def test_enable_all_packages_off(self):
    checkin_test_configure_enables_test(
      self,
      "enable_all_packages_auto",
      "--enable-all-packages=off",
      "\-DTrilinos_ENABLE_TrilinosFramework:BOOL=ON\n",
      notRegexListStr="\-DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON\n",
      modifiedFilesStr="cmake/utils/AppendSet.cmake",
      )


  # C) Test partial actions short of running tests


  def test_default_builds_mpi_debug_pull_only(self):
    checkin_test_run_case(
      self,
      \
      "default_builds_mpi_debug_pull_only",
      \
      "--default-builds=MPI_DEBUG --pull",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      g_expectedRegexUpdatePasses \
      +"Not performing any build cases because no --configure, --build or --test was specified!\n" \
      +"0) MPI_DEBUG => No configure, build, or test for MPI_DEBUG was requested! => Not ready to push!\n" \
      +"A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      +"^PASSED [(]NOT READY TO PUSH[)]: Trilinos:\n"
      )


  def test_default_builds_mpi_debug_enable_all_packages_on_pull_only(self):
    checkin_test_run_case(
      self,
      \
      "default_builds_mpi_debug_enable_all_packages_on_pull_only",
      \
      "--default-builds=MPI_DEBUG --enable-all-packages=on --pull",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsStatusPullPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      g_expectedRegexUpdatePasses \
      +"enable-all-packages=on => git diffs w.r.t. tracking branch .will not. be needed to look for changed files!\n" \
      +"Doing a pull so all repos must be on a branch and have a tracking branch!\n" \
      +"Not performing any build cases because no --configure, --build or --test was specified!\n" \
      +"A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      )


  def test_detached_head_fail(self):
    checkin_test_run_case(
      self,
      \
      "detached_head_fail",
      \
      "--default-builds=MPI_DEBUG --send-email-to=",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsNoTrackingBranchPass(branch="HEAD") \
      ,
      \
      False,
      \
      "Need git diffs w.r.t. tracking branch so all repos must be on a branch and have a tracking branch!\n" \
      +"Error, the base repo is in a detached head state which is not allowed in this case!\n"
      )


  def test_missing_tracking_branch_fail(self):
    checkin_test_run_case(
      self,
      \
      "missing_tracking_branch_fail",
      \
      "--default-builds=MPI_DEBUG --send-email-to=",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsNoTrackingBranchPass() \
      ,
      \
      False,
      \
      "Need git diffs w.r.t. tracking branch so all repos must be on a branch and have a tracking branch!\n" \
      +"Error, the base repo is not on a tracking branch which is not allowed in this case!\n"
      )


  def test_extra_repo_detached_head_0_fail(self):
    checkin_test_run_case(
      self,
      \
      "extra_repo_detached_head_0_fail",
      \
      "--extra-repos=preCopyrightTrilinos --default-builds=MPI_DEBUG --send-email-to=",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsNoTrackingBranchPass(branch="HEAD") \
      +cmndinterceptsGetRepoStatsPass() \
      ,
      \
      False,
      \
      "Error, the base repo is in a detached head state which is not allowed in this case!\n"
      )


  def test_extra_repo_detached_head_1_fail(self):
    checkin_test_run_case(
      self,
      \
      "extra_repo_detached_head_0_fail",
      \
      "--extra-repos=preCopyrightTrilinos --default-builds=MPI_DEBUG --send-email-to=",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +cmndinterceptsGetRepoStatsNoTrackingBranchPass(branch="HEAD") \
      ,
      \
      False,
      \
      "Error, the repo .preCopyrightTrilinos. is in a detached head state which is not allowed in this case!\n"
      )


  def test_extra_repo_missing_tracking_branch_0_fail(self):
    checkin_test_run_case(
      self,
      \
      "extra_repo_missing_tracking_branch_0_fail",
      \
      "--extra-repos=preCopyrightTrilinos --default-builds=MPI_DEBUG --send-email-to=",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsNoTrackingBranchPass() \
      +cmndinterceptsGetRepoStatsPass() \
      ,
      \
      False,
      \
      "Error, the base repo is not on a tracking branch which is not allowed in this case!\n"
      )


  def test_extra_repo_missing_tracking_branch_1_fail(self):
    checkin_test_run_case(
      self,
      \
      "extra_repo_missing_tracking_branch_0_fail",
      \
      "--extra-repos=preCopyrightTrilinos --default-builds=MPI_DEBUG --send-email-to=",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +cmndinterceptsGetRepoStatsNoTrackingBranchPass() \
      ,
      \
      False,
      \
      "Error, the repo .preCopyrightTrilinos. is not on a tracking branch which is not allowed in this case!\n"
      )


  def test_default_builds_mpi_debug_enable_all_packages_off_enable_packages(self):
    checkin_test_run_case(
      self,
      \
      "default_builds_mpi_debug_enable_all_packages_off_enable_packages",
      \
      "--default-builds=MPI_DEBUG --enable-all-packages=off --enable-packages=Teuchos --allow-no-pull",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "enable-packages!=.. and --enable-all-packages=.off. => git diffs w.r.t. tracking branch .will not. be needed to look for changed files!\n" \
      +"No need for repos to be on a branch with a tracking branch!\n" \
      +"Skipping getting list of modified files because not needed!\n" \
      +"Not performing any build cases because no --configure, --build or --test was specified!\n" \
      +"PASSED [(]NOT READY TO PUSH[)]:\n" \
      )


  def test_default_builds_mpi_debug_enable_all_packages_on_push_only(self):
    checkin_test_run_case(
      self,
      \
      "default_builds_mpi_debug_enable_all_packages_on_push_only",
      \
      "--default-builds=MPI_DEBUG --enable-all-packages=on --push",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      "enable-all-packages=on => git diffs w.r.t. tracking branch .will not. be needed to look for changed files!\n" \
      +"Doing a push so all repos must be on a branch and have a tracking branch!\n" \
      +"Skipping all pulls on request!\n" \
      +"No previous successful pull is still current!\n" \
      +"A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      )


  def test_default_builds_mpi_debug_pull_skip_push_readiness_check(self):
    checkin_test_run_case(
      self,
      \
      "default_builds_mpi_debug_pull_skip_push_readiness_check",
      \
      "--default-builds=MPI_DEBUG --pull --skip-push-readiness-check",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      ,
      \
      True,
      \
      g_expectedRegexUpdatePasses \
      +"Skipping push readiness check on request!\n" \
      +"Not performing push or sending out push readiness status on request!\n" \
      "^NOT READY TO PUSH$\n" \
      +"REQUESTED ACTIONS: PASSED\n"
      )


  def test_default_builds_mpi_debug_pull_extra_pull_only(self):
    checkin_test_run_case(
      self,
      \
      "default_builds_mpi_debug_pull_extra_pull_only",
      \
      "--pull --extra-pull-from=somerepo:remotebranch",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsStatusPullPasses \
      +"IT: git pull somerepo remotebranch; 0; 'git extra pull passed'\n"
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      g_expectedRegexUpdatePasses \
      +"Pulling in updates to local repo .. from .somerepo remotebranch.\n" \
      +"git pull somerepo remotebranch\n" \
      +"Not performing any build cases because no --configure, --build or --test was specified!\n" \
      +"A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      +"^PASSED [(]NOT READY TO PUSH[)]: Trilinos:\n"
      )


  def test_default_builds_mpi_debug_extra_pull_only(self):
    checkin_test_run_case(
      self,
      \
      "default_builds_mpi_debug_extra_pull_only",
      \
      "--extra-pull-from=machine:master",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      "Skipping all pulls on request!\n" \
      +"Not performing any build cases because no --configure, --build or --test was specified!\n" \
      +"A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      +"^INITIAL PULL FAILED: Trilinos:\n"
      )


  def test_default_builds_mpi_debug_configure_only(self):
    checkin_test_run_case(
      self,
      \
      "default_builds_mpi_debug_configure_only",
      \
      "--default-builds=MPI_DEBUG --pull --configure",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsConfigPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      g_expectedRegexUpdateWithBuildCasePasses+ \
      "Configure passed!\n" \
      "touch configure.success\n" \
      "Skipping the build on request!\n" \
      "Skipping the tests on request!\n" \
      "0) MPI_DEBUG => passed: configure-only passed => Not ready to push!\n" \
      "A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      "PASSED [(]NOT READY TO PUSH[)]: Trilinos:\n"
      )


  def test_default_builds_mpi_debug_build_only(self):
    checkin_test_run_case(
      self,
      \
      "default_builds_mpi_debug_build_only",
      \
      "--make-options=-j3 --default-builds=MPI_DEBUG --pull --configure --build",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsConfigBuildPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      g_expectedRegexUpdateWithBuildCasePasses+ \
      "Configure passed!\n" \
      "touch configure.success\n" \
      "Build passed!\n" \
      "Skipping the tests on request!\n" \
      "0) MPI_DEBUG => passed: build-only passed => Not ready to push!\n" \
      "A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      "PASSED [(]NOT READY TO PUSH[)]: Trilinos:\n"
      )


  # D) Test --extra-builds and --st-extra-builds


  def test_extra_builds_read_config_file(self):
    
    testName = "extra_builds_read_config_file"

    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)

    writeStrToFile(testBaseDir+"/COMMON.config",
      "-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON\n" \
      +"-DBUILD_SHARED:BOOL=ON\n" \
      +"-DTPL_BLAS_LIBRARIES:PATH=/usr/local/libblas.a\n" \
      +"-DTPL_LAPACK_LIBRARIES:PATH=/usr/local/liblapack.a\n" \
      )

    writeStrToFile(testBaseDir+"/SERIAL_DEBUG_BOOST_TRACING.config",
      "-DTPL_ENABLE_BOOST:BOOL=ON\n" \
      +"-DTeuchos_ENABLE_RCP_NODE_TRACING:BOOL=ON\n" \
      )

    checkin_test_configure_test(
      \
      self,
      \
      testName,
      \
      "--without-default-builds --extra-builds=SERIAL_DEBUG_BOOST_TRACING",
      \
      [
      ("SERIAL_DEBUG_BOOST_TRACING/do-configure.base",
       "\-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON\n" \
       +"\-DBUILD_SHARED:BOOL=ON\n" \
       +"\-DTPL_BLAS_LIBRARIES:PATH=/usr/local/libblas.a\n" \
       +"\-DTPL_LAPACK_LIBRARIES:PATH=/usr/local/liblapack.a\n"
       +"\-DTPL_ENABLE_BOOST:BOOL=ON\n" \
       +"\-DTeuchos_ENABLE_RCP_NODE_TRACING:BOOL=ON\n" \
       ),
      ]
      )


  def test_extra_builds_missing_config_file_fail(self):
    checkin_test_run_case(
      \
      self,
      \
      "extra_builds_missing_config_file_fail",
      \
      "--extra-builds=SERIAL_DEBUG_BOOST_TRACING",
      \
      "", # No shell commands!
      \
      False,
      \
      "Error, the extra build configuration file SERIAL_DEBUG_BOOST_TRACING.config" \
      +" does not exit!\n" \
      ,
      grepForFinalPassFailStr=False \
      )


  def test_st_extra_builds_pt_only_pass(self):
    
    testName = "st_extra_builds_pt_only_pass"

    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)

    writeStrToFile(testBaseDir+"/COMMON.config",
      "-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON\n" \
      +"-DBUILD_SHARED:BOOL=ON\n" \
      +"-DTPL_BLAS_LIBRARIES:PATH=/usr/local/libblas.a\n" \
      +"-DTPL_LAPACK_LIBRARIES:PATH=/usr/local/liblapack.a\n" \
      )

    writeStrToFile(testBaseDir+"/MPI_DEBUG_ST.config",
      "-DTPL_ENABLE_MPI:BOOL=ON\n" \
      +"-DTeuchos_ENABLE_SECONDARY_TESTED_CODE:BOOL=ON\n" \
      )

    modifiedFilesStr = "packages/teuchos/CMakeLists.txt"

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      "--make-options=-j3 --ctest-options=-j5" \
      +" --default-builds=MPI_DEBUG --do-all --push " \
      +" --st-extra-builds=MPI_DEBUG_ST" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsFinalPushPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "passed: Trilinos/MPI_DEBUG: passed=100,notpassed=0\n" \
      +"passed: Trilinos/MPI_DEBUG_ST: passed=100,notpassed=0\n" \
      +"0) MPI_DEBUG => passed: passed=100,notpassed=0\n" \
      +"2) MPI_DEBUG_ST => passed: passed=100,notpassed=0\n" \
      +"^DID PUSH\n" \
      )


  def test_ss_extra_builds_pt_only_pass(self):
    
    testName = "ss_extra_builds_pt_only_pass"

    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)

    writeStrToFile(testBaseDir+"/COMMON.config",
      "-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON\n" \
      +"-DBUILD_SHARED:BOOL=ON\n" \
      +"-DTPL_BLAS_LIBRARIES:PATH=/usr/local/libblas.a\n" \
      +"-DTPL_LAPACK_LIBRARIES:PATH=/usr/local/liblapack.a\n" \
      )

    writeStrToFile(testBaseDir+"/MPI_DEBUG_ST.config",
      "-DTPL_ENABLE_MPI:BOOL=ON\n" \
      +"-DTeuchos_ENABLE_SECONDARY_TESTED_CODE:BOOL=ON\n" \
      )

    modifiedFilesStr = "packages/teuchos/CMakeLists.txt"

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      "--make-options=-j3 --ctest-options=-j5" \
      +" --default-builds= --do-all --push " \
      +" --ss-extra-builds=MPI_DEBUG_ST" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsFinalPushPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "WARNING: --ss-extra-builds is deprecated!  Use --st-extra-builds instead!\n" \
      +"passed: Trilinos/MPI_DEBUG_ST: passed=100,notpassed=0\n" \
      +"2) MPI_DEBUG_ST => passed: passed=100,notpassed=0\n" \
      +"^DID PUSH\n" \
      +"FINAL WARNING: stop using deprecated --ss-extra-builds!  Use --st-extra-builds instead!\n" \
      )


  def test_ss_extra_builds_and_ss_extra_builds_fails(self):
    
    testName = "ss_extra_builds_and_ss_extra_builds_fails"

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      "--make-options=-j3 --ctest-options=-j5" \
      +" --default-builds= --do-all --push " \
      +" --st-extra-builds=SERIAL_RELEASE_ST" \
      +" --ss-extra-builds=MPI_DEBUG_ST" \
      ,
      \
      "" \
      ,
      \
      False,
      \
      "WARNING: --ss-extra-builds is deprecated!  Use --st-extra-builds instead!\n" \
      +"ERROR: Can.t set deprecated --ss-extra-builds and --st-extra-builds together!\n" \
      ,
      grepForFinalPassFailStr=False \
      )


  def test_st_extra_builds_skip_case_no_email_ex_only_pass(self):
    
    testName = "st_extra_builds_skip_case_no_email_ex_only_pass"

    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)

    writeStrToFile(testBaseDir+"/MPI_DEBUG_ST.config",
      "-DTPL_ENABLE_MPI:BOOL=ON\n" \
      )

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      "--make-options=-j3 --ctest-options=-j5" \
      +" --default-builds=MPI_DEBUG" \
      +" --skip-case-no-email --do-all --push " \
      +" --extra-builds=MPI_DEBUG_ST" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsPullOnlyPasses \
      +"IT: git diff --name-status origin/trackingbranch; 0; 'M\tpackages/stokhos/CMakeLists.txt'\n" \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsFinalPushPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "Skipping sending final status email for MPI_DEBUG because it had no packages enabled and --skip-case-no-email was set!\n" \
      +"^DID PUSH\n" \
      )


  def test_st_extra_builds_st_only_pass(self):
    
    testName = "st_extra_builds_st_only_pass"

    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)

    writeStrToFile(testBaseDir+"/COMMON.config",
      "-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON\n" \
      +"-DBUILD_SHARED:BOOL=ON\n" \
      +"-DTPL_BLAS_LIBRARIES:PATH=/usr/local/libblas.a\n" \
      +"-DTPL_LAPACK_LIBRARIES:PATH=/usr/local/liblapack.a\n" \
      )

    writeStrToFile(testBaseDir+"/MPI_DEBUG_ST.config",
      "-DTPL_ENABLE_MPI:BOOL=ON\n" \
      +"-DTeuchos_ENABLE_SECONDARY_TESTED_CODE:BOOL=ON\n" \
      )

    modifiedFilesStr = ""

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      "--make-options=-j3 --ctest-options=-j5" \
      +" --default-builds=MPI_DEBUG --do-all --push " \
      +" --st-extra-builds=MPI_DEBUG_ST --enable-packages=Phalanx" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsFinalPushPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "Phalanx of type ST is being excluded because it is not in the valid list of package types .PT.\n" \
      +"passed: Trilinos/MPI_DEBUG: skipped configure, build, test due to no enabled packages\n" \
      +"passed: Trilinos/MPI_DEBUG_ST: passed=100,notpassed=0\n" \
      +"0) MPI_DEBUG => Skipped configure, build, test due to no enabled packages! => Does not affect push readiness!\n" \
      +"2) MPI_DEBUG_ST => passed: passed=100,notpassed=0\n" \
      +"^DID PUSH\n" \
      )


  def test_st_extra_builds_pt_st_pass(self):
    
    testName = "st_extra_builds_pt_st_pass"

    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)

    writeStrToFile(testBaseDir+"/COMMON.config",
      "-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON\n" \
      +"-DBUILD_SHARED:BOOL=ON\n" \
      +"-DTPL_BLAS_LIBRARIES:PATH=/usr/local/libblas.a\n" \
      +"-DTPL_LAPACK_LIBRARIES:PATH=/usr/local/liblapack.a\n" \
      )

    writeStrToFile(testBaseDir+"/MPI_DEBUG_ST.config",
      "-DTPL_ENABLE_MPI:BOOL=ON\n" \
      +"-DTeuchos_ENABLE_SECONDARY_TESTED_CODE:BOOL=ON\n" \
      )

    modifiedFilesStr = ""

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      "--make-options=-j3 --ctest-options=-j5" \
      +" --default-builds=MPI_DEBUG --do-all --push " \
      +" --st-extra-builds=MPI_DEBUG_ST --enable-packages=Teuchos,Phalanx" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsFinalPushPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "Phalanx of type ST is being excluded because it is not in the valid list of package types .PT.\n" \
      +"passed: Trilinos/MPI_DEBUG: passed=100,notpassed=0\n" \
      +"passed: Trilinos/MPI_DEBUG_ST: passed=100,notpassed=0\n" \
      +"0) MPI_DEBUG => passed: passed=100,notpassed=0\n" \
      +"2) MPI_DEBUG_ST => passed: passed=100,notpassed=0\n" \
      +"^DID PUSH\n" \
      )


  def test_st_extra_builds_ex_only_fail(self):
    
    testName = "st_extra_builds_ex_only_fail"

    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)

    writeStrToFile(testBaseDir+"/COMMON.config",
      "-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON\n" \
      +"-DBUILD_SHARED:BOOL=ON\n" \
      +"-DTPL_BLAS_LIBRARIES:PATH=/usr/local/libblas.a\n" \
      +"-DTPL_LAPACK_LIBRARIES:PATH=/usr/local/liblapack.a\n" \
      )

    writeStrToFile(testBaseDir+"/MPI_DEBUG_ST.config",
      "-DTPL_ENABLE_MPI:BOOL=ON\n" \
      +"-DTeuchos_ENABLE_SECONDARY_TESTED_CODE:BOOL=ON\n" \
      )

    modifiedFilesStr = ""

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      " --default-builds=MPI_DEBUG --send-email-to=" \
      +" --make-options=-j3 --ctest-options=-j5" \
      +" --do-all --push" \
      +" --st-extra-builds=MPI_DEBUG_ST --enable-packages=ThyraCrazyStuff" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsLogCommitsPasses \
      ,
      \
      False,
      \
      "ThyraCrazyStuff of type EX is being excluded because it is not in the valid list of package types .PT.\n" \
      "ThyraCrazyStuff of type EX is being excluded because it is not in the valid list of package types .PT,ST.\n" \
      +"passed: Trilinos/MPI_DEBUG: skipped configure, build, test due to no enabled packages\n" \
      +"passed: Trilinos/MPI_DEBUG_ST: skipped configure, build, test due to no enabled packages\n" \
      +"There were no successful attempts to configure/build/test!\n" \
      +"  => A PUSH IS .NOT. READY TO BE PERFORMED!\n" \
      +"^PUSH FAILED\n" \
      +"^REQUESTED ACTIONS: FAILED\n" \
      )


  def test_st_extra_builds_extra_builds_ex_only_pass(self):
    
    testName = "st_extra_builds_extra_builds_ex_only_pass"

    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)

    writeStrToFile(testBaseDir+"/COMMON.config",
      "-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON\n" \
      +"-DBUILD_SHARED:BOOL=ON\n" \
      +"-DTPL_BLAS_LIBRARIES:PATH=/usr/local/libblas.a\n" \
      +"-DTPL_LAPACK_LIBRARIES:PATH=/usr/local/liblapack.a\n" \
      )

    writeStrToFile(testBaseDir+"/MPI_DEBUG_ST.config",
      "-DTPL_ENABLE_MPI:BOOL=ON\n" \
      +"-DTeuchos_ENABLE_SECONDARY_TESTED_CODE:BOOL=ON\n" \
      )

    writeStrToFile(testBaseDir+"/SERIAL_RELEASE_ST.config",
      "-DTPL_ENABLE_MPI:BOOL=OFF\n" \
      +"-DTeuchos_ENABLE_SECONDARY_TESTED_CODE:BOOL=ON\n" \
      )

    modifiedFilesStr = ""

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      " --default-builds=MPI_DEBUG" \
      +" --make-options=-j3 --ctest-options=-j5" \
      +" --do-all --push" \
      +" --st-extra-builds=MPI_DEBUG_ST" \
      +" --extra-builds=SERIAL_RELEASE_ST" \
      +" --enable-packages=ThyraCrazyStuff" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsFinalPushPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "ThyraCrazyStuff of type EX is being excluded because it is not in the valid list of package types .PT.\n" \
      "ThyraCrazyStuff of type EX is being excluded because it is not in the valid list of package types .PT,ST.\n" \
      +"passed: Trilinos/MPI_DEBUG: skipped configure, build, test due to no enabled packages\n" \
      +"passed: Trilinos/MPI_DEBUG_ST: skipped configure, build, test due to no enabled packages\n" \
      +"passed: Trilinos/SERIAL_RELEASE_ST: passed=100,notpassed=0\n" \
      +"0) MPI_DEBUG => Skipped configure, build, test due to no enabled packages! => Does not affect push readiness!\n" \
      +"2) MPI_DEBUG_ST => Skipped configure, build, test due to no enabled packages! => Does not affect push readiness!\n" \
      +"3) SERIAL_RELEASE_ST => passed: passed=100,notpassed=0\n" \
      +"^DID PUSH\n" \
      )

  # E) Test intermediate states with rerunning to fill out


  # ToDo: Add test for pull followed by configure
  # ToDo: Add test for configure followed by build
  # ToDo: Add test for build followed by test


  def test_do_all_default_builds_mpi_debug_then_push_pass(self):

    testName = "do_all_default_builds_mpi_debug_then_push_pass"

    # Do the build/test only first (ready to push)
    g_test_do_all_default_builds_mpi_debug_pass(self, testName)

    # Do the push after the fact
    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      "--make-options=-j3 --ctest-options=-j5 --default-builds=MPI_DEBUG --push",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsFinalPushPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "0) MPI_DEBUG => passed: passed=100,notpassed=0\n" \
      "0) MPI_DEBUG Results:\n" \
      +"=> A PUSH IS READY TO BE PERFORMED!\n" \
      +"^DID PUSH: Trilinos:\n" \
      
      )


  def test_do_all_default_builds_mpi_debug_then_empty(self):

    testName = "do_all_default_builds_mpi_debug_then_empty"

    # Do the build/test only first (ready to push)
    g_test_do_all_default_builds_mpi_debug_pass(self, testName)

    # Check the status after (no action arguments)
    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      "--make-options=-j3 --ctest-options=-j5 --default-builds=MPI_DEBUG",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +"IT: git diff --name-status origin/trackingbranch; 0; 'git diff passed'\n" \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "0) MPI_DEBUG => passed: passed=100,notpassed=0\n" \
      "=> A PUSH IS READY TO BE PERFORMED!\n" \
      "^PASSED [(]READY TO PUSH[)]: Trilinos:\n"
      )


  def test_st_extra_builds_st_do_all_then_empty(self):

    testName = "test_st_extra_builds_st_do_all_then_empty"

    # --do-all without the push (ready to push)
    g_test_st_extra_builds_st_do_all_pass(self, testName)

    # Follow-up status (should be ready to push)

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      " --default-builds=MPI_DEBUG --st-extra-builds=MPI_DEBUG_ST --enable-packages=Phalanx" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "0) MPI_DEBUG => passed: skipped configure, build, test due to no enabled packages\n" \
      +"2) MPI_DEBUG_ST => passed: passed=100,notpassed=0\n" \
      +"^PASSED [(]READY TO PUSH[)]\n" \
      )


  def test_st_extra_builds_st_do_all_then_push(self):

    testName = "test_st_extra_builds_st_do_all_then_push"

    # --do-all without the push (ready to push)
    g_test_st_extra_builds_st_do_all_pass(self, testName)

    # Follow-up status (should be ready to push)

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      " --default-builds=MPI_DEBUG --st-extra-builds=MPI_DEBUG_ST --enable-packages=Phalanx" \
      +" --push"
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsFinalPushPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      "0) MPI_DEBUG => passed: skipped configure, build, test due to no enabled packages\n" \
      +"2) MPI_DEBUG_ST => passed: passed=100,notpassed=0\n" \
      +"^DID PUSH\n" \
      )


  # ToDo: On all of these below check that the right files are being deleted!

  # ToDo: Add test for removing files on pull (fail immediately)
  # ToDo: Add test for removing files on configure (fail immediately)
  # ToDo: Add test for removing files on build (fail immediately)
  # ToDo: Add test for removing files on test (fail immediately)


  # F) Test various failing use cases


  def test_enable_packages_error(self):
    checkin_test_run_case(
      \
      self,
      \
      "enable_packages_error",
      \
      "--enable-packages=TEuchos" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      ,
      \
      False,
      \
      "Error, invalid package name TEuchos in --enable-packages=TEuchos." \
      "  The valid package names include: .*Teuchos, .*\n" \
      ,
      grepForFinalPassFailStr=False \
      )


  def test_enable_extra_packages_error(self):
    checkin_test_run_case(
      \
      self,
      \
      "enable_extra_packages_error",
      \
      "--enable-extra-packages=RTOP" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      ,
      \
      False,
      \
      "Error, invalid package name RTOP in --enable-extra-packages=RTOP." \
      "  The valid package names include: .*RTOp, .*\n" \
      ,
      grepForFinalPassFailStr=False \
      )


  def test_disable_packages_error(self):
    checkin_test_run_case(
      \
      self,
      \
      "disable_packages_error",
      \
      "--disable-packages=TEuchos" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      ,
      \
      False,
      \
      "Error, invalid package name TEuchos in --disable-packages=TEuchos." \
      "  The valid package names include: .*Teuchos, .*\n" \
      ,
      grepForFinalPassFailStr=False \
      )


  def test_do_all_local_do_all(self):
    checkin_test_run_case(
      \
      self,
      \
      "do_all_local_do_all",
      \
      "--do-all --local-do-all" \
      ,
      \
      "" \
      ,
      \
      False,
      \
      "Error, you can not use --do-all and --local-do-all together!\n" \
      ,
      grepForFinalPassFailStr=False \
      )


  def test_do_all_allow_no_pull(self):
    checkin_test_run_case(
      \
      self,
      \
      "do_all_allow_no_pull",
      \
      "--do-all --allow-no-pull" \
      ,
      \
      "" \
      ,
      \
      False,
      \
      "Error, you can not use --do-all and --allow-no-pull together!\n" \
      ,
      grepForFinalPassFailStr=False \
      )


  def test_do_all_default_builds_mpi_debug_unstaged_changed_files_fail(self):
    checkin_test_run_case(
      \
      self,
      \
      "do_all_default_builds_mpi_debug_unstaged_changed_files_fail",
      \
      "--do-all",
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass(changedFile="M  newfile") \
      +"IT: git status; 0; 'Git status returned changed and staged but not committed'\n" \
      +g_cmndinterceptsLogCommitsPasses \
      ,
      \
      False,
      \
      "ERROR: There are changed uncommitted files => cannot continue!\n" \
      "Git status returned changed and staged but not committed\n" \
      "Pull failed!\n" \
      "Not running any build/test cases because the pull failed!\n" \
      "A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      "INITIAL PULL FAILED: Trilinos:\n"
      )


  def test_do_all_default_builds_mpi_debug_staged_uncommitted_files_fail(self):
    checkin_test_run_case(
      \
      self,
      \
      "do_all_default_builds_mpi_debug_staged_uncommitted_files_fail",
      \
      "--do-all",
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass(changedFile="MM somefile") \
      +"IT: git status; 0; 'Git status returned both changed and staged but not committed'\n" \
      +g_cmndinterceptsLogCommitsPasses \
      ,
      \
      False,
      \
      "ERROR: There are changed uncommitted files => cannot continue!\n" \
      "Git status returned both changed and staged but not committed\n" \
      "Pull failed!\n" \
      "Not running any build/test cases because the pull failed!\n" \
      "A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      "INITIAL PULL FAILED: Trilinos:\n"
      )


  def test_do_all_default_builds_mpi_debug_unknown_files_fail(self):
    checkin_test_run_case(
      \
      self,
      \
      "do_all_default_builds_mpi_debug_unknown_files_fail",
      \
      "--do-all",
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass(changedFile="?? newfile") \
      +"IT: git status; 0; 'New unknown files'\n" \
      +g_cmndinterceptsLogCommitsPasses \
      ,
      \
      False,
      \
      "ERROR: There are newly created uncommitted files => Cannot continue!\n" \
      "Pull failed!\n" \
      "Not running any build/test cases because the pull failed!\n" \
      "A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      "INITIAL PULL FAILED: Trilinos:\n"
      )


  def test_do_all_default_builds_mpi_debug_pull_fail(self):
    checkin_test_run_case(
      \
      self,
      \
      "do_all_default_builds_mpi_debug_pull_fail",
      \
      "--do-all",
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +"IT: git pull; 1; 'git pull failed'\n" \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      "Pull failed!\n" \
      "Skipping getting list of modified files because pull failed!\n" \
      "A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      "INITIAL PULL FAILED: Trilinos:\n"
      )


  def test_illegal_enables_fail(self):
    
    testName = "illegal_enables_fail"

    testBaseDir = create_checkin_test_case_dir(testName, g_verbose)

    writeStrToFile(testBaseDir+"/COMMON.config",
      "-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON\n" \
      +"-DBUILD_SHARED:BOOL=ON\n" \
      +"-DTPL_BLAS_LIBRARIES:PATH=/usr/local/libblas.a\n" \
      +"-DTPL_LAPACK_LIBRARIES:PATH=/usr/local/liblapack.a\n" \
      +"-DTPL_ENABLE_BOOST:BOOL=ON\n" \
      +"-DTrilinos_ENABLE_TriKota:BOOL=ON\n" \
      +"-DTrilinos_ENABLE_WebTrilinos=ON\n" \
      )

    writeStrToFile(testBaseDir+"/MPI_DEBUG.config",
      "-DTPL_ENABLE_MPI:BOOL=ON\n" \
      +"-DTPL_ENABLE_CUDA:BOOL=ON\n" \
      +"-DTrilinos_ENABLE_STK:BOOL=ON\n" \
      +"-DTrilinos_ENABLE_Phalanx=ON\n" \
      +"-DTrilinos_ENABLE_Sundance=OFF\n" \
      )

    checkin_test_run_case(
      \
      self,
      \
      testName,
      \
      "--default-builds=MPI_DEBUG --configure --allow-no-pull",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      "ERROR: Illegal TPL enable -DTPL_ENABLE_BOOST:BOOL=ON in ../COMMON.config!\n" \
      +"ERROR: Illegal enable -DTrilinos_ENABLE_TriKota:BOOL=ON in ../COMMON.config!\n" \
      +"ERROR: Illegal enable -DTrilinos_ENABLE_WebTrilinos=ON in ../COMMON.config!\n" \
      +"ERROR: Illegal TPL enable -DTPL_ENABLE_CUDA:BOOL=ON in ../MPI_DEBUG.config!\n" \
      +"ERROR: Illegal TPL enable -DTPL_ENABLE_MPI:BOOL=ON in ../MPI_DEBUG.config!\n" \
      +"ERROR: Illegal enable -DTrilinos_ENABLE_STK:BOOL=ON in ../MPI_DEBUG.config!\n" \
      +"ERROR: Illegal enable -DTrilinos_ENABLE_Phalanx=ON in ../MPI_DEBUG.config!\n" \
      +"SKIPPED: MPI_DEBUG configure skipped because pre-configure failed (see above)!\n" \
      +"0) MPI_DEBUG => FAILED: pre-configure failed => Not ready to push!\n" \
      +"Configure: FAILED\n" \
      +"FAILED CONFIGURE/BUILD/TEST: Trilinos:\n" \
      +"REQUESTED ACTIONS: FAILED\n" \
      ,
      failRegexStrList = \
      "ERROR: Illegal enable -DTrilinos_ENABLE_Sundance=OFF\n" # Package disables are okay but not great
      )

    self.assertEqual(os.path.exists(testBaseDir+"/MPI_DEBUG/do-configure.base"), False)
    self.assertEqual(os.path.exists(testBaseDir+"/MPI_DEBUG/do-configure"), False)


  def test_do_all_default_builds_mpi_debug_configure_fail(self):
    checkin_test_run_case(
      self,
      \
      "do_all_default_builds_mpi_debug_configure_fail",
      \
      "--do-all --default-builds=MPI_DEBUG",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +"IT: \./do-configure; 1; 'do-configure failed'\n" \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      g_expectedRegexUpdateWithBuildCasePasses \
      +"Configure failed returning 1!\n" \
      +"The configure FAILED!\n" \
      +"The build was never attempted!\n" \
      +"The tests were never even run!\n" \
      +"FAILED: configure failed\n" \
      +"A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      +"FAILED CONFIGURE/BUILD/TEST: Trilinos:\n" \
      )


  def test_do_all_default_builds_mpi_debug_build_fail(self):
    checkin_test_run_case(
      \
      self,
      \
      "do_all_default_builds_mpi_debug_build_fail",
      \
      "--do-all --default-builds=MPI_DEBUG --make-options=-j3 --ctest-options=-j5" \
      +" --send-build-case-email=only-on-failure" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +"IT: \./do-configure; 0; 'do-configure passed'\n" \
      +"IT: make -j3; 1; 'make filed'\n" \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      g_expectedRegexUpdateWithBuildCasePasses \
      +g_expectedRegexConfigPasses \
      +g_expectedRegexTestNotRun \
      +g_expectedRegexBuildFailed \
      +"0) MPI_DEBUG => FAILED: build failed => Not ready to push!\n" \
      +"1) SERIAL_RELEASE => Test case SERIAL_RELEASE was not run! => Does not affect push readiness!\n" \
      +g_expectedCommonOptionsSummary \
      +"A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      +"^FAILED CONFIGURE/BUILD/TEST: Trilinos:\n"
      )
    # NOTE: Above test ensures that setting
    # --send-build-case-email=only-on-failure still allows the build case
    # email to go out if the build fails.


  def test_send_build_case_email_never_do_all_default_builds_mpi_debug_build_fail(self):
    checkin_test_run_case(
      \
      self,
      \
      "send_build_case_email_never_do_all_default_builds_mpi_debug_build_fail",
      \
      "--do-all --default-builds=MPI_DEBUG --make-options=-j3 --ctest-options=-j5" \
      +" --send-build-case-email=never" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +"IT: \./do-configure; 0; 'do-configure passed'\n" \
      +"IT: make -j3; 1; 'make filed'\n" \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      g_expectedRegexUpdateWithBuildCasePasses \
      +g_expectedRegexConfigPasses \
      +g_expectedRegexTestNotRun \
      +g_expectedRegexBuildFailed \
      +"Skipping sending build/test case email because everything passed and --send-build-case-email=never was set\n" \
      +"0) MPI_DEBUG => FAILED: build failed => Not ready to push!\n" \
      +"1) SERIAL_RELEASE => Test case SERIAL_RELEASE was not run! => Does not affect push readiness!\n" \
      +g_expectedCommonOptionsSummary \
      +"A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      +"^FAILED CONFIGURE/BUILD/TEST: Trilinos:\n"
      )
    # NOTE: Above test ensures that setting
    # --send-build-case-email=never will avoid sending out build case emails 


  def test_do_all_default_builds_mpi_debug_test_fail(self):
    checkin_test_run_case(
      \
      self,
      \
      "do_all_default_builds_mpi_debug_test_fail",
      \
      "--do-all --default-builds=MPI_DEBUG --make-options=-j3 --ctest-options=-j5",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +"IT: \./do-configure; 0; 'do-configure passed'\n" \
      +"IT: make -j3; 0; 'make passed'\n" \
      +"IT: ctest -j5; 1; '80% tests passed, 20 tests failed out of 100.\n" \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,      \
      False,
      \
      g_expectedRegexUpdateWithBuildCasePasses \
      +g_expectedRegexConfigPasses \
      +g_expectedRegexBuildPasses \
      +"FAILED: ctest failed returning 1!\n" \
      +"testResultsLine = .80% tests passed, 20 tests failed out of 100.\n" \
      +"0) MPI_DEBUG => FAILED: passed=80,notpassed=20\n" \
      +"1) SERIAL_RELEASE => Test case SERIAL_RELEASE was not run! => Does not affect push readiness!\n" \
      +g_expectedCommonOptionsSummary \
      +"Test: FAILED\n" \
      +"A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      +"^FAILED CONFIGURE/BUILD/TEST: Trilinos:\n" \
      )


  def test_do_all_push_default_builds_mpi_debug_final_pull_fail(self):
    checkin_test_run_case(
      \
      self,
      \
      "do_all_push_default_builds_mpi_debug_final_pull_fail",
      \
      "--default-builds=MPI_DEBUG --make-options=-j3 --ctest-options=-j5" \
      " --do-all --push" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +"IT: git pull && git rebase origin/trackingbranch; 1; 'final git pull FAILED'\n" \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,      \
      False,
      \
      g_expectedRegexUpdateWithBuildCasePasses \
      +g_expectedRegexConfigPasses \
      +g_expectedRegexBuildPasses \
      +g_expectedRegexTestPasses \
      +g_expectedCommonOptionsSummary \
      +"A PUSH IS READY TO BE PERFORMED!\n" \
      +"'': Pull failed!\n" \
      +"Final pull failed!\n" \
      +"Skippng appending test results due to prior errors!\n" \
      +"Not performing push due to prior errors!\n" \
      +"FINAL PULL FAILED: Trilinos:\n" \
      +"To find out more about this failure, grep the .checkin-test.out. log\n" \
      )


  def test_do_all_push_default_builds_mpi_debug_final_commit_fail(self):
    checkin_test_run_case(
      \
      self,
      \
      "do_all_push_default_builds_mpi_debug_final_commit_fail",
      \
      "--default-builds=MPI_DEBUG --make-options=-j3 --ctest-options=-j5" \
      " --do-all --push" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +"IT: git pull && git rebase origin/trackingbranch; 0; 'final git pull and rebase passed'\n" \
      +g_cmnginterceptsGitLogCmnds \
      +"IT: git commit --amend -F .*; 1; 'Amending the last commit FAILED'\n" \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      g_expectedRegexUpdateWithBuildCasePasses \
      +g_expectedRegexConfigPasses \
      +g_expectedRegexBuildPasses \
      +g_expectedRegexTestPasses \
      +g_expectedCommonOptionsSummary \
      +"A PUSH IS READY TO BE PERFORMED!\n" \
      +"Final pull passed!\n" \
      +"Attempting to amend the final commit message ...\n" \
      +"Appending test results to last commit failed!\n" \
      +"Not performing push due to prior errors!\n" \
      +"AMEND COMMIT FAILED: Trilinos:\n" \
      +"To find out more about this failure, grep the .checkin-test.out. log\n" \
      )


  def test_do_all_push_default_builds_mpi_debug_push_fail(self):
    checkin_test_run_case(
      \
      self,
      \
      "do_all_push_default_builds_mpi_debug_push_fail",
      \
      "--default-builds=MPI_DEBUG --make-options=-j3 --ctest-options=-j5" \
      " --do-all --push" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +"IT: git pull && git rebase origin/trackingbranch; 0; 'final git pull and rebase passed'\n" \
      +g_cmnginterceptsGitLogCmnds \
      +"IT: git commit --amend -F .*; 0; 'Amending the last commit passed'\n" \
      +g_cmndinterceptsLogCommitsPasses \
      +"IT: git push origin currentbranch:trackingbranch; 1; 'push FAILED'\n"
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      g_expectedRegexUpdateWithBuildCasePasses \
      +g_expectedRegexConfigPasses \
      +g_expectedRegexBuildPasses \
      +g_expectedRegexTestPasses \
      +g_expectedCommonOptionsSummary \
      +"A PUSH IS READY TO BE PERFORMED!\n" \
      +"Final pull passed!\n" \
      +"Appending test results to last commit passed!\n" \
      +"Push failed!\n" \
      +"PUSH FAILED: Trilinos:\n" \
      +"To find out more about this failure, grep the .checkin-test.out. log\n" \
      )


  def test_do_all_push_no_local_commits_push_fail(self):
    checkin_test_run_case(
      \
      self,
      \
      "do_all_push_no_local_commits_push_fail",
      \
      " --enable-packages=Teuchos" \
      " --make-options=-j3 --ctest-options=-j5 --do-all --push",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsNoChangesPullPasses \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +"IT: git pull && git rebase origin/trackingbranch; 0; 'final git pull and rebase passed'\n" \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      g_expectedRegexUpdateWithBuildCasePasses \
      +g_expectedRegexExplicitConfigPasses \
      +g_expectedRegexBuildPasses \
      +g_expectedRegexTestPasses \
      +"0) MPI_DEBUG => passed: passed=100,notpassed=0\n" \
      +"1) SERIAL_RELEASE => passed: passed=100,notpassed=0\n" \
      +"=> A PUSH IS READY TO BE PERFORMED!\n" \
      +"No local commits exit!\n" \
      +"Skipping amending last commit because there are no local commits!\n" \
      +"Attempting to do the push ...\n" \
      +"Skipping push to .. because there are no commits!\n" \
      +"Push failed because the push was never attempted!\n" \
      +"^PUSH FAILED: Trilinos:\n" \
      )


  def test_do_all_default_builds_mpi_debug_push_no_tests_fail(self):
    checkin_test_run_case(
      \
      self,
      \
      "do_all_default_builds_mpi_debug_push_no_tests_fail",
      \
      "--make-options=-j3 --ctest-options=-j5 --default-builds=MPI_DEBUG --do-all --push",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsConfigBuildPasses \
      +"IT: ctest -j5; 0; 'No tests were found!!!'\n" \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      g_expectedRegexConfigPasses \
      +g_expectedRegexBuildPasses \
      +"No tests failed!\n"\
      +"CTest was invoked but no tests were run!\n"\
      +"At least one of the actions (pull, configure, built, test) failed or was not performed correctly!\n" \
       +"0) MPI_DEBUG => FAILED: no tests run\n" \
      +"=> A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      +"^FAILED CONFIGURE/BUILD/TEST: Trilinos:\n" \
      +"REQUESTED ACTIONS: FAILED\n" \
      )


  def test_local_do_all_default_builds_mpi_debug_push_fail(self):
    checkin_test_run_case(
      \
      self,
      \
      "local_do_all_default_builds_mpi_debug_push_fail",
      \
      "--make-options=-j3 --ctest-options=-j5 --default-builds=MPI_DEBUG --local-do-all --push",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsDiffOnlyPasses \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      g_expectedRegexConfigPasses \
      +g_expectedRegexBuildPasses \
      +g_expectedRegexTestPasses \
      +"0) MPI_DEBUG => passed: passed=100,notpassed=0\n" \
      +"A current successful pull does \*not\* exist => Not ready for final push!\n"\
      +"=> A PUSH IS \*NOT\* READY TO BE PERFORMED!\n"\
      +"^ABORTED COMMIT/PUSH: Trilinos:\n" \
      +"REQUESTED ACTIONS: FAILED\n" \
      )


  def test_extra_repo_1_no_changes_do_all_push_fail(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_no_changes_do_all_push_fail",
      \
      "--make-options=-j3 --ctest-options=-j5" \
      " --enable-packages=Teuchos" \
      " --extra-repos=preCopyrightTrilinos --default-builds=MPI_DEBUG --do-all --push", \
      \
      g_cmndinterceptsExtraRepo1NoChangesDoAllUpToPush \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      "Skipping push to .. because there are no commits!\n" \
      "Skipping push to .preCopyrightTrilinos. because there are no commits!\n" \
      +"Push failed because the push was never attempted!\n" \
      +"PUSH FAILED: Trilinos:\n" \
      +"REQUESTED ACTIONS: FAILED\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_send_email_only_on_failure_do_all_mpi_debug_build_configure_fail(self):
    checkin_test_run_case(
      \
      self,
      \
      "send_email_only_on_failure_do_all_mpi_debug_build_configure_fail",
      \
      "--make-options=-j3 --ctest-options=-j5" \
      +" --send-email-only-on-failure" \
      +" --do-all --push" \
      ,
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +"IT: \./do-configure; 1; 'do-configure failed'\n" \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsConfigBuildTestPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      g_expectedRegexUpdateWithBuildCasePasses \
      +"Configure failed returning 1!\n" \
      +"The configure FAILED!\n" \
      +"The build was never attempted!\n" \
      +"The tests were never even run!\n" \
      +"FAILED: configure failed\n" \
      +g_expectedRegexConfigPasses \
      +g_expectedRegexBuildPasses \
      +g_expectedRegexTestPasses \
      +g_expectedCommonOptionsSummary \
      +"Running: mailx -s .FAILED: Trilinos/MPI_DEBUG: configure failed. bogous@somwhere.com\n" \
      +"SERIAL_RELEASE: Skipping sending build/test case email because it passed and --send-email-only-on-failure was set!\n" \
      +"Running: mailx -s .FAILED CONFIGURE/BUILD/TEST: Trilinos: .* bogous@somwhere.com\n" \
      )


  def test_extra_repo_1_mispell_repo_fail(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "test_extra_repo_1_mispell_repo_fail",
      \
      " --extra-repos=preCopyrightTrilinosMispell", \
      \
      g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      "Error, the specified git repo .preCopyrightTrilinosMispell. directory .*preCopyrightTrilinosMispell. does not exist!\n"
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      ,
      grepForFinalPassFailStr=False \
      )


  def test_extra_repo_1_initial_trilinos_pull_fail(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_initial_trilinos_pull_fail",
      \
      " --extra-repos=preCopyrightTrilinos --default-builds=MPI_DEBUG --pull", \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsPullOnlyFails \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      "pullInitial.out\n" \
      +"Pull failed!\n" \
      +"Skipping getting list of modified files because pull failed!\n" \
      +"INITIAL PULL FAILED: Trilinos:\n" \
      +"REQUESTED ACTIONS: FAILED\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_1_initial_extra_repo_pull_fail(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_initial_extra_repo_pull_fail",
      \
      " --extra-repos=preCopyrightTrilinos --default-builds=MPI_DEBUG --pull", \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyFails \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      "pullInitial.out\n" \
      "pullInitial.preCopyrightTrilinos.out\n" \
      +"Pull failed!\n" \
      +"Skipping getting list of modified files because pull failed!\n" \
      +"INITIAL PULL FAILED: Trilinos:\n" \
      +"REQUESTED ACTIONS: FAILED\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_1_extra_pull_trilinos_fail(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_extra_pull_trilinos_fail",
      \
      " --extra-repos=preCopyrightTrilinos --default-builds=MPI_DEBUG --pull --extra-pull-from=ssg:master", \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyFails \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      "pullInitial.out\n" \
      "pullInitial.preCopyrightTrilinos.out\n" \
      "pullInitialExtra.out\n" \
      +"Pull failed!\n" \
      +"Skipping getting list of modified files because pull failed!\n" \
      +"INITIAL PULL FAILED: Trilinos:\n" \
      +"REQUESTED ACTIONS: FAILED\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_1_extra_pull_extra_repo_fail(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_extra_pull_extra_repo_fail",
      \
      " --extra-repos=preCopyrightTrilinos --default-builds=MPI_DEBUG --pull --extra-pull-from=ssg:master", \
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +cmndinterceptsGetRepoStatsPass() \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyPasses \
      +g_cmndinterceptsPullOnlyFails \
      +cmndinterceptsGetRepoStatsPass() \
      +cmndinterceptsGetRepoStatsPass() \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      "pullInitial.out\n" \
      "pullInitial.preCopyrightTrilinos.out\n" \
      "pullInitialExtra.out\n" \
      "pullInitialExtra.preCopyrightTrilinos.out\n" \
      "Pull failed!\n" \
      "Skipping getting list of modified files because pull failed!\n" \
      "INITIAL PULL FAILED: Trilinos:\n" \
      "REQUESTED ACTIONS: FAILED\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_1_do_all_final_pull_trilinos_fails(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_do_all_final_pull_trilinos_fails",
      \
      "--make-options=-j3 --ctest-options=-j5" \
      " --extra-repos=preCopyrightTrilinos --default-builds=MPI_DEBUG --do-all --push", \
      \
      g_cmndinterceptsExtraRepo1DoAllThroughTest \
      +g_cmndinterceptsFinalPullRebaseFails \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail
      ,
      \
      False,
      \
      "pullFinal.out\n" \
      "FINAL PULL FAILED: Trilinos:\n" \
      "REQUESTED ACTIONS: FAILED\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_1_do_all_final_pull_extra_repo_fails(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_do_all_final_pull_extra_repo_fails",
      \
      "--make-options=-j3 --ctest-options=-j5" \
      " --extra-repos=preCopyrightTrilinos --default-builds=MPI_DEBUG --do-all --push", \
      \
      g_cmndinterceptsExtraRepo1DoAllThroughTest \
      +g_cmndinterceptsFinalPullRebasePasses \
      +g_cmndinterceptsFinalPullRebaseFails \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail
      ,
      \
      False,
      \
      "pullFinal.out\n" \
      "pullFinal.preCopyrightTrilinos.out\n" \
      ".preCopyrightTrilinos.: Pull failed!\n" \
      "Final pull failed!\n" \
      "FINAL PULL FAILED: Trilinos:\n" \
      "REQUESTED ACTIONS: FAILED\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_1_do_all_final_amend_trilinos_fails(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_do_all_final_amend_trilinos_fails",
      \
      "--make-options=-j3 --ctest-options=-j5" \
      " --extra-repos=preCopyrightTrilinos --default-builds=MPI_DEBUG --do-all --push", \
      \
      g_cmndinterceptsExtraRepo1DoAllThroughTest \
      +g_cmndinterceptsFinalPullRebasePasses \
      +g_cmndinterceptsFinalPullRebasePasses \
      +g_cmnginterceptsGitLogCmnds \
      +g_cmndinterceptsAmendCommitFails \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail
      ,
      \
      False,
      \
      "commitFinalBody.out\n" \
      "Appending test results to last commit failed!\n" \
      "AMEND COMMIT FAILED: Trilinos:\n" \
      "REQUESTED ACTIONS: FAILED\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_1_do_all_final_amend_extra_repo_fails(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_do_all_final_amend_extra_repo_fails",
      \
      "--make-options=-j3 --ctest-options=-j5" \
      " --extra-repos=preCopyrightTrilinos --default-builds=MPI_DEBUG --do-all --push", \
      \
      g_cmndinterceptsExtraRepo1DoAllThroughTest \
      +g_cmndinterceptsFinalPullRebasePasses \
      +g_cmndinterceptsFinalPullRebasePasses \
      +g_cmnginterceptsGitLogCmnds \
      +g_cmndinterceptsAmendCommitPasses \
      +g_cmnginterceptsGitLogCmnds \
      +g_cmndinterceptsAmendCommitFails \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail
      ,
      \
      False,
      \
      "commitFinalBody.out\n" \
      "commitFinalBody.preCopyrightTrilinos.out\n" \
      "Appending test results to last commit failed!\n" \
      "AMEND COMMIT FAILED: Trilinos:\n" \
      "REQUESTED ACTIONS: FAILED\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_1_do_all_final_push_trilinos_fails(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_do_all_final_push_trilinos_fails",
      \
      "--make-options=-j3 --ctest-options=-j5" \
      " --extra-repos=preCopyrightTrilinos --default-builds=MPI_DEBUG --do-all --push", \
      \
      g_cmndinterceptsExtraRepo1DoAllUpToPush \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsPushOnlyFails \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      "Push failed!\n" \
      "PUSH FAILED: Trilinos:\n" \
      "REQUESTED ACTIONS: FAILED\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  def test_extra_repo_1_do_all_final_push_extra_repo_fails(self):
    projectDepsXmlFileOverride=g_testBaseDir+"/TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml"
    checkin_test_run_case(
      \
      self,
      \
      "extra_repo_1_do_all_final_push_trilinos_fails",
      \
      "--make-options=-j3 --ctest-options=-j5" \
      " --extra-repos=preCopyrightTrilinos --default-builds=MPI_DEBUG --do-all --push", \
      \
      g_cmndinterceptsExtraRepo1DoAllUpToPush \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsPushOnlyPasses \
      +g_cmndinterceptsPushOnlyFails \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      False,
      \
      "Push failed!\n" \
      "PUSH FAILED: Trilinos:\n" \
      "REQUESTED ACTIONS: FAILED\n" \
      ,
      \
      envVars = [ "CHECKIN_TEST_DEPS_XML_FILE_OVERRIDE="+projectDepsXmlFileOverride ]
      )


  # G) Test --use-ninja

  def test_use_ninja_build_only(self):
    checkin_test_run_case(
      self,
      \
      "use_ninja_build_only",
      \
      "--make-options=\"-j6 -k 99999\" --default-builds=MPI_DEBUG" \
      +" --use-ninja --pull --configure --build",
      \
      g_cmndinterceptsDumpDepsXMLFile \
      +g_cmndinterceptsPullPasses \
      +g_cmndinterceptsConfigPasses \
      +"IT: ninja -j6 -k 99999; 0; 'ninja passed'\n" \
      +g_cmndinterceptsSendBuildTestCaseEmail \
      +g_cmndinterceptsLogCommitsPasses \
      +g_cmndinterceptsSendFinalEmail \
      ,
      \
      True,
      \
      g_expectedRegexUpdateWithBuildCasePasses+ \
      "\-\-use-ninja\n" \
      "Configure passed!\n" \
      "touch configure.success\n" \
      "Build passed!\n" \
      "Skipping the tests on request!\n" \
      "0) MPI_DEBUG => passed: build-only passed => Not ready to push!\n" \
      "A PUSH IS \*NOT\* READY TO BE PERFORMED!\n" \
      "^PASSED [(]NOT READY TO PUSH[)]: Trilinos:\n" \
      ,
      [
        ("MPI_DEBUG/do-configure.base",
         "\-GNinja\n"\
           +"\-DTrilinos_ENABLE_TESTS:BOOL=ON\n" \
           +"\-DCMAKE_BUILD_TYPE:STRING=RELEASE\n" \
           +"\-DTrilinos_ENABLE_DEBUG:BOOL=ON\n" \
           )
        ]
      )


def suite():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(testCheckinTest))
    return suite


from optparse import OptionParser


if __name__ == '__main__':

  from GetWithCmake import *
  
  if os.path.exists(g_checkin_test_tests_dir):
    echoRunSysCmnd("rm -rf "+g_checkin_test_tests_dir, verbose=g_verbose)

  unittest.main()
