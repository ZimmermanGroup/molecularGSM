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

tribitsDirPath = os.path.abspath(
  os.path.join(
    os.path.dirname(os.path.abspath(__file__)),
    "../..", "tribits"
    )
  )
sys.path = [tribitsDirPath+"/examples/TribitsExampleProject/cmake"] + sys.path

from ProjectCiFileChangeLogic import *
import unittest


#
# Test out the TribitsExampleProject logic for changes requiring a global
# rebuild
#

class test_TribitsExampleProject_ProjectCiFileChangeLogic(unittest.TestCase):


  def test_CMakeLists_txt(self):
    dpcl = ProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'CMakeLists.txt' ), True )


  def test_PackagesList_cmake(self):
    dpcl = ProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'PackagesList.cmake' ), True )


  def test_TPLsList_cmake(self):
    dpcl = ProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'TPLsList.cmake' ), True )


  def test_Version_cmake(self):
    dpcl = ProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'Version.cmake' ), True )


  def test_Anything_cmake(self):
    dpcl = ProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'Anything.cmake' ), True )


  def test_TrilinosCMakeQuickstart_txt(self):
    dpcl = ProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'cmake/TrilinosCMakeQuickstart.txt' ),
      False )


  def test_TPLsList_cmake(self):
    dpcl = ProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'cmake/ExtraRepositoriesList.cmake' ),
      False )


  def test_cmake_ctest(self):
    dpcl = ProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'cmake/ctest/CTestCustom.cmake.in' ),
      True )


  def test_something_cmake_ctest_general_gcc_ctest_serial_debug(self):
    dpcl = ProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'cmake/ctest/general_gcc/ctest_serial_debug.cmake' ),
      False )


  def test_cmake_anything_UnitTests(self):
    dpcl = ProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'cmake/anything/UnitTests/CMakeLists.txt' ),
      False )


  def test_cmake_tpls(self):
    dpcl = ProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'cmake/tpls/FindTPLSimpleTpl.cmake' ),
      True )


  def test_SetNotFound_cmake(self):
    dpcl = ProjectCiFileChangeLogic()
    self.assertEqual( dpcl.isGlobalBuildFileRequiringGlobalRebuild( 'cmake/utils/SetNotFound.cmake' ),
      True )


if __name__ == '__main__':
  unittest.main()
