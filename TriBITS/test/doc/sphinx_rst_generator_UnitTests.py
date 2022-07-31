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

#################################################
# Unit testing code for sphinx_rst_generator.py #
#################################################

import unittest

from FindTestDocDir import *
import sphinx_rst_generator as SRG


#
# Unit tests for sphinx_rst_generator.change_paths_and_get_includes()
#


tribitsGuideBodyCopied_expected = \
r"""This is a stand-in for the TribitsGuidesBody.rst file with indented includes.

Something something:

.. include:: ../copied_files/ProjectName.cmake
   :literal:

4) Add custom CTest -S driver scripts.

  For driving different builds and tests, one needs to set up one or more
  CTest -S driver scripts.  There are various ways to do this but a simple
  approach that avoids duplication is to first create a file like
  ``TribitsExampleProject/cmake/ctest/TribitsExProjCTestDriver.cmake``:

  .. include:: ../copied_files/TribitsExProjCTestDriver.cmake
     :literal:

  and then create a set of CTest -S driver scripts that uses that file.  One
  example is the file
  ``TribitsExampleProject/cmake/ctest/general_gcc/ctest_serial_debug.cmake``:

  .. include:: ../copied_files/ctest_serial_debug.cmake
     :literal:
"""


class test_change_paths_and_get_includes(unittest.TestCase):


  def test_1(self):
    src_file_path = os.path.join(testDocDir,'data')
    source_file = os.path.join(src_file_path,'TribitsGuidesBody.rst')
    start_path = os.path.join(tribitsDir,'doc','sphinx','users_guide')
    rst_dir = os.path.join(tribitsDir,'doc','sphinx','copied_files')
    (abs_path_str, include_file_list) = SRG.change_paths_and_get_includes(
      source_file=source_file, src_file_path=src_file_path,
      start_path=start_path, rst_dir=rst_dir, copy_file=True)
    #print("abs_path_str = "+str(abs_path_str))
    #print("include_file_list = "+str(include_file_list))
    self.maxDiff = None
    self.assertEqual(abs_path_str, tribitsGuideBodyCopied_expected)
    include_file_list_expected = set()
    self.assertEqual(include_file_list, include_file_list_expected)


if __name__ == '__main__':
  unittest.main()
