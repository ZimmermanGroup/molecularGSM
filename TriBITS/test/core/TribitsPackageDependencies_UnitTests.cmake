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

# Echo input arguments
message("PROJECT_NAME = '${PROJECT_NAME}'")
message("${PROJECT_NAME}_TRIBITS_DIR = '${${PROJECT_NAME}_TRIBITS_DIR}'")
message("CURRENT_TEST_DIRECTORY = '${CURRENT_TEST_DIRECTORY}'")

set( CMAKE_MODULE_PATH
  "${${PROJECT_NAME}_TRIBITS_DIR}/core/utils"
  "${${PROJECT_NAME}_TRIBITS_DIR}/core/package_arch"
  )

include(TribitsPackageDependencies)

include(UnitTestHelpers)
include(GlobalNullSet)


#####################################################################
#
# Unit tests for code in TribitsPackageDependencies.cmake
#
#####################################################################


#
# Tests for tribits_extpkg_get_dep_name_and_vis()
#


function(unittest_tribits_extpkg_get_dep_name_and_vis)

  message("\n***")
  message("*** Testing tribits_extpkg_get_dep_name_and_vis()")
  message("***\n")

  set(MESSAGE_WRAPPER_UNIT_TEST_MODE  TRUE)

  message("\nTesting default visibility\n")
  global_set(MESSAGE_WRAPPER_INPUT "")
  tribits_extpkg_get_dep_name_and_vis(
    SomePackage  upstreamTplDepNameOut  upstreamTplDepVisOut)
  unittest_compare_const( upstreamTplDepNameOut
    "SomePackage" )
  unittest_compare_const( upstreamTplDepVisOut
    "PRIVATE" )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT "")

  message("\nTesting PUBLIC\n")
  global_set(MESSAGE_WRAPPER_INPUT "")
  tribits_extpkg_get_dep_name_and_vis(
    SomePackage:PUBLIC  upstreamTplDepNameOut  upstreamTplDepVisOut)
  unittest_compare_const( upstreamTplDepNameOut
    SomePackage )
  unittest_compare_const( upstreamTplDepVisOut
    "PUBLIC" )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT "")

  message("\nTesting PRIVATE\n")
  global_set(MESSAGE_WRAPPER_INPUT "")
  tribits_extpkg_get_dep_name_and_vis(
    SomePackage:PRIVATE  upstreamTplDepNameOut  upstreamTplDepVisOut)
  unittest_compare_const( upstreamTplDepNameOut
    SomePackage )
  unittest_compare_const( upstreamTplDepVisOut
    "PRIVATE" )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT "")

  message("\nTesting invalid visibility\n")
  global_set(MESSAGE_WRAPPER_INPUT "")
  tribits_extpkg_get_dep_name_and_vis(
    SomePackage:SPELLEDWRONG  upstreamTplDepNameOut  upstreamTplDepVisOut)
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;ERROR: 'SomePackage:SPELLEDWRONG' has invalid visibility 'SPELLEDWRONG'.;  Only 'PUBLIC' or 'PRIVATE' allowed!")

  message("\nTesting more than two elements splitting on ':'\n")
  global_set(MESSAGE_WRAPPER_INPUT "")
  tribits_extpkg_get_dep_name_and_vis(
    SomePackage:BadExtraEntry:PRIVATE  upstreamTplDepNameOut  upstreamTplDepVisOut)
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;ERROR: 'SomePackage:BadExtraEntry:PRIVATE' has 2 ':' but only 1 is allowed!")

  # Reset global state
  global_set(MESSAGE_WRAPPER_INPUT "")

endfunction()


#
# Tests for tribits_extpkg_define_dependencies()
#


function(unittest_tribits_extpkg_define_dependencies_basic)

  message("\n***")
  message("*** ${CMAKE_CURRENT_FUNCTION}()")
  message("***\n")

  tribits_extpkg_define_dependencies(someExtPkg
    DEPENDENCIES  upPkg1  upPkg2:PUBLIC  upPkg3:PRIVATE  upPkg4)
  unittest_compare_const( someExtPkg_LIB_ALL_DEPENDENCIES
    "upPkg1;upPkg2:PUBLIC;upPkg3:PRIVATE;upPkg4" )

endfunction()


#
# Tests for tribits_extpkg_setup_enabled_dependencies()
#


function(unittest_tribits_extpkg_setup_enabled_dependencies)

  message("\n***")
  message("*** ${CMAKE_CURRENT_FUNCTION}()")
  message("***\n")

  tribits_extpkg_define_dependencies(someExtPkg
    DEPENDENCIES  upPkg1  upPkg2:PUBLIC  upPkg3:PRIVATE  upPkg4)
  set(TPL_ENABLE_someExtPkg  ON)
  set(TPL_ENABLE_upPkg1 ON)
  set(TPL_ENABLE_upPkg3 ON)
  tribits_extpkg_setup_enabled_dependencies(someExtPkg)
  unittest_compare_const( someExtPkg_LIB_ENABLED_DEPENDENCIES
    "upPkg1;upPkg3:PRIVATE" )

endfunction()


#####################################################################
#
# Execute the unit tests
#
#####################################################################

unittest_initialize_vars()

#
# Run the unit tests
#

unittest_tribits_extpkg_get_dep_name_and_vis()

unittest_tribits_extpkg_define_dependencies_basic()

unittest_tribits_extpkg_setup_enabled_dependencies()

# Pass in the number of expected tests that must pass!
unittest_final_result(13)
