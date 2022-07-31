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


########################################################################
# Test RPATH handling
########################################################################


#
# Common configure arguments for all for the
# TribitsExampleProject_SimpleTpl_RPATH_XXX tests.
#

set(TEST_RPATH_COMMON_CONFIG_ARGS
  ${TribitsExampleProject_COMMON_CONFIG_ARGS}
  -DBUILD_SHARED_LIBS=ON
  -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
  -DTribitsExProj_ENABLE_Fortran=OFF
  -DTribitsExProj_ENABLE_SimpleCxx=ON
  -DTribitsExProj_ENABLE_TESTS=ON
  -DTPL_ENABLE_SimpleTpl=ON
  -DSimpleTpl_INCLUDE_DIRS=${SimpleTpl_install_SHARED_DIR}/install/include
  -DSimpleTpl_LIBRARY_DIRS=${SimpleTpl_install_SHARED_DIR}/install/lib
  )

set(LD_LIBRARY_PATH_ORIG $ENV{LD_LIBRARY_PATH})


#
# Test default TriBITS RPATH setting
#

set(RPATH_CURRENT_TEST_DIR
  ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_TribitsExampleProject_SimpleTpl_RPATH_default)

if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
  set(RPATH_GREP_STR "@rpath/libsimplecxx[.].*[.]dylib;@rpath/libsimpletpl[.]dylib")
elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
  set(RPATH_GREP_STR
    "R.*PATH *${RPATH_CURRENT_TEST_DIR}/install/lib:${SimpleTpl_install_SHARED_DIR}/install/lib")
  # NOTE: Above matches both RPATH and RUNPATH which are used on different
  # Linux systems.
endif()

tribits_add_advanced_test( TribitsExampleProject_SimpleTpl_RPATH_default
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  EXCLUDE_IF_NOT_TRUE RPATH_INSPECT_CMND

  TEST_0 CMND ${CMAKE_COMMAND}
    MESSAGE "Do default configure for auto RPATH to install dir"
    ARGS
      ${TEST_RPATH_COMMON_CONFIG_ARGS}
      -DCMAKE_INSTALL_PREFIX=${RPATH_CURRENT_TEST_DIR}/install
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "Processing enabled package: SimpleCxx .Libs, Tests, Examples."
      "Configuring done"
      "Generating done"
  # Above tests the standard install lib location

  TEST_1 CMND make ARGS ${CTEST_BUILD_FLAGS}
    MESSAGE "Build the default 'all' target using raw 'make'"
    PASS_REGULAR_EXPRESSION_ALL
      "Built target simplecxx"
      "Built target simplecxx-helloworld"
      "Built target SimpleCxx_HelloWorldTests"

  TEST_2 CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
    MESSAGE "Run all the tests with raw 'ctest'"
    PASS_REGULAR_EXPRESSION_ALL
      "SimpleCxx_HelloWorldTests${TEST_MPI_1_SUFFIX} .* Passed"
      "SimpleCxx_HelloWorldProg${TEST_MPI_1_SUFFIX} .* Passed"
      "100% tests passed, 0 tests failed out of 2"

  TEST_3 CMND make ARGS ${CTEST_BUILD_FLAGS} install
    PASS_REGULAR_EXPRESSION_ALL
      "-- Installing: .*/install/bin/simplecxx-helloworld"
      "-- Installing: .*/install/lib/libsimplecxx[.].*${SHARED_LIB_EXT}"

  TEST_4 CMND ${RPATH_INSPECT_CMND}
    ARGS  ${RPATH_INSPECT_ARG}  install/lib/libsimplecxx.${SHARED_LIB_EXT}
    PASS_REGULAR_EXPRESSION_ALL  "${RPATH_GREP_STR}"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_5
    MESSAGE "Run installed executable without having to set env"
    CMND ${RPATH_CURRENT_TEST_DIR}/install/bin/simplecxx-helloworld
    PASS_REGULAR_EXPRESSION_ALL
      "Hello World"
      "Cube.3. = 27"  # This comes from the SimpleTpl TPL

  ADDED_TEST_NAME_OUT RPATH_CURRENT_TEST_NAME_NAME
  )

if (RPATH_CURRENT_TEST_NAME_NAME)
  set_tests_properties(${RPATH_CURRENT_TEST_NAME_NAME}
    PROPERTIES DEPENDS ${SimpleTpl_install_SHARED_NAME} )
endif()


# NOTE: The above test does some more detailed checking for this first test
# case.  Future test cases will not do as much checking but only change what
# is changing.


#
# Test setting <project>_SET_INSTALL_RPATH=FALSE
#

set(RPATH_CURRENT_TEST_DIR
  ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_TribitsExampleProject_SimpleTpl_RPATH_no_SET_INSTALL_RPATH)

if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
  set(RPATH_GREP_STR "@rpath/libsimplecxx[.].*[.]dylib;@rpath/libsimpletpl[.]dylib")
elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
  set(RPATH_GREP_STR
    "R.*PATH *${SimpleTpl_install_SHARED_DIR}/install/lib")
endif()

tribits_add_advanced_test( TribitsExampleProject_SimpleTpl_RPATH_no_SET_INSTALL_RPATH
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  EXCLUDE_IF_NOT_TRUE RPATH_INSPECT_CMND

  TEST_0 CMND ${CMAKE_COMMAND}
    MESSAGE "Do configure with -DTribitsExProj_SET_INSTALL_RPATH=OFF"
    ARGS
      ${TEST_RPATH_COMMON_CONFIG_ARGS}
      -DCMAKE_INSTALL_PREFIX=${RPATH_CURRENT_TEST_DIR}/install2
      -DTribitsExProj_SET_INSTALL_RPATH=OFF
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject

  TEST_1 CMND make ARGS ${CTEST_BUILD_FLAGS} install

  TEST_2 CMND ${RPATH_INSPECT_CMND}
    ARGS  ${RPATH_INSPECT_ARG}  install2/lib/libsimplecxx.${SHARED_LIB_EXT}
    PASS_REGULAR_EXPRESSION_ALL  "${RPATH_GREP_STR}"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_3 CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
    MESSAGE "Run the executables from the build directory which shows that RPATH is set in build tree."
    PASS_REGULAR_EXPRESSION_ALL
      "100% tests passed, 0 tests failed out of 2"

  TEST_4
    MESSAGE "Run installed executable which should fail"
    CMND ${RPATH_CURRENT_TEST_DIR}/install/bin/simplecxx-helloworld
    WILL_FAIL

  ADDED_TEST_NAME_OUT RPATH_CURRENT_TEST_NAME_NAME
  )

if (RPATH_CURRENT_TEST_NAME_NAME)
  set_tests_properties(${RPATH_CURRENT_TEST_NAME_NAME}
    PROPERTIES DEPENDS ${SimpleTpl_install_SHARED_NAME} )
endif()


# Run the executable built and installed above setting LD_LIBRARY_PATH
tribits_add_test(
  simplecxx-helloworld  NOEXEPREFIX  NOEXESUFFIX
    DIRECTORY ${RPATH_CURRENT_TEST_DIR}/install2/bin
  NAME TribitsExampleProject_SimpleTpl_RPATH_no_SET_INSTALL_RPATH_run
  EXCLUDE_IF_NOT_TRUE IS_REAL_LINUX_SYSTEM
  ENVIRONMENT
    LD_LIBRARY_PATH=${RPATH_CURRENT_TEST_DIR}/install2/lib:${LD_LIBRARY_PATH_ORIG}
  ADDED_TESTS_NAMES_OUT TribitsExampleProject_SimpleTpl_RPATH_no_SET_INSTALL_RPATH_run_NAMES
  )
if (TribitsExampleProject_SimpleTpl_RPATH_no_SET_INSTALL_RPATH_run_NAMES
  AND RPATH_CURRENT_TEST_NAME_NAME
  )
  set_tests_properties(
    ${TribitsExampleProject_SimpleTpl_RPATH_no_SET_INSTALL_RPATH_run_NAMES}
    PROPERTIES DEPENDS ${RPATH_CURRENT_TEST_NAME_NAME} )
endif()


#
# Test setting CMAKE_INSTALL_RPATH="<path0>:<path1>:..."
#

set(RPATH_CURRENT_TEST_DIR
  ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_TribitsExampleProject_SimpleTpl_RPATH_CMAKE_INSTALL_RPATH)

if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
  set(RPATH_GREP_STR "@rpath/libsimplecxx[.].*[.]dylib;@rpath/libsimpletpl[.]dylib")
elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
  set(RPATH_GREP_STR
    "R.*PATH *${RPATH_CURRENT_TEST_DIR}/install2/nonstd_lib_location:${SimpleTpl_install_SHARED_DIR}/install/lib")
endif()

tribits_add_advanced_test( TribitsExampleProject_SimpleTpl_RPATH_CMAKE_INSTALL_RPATH
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  EXCLUDE_IF_NOT_TRUE RPATH_INSPECT_CMND

  TEST_0 CMND ${CMAKE_COMMAND}
    MESSAGE "Do configure with CMAKE_INSTALL_RPATH set to moved install dir install2/"
    ARGS
      ${TEST_RPATH_COMMON_CONFIG_ARGS}
      -DCMAKE_INSTALL_PREFIX=${RPATH_CURRENT_TEST_DIR}/install
      -DTribitsExProj_INSTALL_LIB_DIR:STRING=nonstd_lib_location
      -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=FALSE
      -DCMAKE_INSTALL_RPATH="${RPATH_CURRENT_TEST_DIR}/install2/nonstd_lib_location:${SimpleTpl_install_SHARED_DIR}/install/lib"
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject
  # Above tests with a non-standard lib location to see that TriBITS has the
  # right logic in this case.

  TEST_1 CMND make ARGS ${CTEST_BUILD_FLAGS} install
    PASS_REGULAR_EXPRESSION_ALL
      "-- Installing: .*/install/bin/simplecxx-helloworld"
      "-- Installing: .*/install/nonstd_lib_location/libsimplecxx[.].*${SHARED_LIB_EXT}"

  TEST_2 CMND ${RPATH_INSPECT_CMND}
    ARGS  ${RPATH_INSPECT_ARG}  install/nonstd_lib_location/libsimplecxx.${SHARED_LIB_EXT}
    PASS_REGULAR_EXPRESSION_ALL  "${RPATH_GREP_STR}"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_3 CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
    MESSAGE "Run the executables from the build directory which shows that RPATH is set in build tree."
    PASS_REGULAR_EXPRESSION_ALL
      "100% tests passed, 0 tests failed out of 2"

  TEST_4
    MESSAGE "Run installed executable which should fail"
    CMND ${RPATH_CURRENT_TEST_DIR}/install/bin/simplecxx-helloworld
    WILL_FAIL

  TEST_5
    MESSAGE "Move from install/ to install2/ which should match CMAKE_INSTALL_RPATH"
    CMND mv ARGS install/ install2/

  TEST_6
    MESSAGE "Run from the moved install2/ dir which should pass"
    CMND ${RPATH_CURRENT_TEST_DIR}/install2/bin/simplecxx-helloworld
    PASS_REGULAR_EXPRESSION_ALL
      "Hello World"
      "Cube.3. = 27"  # This comes from the SimpleTpl TPL

  ADDED_TEST_NAME_OUT RPATH_CURRENT_TEST_NAME_NAME
  )

if (RPATH_CURRENT_TEST_NAME_NAME)
  set_tests_properties(${RPATH_CURRENT_TEST_NAME_NAME}
    PROPERTIES DEPENDS ${SimpleTpl_install_SHARED_NAME} )
endif()


#
# Test setting -DCMAKE_SKIP_INSTALL_RPATH=TRUE
#

set(RPATH_CURRENT_TEST_DIR
  ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_TribitsExampleProject_SimpleTpl_RPATH_CMAKE_SKIP_INSTALL_RPATH)

if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
  set(RPATH_GREP_STR "@rpath/libsimplecxx[.].*[.]dylib;@rpath/libsimpletpl[.]dylib")
elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
  set(RPATH_GREP_STR "") # Can't look for RPATH at all
endif()

tribits_add_advanced_test( TribitsExampleProject_SimpleTpl_RPATH_CMAKE_SKIP_INSTALL_RPATH
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  EXCLUDE_IF_NOT_TRUE RPATH_INSPECT_CMND

  TEST_0 CMND ${CMAKE_COMMAND}
    MESSAGE "Do configure with -DTribitsExProj_SET_INSTALL_RPATH=OFF"
    ARGS
      ${TEST_RPATH_COMMON_CONFIG_ARGS}
      -DCMAKE_INSTALL_PREFIX=${RPATH_CURRENT_TEST_DIR}/install3
      -DCMAKE_SKIP_INSTALL_RPATH=TRUE
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject

  TEST_1 CMND make ARGS ${CTEST_BUILD_FLAGS} install
    MESSAGE "Install and grep to check for expected RPATH"
    PASS_REGULAR_EXPRESSION_ALL
      "-- Installing: .*/install3/bin/simplecxx-helloworld"
      "-- Installing: .*/install3/lib/libsimplecxx[.].*${SHARED_LIB_EXT}"

  TEST_2 CMND ${RPATH_INSPECT_CMND}
    ARGS  ${RPATH_INSPECT_ARG}  install3/lib/libsimplecxx.${SHARED_LIB_EXT}
    PASS_REGULAR_EXPRESSION_ALL  "${RPATH_GREP_STR}"
    FAIL_REGULAR_EXPRESSION  "RPATH.*${SimpleTpl_install_SHARED_DIR}/install/lib"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_3 CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
    MESSAGE "Run the executables from the build directory which shows that RPATH is set in build tree."
    PASS_REGULAR_EXPRESSION_ALL
      "100% tests passed, 0 tests failed out of 2"

  TEST_4
    MESSAGE "Run installed executable which should fail"
    CMND ${RPATH_CURRENT_TEST_DIR}/install3/bin/simplecxx-helloworld
    WILL_FAIL

  ADDED_TEST_NAME_OUT RPATH_CURRENT_TEST_NAME_NAME
  )

if (RPATH_CURRENT_TEST_NAME_NAME)
  set_tests_properties(${RPATH_CURRENT_TEST_NAME_NAME}
    PROPERTIES DEPENDS ${SimpleTpl_install_SHARED_NAME} )
endif()


# Run the executable built and installed above setting LD_LIBRARY_PATH
tribits_add_test(
  simplecxx-helloworld  NOEXEPREFIX  NOEXESUFFIX
    DIRECTORY ${RPATH_CURRENT_TEST_DIR}/install3/bin
  NAME TribitsExampleProject_SimpleTpl_RPATH_CMAKE_SKIP_INSTALL_RPATH_run
  EXCLUDE_IF_NOT_TRUE IS_REAL_LINUX_SYSTEM
  PASS_REGULAR_EXPRESSION
    "Hello World"
  ENVIRONMENT
    LD_LIBRARY_PATH=${RPATH_CURRENT_TEST_DIR}/install3/lib:${SimpleTpl_install_SHARED_DIR}/install/lib:${LD_LIBRARY_PATH_ORIG}
  ADDED_TESTS_NAMES_OUT TribitsExampleProject_SimpleTpl_RPATH_CMAKE_SKIP_INSTALL_RPATH_run_NAMES
  )
if (TribitsExampleProject_SimpleTpl_RPATH_CMAKE_SKIP_INSTALL_RPATH_run_NAMES
  AND RPATH_CURRENT_TEST_NAME_NAME
  )
  set_tests_properties(
    ${TribitsExampleProject_SimpleTpl_RPATH_CMAKE_SKIP_INSTALL_RPATH_run_NAMES}
    PROPERTIES DEPENDS ${RPATH_CURRENT_TEST_NAME_NAME} )
endif()


#
# Test cmake/ProjectCompilerPostConfig.cmake
#

tribits_add_advanced_test( TribitsExampleProject_ProjectCompilerPostConfig
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  XHOSTTYPE Darwin

  TEST_0
    MESSAGE "Copy TribitsExampleProject so that we can copy in cmake/ProjectCompilerPostConfig.cmake."
    CMND cp
    ARGS -r ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject .

  TEST_1
    MESSAGE "Copy in dummy cmake/ProjectCompilerPostConfig.cmake."
    CMND cp
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/DummyProjectCompilerPostConfig.cmake
      TribitsExampleProject/cmake/ProjectCompilerPostConfig.cmake

  TEST_2
    MESSAGE "Configure SimpleCxx and trace includes"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_Fortran=OFF
      -DTribitsExProj_ENABLE_DEBUG=OFF
      -DTribitsExProj_TRACE_FILE_PROCESSING=ON
      -DTribitsExProj_ENABLE_SimpleCxx=ON
      -DCMAKE_VERBOSE_MAKEFILE=ON
      TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "-- File Trace: PROJECT    INCLUDE    .*/cmake/ProjectCompilerPostConfig[.]cmake"
      "Configuring done"
      "Generating done"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_3
    MESSAGE "VERBOSE Build the simplecxx lib and verify that the new compiler flags is added "
    CMND make
    PASS_REGULAR_EXPRESSION_ALL
      "DDUMMY_DEFINE_JUST_TO_TEST_COMILER_POST_CONFIG"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  )
