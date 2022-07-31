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
# Unit testing code for SnapshotDir.py #
########################################

from unittest_helpers import *

from SnapshotDir import *
import unittest
import re


#
# Unit test support code
#


scriptsDir = getScriptBaseDir()


class WriteToString:
  def __init__(self):
    self.str = ""
  def write(self, s):
    self.str += s;
  def flush(self):
    None
  def getStr(self):
    return self.str


def getDummyDefaultOptions():
  dummyDefaultOptions = DefaultOptions()
  dummyDefaultOptions.setDefaultOrigDir("dummy/orig/dir/")
  dummyDefaultOptions.setDefaultDestDir("dummy/dest/dir/")
  return dummyDefaultOptions


# Run a snapshot-dir.py test case using mock commands all in memory without
# actually doing anyting on the disk or the filesystem.
#
def runSnapshotDirTestCase(testObject, cmndLineArgsList, cmndInterceptList,
  passRegexExpressionsList, defaultOptions=None \
  ):

  # Set up default options
  if not defaultOptions:
    defaultOptions = getDummyDefaultOptions()

  # Set up the command intercepts
  g_sysCmndInterceptor.readCommandsFromStr("".join(cmndInterceptList))
  g_sysCmndInterceptor.setAllowExtraCmnds(False)

  # Run the command, intercept the output, and test it
  sout = WriteToString()

  rtn = snapshotDirMainDriver(cmndLineArgsList, defaultOptions, sout)
  g_sysCmndInterceptor.assertAllCommandsRun()
  ostr = sout.getStr()
  #print("ostr =", ostr)
  for passRegexExpr in passRegexExpressionsList:
    try:
      testObject.assert_(re.search(passRegexExpr, ostr))
    except Exception as e:
      print("\n\nCould not find regex='" + passRegexExpr + "' in generated "
            "output:\n")
      print(sout.getStr() + "\n\n")
      printStackTrace()
      raise


#
# Standard commands used in snapshot-dir.py
#

g_gitDiffHead = "IT: git diff --name-status HEAD -- \.; 0;''\n"

g_gitRevParse = "IT: git rev-parse --abbrev-ref --symbolic-full-name ..u.; 0; 'remotename/remotebranch'\n"

g_gitRevParseDetailedHead = "IT: git rev-parse --abbrev-ref --symbolic-full-name ..u.; 1; ''\n"

g_gitRemote = "IT: git remote -v; 0; 'remotename\tsome-url-location (fetch)'\n"

g_gitDescribe = "IT: git describe; 0; 'v1.2.3-225-g9877045'\n"

g_gitLog = "IT: git log  --pretty=.*; 0; 'one commit msg'\n"

g_gitClean = "IT: git clean -xdf; 0; 'clean passed'\n"

g_rsync = "IT: rsync -cav --delete --exclude=.* dummy/orig-dir/ dummy/dest-dir/; 0; 'sync passed'\n"

g_gitLogSha1 = "IT: git log -1 --pretty=format:'.h' -- [.]; 0; 'abc123'\n"

g_gitAdd = "IT: git add \.; 0; 'added some files'\n"

g_gitCommit = "IT: git commit -m .+; 0; 'did a commit'\n"

g_gitCommit_no_verify = "IT: git commit --no-verify -m .+; 0; 'did a commit'\n"


#
# Unit test SnapshotDir
#

class test_snapshot_dir(unittest.TestCase):


  def test_show_defaults(self):
    runSnapshotDirTestCase(
      self,
      ["--show-defaults"],
      [],
      [
        "Script: snapshot-dir\.py",
        "--orig-dir='dummy/orig/dir/'",
        "--dest-dir='dummy/dest/dir/'"
        ]
      )


  def test_show_defaults_with_exclude(self):
    runSnapshotDirTestCase(
      self,
      ["--show-defaults", "--exclude", "foo", "bar*", "baz/"],
      [],
      [
        "Script: snapshot-dir\.py",
        "--orig-dir='dummy/orig/dir/'",
        "--dest-dir='dummy/dest/dir/'",
        "--exclude foo bar\* baz/"
        ]
      )


  def test_override_orig_dest_dirs(self):
    runSnapshotDirTestCase(
      self,
      ["--show-defaults", "--orig-dir=new/orig-dir/", "--dest-dir=new/dest-dir/"],
      [],
      [
        "Script: snapshot-dir\.py",
        "--orig-dir='new/orig-dir/'",
        "--dest-dir='new/dest-dir/'"
        ]
     )


  def test_snapshot_default(self):
    runSnapshotDirTestCase(
      self,
      ["--orig-dir=dummy/orig-dir/", "--dest-dir=dummy/dest-dir/"],
      [
        g_gitDiffHead,
        g_gitDiffHead,
        g_gitRevParse,
        g_gitRemote,
        g_gitDescribe,
        g_gitLog,
        g_rsync,
        g_gitLogSha1,
        g_gitAdd,
        g_gitCommit,
        ],
      [
        "Script: snapshot-dir\.py",
        "--orig-dir='dummy/orig-dir/'",
        "--dest-dir='dummy/dest-dir/'",
        "origin remote name = 'remotename'",
        "origin remote branch = 'remotebranch'",
        "origin remote URL = 'some-url-location'",
        "Git describe = 'v1.2.3-225-g9877045'",
        "Automatic snapshot commit from orig-dir at abc123",
        "Origin repo remote tracking branch: 'remotename/remotebranch'",
        "Origin repo remote repo URL: 'remotename = some-url-location'",
        "one commit msg"
        ]
     )


  def test_snapshot_default_no_op(self):
    runSnapshotDirTestCase(
      self,
      ["--orig-dir=dummy/orig-dir/", "--dest-dir=dummy/dest-dir/", "--no-op"],
      [
        g_gitDiffHead,
        g_gitDiffHead,
        g_gitRevParse,
        g_gitRemote,
        g_gitDescribe,
        g_gitLog,
        g_gitLogSha1,
        ],
      [
        "Script: snapshot-dir\.py",
        "--orig-dir='dummy/orig-dir/'",
        "--dest-dir='dummy/dest-dir/'",
        "origin remote name = 'remotename'",
        "origin remote branch = 'remotebranch'",
        "origin remote URL = 'some-url-location'",
        "Git describe = 'v1.2.3-225-g9877045'",
        "Would be running: rsync -cav --delete --exclude=\\\[.]git dummy/orig-dir/ dummy/dest-dir/",
        "Automatic snapshot commit from orig-dir at abc123",
        "Origin repo remote tracking branch: 'remotename/remotebranch'",
        "Origin repo remote repo URL: 'remotename = some-url-location'",
        "one commit msg",
        "Would be running: git add .",
        "Would be running: git commit -m \"<commit-msg>\"",
        ]
     )


  def test_snapshot_detached_head(self):
    runSnapshotDirTestCase(
      self,
      ["--orig-dir=dummy/orig-dir/", "--dest-dir=dummy/dest-dir/"],
      [
        g_gitDiffHead,
        g_gitDiffHead,
        g_gitRevParseDetailedHead,
        g_gitRemote,
        g_gitDescribe,
        g_gitLog,
        g_rsync,
        g_gitLogSha1,
        g_gitAdd,
        g_gitCommit,
        ],
      [
        "Script: snapshot-dir\.py",
        "--orig-dir='dummy/orig-dir/'",
        "--dest-dir='dummy/dest-dir/'",
        "origin remote name = 'remotename'",
        "origin remote branch = ''",
        "origin remote URL = 'some-url-location'",
        "Git describe = 'v1.2.3-225-g9877045'",
        "Automatic snapshot commit from orig-dir at abc123",
        "Origin repo remote repo URL: 'remotename = some-url-location'",
        "one commit msg"
        ]
     )


  def test_snapshot_default_missing_trailing_slash(self):
    runSnapshotDirTestCase(
      self,
      ["--orig-dir=dummy/orig-dir", "--dest-dir=dummy/dest-dir"],
      [
        g_gitDiffHead,
        g_gitDiffHead,
        g_gitRevParse,
        g_gitRemote,
        g_gitDescribe,
        g_gitLog,
        g_rsync,
        g_gitLogSha1,
        g_gitAdd,
        g_gitCommit,
        ],
      [
        "Script: snapshot-dir\.py",
        "--orig-dir='dummy/orig-dir'",
        "--dest-dir='dummy/dest-dir'",
        ]
     )


  def test_snapshot_default_with_exclude(self):
    runSnapshotDirTestCase(
      self,
      ["--orig-dir=dummy/orig-dir/", "--dest-dir=dummy/dest-dir/",
       "--exclude", "foo", "bar*", "baz/"],
      [
        g_gitDiffHead,
        g_gitDiffHead,
        g_gitRevParse,
        g_gitRemote,
        g_gitDescribe,
        g_gitLog,
        g_rsync,
        g_gitLogSha1,
        g_gitAdd,
        g_gitCommit,
        ],
      [
        "Script: snapshot-dir\.py",
        "--orig-dir='dummy/orig-dir/'",
        "--dest-dir='dummy/dest-dir/'",
        "origin remote name = 'remotename'",
        "origin remote branch = 'remotebranch'",
        "origin remote URL = 'some-url-location'",
        "Automatic snapshot commit from orig-dir at abc123",
        "Origin repo remote tracking branch: 'remotename/remotebranch'",
        "Origin repo remote repo URL: 'remotename = some-url-location'",
        "one commit msg",
        "Excluding files/directories/globs: foo bar\* baz/"
        ]
     )


  def test_snapshot_clean_ignored(self):
    runSnapshotDirTestCase(
      self,
      ["--orig-dir=dummy/orig-dir/", "--dest-dir=dummy/dest-dir/",
        "--clean-ignored-files-orig-dir"],
      [
        g_gitDiffHead,
        g_gitDiffHead,
        g_gitClean,
        g_gitRevParse,
        g_gitRemote,
        g_gitDescribe,
        g_gitLog,
        g_rsync,
        g_gitLogSha1,
        g_gitAdd,
        g_gitCommit,
        ],
      [
        "git clean -xdf"
        ]
     )


  def test_snapshot_no_verify_commit(self):
    runSnapshotDirTestCase(
      self,
      ["--orig-dir=dummy/orig-dir/", "--dest-dir=dummy/dest-dir/",
        "--no-verify-commit"],
      [
        g_gitDiffHead,
        g_gitDiffHead,
        g_gitRevParse,
        g_gitRemote,
        g_gitDescribe,
        g_gitLog,
        g_rsync,
        g_gitLogSha1,
        g_gitAdd,
        g_gitCommit_no_verify,
        ],
      [
        "Running: git commit --no-verify -m"
        ]
     )


  def test_snapshot_skip_commit(self):
    runSnapshotDirTestCase(
      self,
      ["--orig-dir=dummy/orig-dir/", "--dest-dir=dummy/dest-dir/",
        "--skip-commit"],
      [
        g_gitDiffHead,
        g_gitDiffHead,
        g_gitRevParse,
        g_gitRemote,
        g_gitDescribe,
        g_gitLog,
        g_rsync,
        g_gitLogSha1,
        ],
      [
        "Skipping commit on request"
        ]
     )


  # ToDo: Test assert failure of clean origDir ...

  # ToDo: Test skipping test of clean origDir ...

  # ToDo: Test assert failure of clean destDir ...

  # ToDo: Test skipping test of clean destDir ...

  # ToDo: Test failing to acquire origin remote URL ...

  # ToDo: Test failure to acquire origin commit ...

  # ToDo: Test failing to create commit in dest repo ...


if __name__ == '__main__':
  unittest.main()
