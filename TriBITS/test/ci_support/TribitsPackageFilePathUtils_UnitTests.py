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

import os
import sys

ciSupportDir = os.path.abspath(
  os.path.join(
    os.path.dirname(os.path.abspath(__file__)),
    "../..", "tribits/ci_support"
    )
  )
sys.path = [ciSupportDir] + sys.path

from TribitsPackageFilePathUtils import *
import unittest


#
# Test isGlobalBuildFileRequiringGlobalRebuild
#

class test_DefaultProjectCiFileChangeLogic(unittest.TestCase):


  def test_CMakeLists_txt(self):
    dpcl = DefaultProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'CMakeLists.txt' ), True )


  def test_PackagesList_cmake(self):
    dpcl = DefaultProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'PackagesList.cmake' ), False )


  def test_TPLsList_cmake(self):
    dpcl = DefaultProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'TPLsList.cmake' ), False )


  def test_Version_cmake(self):
    dpcl = DefaultProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'Version.cmake' ), True )


  def test_Anything_cmake(self):
    dpcl = DefaultProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'Anything.cmake' ), True )


  def test_TrilinosCMakeQuickstart_txt(self):
    dpcl = DefaultProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'cmake/TrilinosCMakeQuickstart.txt' ),
      False )


  def test_TPLsList_cmake(self):
    dpcl = DefaultProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'cmake/ExtraRepositoriesList.cmake' ),
      False )


  def test_experimental_build_test_cmake(self):
    dpcl = DefaultProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'cmake/ctest/experimental_build_test.cmake' ),
      False )


  def test_cmake_ctest_drivers_something(self):
    dpcl = DefaultProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'something/cmake/ctest/drivers/machine/somefile.cmake' ),
      False )


  def test_something_cmake_ctest_drivers_something(self):
    dpcl = DefaultProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'cmake/ctest/drivers/machine/somefile.cmake' ),
      False )


  def test_cmake_UnitTests(self):
    dpcl = DefaultProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'cmake/anything/UnitTests/CMakeLists.txt' ),
      False )


  def test_FindTPLBLAS_cmake(self):
    dpcl = DefaultProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'cmake/TPLs/FindTPLBLAS.cmake' ),
      False )


  def test_FindTPLLAPACK_cmake(self):
    dpcl = DefaultProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'cmake/TPLs/FindTPLLAPACK.cmake' ),
      False )


  def test_FindTPLMPI_cmake(self):
    dpcl = DefaultProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'cmake/TPLs/FindTPLMPI.cmake' ),
      False )


  def test_FindTPLDummy_cmake(self):
    dpcl = DefaultProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'cmake/TPLs/FindTPLDummy.cmake' ),
      False )


  def test_SetNotFound_cmake(self):
    dpcl = DefaultProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'cmake/utils/SetNotFound.cmake' ),
      True )


testingTrilinosDepsXmlInFile = getScriptBaseDir()+"/TrilinosPackageDependencies.gold.xml"
trilinosDependencies = getProjectDependenciesFromXmlFile(testingTrilinosDepsXmlInFile)
  
#print "\ntrilinosDependencies:\n", trilinosDependencies


updateOutputStr = """
? packages/triutils/doc/html
M CMakeLists.txt
M cmake/python/checkin-test.py
M cmake/python/dump-cdash-deps-xml-file.py
A packages/nox/src/dummy.C
P packages/stratimikos/dummy.blah
M packages/thyra/src/Thyra_ConfigDefs.hpp
M packages/thyra/CMakeLists.txt
M packages/ifpack2/CMakeLists.txt
M demos/FEApp/src/CMakeLists.txt
"""

updateOutputList = updateOutputStr.split("\n")


class testProjectPackageFilePathUtils(unittest.TestCase):


  def test_getPackageNameFromPath_01(self):
    self.assertEqual(
      getPackageNameFromPath( trilinosDependencies, 'packages/teuchos/CMakeLists.txt' ),
      'Teuchos' )


  def test_getPackageNameFromPath_02(self):
    self.assertEqual(
      getPackageNameFromPath( trilinosDependencies, 'packages/thyra/src/blob.cpp' ),
      'ThyraCoreLibs' )


  def test_getPackageNameFromPath_03(self):
    self.assertEqual(
      getPackageNameFromPath( trilinosDependencies, 'cmake/CMakeLists.txt' ),
      'TrilinosFramework' )


  def test_getPackageNameFromPath_04(self):
    self.assertEqual(
      getPackageNameFromPath( trilinosDependencies, 'cmake/CMakeLists.txt' ),
      'TrilinosFramework' )


  def test_getPackageNameFromPath_noMatch(self):
    self.assertEqual(
      getPackageNameFromPath( trilinosDependencies, 'packages/blob/blob' ), '' )


  def test_extractFilesListMatchingPattern_01(self):

    modifedFilesList = extractFilesListMatchingPattern( updateOutputList,
      re.compile(r"^[MA] (.+)$") )

    modifedFilesList_expected = \
      [
        "CMakeLists.txt",
        "cmake/python/checkin-test.py",
        "cmake/python/dump-cdash-deps-xml-file.py",
        "packages/nox/src/dummy.C",
        "packages/thyra/src/Thyra_ConfigDefs.hpp",
        "packages/thyra/CMakeLists.txt",
        "packages/ifpack2/CMakeLists.txt",
        "demos/FEApp/src/CMakeLists.txt",
      ]

    self.assertEqual( modifedFilesList, modifedFilesList_expected )


  def test_getPackagesListFromFilePathsList_01(self):

    filesList = extractFilesListMatchingPattern( updateOutputList,
      re.compile(r"^[AMP] (.+)$") )
    
    packagesList = getPackagesListFromFilePathsList( trilinosDependencies, filesList )

    packagesList_expected = \
      [u"TrilinosFramework", u"Stratimikos", u"ThyraCoreLibs", u"Thyra"]

    self.assertEqual( packagesList, packagesList_expected )


  def test_get_trilinos_packages_from_files_list_01(self):

    writeStrToFile( "modifiedFiles.txt",
      "CMakeLists.txt\n" \
      "cmake/python/checkin-test.py\n" \
      "cmake/python/dump-cdash-deps-xml-file.py\n" \
      "packages/thyra/src/Thyra_ConfigDefs.hpp\n" \
      "packages/thyra/CMakeLists.txt\n" \
      )

    self.assertEqual(
      getCmndOutput(ciSupportDir+"/get-tribits-packages-from-files-list.py" \
        " --files-list-file=modifiedFiles.txt --deps-xml-file="+testingTrilinosDepsXmlInFile,
        True),
      "ALL_PACKAGES,TrilinosFramework,ThyraCoreLibs,Thyra"
      )


class testFilterPackagesList(unittest.TestCase):


  def test_get_PT_empty(self):
    self.assertEqual(
      getCmndOutput(ciSupportDir+"/filter-packages-list.py" \
        " --deps-xml-file="+testingTrilinosDepsXmlInFile+"" \
        " --input-packages-list= --keep-test-test-categories=PT",
        True),
      ""
      )


  def test_get_PT(self):
    self.assertEqual(
      getCmndOutput(ciSupportDir+"/filter-packages-list.py" \
        " --deps-xml-file="+testingTrilinosDepsXmlInFile+"" \
        " --input-packages-list=Teuchos,Thyra,Phalanx,Stokhos --keep-test-test-categories=PT",
        True),
      "Teuchos,Thyra"
      )


  def test_get_PT_ALL_PACKAGES_begining(self):
    self.assertEqual(
      getCmndOutput(ciSupportDir+"/filter-packages-list.py" \
        " --deps-xml-file="+testingTrilinosDepsXmlInFile+"" \
        " --input-packages-list=ALL_PACKAGES,Teuchos,Thyra,Phalanx,Stokhos --keep-test-test-categories=PT",
        True),
      "ALL_PACKAGES,Teuchos,Thyra"
      )


  def test_get_PT_ALL_PACKAGES_end(self):
    self.assertEqual(
      getCmndOutput(ciSupportDir+"/filter-packages-list.py" \
        " --deps-xml-file="+testingTrilinosDepsXmlInFile+"" \
        " --input-packages-list=Teuchos,Thyra,Phalanx,Stokhos,ALL_PACKAGES --keep-test-test-categories=PT",
        True),
      "Teuchos,Thyra,ALL_PACKAGES"
      )


  def test_get_PT_ALL_PACKAGS_middle(self):
    self.assertEqual(
      getCmndOutput(ciSupportDir+"/filter-packages-list.py" \
        " --deps-xml-file="+testingTrilinosDepsXmlInFile+"" \
        " --input-packages-list=Teuchos,ALL_PACKAGES,Thyra,Phalanx,Stokhos --keep-test-test-categories=PT",
        True),
      "Teuchos,ALL_PACKAGES,Thyra"
      )


  def test_get_PT_ST(self):
    self.assertEqual(
      getCmndOutput(ciSupportDir+"/filter-packages-list.py" \
        " --deps-xml-file="+testingTrilinosDepsXmlInFile+"" \
        " --input-packages-list=Teuchos,Thyra,Phalanx,Stokhos --keep-test-test-categories=PT,ST",
        True),
      "Teuchos,Thyra,Phalanx"
      )


  def test_get_PT_ST_EX(self):
    self.assertEqual(
      getCmndOutput(ciSupportDir+"/filter-packages-list.py" \
        " --deps-xml-file="+testingTrilinosDepsXmlInFile+"" \
        " --input-packages-list=Teuchos,Thyra,Phalanx,Stokhos --keep-test-test-categories=PT,ST,EX",
        True),
      "Teuchos,Thyra,Phalanx,Stokhos"
      )


  def test_get_ST(self):
    self.assertEqual(
      getCmndOutput(ciSupportDir+"/filter-packages-list.py" \
        " --deps-xml-file="+testingTrilinosDepsXmlInFile+"" \
        " --input-packages-list=Teuchos,Thyra,Phalanx,Stokhos --keep-test-test-categories=ST",
        True),
      "Phalanx"
      )


  def test_get_PT_EX(self):
    self.assertEqual(
      getCmndOutput(ciSupportDir+"/filter-packages-list.py" \
        " --deps-xml-file="+testingTrilinosDepsXmlInFile+"" \
        " --input-packages-list=Teuchos,Thyra,Phalanx,Stokhos --keep-test-test-categories=PT,EX",
        True),
      "Teuchos,Thyra,Stokhos"
      )


def suite():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(testTrilinosPackageFilePathUtils))
    return suite


if __name__ == '__main__':
  unittest.main()
