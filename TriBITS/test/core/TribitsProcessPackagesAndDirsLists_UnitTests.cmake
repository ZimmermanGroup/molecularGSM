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

cmake_minimum_required(VERSION 3.17.0 FATAL_ERROR)

message("PROJECT_NAME = ${PROJECT_NAME}")
message("${PROJECT_NAME}_TRIBITS_DIR = ${${PROJECT_NAME}_TRIBITS_DIR}")

set( CMAKE_MODULE_PATH
  "${${PROJECT_NAME}_TRIBITS_DIR}/core/utils"
  "${${PROJECT_NAME}_TRIBITS_DIR}/core/package_arch"
  )

include(TribitsProcessPackagesAndDirsLists)
include(UnitTestHelpers)
include(GlobalSet)


#####################################################################
#
# Unit tests for code in TribitsProcessPackagesAndDirsLists.cmake
#
#####################################################################


function(unittest_basic_package_list_read)

  message("\n***")
  message("*** Testing the basic reading of packages list")
  message("***\n")

  set( ${PROJECT_NAME}_PACKAGES_AND_DIRS_AND_CLASSIFICATIONS
    Package0     packages/package0  PT
    Package1     packages/package1  ST
    Package2     packages/package2  EX
    )

  set(PACKAGE_ABS_DIR "DummyBase")
  set(${PROJECT_NAME}_IGNORE_PACKAGE_EXISTS_CHECK TRUE)

  tribits_process_packages_and_dirs_lists(${PROJECT_NAME} ".")

  unittest_compare_const( ${PROJECT_NAME}_PACKAGES
    "Package0;Package1;Package2")
  unittest_compare_const( ${PROJECT_NAME}_NUM_PACKAGES 3 )
  unittest_compare_const( ${PROJECT_NAME}_LAST_PACKAGE_IDX 2 )
  unittest_compare_const( ${PROJECT_NAME}_REVERSE_PACKAGES
    "Package2;Package1;Package0")
  unittest_compare_const( Package0_SOURCE_DIR ${PROJECT_SOURCE_DIR}/packages/package0 )
  unittest_compare_const( Package1_SOURCE_DIR ${PROJECT_SOURCE_DIR}/packages/package1 )
  unittest_compare_const( Package2_SOURCE_DIR ${PROJECT_SOURCE_DIR}/packages/package2 )
  unittest_compare_const( Package0_REL_SOURCE_DIR packages/package0 )
  unittest_compare_const( Package1_REL_SOURCE_DIR packages/package1 )
  unittest_compare_const( Package2_REL_SOURCE_DIR packages/package2 )
  unittest_compare_const( Package0_TESTGROUP PT )
  unittest_compare_const( Package1_TESTGROUP ST )
  unittest_compare_const( Package2_TESTGROUP EX )

endfunction()


function(unittest_basic_package_list_read_abs_pacakge_dir)

  message("\n***")
  message("*** Testing the basic reading of packages list with abs package dir")
  message("***\n")

  set( ${PROJECT_NAME}_PACKAGES_AND_DIRS_AND_CLASSIFICATIONS
    Package0     packages/package0  PT
    Package1     ${PROJECT_SOURCE_DIR}/Package1  ST
    Package2     /home/me/Package2  EX
    )

  #set(TRIBITS_PROCESS_PACKAGES_AND_DIRS_LISTS_VERBOSE ON)

  set(PACKAGE_ABS_DIR "DummyBase")
  set(${PROJECT_NAME}_IGNORE_PACKAGE_EXISTS_CHECK TRUE)

  set(MESSAGE_WRAPPER_UNIT_TEST_MODE ON)

  tribits_process_packages_and_dirs_lists(${PROJECT_NAME} ".")

  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- ;PROJECT_SOURCE_DIR_BASE_MATCH='/home/me/DummyProject';FATAL_ERROR;Error: The package 'Package2' was given an absolute directory '/home/me/Package2' which is *not* under the project's source directory '/home/me/DummyProject/'!;-- ;DummyProject_NUM_PACKAGES='3'"
    )
  unittest_compare_const( ${PROJECT_NAME}_PACKAGES
    "Package0;Package1;Package2")
  unittest_compare_const( ${PROJECT_NAME}_NUM_PACKAGES 3 )
  unittest_compare_const( ${PROJECT_NAME}_LAST_PACKAGE_IDX 2 )
  unittest_compare_const( ${PROJECT_NAME}_REVERSE_PACKAGES
    "Package2;Package1;Package0")
  unittest_compare_const( Package0_TESTGROUP PT)
  unittest_compare_const( Package1_TESTGROUP ST)
  unittest_compare_const( Package2_TESTGROUP EX)

endfunction()


function(unittest_basic_package_list_read_ps_ss_backward_compatible)

  message("\n***")
  message("*** Testing the basic reading of packages list (backward compatible)")
  message("***\n")

  set( ${PROJECT_NAME}_PACKAGES_AND_DIRS_AND_CLASSIFICATIONS
    Package0     packages/package0  PS
    Package1     packages/package1  SS
    Package2     packages/package2  EX
    )

  set(PACKAGE_ABS_DIR "DummyBase")
  set(${PROJECT_NAME}_IGNORE_PACKAGE_EXISTS_CHECK TRUE)

  tribits_process_packages_and_dirs_lists(${PROJECT_NAME} ".")

  unittest_compare_const( ${PROJECT_NAME}_PACKAGES
    "Package0;Package1;Package2")
  unittest_compare_const( ${PROJECT_NAME}_NUM_PACKAGES 3 )
  unittest_compare_const( ${PROJECT_NAME}_LAST_PACKAGE_IDX 2 )
  unittest_compare_const( ${PROJECT_NAME}_REVERSE_PACKAGES
    "Package2;Package1;Package0")
  unittest_compare_const( Package0_TESTGROUP PT)
  unittest_compare_const( Package1_TESTGROUP ST)
  unittest_compare_const( Package2_TESTGROUP EX)

endfunction()


function(unittest_elevate_st_to_pt)

  message("\n***")
  message("*** Testing elevating ST packages to PT packages")
  message("***\n")

  set( ${PROJECT_NAME}_PACKAGES_AND_DIRS_AND_CLASSIFICATIONS
    Package0     packages/package0  PS
    Package1     packages/package1  SS
    Package2     packages/package2  EX
    )

  set(PACKAGE_ABS_DIR "DummyBase")
  set(${PROJECT_NAME}_IGNORE_PACKAGE_EXISTS_CHECK TRUE)

  # Make all ST packages PT packages!
  set(${PROJECT_NAME}_ELEVATE_ST_TO_PT TRUE)

  tribits_process_packages_and_dirs_lists(${PROJECT_NAME} ".")

  unittest_compare_const( Package0_TESTGROUP PT)
  unittest_compare_const( Package1_TESTGROUP PT)
  unittest_compare_const( Package2_TESTGROUP EX)

endfunction()


function(unittest_elevate_ss_to_ps_backward_compatible)

  message("\n***")
  message("*** Testing elevating SS packages to PS packages (backward compatible)")
  message("***\n")

  set( ${PROJECT_NAME}_PACKAGES_AND_DIRS_AND_CLASSIFICATIONS
    Package0     packages/package0  PS
    Package1     packages/package1  SS
    Package2     packages/package2  EX
    )

  set(PACKAGE_ABS_DIR "DummyBase")
  set(${PROJECT_NAME}_IGNORE_PACKAGE_EXISTS_CHECK TRUE)

  # Make all ST packages PT packages!
  set(${PROJECT_NAME}_ELEVATE_SS_TO_PS TRUE)

  tribits_process_packages_and_dirs_lists(${PROJECT_NAME} ".")

  unittest_compare_const( Package0_TESTGROUP PT)
  unittest_compare_const( Package1_TESTGROUP PT)
  unittest_compare_const( Package2_TESTGROUP EX)

endfunction()



#####################################################################
#
# Execute the unit tests
#
#####################################################################

unittest_initialize_vars()

# Set common/base options
set(PROJECT_NAME "DummyProject")
set(PROJECT_SOURCE_DIR "/home/me/DummyProject")

unittest_basic_package_list_read()
unittest_basic_package_list_read_abs_pacakge_dir()
unittest_basic_package_list_read_ps_ss_backward_compatible()
unittest_elevate_st_to_pt()
unittest_elevate_ss_to_ps_backward_compatible()

# Pass in the number of expected tests that must pass!
unittest_final_result(34)
