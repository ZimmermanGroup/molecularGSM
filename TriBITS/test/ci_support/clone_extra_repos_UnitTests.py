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

#########################################################
# Unit testing code for TribitsPackageFilePathUtils.py #
######################################################### 

from FindCISupportDir import *

from clone_extra_repos import *

import sys
import unittest

#
# Mock options
#

class MockOptions:

  def __init__(self, verbLevel=1):
   self.verbLevel = verbLevel


#
# Test getExtraReposDictListFromCmakefile()
#

g_extraReposDictListFull = [
  {'CATEGORY': 'Continuous', 'NAME': 'ExtraRepo1', 'REPOURL': 'someurl.com:/ExtraRepo1', 'REPOTYPE': 'GIT', 'HASPKGS': 'HASPACKAGES', 'PREPOST': 'POST', 'DIR': 'ExtraRepo1'},
  {'CATEGORY': 'Nightly', 'NAME': 'ExtraRepo2', 'REPOURL': 'someurl2.com:/ExtraRepo2', 'REPOTYPE': 'GIT', 'HASPKGS': 'NOPACKAGES', 'PREPOST': 'POST', 'DIR': 'packages/SomePackage/Blah'},
  {'CATEGORY': 'Continuous', 'NAME': 'ExtraRepo3', 'REPOURL': 'someurl3.com:/ExtraRepo3', 'REPOTYPE': 'HG', 'HASPKGS': 'HASPACKAGES', 'PREPOST': 'POST', 'DIR': 'ExtraRepo3'},
  {'CATEGORY': 'Nightly', 'NAME': 'ExtraRepo4', 'REPOURL': 'someurl4.com:/ExtraRepo4', 'REPOTYPE': 'SVN', 'HASPKGS': 'HASPACKAGES', 'PREPOST': 'POST', 'DIR': 'ExtraRepo4'},
  ]

class test_getExtraReposDictListFromCmakefile(unittest.TestCase):


  def test_ExtraReposList_Nightly(self):
    global g_withCmake
    extraReposDict = getExtraReposDictListFromCmakefile(
      projectDir = tribitsDir+"/examples/MockTrilinos",
      extraReposFile = testCiSupportDir+"/ExtraReposList.cmake",
      withCmake = g_withCmake,
      extraReposType = "Nightly",
      extraRepos = "",
      verbose=False)
    extraReposDict_expected = g_extraReposDictListFull
    self.assertEqual(extraReposDict, extraReposDict_expected)


  def test_ExtraReposList_Continuous(self):
    global g_withCmake
    extraReposDict = getExtraReposDictListFromCmakefile(
      projectDir = tribitsDir+"/examples/MockTrilinos",
      extraReposFile = testCiSupportDir+"/ExtraReposList.cmake",
      withCmake = g_withCmake,
      extraReposType = "Continuous",
      extraRepos = "",
      verbose=False)
    extraReposDict_expected = [ g_extraReposDictListFull[0], g_extraReposDictListFull[2] ]
    self.assertEqual(extraReposDict, extraReposDict_expected)


  def test_ExtraReposList_extraRepos_1(self):
    global g_withCmake
    extraReposDict = getExtraReposDictListFromCmakefile(
      projectDir = tribitsDir+"/examples/MockTrilinos",
      extraReposFile = testCiSupportDir+"/ExtraReposList.cmake",
      withCmake = g_withCmake,
      extraReposType = "Nightly",
      extraRepos = "ExtraRepo2",
      verbose=False)
    extraReposDict_expected = [ g_extraReposDictListFull[1] ]
    self.assertEqual(extraReposDict, extraReposDict_expected)


  def test_ExtraReposList_extraRepos_2(self):
    global g_withCmake
    extraReposDict = getExtraReposDictListFromCmakefile(
      projectDir = tribitsDir+"/examples/MockTrilinos",
      extraReposFile = testCiSupportDir+"/ExtraReposList.cmake",
      withCmake = g_withCmake,
      extraReposType = "Nightly",
      extraRepos = "ExtraRepo2,ExtraRepo4",
      verbose=False)
    extraReposDict_expected = [ g_extraReposDictListFull[1], g_extraReposDictListFull[3] ]
    self.assertEqual(extraReposDict, extraReposDict_expected)


#
# Test filterOutNotExtraRepos()
#

class test_filterOutNotExtraRepos(unittest.TestCase):


  def test_notExtraRepos_2(self):
    extraRepoDict2 = \
      {'CATEGORY': 'Nightly', 'NAME': 'ExtraRepo2',
       'REPOURL': 'someurl2.com:/ExtraRepo2', 'REPOTYPE': 'GIT',
       'HASPKGS': 'NOPACKAGES', 'PREPOST': 'POST', 'DIR': 'packages/SomePackage/Blah'}
    extraRepoDict4 = \
      {'CATEGORY': 'Nightly', 'NAME': 'ExtraRepo4',
       'REPOURL': 'someurl4.com:/ExtraRepo4', 'REPOTYPE': 'SVN',
       'HASPKGS': 'HASPACKAGES', 'PREPOST': 'POST', 'DIR': 'ExtraRepo4'}
    extraReposDict = filterOutNotExtraRepos(
      [
        {'NAME': 'ExtraRepo1'},
        extraRepoDict2,
        {'NAME': 'ExtraRepo3'},
        extraRepoDict4,
        ],
      ["ExtraRepo1", "ExtraRepo3"]
      )
    extraReposDict_expected = [
      extraRepoDict2,
      extraRepoDict4,
      ]
    self.assertEqual(extraReposDict, extraReposDict_expected)


#
# Test getExtraReposTable()
#

class test_getExtraReposTable(unittest.TestCase):


  def test_ExtraReposList_extraRepos_2(self):
    global g_withCmake
    projectDir = tribitsDir+"/examples/MockTrilinos"
    extraReposFile = testCiSupportDir+"/ExtraReposList.cmake"
    withCmake = g_withCmake
    extraReposType = "Nightly"
    extraRepos = "ExtraRepo1,ExtraRepo3"
    extraReposDict = getExtraReposDictListFromCmakefile(projectDir, extraReposFile,
      withCmake, extraReposType, extraRepos, verbose=False)
    extraReposTable = getExtraReposTable(extraReposDict)
    extraReposTable_expected = \
      "------------------------------------------------------------------------------\n" \
      "| ID | Repo Name  | Repo Dir   | VC  | Repo URL                 | Category   |\n" \
      "|----|------------|------------|-----|--------------------------|------------|\n" \
      "|  1 | ExtraRepo1 | ExtraRepo1 | GIT | someurl.com:/ExtraRepo1  | Continuous |\n" \
      "|  2 | ExtraRepo3 | ExtraRepo3 | HG  | someurl3.com:/ExtraRepo3 | Continuous |\n" \
      "------------------------------------------------------------------------------\n"
    self.assertEqual(extraReposTable, extraReposTable_expected)


#
# Test isVerbosityLevel()
#

class test_isVerbosityLevel(unittest.TestCase):

  def test_none(self):
    inOptions = MockOptions("none")
    self.assertEqual(isVerbosityLevel(inOptions, "none"), True)
    self.assertEqual(isVerbosityLevel(inOptions, "minimal"), False)
    self.assertEqual(isVerbosityLevel(inOptions, "more"), False)
    self.assertEqual(isVerbosityLevel(inOptions, "most"), False)

  def test_minimal(self):
    inOptions = MockOptions("minimal")
    self.assertEqual(isVerbosityLevel(inOptions, "none"), True)
    self.assertEqual(isVerbosityLevel(inOptions, "minimal"), True)
    self.assertEqual(isVerbosityLevel(inOptions, "more"), False)
    self.assertEqual(isVerbosityLevel(inOptions, "most"), False)

  def test_more(self):
    inOptions = MockOptions("more")
    self.assertEqual(isVerbosityLevel(inOptions, "none"), True)
    self.assertEqual(isVerbosityLevel(inOptions, "minimal"), True)
    self.assertEqual(isVerbosityLevel(inOptions, "more"), True)
    self.assertEqual(isVerbosityLevel(inOptions, "most"), False)

  def test_most(self):
    inOptions = MockOptions("most")
    self.assertEqual(isVerbosityLevel(inOptions, "none"), True)
    self.assertEqual(isVerbosityLevel(inOptions, "minimal"), True)
    self.assertEqual(isVerbosityLevel(inOptions, "more"), True)
    self.assertEqual(isVerbosityLevel(inOptions, "most"), True)


#
# Test parseRawSshGitoliteRootInfoOutput()
#

g_rawSshGitoliteRootInfoOutput_1 = \
r"""hello 8vt, this is git@casl-dev running gitolite3 v3.6-11-g07ce4b9 on git 1.7.1

 R W	ExtraRepo1
 R  	ExtraRepo2
 R W	SomeOtherRepo
"""


class test_parseRawSshGitoliteRootInfoOutput(unittest.TestCase):

  def test_reposAreListed(self):
    gitoliteReposList = parseRawSshGitoliteRootInfoOutput(
      g_rawSshGitoliteRootInfoOutput_1)
    gitoliteReposList_expected = ["ExtraRepo1", "ExtraRepo2", "SomeOtherRepo" ]
    self.assertEqual(gitoliteReposList, gitoliteReposList_expected)


#
# Test filterOutMissingGitoliteRepos()
#


class test_filterOutMissingGitoliteRepos(unittest.TestCase):

  def test_removeSomeRepos(self):
    extraReposDictList = filterOutMissingGitoliteRepos(
      g_extraReposDictListFull, [ "ExtraRepo1", "ExtraRepo2", "ExtraRepo4" ])
    extraReposDictList_expected = [ g_extraReposDictListFull[0],
      g_extraReposDictListFull[1], g_extraReposDictListFull[3] ]
    self.assertEqual(extraReposDictList, extraReposDictList_expected)


#
# Test clone_extra_repos.py
#


def clone_extra_repos_cmnd(extraOptions, throwOnError=True):
  global g_withCmake
  cmnd = ciSupportDir+"/clone_extra_repos.py --with-cmake="+g_withCmake+" "+extraOptions
  return getCmndOutput(
    cmnd,
    workingDir=tribitsDir+"/examples/MockTrilinos",
    stripTrailingSpaces=False,
    getStdErr=False,
    throwOnError=throwOnError)


def getScriptEchoCmndLine(
  extraRepos="", extraReposFile="cmake/ExtraRepositoriesList.cmake",
  notExtraRepos="", doClone=True, doOp=True, verbosity="more",
  createGitdistFile="",
  ):
  global g_withCmake
  cmndLineEchoStr = \
    "\n**************************************************************************\n" \
    "Script: clone_extra_repos.py \\\n" \
    "  --extra-repos='"+extraRepos+"' \\\n" \
    "  --not-extra-repos='"+notExtraRepos+"' \\\n" \
    "  --extra-repos-file='"+extraReposFile+"' \\\n" \
    "  --extra-repos-type='Nightly' \\\n" \
    "  --gitolite-root='' \\\n" \
    "  --with-cmake='"+g_withCmake+"' \\\n" \
    "  --verbosity='"+verbosity+"' \\\n"
  if doClone: cmndLineEchoStr += "  --do-clone \\\n"
  else:       cmndLineEchoStr += "  --skip-clone \\\n"
  if doOp:    cmndLineEchoStr += "  --do-op \\\n"
  else:       cmndLineEchoStr += "  --no-op \\\n"
  cmndLineEchoStr += \
    "  --create-gitdist-file='"+createGitdistFile+"' \\\n" \
    "\n"
  return cmndLineEchoStr
  

class test_clone_extra_repos(unittest.TestCase):

  def test_skip_clone_verbosity_none(self):
    output = clone_extra_repos_cmnd("--skip-clone --verbosity=none")
    output_expected = ""
    self.assertEqual(output, output_expected)

  def test_skip_clone_verbosity_mimimal(self):
    output = clone_extra_repos_cmnd("--skip-clone --verbosity=minimal")
    output_expected = getScriptEchoCmndLine(doClone=False, verbosity="minimal")
    self.assertEqual(output, output_expected)

  def test_skip_clone_verbosity_more(self):
    output = clone_extra_repos_cmnd("--skip-clone --verbosity=more")
    output_expected = \
      getScriptEchoCmndLine(doClone=False, verbosity="more") + \
        "\n" \
        "***\n" \
        "*** List of selected extra repos to clone:\n" \
        "***\n\n" \
        "-----------------------------------------------------------------------------------------------------------\n" \
        "| ID | Repo Name            | Repo Dir                | VC  | Repo URL                       | Category   |\n" \
        "|----|----------------------|-------------------------|-----|--------------------------------|------------|\n" \
        "|  1 | preCopyrightTrilinos | preCopyrightTrilinos    | GIT | url1:/git/preCopyrightTrilinos | Continuous |\n" \
        "|  2 | extraTrilinosRepo    | extraTrilinosRepo       | GIT | usr2:/git/extraTrilinosRepo    | Nightly    |\n" \
        "|  3 | Dakota               | packages/TriKota/Dakota | GIT | url3:/git/Dakota               | Continuous |\n" \
        "-----------------------------------------------------------------------------------------------------------\n" \
        "\n"
    self.assertEqual(output, output_expected)

  def test_clone_git_repos_already_exists(self):
    output = clone_extra_repos_cmnd(
      "--verbosity=minimal --extra-repos=preCopyrightTrilinos,extraTrilinosRepo")
    output_expected = \
      getScriptEchoCmndLine(verbosity="minimal",
                              extraRepos="preCopyrightTrilinos,extraTrilinosRepo") + \
        "\n" \
        "***\n" \
        "*** Clone the selected extra repos:\n" \
        "***\n" \
        "\n" \
        "\n" \
        "Cloning repo preCopyrightTrilinos ...\n" \
        "\n" \
        "  ==> Repo dir = 'preCopyrightTrilinos' already exists.  Skipping clone!\n" \
        "\n" \
        "Cloning repo extraTrilinosRepo ...\n" \
        "\n" \
        "  ==> Repo dir = 'extraTrilinosRepo' already exists.  Skipping clone!\n"
    self.assertEqual(output, output_expected)

  def test_clone_git_repos_1_2(self):
    extraReposFile = testCiSupportDir+"/ExtraReposList.cmake"
    output = clone_extra_repos_cmnd(
      "--verbosity=minimal --extra-repos=ExtraRepo1,ExtraRepo2" \
      " --extra-repos-file="+extraReposFile+" --no-op")
    output_expected = \
      getScriptEchoCmndLine(verbosity="minimal",
                              extraRepos="ExtraRepo1,ExtraRepo2",
                              extraReposFile=extraReposFile,
                              doOp=False) + \
        "\n" \
        "***\n" \
        "*** Clone the selected extra repos:\n" \
        "***\n" \
        "\n" \
        "\n" \
        "Cloning repo ExtraRepo1 ...\n" \
        "\n" \
        "Running: git clone someurl.com:/ExtraRepo1 ExtraRepo1\n" \
        "\n" \
        "Cloning repo ExtraRepo2 ...\n" \
        "\n" \
        "Running: git clone someurl2.com:/ExtraRepo2 packages/SomePackage/Blah\n"
    self.assertEqual(output, output_expected)

  def test_clone_git_repos_not_3_4(self):
    extraReposFile = testCiSupportDir+"/ExtraReposList.cmake"
    output = clone_extra_repos_cmnd(
      "--verbosity=minimal --not-extra-repos=ExtraRepo3,ExtraRepo4" \
      " --extra-repos-file="+extraReposFile+" --no-op")
    output_expected = \
      getScriptEchoCmndLine(verbosity="minimal",
                              notExtraRepos="ExtraRepo3,ExtraRepo4",
                              extraReposFile=extraReposFile,
                              doOp=False) + \
        "\n" \
        "***\n" \
        "*** Filtering the set of extra repos based on --not-extra-repos:\n" \
        "***\n" \
        "\n" \
        "Excluding extra repo 'ExtraRepo3'!\n" \
        "Excluding extra repo 'ExtraRepo4'!\n" \
        "\n" \
        "***\n" \
        "*** Clone the selected extra repos:\n" \
        "***\n" \
        "\n" \
        "\n" \
        "Cloning repo ExtraRepo1 ...\n" \
        "\n" \
        "Running: git clone someurl.com:/ExtraRepo1 ExtraRepo1\n" \
        "\n" \
        "Cloning repo ExtraRepo2 ...\n" \
        "\n" \
        "Running: git clone someurl2.com:/ExtraRepo2 packages/SomePackage/Blah\n"
    self.assertEqual(output, output_expected)

  def test_generate_gitdist_file(self):
    extraReposFile = testCiSupportDir+"/ExtraReposList.cmake"
    gitdistFile = os.getcwd()+"/gitdist_file"
    output = clone_extra_repos_cmnd(
      "--verbosity=minimal --skip-clone --extra-repos-file="+extraReposFile+ \
      " --create-gitdist-file="+gitdistFile)
    output_expected = \
      getScriptEchoCmndLine(verbosity="minimal",
                              doClone=False,
                              extraReposFile=extraReposFile,
                              createGitdistFile=gitdistFile) + \
        "\n" \
        "***\n" \
        "*** Create the gitdist file:\n" \
        "***\n" \
        "\n" \
        "Writing the file '"+gitdistFile+"' ...\n"
    self.assertEqual(output, output_expected)
    with open(gitdistFile, 'r') as fileHandle:
      gitdistFileStr = fileHandle.read()
    gitdistFileStr_expected = \
      "ExtraRepo1\n" \
      "packages/SomePackage/Blah\n" \
      "ExtraRepo3\n" \
      "ExtraRepo4\n"
    self.assertEqual(gitdistFileStr, gitdistFileStr_expected)


if __name__ == '__main__':

  from GetWithCmake import *

  unittest.main()
