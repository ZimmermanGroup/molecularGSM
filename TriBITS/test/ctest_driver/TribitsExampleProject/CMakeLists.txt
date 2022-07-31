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


###############################################################################
#
# Strong automated testing of TribitsCTestDriverCore.cmake and
# 'dashboard' target using real TribitsExampleProject.
#
# The tests in this CMakeLists.txt file use the TribitsExampleProject to do
# real testing of TribitsCTestDriverCore.cmake using real CTest -S script
# functions and can submit to a CDash site.  By default, these tests don't
# submit to CDash.  To get them to submit to CDash, you must configure this
# TriBITS project with::
#
#   -D${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_TO=<site-name>
#
# where ``<cite-name>`` is one of the following values:
#
# * Empty "" (the default)
#
# * "TESTING_SANDIA_CDASH": Submits to TribitsExProj on testing.sandia.gov/cdash
#
# * "TESTING_DEV_SANDIA_CDASH": Submits to TribitsExProj on testing-dev.sandia.gov/cdash
#
# * "EXP_CDASH": Submits to TribitsExProj on exp.cdash.org
#
# * "CUSTOM": Submits to arbitrary CDash site prvoided by the vars.
#
#   * -D${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_DROP_SITE=someurl.com
#
#   * -D${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_PROJECT_NAME=TribitsExProj (the default)
#
#   * -D${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_DROP_LOCATION="cdash/submit.php?project=TribitsExProj"
#
# In addition, tests can also be added to test coverage testing and memory
# testing.
#
# To enable the coverage testing tests (which assumes gcov and GCC), use:
#
#   -D${PACKAGE_NAME}_CTEST_DRIVER_COVERAGE_TESTS=ON
#
# To enable the memory testing tests (which assumes valgrind is set up to be
# used), use:
#
#   -D${PACKAGE_NAME}_ENABLE_CTEST_DRIVER_MEMORY_TESTS=ON
#
###############################################################################

set(${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_TO ""
  CACHE STRING
  "Determines where CTestDriver test results are submitted to CDash (chose '' (nowhere), 'MY_CDASH', 'TRILINOS_CDASH', or 'EXP_CDASH'"
  )

set(${PACKAGE_NAME}_CTEST_DRIVER_COVERAGE_TESTS OFF
  CACHE BOOL
  "Determine if coverage CTestDriver tests are run (requires gcov on local system)"
  )

set(${PACKAGE_NAME}_CTEST_DRIVER_MEMORY_TESTS OFF
  CACHE BOOL
  "Determine if memory CTestDriver tests are run (requires valgrind on local system)"
  )

#
# Set up common vars/args for all direct CTest Driver tests
#

set(TribitsExProj_DIR "${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject")

set(PBP_COMMON_ENV_ARGS
  # Set back to default in case set in env
  CTEST_TEST_TYPE=
  TribitsExProj_PACKAGES=
  CTEST_EXPLICITLY_ENABLE_IMPLICITLY_ENABLED_PACKAGES=
  CTEST_ENABLE_MODIFIED_PACKAGES_ONLY=OFF
  TribitsExProj_PRE_REPOSITORIES=
  TribitsExProj_EXTRA_REPOSITORIES=
  TribitsExProj_EXTRA_CONFIGURE_OPTIONS=-DTribitsExProj_ENABLE_INSTALL_CMAKE_CONFIG_FILES=OFF 
  CTEST_BUILD_FLAGS=-j2
  CTEST_PARALLEL_LEVEL=1
  # Other args
  TribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
  TribitsExProj_EXTRAREPOS_FILE=NONE
  CTEST_DO_UPDATES=OFF
  CTEST_DO_SUBMIT=OFF
  )

set(CTEST_S_SCRIPT_ARGS
  ${CMAKE_CTEST_COMMAND} -V -S
    ${TribitsExProj_DIR}/cmake/ctest/general_gcc/ctest_serial_debug.cmake
  )

#
# Set up to submit to CDash if requested
#

if ("${${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_TO}" STREQUAL "") 
  set(${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_TO
    $ENV{${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_TO} )
endif()

if ("${${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_TO}" STREQUAL "")
  set(CTEST_DROP_SITE_ENV_ARGS
    CTEST_DO_SUBMIT=OFF
    )
elseif (${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_TO STREQUAL "TESTING_SANDIA_CDASH")
  set(${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_DROP_SITE "testing.sandia.gov")
  set(${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_PROJECT_NAME "TribitsExProj")
  set(${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_DROP_LOCATION "/cdash/submit.php?project=TribitsExProj" )
elseif (${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_TO STREQUAL "TESTING_DEV_SANDIA_CDASH")
  set(${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_DROP_SITE "testing-dev.sandia.gov")
  set(${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_PROJECT_NAME "TribitsExProj")
  set(${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_DROP_LOCATION "/cdash/submit.php?project=TribitsExProj" )
elseif (${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_TO STREQUAL "EXP_CDASH")
  set(${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_DROP_SITE "exp.cdash.org")
  set(${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_PROJECT_NAME "TribitsExProj")
  set(${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_DROP_LOCATION "/submit.php?project=TribitsExProj")
elseif (${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_TO STREQUAL "CUSTOM")
  if ("${${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_DROP_SITE}" STREQUAL "")
    set(${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_DROP_SITE
      $ENV{${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_DROP_SITE} )
  endif()
  if ("${${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_PROJECT_NAME}" STREQUAL "")
    set(${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_PROJECT_NAME
      $ENV{${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_PROJECT_NAME} )
  endif()
  if ("${${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_PROJECT_NAME}" STREQUAL "")
    set(${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_PROJECT_NAME TribitsExProj)
  endif()
  if ("${${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_DROP_LOCATION}" STREQUAL "")
    set(${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_DROP_LOCATION
      $ENV{${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_DROP_LOCATION} )
  endif()
else()
  message(FATAL_ERROR "Error, the value"
    " ${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_TO='${${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_TO}'"
    " is invalid!  Valid choices are '', 'MY_CDASH', 'TRILINOS_CDASH', 'EXP_CDASH',"
    " and 'TESTING_VM_SANDIA_CDASH'!"
   ) 
endif()

if (NOT "${${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_TO}" STREQUAL "")

  message(
    "\nNOTE: The tests ${PACKAGE_NAME}_CTestDriver_XXX are set up to submit CDash at:\n"
    "    CTEST_DROP_SITE='${${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_DROP_SITE}'\n"
    "    CTEST_PROJECT_NAME='${${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_PROJECT_NAME}'\n"
    "    CTEST_DROP_LOCATION='${${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_DROP_LOCATION}'\n"
    )

  set(CTEST_DROP_SITE_ENV_ARGS
    CTEST_DO_SUBMIT=TRUE
    CTEST_DROP_SITE="${${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_DROP_SITE}"
    CTEST_PROJECT_NAME="${${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_PROJECT_NAME}"
    CTEST_DROP_LOCATION="${${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_DROP_LOCATION}"
    )

endif()


#
# Set up arguments for direct configures of TribitsExamleProject to test the
# 'dashboard' target.
#

include("${CMAKE_CURRENT_LIST_DIR}/../../core/ExamplesUnitTests/GetCompilerPassthroughArgs.cmake")


set(CTEST_DROP_SITE_CONFIG_ARGS)
foreach (CTEST_DROP_SITE_ENV_ARG ${CTEST_DROP_SITE_ENV_ARGS})
  list(APPEND CTEST_DROP_SITE_CONFIG_ARGS "-D${CTEST_DROP_SITE_ENV_ARG}")
endforeach()

set(TribitsExampleProject_COMMON_CONFIG_ARGS
  ${COMMON_ENV_ARGS_PASSTHROUGH}
  -DTribitsExProj_ENABLE_Fortran=${${PROJECT_NAME}_ENABLE_Fortran}
  -DTribitsExProj_ENABLE_INSTALL_CMAKE_CONFIG_FILES=OFF
  ${CTEST_DROP_SITE_CONFIG_ARGS}
  )


######################################################################
#
# Test package-by-package mode for TribitsCTestDriverCore.cmake
#
######################################################################


tribits_add_advanced_test( CTestDriver_PBP_ST_ALL_PASS
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
  TEST_0
    MESSAGE "Symlink TribitsExampleProject so no need to clone (which would not work)."
    CMND ln
    ARGS -s ${TribitsExProj_DIR} .
  TEST_1
    MESSAGE "Run ctest driver testing everything but no push."
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${PBP_COMMON_ENV_ARGS}
      TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
      ${CTEST_DROP_SITE_ENV_ARGS}
      CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_PBP_ST_ALL_PASS
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "NONE does not exist, skipping extra repositories"
      "Setting TribitsExProj_ENABLE_ALL_PACKAGES=ON since TribitsExProj_PACKAGES_USER_SELECTED=''"
      "Final set of enabled packages:  SimpleCxx MixedLang WithSubpackages WrapExternal 4"
      "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackagesB WithSubpackagesC WithSubpackages WrapExternal"
      "SimpleCxx: Configure passed"
      "SimpleCxx: Libs build passed"
      "SimpleCxx: All build passed"
      "-- CTEST_CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE='54321'"
      "-- CTEST_CUSTOM_MAXIMUM_FAILED_TEST_OUTPUT_SIZE='123456'"
      "SimpleCxx_HelloWorldProg [.]+ +Passed"
      "MixedLang: Configure passed"
      "MixedLang: Libs build passed"
      "MixedLang: All build passed"
      "MixedLang_RayTracerTests [.]+ +Passed"
      "WithSubpackages: Configure passed"
      "WithSubpackages: Libs build passed"
      "WithSubpackages: All build passed"
      "WithSubpackagesA_test_of_a [.]+ +Passed"
      "WithSubpackagesB_test_of_b [.]+ +Passed"
      "WithSubpackagesC_test_of_c_util [.]+ +Passed"
      "WithSubpackagesC_test_of_c_b_mixed_lang [.]+ +Passed"
      "WrapExternal: Configure passed"
      "WrapExternal: Libs build passed"
      "WrapExternal: All build passed"
      "WrapExternal_run_external_func [.]+ +Passed"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  )
  # NOTE: The above test is the basic mode of testing everything and making
  # sure that the configure, build, and tests all run correctly.  The names of
  # passing tests are grepped for to make sure they run and pass.  And a
  # non-zero return code is mandated.


##############################################################################


tribits_add_advanced_test( CTestDriver_PBP_GlobalConfigureFail
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0
    MESSAGE "Copy the project dir so I can break it!"
    CMND cp
    ARGS -r ${TribitsExProj_DIR} .
  TEST_1
    MESSAGE "Break the outer configure"
    CMND ${CMAKE_CURRENT_SOURCE_DIR}/append_file_with_line.sh
    ARGS TribitsExampleProject/CMakeLists.txt "The outer configure is broken!"
  TEST_2
    MESSAGE "Run ctest driver which should show a failed configure for every package!"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${PBP_COMMON_ENV_ARGS}
      ${CTEST_DROP_SITE_ENV_ARGS}
      CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_PBP_GlobalConfigureFail
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "SimpleCxx FAILED to configure"
      "MixedLang FAILED to configure"
      "WithSubpackages FAILED to configure"
      "Final set packages that failed to configure or have the libraries build: 'SimpleCxx[;]MixedLang[;]WithSubpackages'"
      "Final set of packages that had any failures: 'SimpleCxx[;]MixedLang[;]WithSubpackages'"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL FAILED"
    ALWAYS_FAIL_ON_ZERO_RETURN
  )
  # NOTE: The above test ensures that a global configure failures breaks the
  # configure for all of the packages that are tried.


##############################################################################


tribits_add_advanced_test( CTestDriver_PBP_ST_BreakConfigureOptionalPkg
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
  TEST_0
    MESSAGE "Copy the project dir so I can break it!"
    CMND cp
    ARGS -r ${TribitsExProj_DIR} .
  TEST_1
    MESSAGE "Break the configure for just the optional MixLang package"
    CMND ${CMAKE_CURRENT_SOURCE_DIR}/append_file_with_line.sh
    ARGS TribitsExampleProject/packages/mixed_lang/CMakeLists.txt
      "Configure of mixed_lang is broken!"
  TEST_2
    MESSAGE "Run ctest driver which should show a failed configure for just the MixedLang package!"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${PBP_COMMON_ENV_ARGS}
      TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
      ${CTEST_DROP_SITE_ENV_ARGS}
      CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_PBP_ST_BreakConfigureOptionalPkg
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackagesB WithSubpackagesC WithSubpackages WrapExternal 7"
      "SimpleCxx: Configure passed"
      "SimpleCxx_HelloWorldProg [.]+ +Passed"
      "MixedLang FAILED to configure"
      "CONFIGURE_OPTIONS = '.*[;]-DTribitsExProj_ENABLE_MixedLang:BOOL=OFF[;].*[;]-DTribitsExProj_ENABLE_WithSubpackages:BOOL=ON'"
      "WithSubpackages: Configure passed"
      "WithSubpackagesA_test_of_a [.]+ +Passed"
      "CONFIGURE_OPTIONS = '.*[;]-DTribitsExProj_ENABLE_MixedLang:BOOL=OFF[;].*[;]-DTribitsExProj_ENABLE_WrapExternal:BOOL=ON'"
      "WrapExternal: Configure passed"
      "Final set packages that failed to configure or have the libraries build: 'MixedLang'"
      "Final set of packages that had any failures: 'MixedLang'"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL FAILED"
    ALWAYS_FAIL_ON_ZERO_RETURN
  )
  # NOTE: The above test ensures that the configure failure of an upstream
  # optional package does not break the configure, build, or tests of
  # downstream packages.  It only disables the broken upstream package in
  # downstream packages.


##############################################################################


tribits_add_advanced_test( CTestDriver_PBP_ST_BreakBuildLibOptionalPkg
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
  TEST_0
    MESSAGE "Copy the project dir so I can break it!"
    CMND cp
    ARGS -r ${TribitsExProj_DIR} .
  TEST_1
    MESSAGE "Break the library build for optional MixLang package"
    CMND ${CMAKE_CURRENT_SOURCE_DIR}/append_file_with_line.sh
    ARGS TribitsExampleProject/packages/mixed_lang/src/MixedLang.cpp
      "Build mixed_lang lib is broken!"
  TEST_2
    MESSAGE "Run ctest driver which should show a failed library build for just the MixedLang package!"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${PBP_COMMON_ENV_ARGS}
      TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
      ${CTEST_DROP_SITE_ENV_ARGS}
      CTEST_BUILD_FLAGS=-k
      CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_PBP_ST_BreakBuildLibOptionalPkg
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackagesB WithSubpackagesC WithSubpackages WrapExternal 7"
      "SimpleCxx: Configure passed"
      "SimpleCxx_HelloWorldProg [.]+ +Passed"
      "MixedLang: Configure passed"
      "FAILED library build for package 'MixedLang'"
      "CONFIGURE_OPTIONS = '.*[;]-DTribitsExProj_ENABLE_MixedLang:BOOL=OFF[;].*[;]-DTribitsExProj_ENABLE_WithSubpackages:BOOL=ON'"
      "WithSubpackages: Configure passed"
      "WithSubpackagesA_test_of_a [.]+ +Passed"
      "CONFIGURE_OPTIONS = '.*[;]-DTribitsExProj_ENABLE_MixedLang:BOOL=OFF[;].*[;]-DTribitsExProj_ENABLE_WrapExternal:BOOL=ON'"
      "WrapExternal: Configure passed"
      "Final set packages that failed to configure or have the libraries build: 'MixedLang'"
      "Final set of packages that had any failures: 'MixedLang'"
      "CMake Error at .*/TribitsCTestDriverCore.cmake"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL FAILED"
    ALWAYS_FAIL_ON_ZERO_RETURN
  )
  # NOTE: The above test ensures that the library build failure of an optional
  # upstream package does not break downstream packages.  It only disables the
  # optional upstream package in downstream packages.


##############################################################################


tribits_add_advanced_test( CTestDriver_PBP_ST_BreakBuildAllOptionalPkg
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
  TEST_0
    MESSAGE "Copy the project dir so I can break it!"
    CMND cp
    ARGS -r ${TribitsExProj_DIR} .
  TEST_1
    MESSAGE "Break a test build file only (not the library build)"
    CMND ${CMAKE_CURRENT_SOURCE_DIR}/append_file_with_line.sh
    ARGS TribitsExampleProject/packages/mixed_lang/test/tstRay_Tracer.cc
      "Build mixed_lang test is broken!"
  TEST_2
    MESSAGE "Run ctest driver which should show a failed ALL build (but not a failed lib build) for just the MixedLang package!"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${PBP_COMMON_ENV_ARGS}
      TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
      ${CTEST_DROP_SITE_ENV_ARGS}
      CTEST_BUILD_FLAGS=-k
      CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_PBP_ST_BreakBuildAllOptionalPkg
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackagesB WithSubpackagesC WithSubpackages WrapExternal 7"
      "SimpleCxx: Configure passed"
      "SimpleCxx_HelloWorldProg [.]+ +Passed"
      "MixedLang: Configure passed"
      "MixedLang: Libs build passed"
      "MixedLang: All build FAILED"
      "MixedLang_RayTracerTests .*[*][*][*]Not Run"
      "WithSubpackages: Configure passed"
      "WithSubpackagesA_test_of_a [.]+ +Passed"
      "WithSubpackagesC_test_of_c_b_mixed_lang [.]+ +Passed"
      "WrapExternal: Configure passed"
      "Final set of packages that had any failures: 'MixedLang'"
      "CMake Error at .*/TribitsCTestDriverCore.cmake"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL FAILED"
    FAIL_REGULAR_EXPRESSION
      "CONFIGURE_OPTIONS = '.*[;]-DTribitsExProj_ENABLE_MixedLang:BOOL=OFF[;]"
    ALWAYS_FAIL_ON_ZERO_RETURN
  )
  # NOTE: The above test ensures that the ALL build failure (but not the
  # library build failure) of an optional upstream package does not break
  # downstream packages in any way and does not disable the upstream package.


##############################################################################


tribits_add_advanced_test( CTestDriver_PBP_ST_BreakTestPkg
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
  TEST_0
    MESSAGE "Copy the project dir so I can break it!"
    CMND cp
    ARGS -r ${TribitsExProj_DIR} .
  TEST_1
    MESSAGE "Break a test only (no build failure)"
    CMND ${CMAKE_CURRENT_SOURCE_DIR}/replace_string_in_file.sh
    ARGS TribitsExampleProject/packages/simple_cxx/test/CMakeLists.txt
      "Hello World" "Hellos World"
  TEST_2
    MESSAGE "Run ctest driver which should show a test for the package SimpleCxx!"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${PBP_COMMON_ENV_ARGS}
      TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
      ${CTEST_DROP_SITE_ENV_ARGS}
      CTEST_BUILD_FLAGS=-k
      CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_PBP_ST_BreakTestPkg
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackagesB WithSubpackagesC WithSubpackages WrapExternal 7"
      "SimpleCxx: Configure passed"
      "SimpleCxx_HelloWorldTests [.]+ +Passed"
      "SimpleCxx_HelloWorldProg [.]+[*][*][*]Failed  Required regular expression not found.*Regex=.Hellos World"
      "SimpleCxx: File '.*/TriBITS_CTestDriver_PBP_ST_BreakTestPkg/.*/LastTestsFailed_.*.log' exists so there were failed tests"
      "MixedLang: All build passed"
      "MixedLang_RayTracerTests [.]+ +Passed"
      "WithSubpackages: All build passed"
      "WithSubpackages: Configure passed"
      "WithSubpackagesA_test_of_a [.]+ +Passed"
      "WithSubpackagesC_test_of_c_b_mixed_lang [.]+ +Passed"
      "WrapExternal: All build passed"
      "WrapExternal_run_external_func [.]+ +Passed"
      "Final set of packages that had any failures: 'SimpleCxx'"
      "CMake Error at .*/TribitsCTestDriverCore.cmake"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL FAILED"
    FAIL_REGULAR_EXPRESSION
      "CONFIGURE_OPTIONS = '.*[;]-DTribitsExProj_ENABLE_SimpleCxx:BOOL=OFF[;]"
    ALWAYS_FAIL_ON_ZERO_RETURN
  )
  # NOTE: The above test ensures that a test failure in a package is caught
  # and reported correctly.


##############################################################################


tribits_add_advanced_test( CTestDriver_PBP_ST_BreakConfigureRequiredPkg
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
  TEST_0
    MESSAGE "Copy the project dir so I can break it!"
    CMND cp
    ARGS -r ${TribitsExProj_DIR} .
  TEST_1
    MESSAGE "Break the configure for just the required SimpleCxx package"
    CMND ${CMAKE_CURRENT_SOURCE_DIR}/append_file_with_line.sh
    ARGS TribitsExampleProject/packages/simple_cxx/CMakeLists.txt
      "Configure of SimpleCxx is broken!"
  TEST_2
    MESSAGE "Run ctest driver which should show a failed configure for just the SimpleCxx package!"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${PBP_COMMON_ENV_ARGS}
      TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
      ${CTEST_DROP_SITE_ENV_ARGS}
      CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_PBP_ST_BreakConfigureRequiredPkg
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackagesB WithSubpackagesC WithSubpackages WrapExternal 7"
      "SimpleCxx FAILED to configure"
      "CONFIGURE_OPTIONS = '.*[;]-DTribitsExProj_ENABLE_SimpleCxx:BOOL=OFF[;].*[;]-DTribitsExProj_ENABLE_MixedLang:BOOL=ON'"
      "MixedLang: Configure passed"
      "MixedLang: Libs build passed"
      "MixedLang_RayTracerTests [.]+ +Passed"
      "CONFIGURE_OPTIONS = '.*[;]-DTribitsExProj_ENABLE_SimpleCxx:BOOL=OFF[;].*[;]-DTribitsExProj_ENABLE_WithSubpackages:BOOL=ON'"
      "WithSubpackages: Configure passed"
      "WithSubpackages: Libs build passed"
      "WithSubpackages: All build passed"
      "No tests were found"  # Can't tell which package :-(
      "CONFIGURE_OPTIONS = '.*[;]-DTribitsExProj_ENABLE_SimpleCxx:BOOL=OFF[;].*[;]-DTribitsExProj_ENABLE_WrapExternal:BOOL=ON'"
      "WrapExternal: Configure passed"
      "WrapExternal: All build passed"
      "Final set packages that failed to configure or have the libraries build: 'SimpleCxx'"
      "Final set of packages that had any failures: 'SimpleCxx'"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL FAILED"
    ALWAYS_FAIL_ON_ZERO_RETURN
  )
  # NOTE: The above test ensures that the configure failure of an upstream
  # optional package does not break the configure, build, or tests of
  # downstream packages.  It only disables the broken upstream package in
  # downstream packages.  This provides a robust package-by-package mode where
  # configure errors in required upstream packages result in the graceful
  # disable of downstream packages so that they don't report errors for those
  # packages when the problem is really in upstream packages.  See the var
  # ${PROJECT_NAME}_DISABLE_ENABLED_FORWARD_DEP_PACKAGES that is passed
  # through in package-by-package mode and GitHub PR TriBITSPub/TriBITS#394.


##############################################################################


tribits_add_advanced_test( CTestDriver_PBP_ST_BreakBuildLibRequiredPkg
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
  TEST_0
    MESSAGE "Copy the project dir so I can break it!"
    CMND cp
    ARGS -r ${TribitsExProj_DIR} .
  TEST_1
    MESSAGE "Break the build of a lib for the required SimpleCxx package"
    CMND ${CMAKE_CURRENT_SOURCE_DIR}/append_file_with_line.sh
    ARGS TribitsExampleProject/packages/simple_cxx/src/SimpleCxx_HelloWorld.cpp
      "Build of the SimpleCxx library is broken!"
  TEST_2
    MESSAGE "Run ctest driver which should show a failed lib build for just the SimpleCxx package!"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${PBP_COMMON_ENV_ARGS}
      TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
      ${CTEST_DROP_SITE_ENV_ARGS}
      CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_PBP_ST_BreakBuildLibRequiredPkg
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackagesB WithSubpackagesC WithSubpackages WrapExternal 7"
      "SimpleCxx: Configure passed"
      "FAILED library build for package 'SimpleCxx'"
      "SimpleCxx: Skipping tests since library build failed"
      "CONFIGURE_OPTIONS = '.*[;]-DTribitsExProj_ENABLE_SimpleCxx:BOOL=OFF[;].*[;]-DTribitsExProj_ENABLE_MixedLang:BOOL=ON'"
      "MixedLang: Configure passed"
      "MixedLang: Libs build passed"
      "MixedLang_RayTracerTests [.]+ +Passed"
      "CONFIGURE_OPTIONS = '.*[;]-DTribitsExProj_ENABLE_SimpleCxx:BOOL=OFF[;].*[;]-DTribitsExProj_ENABLE_WithSubpackages:BOOL=ON'"
      "WithSubpackages: Configure passed"
      "WithSubpackages: Libs build passed"
      "WithSubpackages: All build passed"
      "No tests were found"  # Can't tell which package :-(
      "CONFIGURE_OPTIONS = '.*[;]-DTribitsExProj_ENABLE_SimpleCxx:BOOL=OFF[;].*[;]-DTribitsExProj_ENABLE_WrapExternal:BOOL=ON'"
      "WrapExternal: Configure passed"
      "WrapExternal: All build passed"
      "Final set packages that failed to configure or have the libraries build: 'SimpleCxx'"
      "Final set of packages that had any failures: 'SimpleCxx'"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL FAILED"
    ALWAYS_FAIL_ON_ZERO_RETURN
  )
  # NOTE: The above test is similar to the test
  # CTestDriver_PBP_ST_BreakConfigureRequiredPkg except the above test is for
  # the case where a library in a required upstream package does not build and
  # we want those downstream packages with a required dependency on that
  # package to be gracefully disabled.


##############################################################################


tribits_add_advanced_test( CTestDriver_PBP_ST_ALL_COVERAGE
  OVERALL_WORKING_DIRECTORY TEST_NAME
  EXCLUDE_IF_NOT_TRUE  ${PACKAGE_NAME}_CTEST_DRIVER_COVERAGE_TESTS
    ${PROJECT_NAME}_ENABLE_Fortran
  OVERALL_NUM_MPI_PROCS 1
  TEST_0
    MESSAGE "Symlink TribitsExampleProject so no need to clone (which would not work)."
    CMND ln
    ARGS -s ${TribitsExProj_DIR} .
  TEST_1
    MESSAGE "Run ctest driver testing everything but no push."
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${PBP_COMMON_ENV_ARGS}
      TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
	CTEST_DO_COVERAGE_TESTING=TRUE
      ${CTEST_DROP_SITE_ENV_ARGS}
      CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_PBP_ST_ALL_COVERAGE
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "CTEST_DO_COVERAGE_TESTING='TRUE'"
      "Loop through TribitsExProj packages to configure, build, and test"
      "Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx MixedLang WithSubpackages WrapExternal 4"
      "Running coverage for package 'SimpleCxx'"
      "Running coverage for package 'MixedLang'"
      "Running coverage for package 'WithSubpackages'"
      "Running coverage for package 'WrapExternal'"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  )
  # NOTE: The above test is the only coverage test for PBP.


##############################################################################


tribits_add_advanced_test( CTestDriver_PBP_ST_ALL_MEMORY
  OVERALL_WORKING_DIRECTORY TEST_NAME
  EXCLUDE_IF_NOT_TRUE  ${PACKAGE_NAME}_CTEST_DRIVER_MEMORY_TESTS
    ${PROJECT_NAME}_ENABLE_Fortran
  OVERALL_NUM_MPI_PROCS 1
  TEST_0
    MESSAGE "Symlink TribitsExampleProject so no need to clone (which would not work)."
    CMND ln
    ARGS -s ${TribitsExProj_DIR} .
  TEST_1
    MESSAGE "Run ctest driver testing everything but no push."
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${PBP_COMMON_ENV_ARGS}
      TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        CTEST_DO_MEMORY_TESTING=TRUE
      ${CTEST_DROP_SITE_ENV_ARGS}
      CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_PBP_ST_ALL_MEMORY
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "CTEST_DO_MEMORY_TESTING='TRUE'"
      "Loop through TribitsExProj packages to configure, build, and test"
      "Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx MixedLang WithSubpackages WrapExternal 4"
      "Running memory testing for package 'SimpleCxx'"
      "MemCheck .+: SimpleCxx_HelloWorldTests [.]+ +Passed"
      "Running memory testing for package 'MixedLang'"
      "MemCheck .+: MixedLang_RayTracerTests [.]+ +Passed"
      "Running memory testing for package 'WithSubpackages'"
      "MemCheck .+: WithSubpackagesA_test_of_a [.]+ +Passed"
      "Running memory testing for package 'WrapExternal'"
      "MemCheck .+: WrapExternal_run_external_func [.]+ +Passed"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  )
  # NOTE: The above test is the only memory test for PBP.


##############################################################################


tribits_add_advanced_test( CTestDriver_PBP_Dashboard_ST_ALL_PASS
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
  TEST_0 CMND ${CMAKE_COMMAND}
    MESSAGE "Do the initial configure"
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_Fortran=ON
      -DTribitsExProj_ENABLE_ALL_PACKAGES=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      -DTribitsExProj_ENABLE_SECONDARY_TESTED_CODE=ON
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackagesB WithSubpackagesC WithSubpackages WrapExternal 7"
      "Finished configuring TribitsExProj"
      "Generating done"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  TEST_1
    MESSAGE "Run the dashboard target"
    CMND env
    ARGS
      CTEST_BUILD_NAME=${PROJECT_NAME}_CTestDriver_PBP_Dashboard_ST_ALL_PASS
      CTEST_DASHBOARD_ROOT=
      make dashboard
    PASS_REGULAR_EXPRESSION_ALL
      "Running package-by-package experimental dashboard"
      "A[)] Clean out the list of packages"
      "B[)] Run the dashboard command setting the list of packages"
      "Running: env TRIBITS_PROJECT_ROOT=.*/tribits/examples/TribitsExampleProject TribitsExProj_TRIBITS_DIR=.*/tribits TribitsExProj_WARNINGS_AS_ERRORS_FLAGS='' TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=ON CTEST_DO_UPDATES=OFF TribitsExProj_CTEST_DO_ALL_AT_ONCE=FALSE CTEST_DO_COVERAGE_TESTING=OFF CTEST_DO_SUBMIT=.* CTEST_DROP_METHOD=.* CTEST_DROP_SITE=.* CTEST_DROP_LOCATION=.* TribitsExProj_EXTRAREPOS_FILE=.*/tribits/examples/TribitsExampleProject/cmake/ExtraRepositoriesList.cmake TribitsExProj_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE= TribitsExProj_IGNORE_MISSING_EXTRA_REPOSITORIES=FALSE TribitsExProj_EXTRA_REPOSITORIES= TribitsExProj_PACKAGES=SimpleCxx[;]MixedLang[;]WithSubpackagesA[;]WithSubpackagesB[;]WithSubpackagesC[;]WithSubpackages[;]WrapExternal PROJECT_SOURCE_DIR=.*/tribits/examples/TribitsExampleProject .*/bin/ctest -V -S .*/tribits/ctest_driver/experimental_build_test.cmake"
      "-- ENV_TribitsExProj_ENABLE_SECONDARY_TESTED_CODE='ON'"
      "TribitsExProj_ENABLE_SECONDARY_TESTED_CODE='ON'"
      "Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx MixedLang WithSubpackages WrapExternal 4"
      "Loop through TribitsExProj packages to configure, build, and test"
      "0. Processing current package SimpleCxx: libs='ON', tests='ON'"
      "1. Processing current package MixedLang: libs='ON', tests='ON'"
      "2. Processing current package WithSubpackages: libs='ON', tests='ON'"
      "3. Processing current package WrapExternal: libs='ON', tests='ON'"
      "Done with the incremental building and testing of TribitsExProj packages"
      "TRIBITS_CTEST_DRIVER_ERROR_QUEUE is empty. All is well."
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
      "C[)] Clean out the list of packages again to clean the cache file"
      "D[)] Reconfigure with the original package list"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  TEST_2
    MESSAGE "Verify that the right set of packages are set"
    CMND ${CMAKE_COMMAND} ARGS .
    PASS_REGULAR_EXPRESSION_ALL
      "Enabling all required .and optional since TribitsExProj_ENABLE_ALL_OPTIONAL_PACKAGES=ON. upstream SE packages for current set of enabled packages .TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=ON."
      "Final set of enabled packages:  SimpleCxx MixedLang WithSubpackages WrapExternal 4"
      "Generating done"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  )
  # The above test does a lot of checking to really pin down the behavior of
  # the tribits_ctest_driver() function run from the basic 'dashboard' target.
  # Follow-on tests will not check so much output.


##############################################################################


if ( ${PROJECT_NAME}_ENABLE_Fortran )
  set(TribitsExProj_PACKAGES_list
    "SimpleCxx[;]MixedLang[;]WithSubpackagesA[;]WithSubpackages")
else()
  set(TribitsExProj_PACKAGES_list
    "SimpleCxx[;]WithSubpackagesA[;]WithSubpackages")
endif()

tribits_add_advanced_test( CTestDriver_AAO_Dashboard_ForwardArgs
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${CMAKE_COMMAND}
    MESSAGE "Do the initial configure with vars to forward "
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_ALL_PACKAGES=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      # Vars that get forwarded to ctest -S command
      -DTribitsExProj_DASHBOARD_CTEST_ARGS="-VV"
      -DCTEST_BUILD_NAME=MyCustomBuildName
      -DTribitsExProj_ENABLE_SECONDARY_TESTED_CODE=OFF
      -DTribitsExProj_CTEST_DO_ALL_AT_ONCE=TRUE
      -DTribitsExProj_ENABLE_COVERAGE_TESTING=FALSE
      -DCTEST_BUILD_FLAGS="-j6"
      -DCTEST_PARALLEL_LEVEL=10
      -DCTEST_DO_SUBMIT=FALSE
      -DCTEST_DROP_METHOD=OFF
      -DCTEST_DROP_SITE=OFF
      -DCTEST_DROP_LOCATION=OFF
      -DCTEST_DROP_SITE_COVERAGE=FALSE
      -DCTEST_DROP_LOCATION_COVERAGE=FALSE
      -DTRIBITS_2ND_CTEST_DROP_LOCATION=FALSE
      -DTRIBITS_2ND_CTEST_DROP_SITE=FALSE
      -DTribitsExProj_EXTRAREPOS_FILE=FALSE
      -DTribitsExProj_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=NONE
      -DTribitsExProj_IGNORE_MISSING_EXTRA_REPOSITORIES=TRUE
      -DTribitsExProj_EXTRA_REPOSITORIES=DummyRepo1,DummyRepo2,DummyRepo3 # Do not exist!
      #
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "Finished configuring TribitsExProj"
      "Generating done"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  TEST_1
    MESSAGE "Run the dashboard target and check for the correct env vars and stdout"
    CMND env
    ARGS
      CTEST_BUILD_NAME=ThisBuildNameIsIgnored
      CTEST_DASHBOARD_ROOT=
      make dashboard
    PASS_REGULAR_EXPRESSION_ALL
      "Running: env TRIBITS_PROJECT_ROOT=.*/tribits/examples/TribitsExampleProject TribitsExProj_TRIBITS_DIR=.*/tribits TribitsExProj_WARNINGS_AS_ERRORS_FLAGS='' TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=OFF CTEST_DO_UPDATES=OFF TribitsExProj_CTEST_DO_ALL_AT_ONCE=TRUE CTEST_BUILD_NAME=MyCustomBuildName CTEST_DO_COVERAGE_TESTING=FALSE CTEST_BUILD_FLAGS='-j6' CTEST_PARALLEL_LEVEL=10 CTEST_DO_SUBMIT=FALSE CTEST_DROP_METHOD=OFF CTEST_DROP_SITE=OFF CTEST_DROP_LOCATION=OFF CTEST_DROP_SITE_COVERAGE=FALSE CTEST_DROP_LOCATION_COVERAGE=FALSE TRIBITS_2ND_CTEST_DROP_LOCATION=FALSE TRIBITS_2ND_CTEST_DROP_SITE=FALSE TribitsExProj_EXTRAREPOS_FILE=FALSE TribitsExProj_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=NONE TribitsExProj_IGNORE_MISSING_EXTRA_REPOSITORIES=TRUE TribitsExProj_EXTRA_REPOSITORIES=DummyRepo1,DummyRepo2,DummyRepo3 TribitsExProj_PACKAGES=${TribitsExProj_PACKAGES_list} PROJECT_SOURCE_DIR=.*/tribits/examples/TribitsExampleProject .*/ctest -VV -S .*/tribits/ctest_driver/experimental_build_test.cmake"

      "ENV_PROJECT_SOURCE_DIR='.*/tribits/examples/TribitsExampleProject'"
      "ENV_CTEST_BUILD_NAME='MyCustomBuildName'"
      "ENV_TribitsExProj_CTEST_DO_ALL_AT_ONCE='TRUE'"
      "ENV_CTEST_BUILD_FLAGS=''-j6''"
      "ENV_CTEST_PARALLEL_LEVEL='10'"
      "ENV_TribitsExProj_WARNINGS_AS_ERRORS_FLAGS=''''"
      "ENV_CTEST_DO_COVERAGE_TESTING='FALSE'"
      "ENV_CTEST_DO_SUBMIT='FALSE'"
      "ENV_TribitsExProj_ENABLE_SECONDARY_TESTED_CODE='OFF'"
      "ENV_TribitsExProj_PACKAGES='${TribitsExProj_PACKAGES_list}'"
      "ENV_TribitsExProj_EXTRAREPOS_FILE='FALSE'"
      "ENV_TribitsExProj_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE='NONE'"
      "ENV_TribitsExProj_EXTRA_REPOSITORIES='DummyRepo1,DummyRepo2,DummyRepo3'"
      "ENV_TRIBITS_2ND_CTEST_DROP_SITE='FALSE'"
      "ENV_TRIBITS_2ND_CTEST_DROP_LOCATION='FALSE'"

    ALWAYS_FAIL_ON_NONZERO_RETURN
  )
  # The above test makes sure that the right vars set in the CMake project are
  # forwarded to the ctest -S command correctly.  It also ensures that vars
  # set to a "False" value (i.e. that evaluate to false in `if (<var>)`) are
  # also passed through.  The is critical to turn off submits to a secondary
  # CDash site through the TRIBITS_2ND_CTEST_DROP_SITE and
  # TRIBITS_2ND_CTEST_DROP_LOCATION vars (see #370).


##############################################################################


tribits_add_advanced_test( CTestDriver_AAO_Dashboard_Then_NoConfigDashboard
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${CMAKE_COMMAND}
    MESSAGE "Do the initial configure"
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DCTEST_BUILD_NAME=${PROJECT_NAME}_CTestDriver_AAO_Dashboard_Then_NoConfigDashboard
      -DTribitsExProj_CTEST_DO_ALL_AT_ONCE=TRUE
      -DTribitsExProj_ENABLE_ALL_PACKAGES=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      -DTribitsExProj_ENABLE_SECONDARY_TESTED_CODE=ON
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "Finished configuring TribitsExProj"
      "Generating done"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  TEST_1
    MESSAGE "Run the dashboard target with configure"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=
      make dashboard
    PASS_REGULAR_EXPRESSION_ALL
      "Built target dashboard"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  TEST_2
    MESSAGE "Run the dashboard target again without a configure"
    CMND env
    ARGS
      CTEST_DO_CONFIGURE=OFF
      CTEST_DASHBOARD_ROOT=
      make dashboard
    PASS_REGULAR_EXPRESSION_ALL
      "ENV_CTEST_DO_CONFIGURE='OFF'"
      "Skipping configure due to CTEST_DO_CONFIGURE='OFF'"
      "Build PASSED"
      "File '' does NOT exist so all tests passed"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  )
  # The above test ensures that you can do a configure, run the dashboard
  # target, then run 'env CTEST_DO_CONFIGURE=OFF make dashboard' again and
  # have it work correctly.  This was a use case that was broken when I tried
  # it with Trilinos.


##############################################################################


tribits_add_advanced_test( CTestDriver_PBP_Dashboard_ST_PackagesSubset_PASS
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
  TEST_0 CMND ${CMAKE_COMMAND}
    MESSAGE "Do the initial configure"
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_Fortran=ON
      -DTribitsExProj_ENABLE_SimpleCxx=ON
      -DTribitsExProj_ENABLE_WrapExternal=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      -DTribitsExProj_ENABLE_SECONDARY_TESTED_CODE=ON
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "Setting SimpleCxx_ENABLE_TESTS=ON"
      "Setting WrapExternal_ENABLE_TESTS=ON"
      "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackages WrapExternal 5"
      "Processing enabled package: SimpleCxx [(]Libs, Tests, Examples[)]"
      "Processing enabled package: MixedLang [(]Libs[)]"
      "Processing enabled package: WithSubpackages [(]A[)]"
      "Processing enabled package: WrapExternal [(]Libs, Tests, Examples[)]"
      "Generating done"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  TEST_1
    MESSAGE "Run the dashboard target"
    CMND env
    ARGS
      CTEST_BUILD_NAME=${PROJECT_NAME}_CTestDriver_PBP_Dashboard_ST_PackagesSubset_PASS
      CTEST_DASHBOARD_ROOT=
      make dashboard
    PASS_REGULAR_EXPRESSION_ALL
      "Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx WrapExternal 2"
      "Loop through TribitsExProj packages to configure, build, and test"
      "0. Processing current package SimpleCxx: libs='ON', tests='ON'"
      "1. Processing current package WrapExternal: libs='ON', tests='ON'"
      "Done with the incremental building and testing of TribitsExProj packages"
      "TRIBITS_CTEST_DRIVER_ERROR_QUEUE is empty. All is well."
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  TEST_2
    MESSAGE "Verify that the right set of packages are set"
    CMND ${CMAKE_COMMAND} ARGS .
    PASS_REGULAR_EXPRESSION_ALL
      "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackages WrapExternal 5"
      "Processing enabled package: SimpleCxx [(]Libs, Tests, Examples[)]"
      "Processing enabled package: MixedLang [(]Libs[)]"
      "Processing enabled package: WithSubpackages [(]A[)]"
      "Processing enabled package: WrapExternal [(]Libs, Tests, Examples[)]"
      "Generating done"
      "Generating done"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  )
  # The above test makes sure that tribits_ctest_driver() function run from
  # the basic 'dashboard' target only tests the explicitly enabled and tested
  # packages.  It also makes sure that when the project is reconfigured at the
  # end, that it gives the same set of package enables.


##############################################################################


#
# Test package-by-package multi-phase invocation
#


tribits_add_advanced_test( CTestDriver_PBP_PT_ALL_PASS_CALLS_2

  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran

  TEST_0
    MESSAGE "Symlink TribitsExampleProject so no need to clone (which would not work)."
    CMND ln
    ARGS -s ${TribitsExProj_DIR} .

  TEST_1
    MESSAGE "Do start, configure, build and test but don't run tests"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${PBP_COMMON_ENV_ARGS}
      CTEST_DO_TEST=OFF
      ${CTEST_DROP_SITE_ENV_ARGS}
      CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_PBP_PT_ALL_PASS_CALLS_2
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "-- CTEST_DO_NEW_START='TRUE'"
      "-- ENV_CTEST_DO_UPDATES='OFF'"
      "-- CTEST_DO_UPDATES='OFF'"
      "-- CTEST_DO_CONFIGURE='TRUE'"
      "-- CTEST_DO_BUILD='TRUE'"
      "-- CTEST_DO_INSTALL='FALSE'"
      "-- ENV_CTEST_DO_TEST='OFF'"
      "-- CTEST_DO_TEST='OFF'"
      "-- CTEST_DO_COVERAGE_TESTING='OFF'"
      "-- CTEST_DO_MEMORY_TESTING='FALSE'"
      "Start up a new dashboard calling ctest_start[(][.][.][.][)]"
      "Final set of enabled packages:  SimpleCxx MixedLang WithSubpackages 3"
      "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackages"
      "Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx MixedLang WithSubpackages 3"
      "Calling ctest_start[(]Continuous[)]"
      "Run dashboard with model Continuous"
      "SimpleCxx: Configure passed"
      "SimpleCxx: Libs build passed"
      "SimpleCxx: All build passed"
      "SimpleCxx: Skipping running tests due to CTEST_DO_TEST='OFF'"
      "MixedLang: Configure passed"
      "MixedLang: Libs build passed"
      "MixedLang: All build passed"
      "MixedLang: Skipping running tests due to CTEST_DO_TEST='OFF'"
      "WithSubpackages: Configure passed"
      "WithSubpackages: Libs build passed"
      "WithSubpackages: All build passed"
      "WithSubpackages: Skipping running tests due to CTEST_DO_TEST='OFF'"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_2
    MESSAGE "Do run tests only"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${PBP_COMMON_ENV_ARGS}
      CTEST_DO_NEW_START=OFF
      CTEST_DO_CONFIGURE=OFF
      CTEST_DO_BUILD=OFF
      ${CTEST_DROP_SITE_ENV_ARGS}
      CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_PBP_PT_ALL_PASS_CALLS_2
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "-- CTEST_DO_NEW_START='OFF'"
      "-- CTEST_DO_UPDATES='OFF'"
      "-- CTEST_DO_CONFIGURE='OFF'"
      "-- CTEST_DO_BUILD='OFF'"
      "-- CTEST_DO_TEST='TRUE'"
      "Start up a new dashboard calling ctest_start[(][.][.][.][)]"
      "Final set of enabled packages:  SimpleCxx MixedLang WithSubpackages 3"
      "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackages"
      "Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx MixedLang WithSubpackages 3"
      "Use previous dashboard calling ctest_start[(][.][.][.] APPEND[)] due to CTEST_DO_NEW_START='OFF'"
      "Calling ctest_start[(]Continuous[;]APPEND[)]"
      "Run dashboard with model Continuous"
      "SimpleCxx: Configure passed!"
      "SimpleCxx: Skipping build due to CTEST_DO_BUILD='OFF'"
      "SimpleCxx: File '' does NOT exist so all tests passed"
      "MixedLang: Configure passed!"
      "MixedLang: Skipping build due to CTEST_DO_BUILD='OFF'"
      "MixedLang: File '' does NOT exist so all tests passed"
      "WithSubpackages: Configure passed!"
      "WithSubpackages: Skipping build due to CTEST_DO_BUILD='OFF'"
      "-- CTEST_CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE='54321'"
      "-- CTEST_CUSTOM_MAXIMUM_FAILED_TEST_OUTPUT_SIZE='123456'"
      "WithSubpackages: File '' does NOT exist so all tests passed"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  )
# NOTE: The above test shows that you can also do split configure/build and
# testing in two different ctest -S invocations.  The only big downside is
# that you have to reconfigure for each package even if you are not asked to
# configure.  But this very nicely submits to CDash.  Very nice!


##############################################################################


tribits_add_advanced_test( CTestDriver_PBP_PT_ALL_PASS_CALLS_4

  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran

  TEST_0
    MESSAGE "Symlink TribitsExampleProject so no need to clone (which would not work)."
    CMND ln
    ARGS -s ${TribitsExProj_DIR} .

  TEST_1
    MESSAGE "Do start only"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${PBP_COMMON_ENV_ARGS}
      CTEST_DO_CONFIGURE=OFF
      CTEST_DO_BUILD=OFF
      CTEST_DO_TEST=OFF
      ${CTEST_DROP_SITE_ENV_ARGS}
      CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_PBP_PT_ALL_PASS_CALLS_4
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "-- CTEST_DO_NEW_START='TRUE'"
      "-- CTEST_DO_UPDATES='OFF'"
      "-- CTEST_DO_CONFIGURE='OFF'"
      "-- CTEST_DO_BUILD='OFF'"
      "-- CTEST_DO_TEST='OFF'"
      "-- CTEST_DO_COVERAGE_TESTING='OFF'"
      "-- CTEST_DO_MEMORY_TESTING='FALSE'"
      "Start up a new dashboard calling ctest_start[(][.][.][.][)]"
      "Final set of enabled packages:  SimpleCxx MixedLang WithSubpackages 3"
      "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackages"
      "Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx MixedLang WithSubpackages 3"
      "Calling ctest_start[(]Continuous[)]"
      "Run dashboard with model Continuous"
      "Skipping processing anything else since not requested to configure, build, test, or run memory tests"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_2
    MESSAGE "Do configure only"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${PBP_COMMON_ENV_ARGS}
      CTEST_DO_NEW_START=OFF
      CTEST_DO_CONFIGURE=ON
      CTEST_DO_BUILD=OFF
      CTEST_DO_TEST=OFF
      ${CTEST_DROP_SITE_ENV_ARGS}
      CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_PBP_PT_ALL_PASS_CALLS_4
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "-- CTEST_DO_NEW_START='OFF'"
      "-- CTEST_DO_UPDATES='OFF'"
      "-- CTEST_DO_CONFIGURE='ON'"
      "-- CTEST_DO_BUILD='OFF'"
      "-- CTEST_DO_TEST='OFF'"
      "Start up a new dashboard calling ctest_start[(][.][.][.][)]"
      "Final set of enabled packages:  SimpleCxx MixedLang WithSubpackages 3"
      "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackages"
      "Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx MixedLang WithSubpackages 3"
      "Use previous dashboard calling ctest_start[(][.][.][.] APPEND[)] due to CTEST_DO_NEW_START='OFF'"
      "Calling ctest_start[(]Continuous[;]APPEND[)]"
      "Run dashboard with model Continuous"
      "SimpleCxx: Configure passed!"
      "SimpleCxx: Skipping build due to CTEST_DO_BUILD='OFF'"
      "SimpleCxx: Skipping running tests due to CTEST_DO_TEST='OFF'"
      "MixedLang: Configure passed!"
      "MixedLang: Skipping build due to CTEST_DO_BUILD='OFF'"
      "MixedLang: Skipping running tests due to CTEST_DO_TEST='OFF'"
      "WithSubpackages: Configure passed!"
      "WithSubpackages: Skipping build due to CTEST_DO_BUILD='OFF'"
      "WithSubpackages: Skipping running tests due to CTEST_DO_TEST='OFF'"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_3
    MESSAGE "Do build only"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${PBP_COMMON_ENV_ARGS}
      CTEST_DO_NEW_START=OFF
      CTEST_DO_CONFIGURE=OFF
      CTEST_DO_BUILD=ON
      CTEST_DO_TEST=OFF
      ${CTEST_DROP_SITE_ENV_ARGS}
      CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_PBP_PT_ALL_PASS_CALLS_4
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "-- CTEST_DO_NEW_START='OFF'"
      "-- CTEST_DO_UPDATES='OFF'"
      "-- CTEST_DO_CONFIGURE='OFF'"
      "-- CTEST_DO_BUILD='ON'"
      "-- CTEST_DO_TEST='OFF'"
      "Start up a new dashboard calling ctest_start[(][.][.][.][)]"
      "Final set of enabled packages:  SimpleCxx MixedLang WithSubpackages 3"
      "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackages"
      "Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx MixedLang WithSubpackages 3"
      "Use previous dashboard calling ctest_start[(][.][.][.] APPEND[)] due to CTEST_DO_NEW_START='OFF'"
      "Calling ctest_start[(]Continuous[;]APPEND[)]"
      "Run dashboard with model Continuous"
      "SimpleCxx: Configure passed!"
      "SimpleCxx: Libs build passed"
      "SimpleCxx: All build passed"
      "SimpleCxx: Skipping running tests due to CTEST_DO_TEST='OFF'"
      "MixedLang: Configure passed!"
      "MixedLang: Libs build passed"
      "MixedLang: All build passed"
      "MixedLang: Skipping running tests due to CTEST_DO_TEST='OFF'"
      "WithSubpackages: Configure passed!"
      "WithSubpackages: Libs build passed"
      "WithSubpackages: All build passed"
      "WithSubpackages: Skipping running tests due to CTEST_DO_TEST='OFF'"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_4
    MESSAGE "Run tests only"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${PBP_COMMON_ENV_ARGS}
      CTEST_DO_NEW_START=OFF
      CTEST_DO_CONFIGURE=OFF
      CTEST_DO_BUILD=OFF
      ${CTEST_DROP_SITE_ENV_ARGS}
      CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_PBP_PT_ALL_PASS_CALLS_4
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "-- CTEST_DO_NEW_START='OFF'"
      "-- CTEST_DO_UPDATES='OFF'"
      "-- CTEST_DO_CONFIGURE='OFF'"
      "-- CTEST_DO_BUILD='OFF'"
      "-- CTEST_DO_TEST='TRUE'"
      "Start up a new dashboard calling ctest_start[(][.][.][.][)]"
      "Final set of enabled packages:  SimpleCxx MixedLang WithSubpackages 3"
      "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackages"
      "Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx MixedLang WithSubpackages 3"
      "Use previous dashboard calling ctest_start[(][.][.][.] APPEND[)] due to CTEST_DO_NEW_START='OFF'"
      "Calling ctest_start[(]Continuous[;]APPEND[)]"
      "Run dashboard with model Continuous"
      "SimpleCxx: Configure passed!"
      "SimpleCxx: Skipping build due to CTEST_DO_BUILD='OFF'"
      "SimpleCxx: File '' does NOT exist so all tests passed"
      "MixedLang: Configure passed!"
      "MixedLang: Skipping build due to CTEST_DO_BUILD='OFF'"
      "MixedLang: File '' does NOT exist so all tests passed"
      "WithSubpackages: Configure passed!"
      "WithSubpackages: Skipping build due to CTEST_DO_BUILD='OFF'"
      "WithSubpackages: File '' does NOT exist so all tests passed"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  )


######################################################################
#
# Test all-at-once mode for TribitsCTestDriverCore.cmake
#
######################################################################


function(generate_aao_tests)

  set(TribitsExampleProject_AAO_COMMON_CONFIG_ARGS
    ${TribitsExampleProject_COMMON_CONFIG_ARGS}
    -DTribitsExProj_CTEST_DO_ALL_AT_ONCE=TRUE
    )

  set( AAO_COMMON_ENV_ARGS
    ${PBP_COMMON_ENV_ARGS}
    TribitsExProj_CTEST_DO_ALL_AT_ONCE=TRUE
    )

  set(AAO_POSTFIX "P")
  # ToDo: Just replace '${AAO_POSTFIX}' with 'P' once the function
  # generate_aao_tests() is removed!


  ##############################################################################


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_PASS
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
    TEST_0
      MESSAGE "Symlink TribitsExampleProject so no need to clone (which would not work)."
      CMND ln
      ARGS -s ${TribitsExProj_DIR} .
    TEST_1
      MESSAGE "Run ctest driver testing everything but no push."
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        CTEST_PARALLEL_LEVEL=3
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_PASS
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "NONE does not exist, skipping extra repositories"
        "Setting TribitsExProj_ENABLE_ALL_PACKAGES=ON since TribitsExProj_PACKAGES_USER_SELECTED=''"
        "Final set of enabled packages:  SimpleCxx MixedLang WithSubpackages WrapExternal 4"
        "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackagesB WithSubpackagesC WithSubpackages WrapExternal"
        "Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx MixedLang WithSubpackages WrapExternal 4"
        "Configure, build, test and submit results all-at-once for all enabled packages"
	"CTEST_LABELS_FOR_SUBPROJECTS='SimpleCxx[;]MixedLang[;]WithSubpackages[;]WrapExternal'"
        "Configuring [.][.][.]"
        "CONFIGURE_OPTIONS = '-DTribitsExProj_TRIBITS_DIR=.*/tribits[;]-DCTEST_USE_LAUNCHERS:BOOL=1[;]-DTribitsExProj_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=ON[;]-DTribitsExProj_WARNINGS_AS_ERRORS_FLAGS:STRING=[;]-DTribitsExProj_ALLOW_NO_PACKAGES:BOOL=ON[;]-DTribitsExProj_DISABLE_ENABLED_FORWARD_DEP_PACKAGES=ON[;]-DTribitsExProj_DEPS_XML_OUTPUT_FILE:FILEPATH=[;]-DTribitsExProj_ENABLE_SECONDARY_TESTED_CODE:BOOL=TRUE[;]-DTribitsExProj_EXTRAREPOS_FILE:STRING=[;]-DTribitsExProj_IGNORE_MISSING_EXTRA_REPOSITORIES:BOOL=ON[;]-DTribitsExProj_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE:STRING=Continuous[;]-DTribitsExProj_ENABLE_ALL_PACKAGES=ON[;]-DTribitsExProj_ENABLE_TESTS:BOOL=ON[;]-DBUILD_SHARED_LIBS:BOOL=ON[;]-DCMAKE_BUILD_TYPE=DEBUG[;]-DCMAKE_C_COMPILER=gcc[;]-DCMAKE_CXX_COMPILER=g[+][+][;]-DCMAKE_Fortran_COMPILER=gfortran[;]-DTribitsExProj_ENABLE_Fortran=ON[;]-DTribitsExProj_TRACE_ADD_TEST=ON[;]-DTribitsExProj_ENABLE_INSTALL_CMAKE_CONFIG_FILES=OFF'"
        "Configure PASSED"
        "-- CTEST_CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE='54321'"
        "-- CTEST_CUSTOM_MAXIMUM_FAILED_TEST_OUTPUT_SIZE='123456'"
        "Building all targets [.][.][.]"
        "Build output: BUILD_ALL_NUM_ERRORS='0',BUILD_ALL_RETURN_VAL='0'"
        "Build PASSED"
        "Running tests .parallel level 3."
        "SimpleCxx_HelloWorldTests [.]+ +Passed"
        "SimpleCxx_HelloWorldProg [.]+ +Passed"
        "WithSubpackagesA_test_of_a [.]+ +Passed"
        "WithSubpackagesB_test_of_b [.]+ +Passed"
        "WithSubpackagesC_test_of_c_util [.]+ +Passed"
        "WithSubpackagesC_test_of_c_b_mixed_lang [.]+ +Passed"
        "WrapExternal_run_external_func [.]+ +Passed"
	"Skipping converage tests because CTEST_DO_COVERAGE_TESTING='OFF'"
	"Skipping memory testing because CTEST_DO_MEMORY_TESTING='FALSE'"
        "File '' does NOT exist so all tests passed"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    )
    # NOTE: The above test is the basic mode of testing all packages and making
    # sure that the configure, build, and tests all run correctly for
    # everything.  The names of passing tests are grepped for to make sure they
    # run and pass.  And a non-zero return code is mandated.  Also, the full
    # CONFIGURE_OPTIONS list is checked to make sure that it does not change
    # without being noticed.  Follow-on tests will not check all of that.


  ##############################################################################


  set(cdash_build_url_expected_regex
    "https://cdash[.]site[.]com/cdash/index[.]php[?]project=CustomTribitsExProj&filtercount=3&showfilters=1&filtercombine=and&field1=site&compare1=61&value1=My%20Site&field2=buildname&compare2=61&value2=TriBITS_CTestDriver_AAOP_CDASH_URL&field3=buildstamp&compare3=61&value3=[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]-[0-9][0-9][0-9][0-9]-Custom%20CDash%20Group" )
  # NOTE: Above, we have to repeat [0-9] 8 times and 4 times for the regex for
  # the number of digits in the buildstarttime.  CMake regex does not support
  # \d{8}-\d{4} :-(

  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_CDASH_URL
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
    TEST_0
      MESSAGE "Symlink TribitsExampleProject so no need to clone (which would not work)."
      CMND ln
      ARGS -s ${TribitsExProj_DIR} .
    TEST_1
      MESSAGE "Run ctest driver testing everyting but no push."
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        CTEST_PARALLEL_LEVEL=3
        CTEST_DO_SUBMIT=OFF  # Never submit this (see below note)
        CTEST_SITE="My Site"
        CTEST_DROP_SITE="cdash.site.com"
        CTEST_PROJECT_NAME="CustomTribitsExProj"
        CTEST_DROP_LOCATION="/cdash/submit.php?project=CustomTribitsExProj"
        TribitsExProj_PACKAGES=SimpleCxx
        CTEST_TEST_TYPE=Continuous
        TribitsExProj_TRACK="Custom CDash Group" # Does not need to exist!
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_CDASH_URL
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "NONE does not exist, skipping extra repositories"
        "Final set of enabled packages:  SimpleCxx 1"
        "Final set of enabled SE packages:  SimpleCxx 1"
        "Results will be submitted on CDash at the following links:"
        "Link to this build's results on CDash:"
        "${cdash_build_url_expected_regex}"
        "File '' does NOT exist so all tests passed"
        "See results submitted on CDash at the following links:"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    TEST_2
      MESSAGE "Check that CDashResults.txt exists and lists the right URL."
      CMND cat ARGS BUILD/CDashResults.txt
      PASS_REGULAR_EXPRESSION_ALL
        "Link to this build's results on CDash:"
        "${cdash_build_url_expected_regex}"
    )
    # NOTE: The above test is never actually submitted to CDash, even when
    # ${PACKAGE_NAME}_CTEST_DRIVER_SUBMIT_TO is set.  This is so we can
    # exactly control the CDash location vars and do a good test of the
    # construction of the CDash URL.  Note that we don't know the actual
    # buildstamp because the buildstarttime changes everytime the test runs.
    # We only know it is [8 digits]-[4 digits].


  ##############################################################################


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_ST_SKIP_CTEST_ADD_TEST
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran

    TEST_0
      MESSAGE "Symlink TribitsExampleProject so no need to clone (which would not work)."
      CMND ln
      ARGS -s ${TribitsExProj_DIR} .

    TEST_1
      MESSAGE "Run ctest driver testing everything but no push."
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        CTEST_PARALLEL_LEVEL=3
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        TribitsExProj_SKIP_CTEST_ADD_TEST=TRUE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_SKIP_CTEST_ADD_TEST
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "-- ENV_TribitsExProj_SKIP_CTEST_ADD_TEST='TRUE'"
	"-- TribitsExProj_SKIP_CTEST_ADD_TEST='TRUE'"
	"CONFIGURE_OPTIONS = '.*[;]-DTribitsExProj_SKIP_CTEST_ADD_TEST:BOOL=TRUE[;].*'"
        "No tests were found[!][!][!]"
        "File '' does NOT exist so all tests passed[!]"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
      #ALWAYS_FAIL_ON_NONZERO_RETURN

      # NOTE: Above, ctest -S returns with the return code of 255 instead of
      # 0.  It seems that ctest just might be assuming that no tests run means
      # that it was an error.  This is disappointing behavior from CTest here.
      # Apparently, this is a known issue.  See
      # https://cmake.org/pipermail/cmake/2011-January/042366.html
    )


  ##############################################################################


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_ST_INNER_ENABLE_TESTS
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran

    TEST_0
      MESSAGE "Symlink TribitsExampleProject so no need to clone (which would not work)."
      CMND ln
      ARGS -s ${TribitsExProj_DIR} .

    TEST_1
      MESSAGE "Run ctest driver testing everything but no push."
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        CTEST_PARALLEL_LEVEL=3
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        TribitsExProj_INNER_ENABLE_TESTS=OFF
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_INNER_ENABLE_TESTS
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "-- ENV_TribitsExProj_INNER_ENABLE_TESTS='OFF'"
	"-- TribitsExProj_INNER_ENABLE_TESTS='OFF'"
	"CONFIGURE_OPTIONS = '.*[;]-DTribitsExProj_ENABLE_TESTS:BOOL=OFF[;].*'"
        "No tests were found[!][!][!]"
        "File '' does NOT exist so all tests passed[!]"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
      #ALWAYS_FAIL_ON_NONZERO_RETURN
 
     # NOTE: Above, ctest -S returns with the return code of 255 instead of
      # 0.  See comment above for SKIP_CTEST_ADD_TEST test.
    )


  ##############################################################################


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_ST_SimpleCxx_PASS
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
    TEST_0
      MESSAGE "Symlink TribitsExampleProject so no need to clone (which would not work)."
      CMND ln
      ARGS -s ${TribitsExProj_DIR} .
    TEST_1
      MESSAGE "Run ctest -S driver"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        TribitsExProj_PACKAGES=SimpleCxx
        CTEST_EXPLICITLY_ENABLE_IMPLICITLY_ENABLED_PACKAGES=FALSE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_SimpleCxx_PASS
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "NONE does not exist, skipping extra repositories"
        "Setting SimpleCxx_ENABLE_TESTS=ON"
        "Final set of enabled packages:  SimpleCxx 1"
        "Final set of enabled SE packages:  SimpleCxx 1"
        "Final set of non-enabled SE packages:  MixedLang WithSubpackagesA WithSubpackagesB WithSubpackagesC WithSubpackages WrapExternal 6"
        "Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx 1"
        "Configure, build, test and submit results all-at-once for all enabled packages"
        "CTEST_LABELS_FOR_SUBPROJECTS='SimpleCxx'"
        "Configuring [.][.][.]"
        "CONFIGURE_OPTIONS = '-DTribitsExProj_TRIBITS_DIR=.*/tribits[;].+[;]-DTribitsExProj_ENABLE_SimpleCxx=ON[;]-DTribitsExProj_ENABLE_TESTS:BOOL=ON[;]-DBUILD_SHARED_LIBS:BOOL=ON[;].+'"
        "Configure PASSED"
        "Building all targets [.][.][.]"
        "Build output: BUILD_ALL_NUM_ERRORS='0',BUILD_ALL_RETURN_VAL='0'"
        "Build PASSED"
        "Running tests .parallel level 1."
        "SimpleCxx_HelloWorldTests [.]+ +Passed"
        "SimpleCxx_HelloWorldProg [.]+ +Passed"
        "File '' does NOT exist so all tests passed"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    #TEST_2
    #  MESSAGE "Make sure the Note.xml file contains the right contents"
    #  CMND env
    #  ARGS
    #    TRIBITS_PROJECT_NAME=TribitsExProj
    #    TribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
    #    CTEST_BUILD_DIR=BUILD
    #    "${CMAKE_CURRENT_LIST_DIR}/../list_cdash_notes_files_in_xml_file.sh"
    #  PASS_REGULAR_EXPRESSION_ALL
    #    "DUMMY_NO_MATCH"
    #  ALWAYS_FAIL_ON_NONZERO_RETURN
   
    # NOTE: Above, we can't actually grep the notes file because the Notes.xml
    # file does not get created unless you submit!  And the notes files for
    # Updates.txt and UpdateCommandsOutput.txt will not get created unless you
    # do ctest_update().  Therefore, we will need automated tests that
    # actually clone and update repos in order to test the generated Notes.xml
    # file.

    )
    # NOTE: The above test enables just one package with no upstream packages
    # at all.  It should display just a single row on CDash.  We can't detect
    # that here but this submits the data so we can inspect that on CDash.


  ##############################################################################


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_ST_PackagesSubset_PASS
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
    TEST_0
      MESSAGE "Symlink TribitsExampleProject so no need to clone (which would not work)."
      CMND ln
      ARGS -s ${TribitsExProj_DIR} .
    TEST_1
      MESSAGE "Run ctest driver testing everything but no push."
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        TribitsExProj_PACKAGES=SimpleCxx,WrapExternal
        CTEST_EXPLICITLY_ENABLE_IMPLICITLY_ENABLED_PACKAGES=FALSE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_PackagesSubset_PASS
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "NONE does not exist, skipping extra repositories"
        "Setting SimpleCxx_ENABLE_TESTS=ON"
        "Setting WrapExternal_ENABLE_TESTS=ON"
        "Final set of enabled packages:  SimpleCxx MixedLang WithSubpackages WrapExternal 4"
        "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackages WrapExternal 5"
        "Final set of non-enabled SE packages:  WithSubpackagesB WithSubpackagesC 2"
        "Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx WrapExternal 2"
        "Configure, build, test and submit results all-at-once for all enabled packages"
	"CTEST_LABELS_FOR_SUBPROJECTS='SimpleCxx[;]WrapExternal'"
        "Configuring [.][.][.]"
        "CONFIGURE_OPTIONS = '-DTribitsExProj_TRIBITS_DIR=.*/tribits[;].+[;]-DTribitsExProj_ENABLE_SimpleCxx=ON[;]-DTribitsExProj_ENABLE_WrapExternal=ON[;]-DTribitsExProj_ENABLE_TESTS:BOOL=ON[;]-DBUILD_SHARED_LIBS:BOOL=ON[;].+'"
        "Configure PASSED"
        "Building all targets [.][.][.]"
        "Build output: BUILD_ALL_NUM_ERRORS='0',BUILD_ALL_RETURN_VAL='0'"
        "Build PASSED"
        "Running tests .parallel level 1."
        "SimpleCxx_HelloWorldTests [.]+ +Passed"
        "SimpleCxx_HelloWorldProg [.]+ +Passed"
        "WrapExternal_run_external_func [.]+ +Passed"
        "File '' does NOT exist so all tests passed"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    )
    # NOTE: The above test makes sure that requesting a subset of packages build
    # will only enable and test that subset of packages.  Any extra dependency
    # enable/disable type of testing should be done with the much cheaper
    # dependency testing in the file ../MockCTestDriver/CMakeLists.txt.


  ##############################################################################


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_GlobalConfigureFail
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    TEST_0
      MESSAGE "Copy the project dir so I can break it!"
      CMND cp
      ARGS -r ${TribitsExProj_DIR} .
    TEST_1
      MESSAGE "Break the outer configure"
      CMND ${CMAKE_CURRENT_SOURCE_DIR}/append_file_with_line.sh
      ARGS TribitsExampleProject/CMakeLists.txt "The outer configure is broken!"
    TEST_2
      MESSAGE "Run ctest driver which should show a failed configure!"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_GlobalConfigureFail
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "Configure FAILED"
	"Skipping build because configure failed"
	"Skipping tests because configure failed"
	"Done with the all-at-once configure, build, test, and submit of TribitsExProj packages"
	"Final set of packages that had any failures: 'ALL_PACKAGES'"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL FAILED"
      ALWAYS_FAIL_ON_ZERO_RETURN
    TEST_3
      MESSAGE "Make sure the failedPackages.txt file has the right contects!"
      CMND cat
      ARGS
        BUILD/failedPackages.txt
      PASS_REGULAR_EXPRESSION_ALL
        "ALL_PACKAGES"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    )
    # NOTE: The above test ensures that a configure failure is correctly
    # interpreted and processed.


  ##############################################################################


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_ST_PackagesSubset_GlobalConfigureFail
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    TEST_0
      MESSAGE "Copy the project dir so I can break it!"
      CMND cp
      ARGS -r ${TribitsExProj_DIR} .
    TEST_1
      MESSAGE "Break the outer configure"
      CMND ${CMAKE_CURRENT_SOURCE_DIR}/append_file_with_line.sh
      ARGS TribitsExampleProject/CMakeLists.txt "The outer configure is broken!"
    TEST_2
      MESSAGE "Run ctest driver which should show a failed configure!"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        TribitsExProj_PACKAGES=SimpleCxx,WrapExternal
        CTEST_EXPLICITLY_ENABLE_IMPLICITLY_ENABLED_PACKAGES=FALSE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_PackagesSubset_GlobalConfigureFail
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "Configure FAILED"
	"Skipping build because configure failed"
	"Skipping tests because configure failed"
	"Done with the all-at-once configure, build, test, and submit of TribitsExProj packages"
	"Final set of packages that had any failures: 'SimpleCxx[;]WrapExternal'"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL FAILED"
      ALWAYS_FAIL_ON_ZERO_RETURN
    )
    # NOTE: The above test ensures that a configure failure is correctly
    # interpreted and processed.


  ##############################################################################


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_BreakWithSubpackagesALib
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
    TEST_0
      MESSAGE "Copy the project dir so I can break it!"
      CMND cp
      ARGS -r ${TribitsExProj_DIR} .
    TEST_1
      MESSAGE "Break the library build WithSubpackageA"
      CMND ${CMAKE_CURRENT_SOURCE_DIR}/append_file_with_line.sh
      ARGS TribitsExampleProject/packages/with_subpackages/a/A.cpp
        "Build of A.cpp lib is broken!"
    TEST_2
      MESSAGE "Run ctest driver which should show a failed library build for just the MixedLang package!"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_FLAGS=-k
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_BreakWithSubpackagesALib
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackagesB WithSubpackagesC WithSubpackages WrapExternal 7"
        "Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx MixedLang WithSubpackages WrapExternal 4"
        "Configure PASSED"
	"Build output: BUILD_ALL_NUM_ERRORS='[1-2]',BUILD_ALL_RETURN_VAL='[0-2]'"
	"Build FAILED"
        "SimpleCxx_HelloWorldTests [.]+ +Passed"
        "SimpleCxx_HelloWorldProg [.]+ +Passed"
	"MixedLang_RayTracerTests [.]+ +Passed"
	"WithSubpackagesA_test_of_a [.]+[*][*][*]Not Run"
	"WithSubpackagesB_test_of_b [.]+[*][*][*]Not Run"
	"WithSubpackagesB_test_of_b_mixed_lang [.]+[*][*][*]Not Run"
	"WithSubpackagesC_test_of_c_util [.]+[*][*][*]Not Run"
	"WithSubpackagesC_test_of_c [.]+[*][*][*]Not Run"
	"WithSubpackagesC_test_of_c_b_mixed_lang [.]+[*][*][*]Not Run"
        "WrapExternal_run_external_func [.]+[*][*][*]Not Run"
        "File '.+/${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_BreakWithSubpackagesALib/BUILD/Testing/Temporary/LastTestsFailed_.+.log' exists so there were non-passing tests"
        "Final set of packages that had any failures: 'ALL_PACKAGES'"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL FAILED"
      ALWAYS_FAIL_ON_ZERO_RETURN
    TEST_3
      MESSAGE "Make sure the failedPackages.txt file has the right contects!"
      CMND cat
      ARGS
        BUILD/failedPackages.txt
      PASS_REGULAR_EXPRESSION_ALL
        "ALL_PACKAGES"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    )
    # NOTE: The above test ensures that the library build failure of an
    # upstream package is detected in that package and in all all downstream
    # packages.  For CI testing it is fine if downstream package enables are
    # triggered.  Also note that the contents of the failedPackages.txt file
    # is checked to make sure that it is getting written correctly.  That is
    # important for CI servers.


  ##############################################################################


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_ST_PackagesSubset_BreakWithSubpackagesALib
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
    TEST_0
      MESSAGE "Copy the project dir so I can break it!"
      CMND cp
      ARGS -r ${TribitsExProj_DIR} .
    TEST_1
      MESSAGE "Break the library build WithSubpackageA"
      CMND ${CMAKE_CURRENT_SOURCE_DIR}/append_file_with_line.sh
      ARGS TribitsExampleProject/packages/with_subpackages/a/A.cpp
        "Build of A.cpp lib is broken!"
    TEST_2
      MESSAGE "Run ctest driver which should show a failed library build for just the MixedLang package!"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        TribitsExProj_PACKAGES=SimpleCxx,WrapExternal
        CTEST_EXPLICITLY_ENABLE_IMPLICITLY_ENABLED_PACKAGES=FALSE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_FLAGS=-k
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_PackagesSubset_BreakWithSubpackagesALib
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackages WrapExternal 5"
        "Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx WrapExternal 2"
        "Configure PASSED"
	"Build output: BUILD_ALL_NUM_ERRORS='[1-2]',BUILD_ALL_RETURN_VAL='[0-2]'"
	"Build FAILED"
        "SimpleCxx_HelloWorldTests [.]+ +Passed"
        "SimpleCxx_HelloWorldProg [.]+ +Passed"
        "WrapExternal_run_external_func [.]+[*][*][*]Not Run"
        "File '.+/${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_PackagesSubset_BreakWithSubpackagesALib/BUILD/Testing/Temporary/LastTestsFailed_.+.log' exists so there were non-passing tests"
        "Final set of packages that had any failures: 'SimpleCxx[;]WrapExternal'"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL FAILED"
      ALWAYS_FAIL_ON_ZERO_RETURN
    )
    # NOTE: The above test ensures that the library build failure of an
    # upstream package is detected as a break in all downstream packages even
    # though that upstream package is not directly shown on CDash.


  ##############################################################################


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_BreakSimpleCxxTestBuild
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
    TEST_0
      MESSAGE "Copy the project dir so I can break it!"
      CMND cp
      ARGS -r ${TribitsExProj_DIR} .
    TEST_1
      MESSAGE "Break the build of a test only (no lib build failure)"
      CMND ${CMAKE_CURRENT_SOURCE_DIR}/append_file_with_line.sh
      ARGS TribitsExampleProject/packages/simple_cxx/test/SimpleCxx_HelloWorld_Tests.cpp
        "Build of SimpleCxx_HelloWorld_Tests.cpp lib is broken!"
    TEST_2
      MESSAGE "Run ctest driver which should show build failure and failing test"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_FLAGS=-k
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_BreakSimpleCxxTestBuild
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx MixedLang WithSubpackages WrapExternal 4"
        "Configure PASSED"
	"Build output: BUILD_ALL_NUM_ERRORS='[1-2]',BUILD_ALL_RETURN_VAL='[0-2]'"
	"Build FAILED"
        "SimpleCxx_HelloWorldTests [.]+[*][*][*]Not Run"
        "SimpleCxx_HelloWorldProg [.]+ +Passed"
        "WithSubpackagesA_test_of_a [.]+ +Passed"
        "WithSubpackagesB_test_of_b [.]+ +Passed"
        "WithSubpackagesC_test_of_c_util [.]+ +Passed"
        "WithSubpackagesC_test_of_c_b_mixed_lang [.]+ +Passed"
        "WrapExternal_run_external_func [.]+ +Passed"
        "Final set of packages that had any failures: 'ALL_PACKAGES'"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL FAILED"
      ALWAYS_FAIL_ON_ZERO_RETURN
    )
    # NOTE: The above test ensures that a build failure in the test of an
    # upstream package will be caught but will not impact break downstream
    # packages and tests.


  ##############################################################################


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_BreakSimpleCxxTestRun
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
    TEST_0
      MESSAGE "Copy the project dir so I can break it!"
      CMND cp
      ARGS -r ${TribitsExProj_DIR} .
    TEST_1
      MESSAGE "Break a test only (no build failure)"
      CMND ${CMAKE_CURRENT_SOURCE_DIR}/replace_string_in_file.sh
      ARGS TribitsExampleProject/packages/simple_cxx/test/CMakeLists.txt
        "Hello World" "Hellos World"
    TEST_2
      MESSAGE "Run ctest driver which should show a failed library build for just the MixedLang package!"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_FLAGS=-k
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_BreakSimpleCxxTestRun
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx MixedLang WithSubpackages WrapExternal 4"
        "Configure PASSED"
	"Build PASSED"
        "SimpleCxx_HelloWorldTests [.]+ +Passed"
        "SimpleCxx_HelloWorldProg [.]+[*][*][*]Failed"
        "WithSubpackagesA_test_of_a [.]+ +Passed"
        "WithSubpackagesB_test_of_b [.]+ +Passed"
        "WithSubpackagesC_test_of_c_util [.]+ +Passed"
        "WithSubpackagesC_test_of_c_b_mixed_lang [.]+ +Passed"
        "WrapExternal_run_external_func [.]+ +Passed"
        "Final set of packages that had any failures: 'SimpleCxx'"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL FAILED"
      ALWAYS_FAIL_ON_ZERO_RETURN
    )
    # NOTE: The above test ensures that a runt failure in the test of an
    # upstream package will be caught but will not impact break downstream
    # packages and tests.


  ##############################################################################


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_COVERAGE
    OVERALL_WORKING_DIRECTORY TEST_NAME
    EXCLUDE_IF_NOT_TRUE  ${PACKAGE_NAME}_CTEST_DRIVER_COVERAGE_TESTS
      ${PROJECT_NAME}_ENABLE_Fortran
    OVERALL_NUM_MPI_PROCS 1
    TEST_0
      MESSAGE "Symlink TribitsExampleProject so no need to clone (which would not work)."
      CMND ln
      ARGS -s ${TribitsExProj_DIR} .
    TEST_1
      MESSAGE "Run ctest driver testing"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        CTEST_DO_COVERAGE_TESTING=TRUE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_COVERAGE
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "CTEST_DO_COVERAGE_TESTING='TRUE'"
	"Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx MixedLang WithSubpackages WrapExternal 4"
	"Configure, build, test and submit results all-at-once for all enabled packages"
	"Gathering coverage results [.][.][.]"
	"Performing coverage"   # printed by ctest_coverage()
	"Percentage Coverage:"  # printed by ctest_coverage()
	"Done with the all-at-once configure, build, test, and submit of TribitsExProj packages"
	"TRIBITS_CTEST_DRIVER_ERROR_QUEUE is empty. All is well."
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    )
    # NOTE: The above test is the only coverage test for AAO mode!


  ##############################################################################x


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_MEMORY
    OVERALL_WORKING_DIRECTORY TEST_NAME
    EXCLUDE_IF_NOT_TRUE  ${PACKAGE_NAME}_CTEST_DRIVER_MEMORY_TESTS
      ${PROJECT_NAME}_ENABLE_Fortran
    OVERALL_NUM_MPI_PROCS 1
    TEST_0
      MESSAGE "Symlink TribitsExampleProject so no need to clone (which would not work)."
      CMND ln
      ARGS -s ${TribitsExProj_DIR} .
    TEST_1
      MESSAGE "Run ctest driver testing"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        CTEST_DO_MEMORY_TESTING=TRUE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_MEMORY
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "CTEST_DO_MEMORY_TESTING='TRUE'"
	"Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx MixedLang WithSubpackages WrapExternal 4"
	"Configure, build, test and submit results all-at-once for all enabled packages"
	"Running memory tests [.][.][.]"
	"Memory check project"  # Printed by ctest_memcheck()
        "MemCheck .+: SimpleCxx_HelloWorldTests [.]+ +Passed"
        "MemCheck .+: MixedLang_RayTracerTests [.]+ +Passed"
        "MemCheck .+: WithSubpackagesA_test_of_a [.]+ +Passed"
        "MemCheck .+: WrapExternal_run_external_func [.]+ +Passed"
	"Done with the all-at-once configure, build, test, and submit of TribitsExProj packages"
	"TRIBITS_CTEST_DRIVER_ERROR_QUEUE is empty. All is well."
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    )
    # NOTE: The above test is the only memory test for AAO mode!


  ##############################################################################x


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_Dashboard_ST_ALL_PASS
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
    TEST_0 CMND ${CMAKE_COMMAND}
      MESSAGE "Do the initial configure"
      ARGS
        ${TribitsExampleProject_AAO_COMMON_CONFIG_ARGS}
        -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
        -DTribitsExProj_ENABLE_Fortran=ON
        -DTribitsExProj_ENABLE_ALL_PACKAGES=ON
        -DTribitsExProj_ENABLE_TESTS=ON
        -DTribitsExProj_ENABLE_SECONDARY_TESTED_CODE=ON
        ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject
      PASS_REGULAR_EXPRESSION_ALL
        "Enabling all required .and optional since TribitsExProj_ENABLE_ALL_OPTIONAL_PACKAGES=ON. upstream SE packages for current set of enabled packages .TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=ON."
        "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackagesB WithSubpackagesC WithSubpackages WrapExternal 7"
        "Finished configuring TribitsExProj"
        "Generating done"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    TEST_1
      MESSAGE "Run the dashboard target"
      CMND env
      ARGS
        CTEST_BUILD_NAME=${PROJECT_NAME}_CTestDriver_AAO${AAO_POSTFIX}_Dashboard_ST_ALL_PASS
        CTEST_DASHBOARD_ROOT=
        make dashboard
      PASS_REGULAR_EXPRESSION_ALL
        "Running all-at-once experimental dashboard"
	"Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx MixedLang WithSubpackages WrapExternal 4"
	"Configure, build, test and submit results all-at-once for all enabled packages"
        "CTEST_LABELS_FOR_SUBPROJECTS='SimpleCxx[;]MixedLang[;]WithSubpackages[;]WrapExternal'"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    TEST_2
      MESSAGE "Verify that the right set of packages are set"
      CMND ${CMAKE_COMMAND} ARGS .
      PASS_REGULAR_EXPRESSION_ALL
        "Enabling all required .and optional since TribitsExProj_ENABLE_ALL_OPTIONAL_PACKAGES=ON. upstream SE packages for current set of enabled packages .TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=ON."
        "Final set of enabled packages:  SimpleCxx MixedLang WithSubpackages WrapExternal 4"
        "Generating done"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    )


  ##############################################################################x


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_Dashboard_ST_SimpleCxx_PASS
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
    TEST_0 CMND ${CMAKE_COMMAND}
      MESSAGE "Do the initial configure"
      ARGS
        ${TribitsExampleProject_AAO_COMMON_CONFIG_ARGS}
        -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
        -DTribitsExProj_ENABLE_Fortran=ON
        -DTribitsExProj_ENABLE_SimpleCxx=ON
        -DTribitsExProj_ENABLE_TESTS=ON
        -DTribitsExProj_ENABLE_SECONDARY_TESTED_CODE=ON
        ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject
      PASS_REGULAR_EXPRESSION_ALL
        "Setting SimpleCxx_ENABLE_TESTS=ON"
        "Final set of enabled SE packages:  SimpleCxx 1"
        "Processing enabled package: SimpleCxx [(]Libs, Tests, Examples[)]"
        "Generating done"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    TEST_1
      MESSAGE "Run the dashboard target"
      CMND env
      ARGS
        CTEST_BUILD_NAME=${PROJECT_NAME}_CTestDriver_AAO${AAO_POSTFIX}_Dashboard_ST_SimpleCxx_PASS
        CTEST_DASHBOARD_ROOT=
        make dashboard
      PASS_REGULAR_EXPRESSION_ALL
	"Configure, build, test and submit results all-at-once for all enabled packages"
        "Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx 1"
        "CTEST_LABELS_FOR_SUBPROJECTS='SimpleCxx'"
        "TRIBITS_CTEST_DRIVER_ERROR_QUEUE is empty. All is well."
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    TEST_2
      MESSAGE "Verify that the right set of packages are set"
      CMND ${CMAKE_COMMAND} ARGS .
      PASS_REGULAR_EXPRESSION_ALL
        "Final set of enabled SE packages:  SimpleCxx 1"
        "Processing enabled package: SimpleCxx [(]Libs, Tests, Examples[)]"
        "Generating done"
        "Generating done"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    )
    # The above test should result in a single row on CDash for the build.
    # This local checking here can't determine that but it uploads the data on
    # CDash where it can be inspected.


  ##############################################################################


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_Dashboard_ST_PackagesSubset_PASS
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
    TEST_0 CMND ${CMAKE_COMMAND}
      MESSAGE "Do the initial configure"
      ARGS
        ${TribitsExampleProject_AAO_COMMON_CONFIG_ARGS}
        -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
        -DTribitsExProj_ENABLE_Fortran=ON
        -DTribitsExProj_ENABLE_SimpleCxx=ON
        -DTribitsExProj_ENABLE_WrapExternal=ON
        -DTribitsExProj_ENABLE_TESTS=ON
        -DTribitsExProj_ENABLE_SECONDARY_TESTED_CODE=ON
        ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject
      PASS_REGULAR_EXPRESSION_ALL
        "Setting SimpleCxx_ENABLE_TESTS=ON"
        "Setting WrapExternal_ENABLE_TESTS=ON"
        "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackages WrapExternal 5"
        "Processing enabled package: SimpleCxx [(]Libs, Tests, Examples[)]"
        "Processing enabled package: MixedLang [(]Libs[)]"
        "Processing enabled package: WithSubpackages [(]A[)]"
        "Processing enabled package: WrapExternal [(]Libs, Tests, Examples[)]"
        "Generating done"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    TEST_1
      MESSAGE "Run the dashboard target"
      CMND env
      ARGS
        CTEST_BUILD_NAME=${PROJECT_NAME}_CTestDriver_AAO${AAO_POSTFIX}_Dashboard_ST_PackagesSubset_PASS
        CTEST_DASHBOARD_ROOT=
        make dashboard
      PASS_REGULAR_EXPRESSION_ALL
	"Configure, build, test and submit results all-at-once for all enabled packages"
        "Final set of packages to be explicitly processed by CTest/CDash:  SimpleCxx WrapExternal 2"
        "CTEST_LABELS_FOR_SUBPROJECTS='SimpleCxx[;]WrapExternal'"
        "TRIBITS_CTEST_DRIVER_ERROR_QUEUE is empty. All is well."
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    TEST_2
      MESSAGE "Verify that the right set of packages are set"
      CMND ${CMAKE_COMMAND} ARGS .
      PASS_REGULAR_EXPRESSION_ALL
        "Final set of enabled SE packages:  SimpleCxx MixedLang WithSubpackagesA WithSubpackages WrapExternal 5"
        "Processing enabled package: SimpleCxx [(]Libs, Tests, Examples[)]"
        "Processing enabled package: MixedLang [(]Libs[)]"
        "Processing enabled package: WithSubpackages [(]A[)]"
        "Processing enabled package: WrapExternal [(]Libs, Tests, Examples[)]"
        "Generating done"
        "Generating done"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    )


  ##############################################################################x


  #
  # Test all-at-once multi-phase invocation
  #


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_PASS_CALLS_2

    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran

    TEST_0
      MESSAGE "Symlink TribitsExampleProject so no need to clone (which would not work)."
      CMND ln
      ARGS -s ${TribitsExProj_DIR} .

    TEST_1
      MESSAGE "Do start, configure, and build (no testing)"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        CTEST_DO_TEST=OFF
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_PASS_CALLS_2
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "-- CTEST_DO_NEW_START='TRUE'"
	"-- ENV_CTEST_DO_UPDATES='OFF'"
	"-- CTEST_DO_UPDATES='OFF'"
	"-- CTEST_DO_CONFIGURE='TRUE'"
	"-- CTEST_DO_BUILD='TRUE'"
	"-- ENV_CTEST_DO_TEST='OFF'"
	"-- CTEST_DO_TEST='OFF'"
	"-- CTEST_DO_COVERAGE_TESTING='OFF'"
	"-- CTEST_DO_MEMORY_TESTING='FALSE'"
        "Start up a new dashboard calling ctest_start[(][.][.][.][)]"
	"Calling ctest_start[(]Continuous[)]"
	"Run dashboard with model Continuous"
	"Configure PASSED"
	"Building all targets"
	"Build PASSED"
	"Skipping tests because CTEST_DO_TEST='OFF'"
	"Skipping converage tests because CTEST_DO_COVERAGE_TESTING='OFF'"
	"Skipping memory testing because CTEST_DO_MEMORY_TESTING='FALSE'"
	"TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_2
      MESSAGE "Do only testing"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        CTEST_DO_NEW_START=OFF
        CTEST_DO_CONFIGURE=OFF
        CTEST_DO_BUILD=OFF
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_PASS_CALLS_2
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "-- ENV_CTEST_DO_NEW_START='OFF'"
        "-- CTEST_DO_NEW_START='OFF'"
	"-- ENV_CTEST_DO_UPDATES='OFF'"
	"-- CTEST_DO_UPDATES='OFF'"
	"-- ENV_CTEST_DO_CONFIGURE='OFF'"
	"-- CTEST_DO_CONFIGURE='OFF'"
	"-- ENV_CTEST_DO_BUILD='OFF'"
	"-- CTEST_DO_BUILD='OFF'"
	"-- CTEST_DO_TEST='TRUE'"
	"-- CTEST_DO_COVERAGE_TESTING='OFF'"
	"-- CTEST_DO_MEMORY_TESTING='FALSE'"
        "Use previous dashboard calling ctest_start[(][.][.][.] APPEND[)] due to CTEST_DO_NEW_START='OFF'"
        "Calling ctest_start[(]Continuous[;]APPEND[)]"
        "Run dashboard with model Continuous"
	"Skipping configure due to CTEST_DO_CONFIGURE='OFF'"
	"Skipping build due to CTEST_DO_BUILD='OFF'"
        "-- CTEST_CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE='54321'"
        "-- CTEST_CUSTOM_MAXIMUM_FAILED_TEST_OUTPUT_SIZE='123456'"
	"File '' does NOT exist so all tests passed"
	"Skipping converage tests because CTEST_DO_COVERAGE_TESTING='OFF'"
	"Skipping memory testing because CTEST_DO_MEMORY_TESTING='FALSE'"
	"TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    )
  # NOTE: The above test does the build in the first ctest -S invocation and
  # the tests in the second ctest -S invocation.


  ##############################################################################x


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_PASS_CALLS_2_update_fail

    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1

    TEST_0
      MESSAGE "Symlink TribitsExampleProject so no need to clone (which would not work)."
      CMND ln
      ARGS -s ${TribitsExProj_DIR} .

    TEST_1
      MESSAGE "Do start, update, configure, and build (no testing) (update fails)"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        CTEST_DO_UPDATES=ON
        CTEST_DO_TEST=OFF
        CTEST_UPDATE_UNIT_TESTING_MODE=ON
        CTEST_UPDATE_RETURN_VAL=-1
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_PASS_CALLS_2_update_fail
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "Update FAILED[!]"
        "The VC update failed so submitting update so don't perform"
        "Update failed so skipping any further actions[!]"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL FAILED"
      ALWAYS_FAIL_ON_ZERO_RETURN

    TEST_2
      MESSAGE "Do only testing (which should be skipped because update failed)"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        CTEST_DO_NEW_START=OFF
        CTEST_DO_CONFIGURE=OFF
        CTEST_DO_BUILD=OFF
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_PASS_CALLS_2_update_fail
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "Skipping configure due to CTEST_DO_CONFIGURE='OFF'[!]"
        "HOWEVER: A configure was previously attempted but did not pass so consider configure FAILED[!]"
        "Skipping build due to CTEST_DO_BUILD='OFF'[!]"
        "Skipping tests because configure failed[!]"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL FAILED"
      ALWAYS_FAIL_ON_ZERO_RETURN

    )


  ##############################################################################x


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_PASS_CALLS_2_config_fail

    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1

    TEST_0
      MESSAGE "Copy the project dir so I can break it!"
      CMND cp
      ARGS -r ${TribitsExProj_DIR} .

    TEST_1
      MESSAGE "Break the outer configure"
      CMND ${CMAKE_CURRENT_SOURCE_DIR}/append_file_with_line.sh
      ARGS TribitsExampleProject/CMakeLists.txt "The outer configure is broken!"

    TEST_2
      MESSAGE "Do start, configure, and build (no testing) (config fails)"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        CTEST_DO_TEST=OFF
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_PASS_CALLS_2_config_fail
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "Configure FAILED!"
        "Skipping build because configure failed[!]"
        "Skipping tests because CTEST_DO_TEST='OFF'[!]"
        "Final set of packages that had any failures: 'ALL_PACKAGES'"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL FAILED"
      ALWAYS_FAIL_ON_ZERO_RETURN

    TEST_3
      MESSAGE "Do only testing (which should be skipped because config failed)"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        CTEST_DO_NEW_START=OFF
        CTEST_DO_CONFIGURE=OFF
        CTEST_DO_BUILD=OFF
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_PASS_CALLS_2_config_fail
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "Skipping configure due to CTEST_DO_CONFIGURE='OFF'[!]"
        "HOWEVER: A configure was previously attempted but did not pass so consider configure FAILED[!]"
        "Skipping build due to CTEST_DO_BUILD='OFF'[!]"
        "Skipping tests because configure failed[!]"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL FAILED"
      ALWAYS_FAIL_ON_ZERO_RETURN

    )


  ##############################################################################x


  tribits_add_advanced_test( CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_PASS_CALLS_4

    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran

    TEST_0
      MESSAGE "Symlink TribitsExampleProject so no need to clone (which would not work)."
      CMND ln
      ARGS -s ${TribitsExProj_DIR} .

    TEST_1
      MESSAGE "Do start only"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        CTEST_DO_CONFIGURE=OFF
        CTEST_DO_BUILD=OFF
        CTEST_DO_TEST=OFF
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_PASS_CALLS_4
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "-- CTEST_DO_NEW_START='TRUE'"
	"-- CTEST_DO_CONFIGURE='OFF'"
	"-- CTEST_DO_BUILD='OFF'"
	"-- CTEST_DO_TEST='OFF'"
        "Start up a new dashboard calling ctest_start[(][.][.][.][)]"
	"Calling ctest_start[(]Continuous[)]"
	"Run dashboard with model Continuous"
	"Skipping processing anything else since not requested to configure, build, test, or run memory tests"
	"TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_2
      MESSAGE "Do configure only"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        CTEST_DO_NEW_START=OFF
        CTEST_DO_CONFIGURE=ON
        CTEST_DO_BUILD=OFF
        CTEST_DO_TEST=OFF
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_PASS_CALLS_4
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "-- CTEST_DO_NEW_START='OFF'"
	"-- CTEST_DO_CONFIGURE='ON'"
	"-- CTEST_DO_BUILD='OFF'"
	"-- CTEST_DO_TEST='OFF'"
        "Calling ctest_start[(]Continuous[;]APPEND[)]"
	"Configure PASSED"
	"Skipping build due to CTEST_DO_BUILD='OFF'"
	"Skipping tests because CTEST_DO_TEST='OFF'"
	"TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_3
      MESSAGE "Do build only"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        CTEST_DO_NEW_START=OFF
        CTEST_DO_CONFIGURE=OFF
        CTEST_DO_BUILD=ON
        CTEST_DO_TEST=OFF
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_PASS_CALLS_4
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "-- CTEST_DO_NEW_START='OFF'"
	"-- CTEST_DO_CONFIGURE='OFF'"
	"-- CTEST_DO_BUILD='ON'"
	"-- CTEST_DO_TEST='OFF'"
        "Calling ctest_start[(]Continuous[;]APPEND[)]"
	"Skipping configure due to CTEST_DO_CONFIGURE='OFF'"
	"Building all targets"
	"Build PASSED"
	"Skipping tests because CTEST_DO_TEST='OFF'"
	"TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_4
      MESSAGE "Do test only"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        CTEST_DO_NEW_START=OFF
        CTEST_DO_CONFIGURE=OFF
        CTEST_DO_BUILD=OFF
        CTEST_DO_TEST=ON
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${PACKAGE_NAME}_CTestDriver_AAO${AAO_POSTFIX}_ST_ALL_PASS_CALLS_4
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "-- CTEST_DO_NEW_START='OFF'"
	"-- CTEST_DO_CONFIGURE='OFF'"
	"-- CTEST_DO_BUILD='OFF'"
	"-- CTEST_DO_TEST='ON'"
        "Calling ctest_start[(]Continuous[;]APPEND[)]"
	"Skipping configure due to CTEST_DO_CONFIGURE='OFF'"
	"Skipping build due to CTEST_DO_BUILD='OFF'"
	"File '' does NOT exist so all tests passed"
	"TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    )
  # NOTE: The above test does the build in the first ctest -S invocation and
  # the tests in the second ctest -S invocation.


  ##############################################################################x


  #
  # Test CTEST_DO_INSTALL
  #

  set(TEST_NAME_BODY CTestDriver_AAO${AAO_POSTFIX}_CTEST_DO_INSTALL_PASS)
  set(TEST_NAME ${PACKAGE_NAME}_${TEST_NAME_BODY})
  tribits_add_advanced_test( ${TEST_NAME_BODY}
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
    TEST_0
      MESSAGE "Symlink TribitsExampleProject so no need to clone (which would not work)."
      CMND ln
      ARGS -s ${TribitsExProj_DIR} .
    TEST_1
      MESSAGE "Run ctest driver testing everything but no submit to CDash."
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
	TribitsExProj_EXTRA_CONFIGURE_OPTIONS=-DTribitsExProj_ENABLE_INSTALL_CMAKE_CONFIG_FILES=ON
        TribitsExProj_PACKAGES=SimpleCxx,WithSubpackages
        TribitsExProj_CMAKE_INSTALL_PREFIX=install
        CTEST_DO_INSTALL=ON
        CTEST_EXPLICITLY_ENABLE_IMPLICITLY_ENABLED_PACKAGES=FALSE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${TEST_NAME}
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "ENV_CTEST_DO_INSTALL='ON'"
        "CTEST_DO_INSTALL='ON'"
        "Final set of enabled packages:  SimpleCxx MixedLang WithSubpackages 3"
        "Installing [(]i.e. building target 'install_package_by_package'[)] [.][.][.]"
        "Build install output: BUILD_INSTALL_NUM_ERRORS='0',BUILD_INSTALL_RETURN_VAL='0'"
	"Install PASSED!"
        "File '' does NOT exist so all tests passed!"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    TEST_2
      MESSAGE "Run the installed simplecxx-helloworld program."
      CMND ./BUILD/install/bin/simplecxx-helloworld
      PASS_REGULAR_EXPRESSION_ALL
        "Hello World"
        "Debug is enabled!"
        "Sqr[(]3[)] = 9"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    TEST_3
      MESSAGE "look for some installed header files"
      CMND ls ARGS BUILD/install/include
      PASS_REGULAR_EXPRESSION_ALL
        "A.hpp"
        "B.hpp"
        "MixedLang.hpp"
        "SimpleCxx_HelloWorld.hpp"
      ALWAYS_FAIL_ON_NONZERO_RETURN
    )


  ##############################################################################x


  set(TEST_NAME_BODY
    CTestDriver_AAO${AAO_POSTFIX}_CTEST_DO_INSTALL_FAIL_bad_intall_filename)
  set(TEST_NAME ${PACKAGE_NAME}_${TEST_NAME_BODY})
  set(TEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}")
  tribits_add_advanced_test( ${TEST_NAME_BODY}
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran

    TEST_0
      MESSAGE "Copy TribitsExampleProject in so we can break it the install."
      CMND cp
      ARGS -r ${TribitsExProj_DIR} .

    TEST_1
      MESSAGE "Add the install of a file that does not exist!"
      CMND ${CMAKE_CURRENT_SOURCE_DIR}/append_file_with_line.sh
      ARGS
        ${TEST_DIR}/TribitsExampleProject/packages/mixed_lang/src/CMakeLists.txt
        "\ninstall(FILES does_not_exist.txt DESTINATION dummy_intall_dir COMPONENT SimpleCxx)\n"

    TEST_2
      MESSAGE "Run ctest -S driver which includes the install which should fail."
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
	TribitsExProj_EXTRA_CONFIGURE_OPTIONS=-DTribitsExProj_ENABLE_INSTALL_CMAKE_CONFIG_FILES=ON
        TribitsExProj_PACKAGES=SimpleCxx,MixedLang,WithSubpackages
        TribitsExProj_CMAKE_INSTALL_PREFIX=install
        CTEST_BUILD_FLAGS="-j2 -k"  # Build everything that will build!
        CTEST_DO_INSTALL=ON
        CTEST_EXPLICITLY_ENABLE_IMPLICITLY_ENABLED_PACKAGES=FALSE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${TEST_NAME}
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "ENV_CTEST_DO_INSTALL='ON'"
        "CTEST_DO_INSTALL='ON'"
        "Final set of enabled packages:  SimpleCxx MixedLang WithSubpackages 3"
        "CONFIGURE_OPTIONS = '.*[;]-DCMAKE_SKIP_INSTALL_ALL_DEPENDENCY=ON[;].*[;]-DCMAKE_INSTALL_PREFIX=install[;]-DTribitsExProj_ENABLE_INSTALL_CMAKE_CONFIG_FILES=ON'"
        "Build PASSED!"
        "Installing [(]i.e. building target 'install_package_by_package'[)] [.][.][.]"
        "Build install output: BUILD_INSTALL_NUM_ERRORS='(1|2)',BUILD_INSTALL_RETURN_VAL='2'"
	"Install FAILED!"
        "File '' does NOT exist so all tests passed!"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL FAILED"
      ALWAYS_FAIL_ON_ZERO_RETURN

    TEST_3
      MESSAGE "Check the contents of the install dir to see if anything installed"
      CMND ls ARGS -R BUILD/install
    PASS_REGULAR_EXPRESSION_ALL
      "simplecxx-helloworld"      # /bin
      "SimpleCxx_HelloWorld.hpp"  # /include/
      "A.hpp"
      "B.hpp"
      "MixedLang.hpp"
      "Ray_Tracer.hh"
      "TribitsExProj_version.h"
      "BUILD/install/include/wsp_c:"
      "C.hpp"
      "libsimplecxx.so"           # /lib/
      "libmixedlang.so"
      "libpws_a.so"
      "libpws_b.so"
      "libpws_c.so"
    ALWAYS_FAIL_ON_NONZERO_RETURN
    # NOTE: Above proves the 'install_package_by_package' target is working
    # and just calling the 'install' target because the 'install' target
    # aborts in the SimpleCxx install while the 'install_package_by_package'
    # will call install in all of the packages.

  TEST_4
    MESSAGE "Create and configure a dummy project that calls"
      " find_package(TribitsExProj) from the install tree and"
      " request packages that was not even enabled" 
    CMND ${CMAKE_COMMAND}
    ARGS
      -DDUMMY_PROJECT_NAME=DummyProject
      -DDUMMY_PROJECT_DIR=dummy_client_of_TribitsExProj
      -DEXPORT_VAR_PREFIX=TribitsExProj
      -DFIND_PACKAGE_NAME=TribitsExProj
      -DFIND_PACKAGE_COMPONENTS=SimpleCxx,WithSubpackagesA,WrapExternal
      -DCMAKE_PREFIX_PATH=../BUILD/install
      -DCMAKE_COMMAND=${CMAKE_COMMAND}
      -P ${TriBITS_SOURCE_DIR}/test/core/ExamplesUnitTests/RunDummyPackageClientBulid.cmake
    PASS_REGULAR_EXPRESSION_ALL
      "DUMMY_PROJECT_NAME = 'DummyProject'"
      "DUMMY_PROJECT_DIR = 'dummy_client_of_TribitsExProj'"
      "Calling: find_package[(]TribitsExProj REQUIRED COMPONENTS SimpleCxx[;]WithSubpackagesA[;]WrapExternal OPTIONAL_COMPONENTS  [)]"
      "Found package configuration file:"
      "but it set TribitsExProj_FOUND to FALSE so package .TribitsExProj. is"
      "ERROR: Could not find component 'WrapExternal'"
      "-- Configuring incomplete, errors occurred"

    # ToDo: Add another dummy project for the case where components are
    # required and assure that TribitsExProj_FOUND = 'FALSE'.

    )
    # NOTE: That above test tests two primary things:
    #
    # * Install failures due to missing files will be caught and reported.
    #
    # * If an install failure occurs in one package, it will not impact
    #   installs in other packages due to the custom
    #   'install_package_by_package' target.


  ##############################################################################x


  set(TEST_NAME_BODY
    CTestDriver_AAO${AAO_POSTFIX}_CTEST_DO_INSTALL_FAIL_build_failed)
  set(TEST_NAME ${PACKAGE_NAME}_${TEST_NAME_BODY})
  set(TEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}")
  tribits_add_advanced_test( ${TEST_NAME_BODY}
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran

    TEST_0
      MESSAGE "Copy TribitsExampleProject in so we can break it the install."
      CMND cp
      ARGS -r ${TribitsExProj_DIR} .

    TEST_1
      MESSAGE "Add the install of a file that does not exist!"
      CMND ${CMAKE_CURRENT_SOURCE_DIR}/append_file_with_line.sh
      ARGS
        ${TEST_DIR}/TribitsExampleProject/packages/mixed_lang/src/CMakeLists.txt
        "\ninstall(FILES does_not_exist.txt DESTINATION dummy_intall_dir COMPONENT SimpleCxx)\n"

    TEST_2
      MESSAGE "Break the library build WithSubpackageC"
      CMND ${CMAKE_CURRENT_SOURCE_DIR}/append_file_with_line.sh
      ARGS TribitsExampleProject/packages/with_subpackages/c/C.cpp
        "Build of C.cpp lib is broken!"

    TEST_3
      MESSAGE "Run ctest -S driver"
      CMND env
      ARGS
        CTEST_DASHBOARD_ROOT=PWD
        ${AAO_COMMON_ENV_ARGS}
        TribitsExProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
	TribitsExProj_EXTRA_CONFIGURE_OPTIONS=-DTribitsExProj_ENABLE_INSTALL_CMAKE_CONFIG_FILES=ON
        TribitsExProj_PACKAGES=SimpleCxx,MixedLang,WithSubpackages
        TribitsExProj_CMAKE_INSTALL_PREFIX=install
        CTEST_BUILD_FLAGS="-j2 -k"  # Build everything that will build!
        CTEST_DO_INSTALL=ON
        CTEST_EXPLICITLY_ENABLE_IMPLICITLY_ENABLED_PACKAGES=FALSE
        ${CTEST_DROP_SITE_ENV_ARGS}
        CTEST_BUILD_NAME=${TEST_NAME}
        ${CTEST_S_SCRIPT_ARGS}
      PASS_REGULAR_EXPRESSION_ALL
        "Build FAILED!"
	"Install FAILED!"
        "78% tests passed, 2 tests failed out of 9"
	"WithSubpackagesC_test_of_c [(]Not Run[)]"
	"WithSubpackagesC_test_of_c_b_mixed_lang [(]Not Run[)]"
        "TRIBITS_CTEST_DRIVER: OVERALL: ALL FAILED"
      ALWAYS_FAIL_ON_ZERO_RETURN

    TEST_4
      MESSAGE "Check the contents of the install dir"
      CMND ls ARGS -R BUILD/install
    PASS_REGULAR_EXPRESSION_ALL
      "simplecxx-helloworld"      # /bin
      "SimpleCxx_HelloWorld.hpp"  # /include/
      "A.hpp"
      "B.hpp"
      "MixedLang.hpp"
      "Ray_Tracer.hh"
      "TribitsExProj_version.h"
      "libsimplecxx.so"           # /lib/
      "libmixedlang.so"
      "libpws_a.so"
      "libpws_b.so"
    ALWAYS_FAIL_ON_NONZERO_RETURN
    # NOTE: Almost everything will install accept for the WithSubpackageC
    # stuff since that library did not build.

  TEST_5
    MESSAGE "Create and configure a dummy project that calls"
      " find_package(TribitsExProj) where components are optional" 
    CMND ${CMAKE_COMMAND}
    ARGS
      -DDUMMY_PROJECT_NAME=DummyProject
      -DDUMMY_PROJECT_DIR=dummy_client_of_TribitsExProj
      -DEXPORT_VAR_PREFIX=TribitsExProj
      -DFIND_PACKAGE_NAME=TribitsExProj
      -DFIND_PACKAGE_OPTIONAL_COMPONENTS=SimpleCxx,WithSubpackagesA,WithSubpackageC
      -DCMAKE_PREFIX_PATH=../BUILD/install
      -DCMAKE_COMMAND=${CMAKE_COMMAND}
      -P ${TriBITS_SOURCE_DIR}/test/core/ExamplesUnitTests/RunDummyPackageClientBulid.cmake
    PASS_REGULAR_EXPRESSION_ALL
      "DUMMY_PROJECT_NAME = 'DummyProject'"
      "DUMMY_PROJECT_DIR = 'dummy_client_of_TribitsExProj'"
      "Calling: find_package[(]TribitsExProj REQUIRED COMPONENTS  OPTIONAL_COMPONENTS SimpleCxx[;]WithSubpackagesA[;]WithSubpackageC [)]"
      "TribitsExProj_PACKAGE_LIST = 'WithSubpackages[;]WithSubpackagesC[;]WithSubpackagesB[;]WithSubpackagesA[;]MixedLang[;]SimpleCxx'"
      "TribitsExProj_SELECTED_PACKAGE_LIST = 'SimpleCxx[;]WithSubpackagesA'"
      "TribitsExProj_TPL_LIST = 'HeaderOnlyTpl'"
      "TribitsExProj_SimpleCxx_FOUND = 'TRUE'"
      "TribitsExProj_WithSubpackagesA_FOUND = 'TRUE'"
      "TribitsExProj_WithSubpackageC_FOUND = 'FALSE'"
      "TribitsExProj_FOUND = '1'"
      "TribitsExProj::all_selected_libs  INTERFACE_LINK_LIBRARIES: 'SimpleCxx::all_libs[;]WithSubpackagesA::all_libs'"
      "-- Configuring done"
      "-- Generating done"
    ALWAYS_FAIL_ON_NONZERO_RETURN

    # ToDo: Add another dummy project for the case where components are
    # required and assure that TribitsExProj_FOUND = 'FALSE'.

    )
    # NOTE: That above test tests two primary things:
    #
    # * Install failures due to missing files will be caught and reported.
    #
    # * If something fails to build, the install will still be attempted.
    #   This checks that -DCMAKE_SKIP_INSTALL_ALL_DEPENDENCY=ON is set
    #   correctly in the inner configure.
    #
    # * If an install failure occurs in one package, it will not impact
    #   installs in other packages due to the custom
    #   'install_package_by_package' target.
    #
    # * Even if one package breaks, one can still use the
    #   <Project>Config.cmake file if one does not request a broken package
    #   (or any package downstream from a broken package) in specifying
    #   COMPONENTS.
    #

 
endfunction()


generate_aao_tests()
# ToDo: Remove this function and just have the tests at the top level.  (NOTE:
# I don't want to do this yet as I don't want to create merge conflicts with
# another branch I have for TriBITSPub/TriBITS#63.)
