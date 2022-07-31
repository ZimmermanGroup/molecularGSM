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
# Unit testing code for TribitsPackageTestNameUtils.py #
######################################################### 

import os
import sys

ciSupportDir = os.path.abspath(
  os.path.join(
    os.path.dirname(os.path.abspath(__file__)),
    "../..", "tribits/ci_support"
    )
  )
sys.path = [ciSupportDir] + sys.path

from TribitsPackageTestNameUtils import *
import unittest

testingTrilinosDepsXmlInFile = getScriptBaseDir()+"/TrilinosPackageDependencies.gold.xml"
trilinosDependencies = getProjectDependenciesFromXmlFile(testingTrilinosDepsXmlInFile)
  
#print "\ntrilinosDependencies:\n", trilinosDependencies


class test_getPackageNameFromTestName(unittest.TestCase):


  def test_Teuchos_SomeTest1(self):
    self.assertEqual(
      getPackageNameFromTestName( trilinosDependencies, 'Teuchos_SomeTest1' ),
      'Teuchos' )


  def test_Thyra_SomeTest1(self):
    self.assertEqual(
      getPackageNameFromTestName( trilinosDependencies, 'Thyra_SomeTest1' ),
      'Thyra' )


  def test_ThyraCoreLibs_SomeTest2(self):
    self.assertEqual(
      getPackageNameFromTestName( trilinosDependencies, 'ThyraCoreLibs_SomeTest2' ),
      'Thyra' )


  def test_ThyraEpetra_SomeTest3(self):
    self.assertEqual(
      getPackageNameFromTestName( trilinosDependencies, 'ThyraEpetra_SomeTest3' ),
      'Thyra' )


  def test_EpetraExt_SomeTest4(self):
    self.assertEqual(
      getPackageNameFromTestName( trilinosDependencies, 'EpetraExt_SomeTest4' ),
      'EpetraExt' )


class test_getTestNameFromLastTestsFailedLine(unittest.TestCase):


  def test_Teuchos_SomeTest1(self):
    self.assertEqual(
      getTestNameFromLastTestsFailedLine( trilinosDependencies, '1:Teuchos_SomeTest1' ),
      'Teuchos_SomeTest1' )


  def test_ThyraCoreLib_TestName2(self):
    self.assertEqual(
      getTestNameFromLastTestsFailedLine( trilinosDependencies, '50:ThyraCoreLib_TestName2' ),
      'ThyraCoreLib_TestName2' )


class test_getPackageNamesFromLastTestsFailedLines(unittest.TestCase):


  def test_FileList_Empty(self):
    self.assertEqual(
      getPackageNamesFromLastTestsFailedLines(
        trilinosDependencies,
        [],
       ),
      [] )


  def test_FileList_1(self):
    self.assertEqual(
      getPackageNamesFromLastTestsFailedLines(
        trilinosDependencies,
        [
          '20:Teuchos_SomeTest1',
          ] \
       ),
      ['Teuchos'] )


  def test_FileList_2(self):
    self.assertEqual(
      getPackageNamesFromLastTestsFailedLines(
        trilinosDependencies,
        [
          '5:ThyraCoreLibs_SomeTest2',
          '20:Teuchos_SomeTest1',
          ] \
       ),
      ['Thyra', 'Teuchos'] )


  def test_FileList_3_duplicate(self):
    self.assertEqual(
      getPackageNamesFromLastTestsFailedLines(
        trilinosDependencies,
        [
          '5:ThyraCoreLibs_SomeTest2',
          '20:Teuchos_SomeTest1',
          '7:Thyra_SomeTest3',
          ] \
       ),
      ['Thyra', 'Teuchos'] )


  def test_FileList_4_duplicate(self):
    self.assertEqual(
      getPackageNamesFromLastTestsFailedLines(
        trilinosDependencies,
        [
          '5:ThyraCoreLibs_SomeTest2',
          '20:Teuchos_SomeTest1',
          '7:Thyra_SomeTest3',
          '20:Teuchos_SomeTest4',
          ] \
       ),
      ['Thyra', 'Teuchos'] )


def suite():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(testTrilinosPackageFilePathUtils))
    return suite


if __name__ == '__main__':
  unittest.main()
