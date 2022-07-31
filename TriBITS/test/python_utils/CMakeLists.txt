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

tribits_add_advanced_test( GeneralScriptSupport_UnitTests
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE} 
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/GeneralScriptSupport_UnitTests.py -v
    PASS_REGULAR_EXPRESSION "OK"
  )

tribits_add_advanced_test( mockprogram_UnitTests
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE} 
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/mockprogram_UnitTests.py -v
    PASS_REGULAR_EXPRESSION "OK"
  XHOST travis-ci-server-linux
  )

tribits_add_advanced_test( lower_case_cmake_UnitTests
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE} 
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/lower_case_cmake_UnitTests.py -v
    PASS_REGULAR_EXPRESSION "OK"
  XHOST travis-ci-server-linux
  )

tribits_add_advanced_test( gitdist_UnitTests
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE} 
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/gitdist_UnitTests.py -v
    PASS_REGULAR_EXPRESSION "OK"
  XHOST travis-ci-server-linux
  )

tribits_add_advanced_test( SnapshotDir_UnitTests
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE} 
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/SnapshotDir_UnitTests.py -v
    PASS_REGULAR_EXPRESSION "OK"
  )

tribits_add_advanced_test( extract_rst_cmake_doc_UnitTests
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE} 
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/extract_rst_cmake_doc_UnitTests.py -v
    PASS_REGULAR_EXPRESSION "OK"
  )

set(EXTRACT_RST_CMAKE_DOC
  ${${PROJECT_NAME}_TRIBITS_DIR}/python_utils/extract_rst_cmake_doc.py)

set(EXTRACT_RST_CMAKE_DOC_DATA_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/extract_rst_cmake_doc)

tribits_add_advanced_test( extract_rst_cmake_doc_help
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE} 
    ARGS ${EXTRACT_RST_CMAKE_DOC}
      --help
    PASS_REGULAR_EXPRESSION_ALL
      "extract_rst_cmake_doc.py .OPTIONS."
      "This script implements a system to extract RST-formatted"
  )

tribits_add_advanced_test( extract_rst_cmake_doc_extract_1_block_1_file
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE} 
    ARGS ${EXTRACT_RST_CMAKE_DOC}
      --extract-from=${EXTRACT_RST_CMAKE_DOC_DATA_DIR}/simpleDocText1.cmake
      --do-trace
    PASS_REGULAR_EXPRESSION_ALL
      "Script: extract_rst_cmake_doc.py"
      "  --extract-from='${EXTRACT_RST_CMAKE_DOC_DATA_DIR}/simpleDocText1.cmake'"
      "  --file-extensions='.cmake'"
      "  --rst-file-pairs=''"
      "  --no-dump-rst-blocks"
      "  --do-trace"
      "Warning: --rst-file-pairs is empty and no RST comment blocks will be set!"
      "Extracting RST documentation blocks in --extract-from:"
      "Extracting 'MACRO' block 'some_macro_name1..' from .*/simpleDocText1.cmake:7:"
  )

tribits_add_advanced_test( extract_rst_cmake_doc_extract_2_blocks_2_files
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE} 
    ARGS ${EXTRACT_RST_CMAKE_DOC}
      --extract-from=${EXTRACT_RST_CMAKE_DOC_DATA_DIR}/simpleDocText1.cmake,${EXTRACT_RST_CMAKE_DOC_DATA_DIR}/simpleDocText2.cmake
      --do-trace
    PASS_REGULAR_EXPRESSION_ALL
      "Extracting 'MACRO' block 'some_macro_name1..' from .*/simpleDocText1.cmake:7:"
      "Extracting 'FUNCTION' block 'SOME_FUNC_NAME2..' from .*/simpleDocText2.cmake:7:"
  )

tribits_add_advanced_test( extract_rst_cmake_doc_extract_2_blocks_glob_dir
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE} 
    ARGS ${EXTRACT_RST_CMAKE_DOC}
      --extract-from=${EXTRACT_RST_CMAKE_DOC_DATA_DIR}/
      --do-trace
    PASS_REGULAR_EXPRESSION_ALL
      "Extracting 'MACRO' block 'some_macro_name1..' from .*/simpleDocText1.cmake:7:"
      "Extracting 'FUNCTION' block 'SOME_FUNC_NAME2..' from .*/simpleDocText2.cmake:7:"
  )

tribits_add_advanced_test( extract_rst_cmake_doc_extract_and_replace_2_blocks
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE} 
    ARGS ${EXTRACT_RST_CMAKE_DOC}
      --extract-from=${EXTRACT_RST_CMAKE_DOC_DATA_DIR}/
      --rst-file-pairs=${EXTRACT_RST_CMAKE_DOC_DATA_DIR}/simpleTemplate1.rst:simple.rst
      --do-trace
    PASS_REGULAR_EXPRESSION_ALL
      "Extracting 'MACRO' block 'some_macro_name1..' from .*/simpleDocText1.cmake:7"
      "Extracting 'FUNCTION' block 'SOME_FUNC_NAME2..' from .*/simpleDocText2.cmake:7"
      "Replacing 'FUNCTION' block 'SOME_FUNC_NAME2..' in .*/simpleTemplate1.rst:7"
      "Replacing 'MACRO' block 'some_macro_name1..' in .*/simpleTemplate1.rst:11"
  TEST_1 CMND ${CMAKE_COMMAND}
    ARGS -E compare_files simple.rst
      ${EXTRACT_RST_CMAKE_DOC_DATA_DIR}/test_replace_1_block_1_file.rst.gold
  )

tribits_add_advanced_test( extract_rst_cmake_doc_extract_and_replace_2_blocks_notrace
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE} 
    ARGS ${EXTRACT_RST_CMAKE_DOC}
      --extract-from=${EXTRACT_RST_CMAKE_DOC_DATA_DIR}/
      --rst-file-pairs=${EXTRACT_RST_CMAKE_DOC_DATA_DIR}/simpleTemplate1.rst:simple.rst
  TEST_1 CMND ${CMAKE_COMMAND}
    ARGS -E compare_files simple.rst
      ${EXTRACT_RST_CMAKE_DOC_DATA_DIR}/test_replace_1_block_1_file.rst.gold
  )

# ToDo: Put in test extracts a single RST block and writes a single RST
# template file.

# ToDo: Put in test that extracts multiple RST blocks from multiple *.cmake
# files and writes to multiple RST template files.

# ToDo: Put in test that does not write all of the extracted RST blocks and
# prints out those that were not substituted.
