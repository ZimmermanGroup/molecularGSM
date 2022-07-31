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

import pprint
g_pp = pprint.PrettyPrinter(indent=4)

testingTrilinosDepsXmlInFile = getScriptBaseDir()+"/TrilinosPackageDependencies.gold.xml"


#
# Test getProjectDependenciesFromXmlFile
#

class test_getProjectDependenciesFromXmlFile(unittest.TestCase):


  def test_read_1(self):
    td = getProjectDependenciesFromXmlFile(testingTrilinosDepsXmlInFile)
    #print(str(td))
    self.assertEqual(td.getProjectName(), "Trilinos")
    self.assertEqual(td.getProjectBaseDirName(), "MockTrilinos")
    self.assertEqual(td.numPackages(), 30)
    self.assertEqual(td.packageNameToID("Teuchos"), 1)
    self.assertEqual(td.packageNameToID("ThyraCoreLibs"), 11)
    self.assertEqual(td.packageNameToID("Thyra"), 17)
    self.assertEqual(td.packageNameToID("Phalanx"), 28)
    self.assertEqual(td.packageNameToID("Panzer"), 29)
    self.assertEqual(td.getPackageByID(1).packageName, "Teuchos")
    self.assertEqual(td.getPackageByID(11).packageName, "ThyraCoreLibs")
    tlPackagesNamesList = td.getPackagesNamesList()
    #print(str(tlPackagesNamesList))
    self.assertEqual(len(tlPackagesNamesList), 24)
    self.assertEqual(tlPackagesNamesList[0], "TrilinosFramework")
    self.assertEqual(tlPackagesNamesList[1], "Teuchos")
    self.assertEqual(tlPackagesNamesList[11], "Thyra")
    self.assertEqual(tlPackagesNamesList[22], "Phalanx")
    self.assertEqual(tlPackagesNamesList[23], "Panzer")
    sePackagesNamesList = td.getPackagesNamesList(False)
    self.assertEqual(len(sePackagesNamesList), 30)
    #print(str(sePackagesNamesList))
    self.assertEqual(sePackagesNamesList[0], "TrilinosFramework")
    self.assertEqual(sePackagesNamesList[1], "Teuchos")
    self.assertEqual(sePackagesNamesList[11], "ThyraCoreLibs")
    self.assertEqual(sePackagesNamesList[17], "Thyra")
    self.assertEqual(sePackagesNamesList[28], "Phalanx")
    self.assertEqual(sePackagesNamesList[29], "Panzer")
    self.assertEqual(td.getPackageByName("Teuchos").packageName, "Teuchos")
    self.assertEqual(td.getPackageByName("Thyra").packageName, "Thyra")
    self.assertEqual(td.getPackageByDir("packages/zoltan").packageName, "Zoltan")
    self.assertEqual(td.getPackageNameFromPath("packages/zoltan/"), "Zoltan")
    self.assertEqual(td.getPackageNameFromTestName("Zoltan_great_test"), "Zoltan")
    self.assertEqual(td.getPackageNameFromTestName("ThyraCoreLibs_atest"), "Thyra")
    self.assertEqual(td.getPackageNameFromTestName("Thyra_atest"), "Thyra")


def suite():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(testTrilinosPackageFilePathUtils))
    return suite


if __name__ == '__main__':
  unittest.main()
