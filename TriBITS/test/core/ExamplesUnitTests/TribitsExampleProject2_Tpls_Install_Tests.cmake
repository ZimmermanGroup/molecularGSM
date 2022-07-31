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
# TribitsExampleProject2 TPLs Install Tests
########################################################################


function(TribitsExampleProject2_Tpls_install_tests sharedOrStatic)

  if (sharedOrStatic STREQUAL "SHARED")
    set(buildSharedLibsArg
      -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE
      -DCMAKE_MACOSX_RPATH=TRUE)
  elseif (sharedOrStatic STREQUAL "STATIC")
    set(buildSharedLibsArg -DBUILD_SHARED_LIBS=OFF)
  else()
    message(FATAL_ERROR "Invalid value for sharedOrStatic='${sharedOrStatic}'!")
  endif()

  # A) Build and install Tpl1, Tpl2, Tpl3, Tpl4

  set(testNameBase TribitsExampleProject2_Tpls_install_${sharedOrStatic})
  set(testName ${PACKAGE_NAME}_${testNameBase})
  set(testDir ${CMAKE_CURRENT_BINARY_DIR}/${testName})

  tribits_add_advanced_test( ${testNameBase}
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1

    TEST_0
      MESSAGE "Copy source for Tpl1"
      CMND ${CMAKE_COMMAND}
      ARGS -E copy_directory ${${PROJECT_NAME}_TRIBITS_DIR}/examples/tpls/Tpl1 .
      WORKING_DIRECTORY Tpl1

    TEST_1
      MESSAGE "Configure Tpl1"
      WORKING_DIRECTORY build_tpl1
      CMND ${CMAKE_COMMAND}
      ARGS
        ${SERIAL_PASSTHROUGH_CONFIGURE_ARGS}
        ${buildSharedLibsArg}
        -DCMAKE_BUILD_TYPE=RelWithDepInfo
        -DCMAKE_INSTALL_PREFIX=${testDir}/install_tpl1
        -DCMAKE_INSTALL_INCLUDEDIR=include
        -DCMAKE_INSTALL_LIBDIR=lib
        ${testDir}/Tpl1
      PASS_REGULAR_EXPRESSION_ALL
        "Configuring done"
        "Generating done"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_2
      MESSAGE "Build and install Tpl1"
      WORKING_DIRECTORY build_tpl1
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND make ARGS ${CTEST_BUILD_FLAGS} install
      PASS_REGULAR_EXPRESSION_ALL
        "Built target tpl1"
        "Installing: ${testDir}/install_tpl1/lib/libtpl1[.]"
        "Installing: ${testDir}/install_tpl1/include/Tpl1.hpp"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_3
      MESSAGE "Delete source and build directory for Tpl1"
      CMND ${CMAKE_COMMAND} ARGS -E rm -rf Tpl1 build_tpl1

    TEST_4
      MESSAGE "Copy source for Tpl2"
      CMND ${CMAKE_COMMAND}
      ARGS -E copy_directory ${${PROJECT_NAME}_TRIBITS_DIR}/examples/tpls/Tpl2 .
      WORKING_DIRECTORY Tpl2

    TEST_5
      MESSAGE "Configure Tpl2"
      WORKING_DIRECTORY build_tpl2
      CMND ${CMAKE_COMMAND}
      ARGS
        ${SERIAL_PASSTHROUGH_CONFIGURE_ARGS}
        ${buildSharedLibsArg}
        -DCMAKE_PREFIX_PATH="${testDir}/install_tpl1"
        -DCMAKE_BUILD_TYPE=RelWithDepInfo
        -DCMAKE_INSTALL_PREFIX=${testDir}/install_tpl2
        -DCMAKE_INSTALL_INCLUDEDIR=include
        -DCMAKE_INSTALL_LIBDIR=lib
        ${testDir}/Tpl2
      PASS_REGULAR_EXPRESSION_ALL
        "Configuring done"
        "Generating done"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_6
      MESSAGE "Build and install Tpl2"
      WORKING_DIRECTORY build_tpl2
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND make ARGS ${CTEST_BUILD_FLAGS} install
      PASS_REGULAR_EXPRESSION_ALL
        "Built target tpl2"
        "Installing: ${testDir}/install_tpl2/lib/libtpl2a[.]"
        "Installing: ${testDir}/install_tpl2/include/Tpl2a.hpp"
        "Installing: ${testDir}/install_tpl2/lib/libtpl2b[.]"
        "Installing: ${testDir}/install_tpl2/include/Tpl2b.hpp"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_7
      MESSAGE "Delete source and build directory for Tpl2"
      CMND ${CMAKE_COMMAND} ARGS -E rm -rf Tpl2 build_tpl2

    TEST_8
      MESSAGE "Copy source for Tpl3"
      CMND ${CMAKE_COMMAND}
      ARGS -E copy_directory ${${PROJECT_NAME}_TRIBITS_DIR}/examples/tpls/Tpl3 .
      WORKING_DIRECTORY Tpl3

    TEST_9
      MESSAGE "Configure Tpl3"
      WORKING_DIRECTORY build_tpl3
      CMND ${CMAKE_COMMAND}
      ARGS
        ${SERIAL_PASSTHROUGH_CONFIGURE_ARGS}
        ${buildSharedLibsArg}
        -DCMAKE_PREFIX_PATH="${testDir}/install_tpl2"
        -DCMAKE_BUILD_TYPE=RelWithDepInfo
        -DCMAKE_INSTALL_PREFIX=${testDir}/install_tpl3
        -DCMAKE_INSTALL_INCLUDEDIR=include
        -DCMAKE_INSTALL_LIBDIR=lib
        ${testDir}/Tpl3
      PASS_REGULAR_EXPRESSION_ALL
        "Configuring done"
        "Generating done"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_10
      MESSAGE "Build and install Tpl3"
      WORKING_DIRECTORY build_tpl3
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND make ARGS ${CTEST_BUILD_FLAGS} install
      PASS_REGULAR_EXPRESSION_ALL
        "Built target tpl3"
        "Installing: ${testDir}/install_tpl3/lib/libtpl3[.]"
        "Installing: ${testDir}/install_tpl3/include/Tpl3.hpp"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_11
      MESSAGE "Delete source and build directory for Tpl3"
      CMND ${CMAKE_COMMAND} ARGS -E rm -rf Tpl3 build_tpl3

    TEST_12
      MESSAGE "Copy source for Tpl4"
      CMND ${CMAKE_COMMAND}
      ARGS -E copy_directory ${${PROJECT_NAME}_TRIBITS_DIR}/examples/tpls/Tpl4 .
      WORKING_DIRECTORY Tpl4

    TEST_13
      MESSAGE "Configure Tpl4"
      WORKING_DIRECTORY build_tpl4
      CMND ${CMAKE_COMMAND}
      ARGS
        ${SERIAL_PASSTHROUGH_CONFIGURE_ARGS}
        ${buildSharedLibsArg}
        -DCMAKE_PREFIX_PATH="${testDir}/install_tpl3<semicolon>${testDir}/install_tpl2"
        -DCMAKE_BUILD_TYPE=RelWithDepInfo
        -DCMAKE_INSTALL_PREFIX=${testDir}/install_tpl4
        -DCMAKE_INSTALL_INCLUDEDIR=include
        -DCMAKE_INSTALL_LIBDIR=lib
        ${testDir}/Tpl4
      PASS_REGULAR_EXPRESSION_ALL
        "Configuring done"
        "Generating done"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_14
      MESSAGE "Build and install Tpl4"
      WORKING_DIRECTORY build_tpl4
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND make ARGS ${CTEST_BUILD_FLAGS} install
      PASS_REGULAR_EXPRESSION_ALL
      "Installing: .*/install_tpl4/lib/cmake/Tpl4/Tpl4ConfigTargets[.]cmake"
      "Installing: .*/install_tpl4/lib/cmake/Tpl4/Tpl4Config[.]cmake"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_15
      MESSAGE "Delete source and build directory for Tpl4"
      CMND ${CMAKE_COMMAND} ARGS -E rm -rf Tpl4 build_tpl4

     LIST_SEPARATOR <semicolon>

      ADDED_TEST_NAME_OUT
        TribitsExampleProject2_Tpls_install_${sharedOrStatic}_NAME
    )
    # NOTE: The above test installs each TPL into its own install directory.
    # This is too increase the testing effect downstream to ensure that
    # include directories are handled correctly in the tests and with TriBITS.

  # Name of added test to use to create test dependencies
  set(TribitsExampleProject2_Tpls_install_${sharedOrStatic}_NAME
    ${TribitsExampleProject2_Tpls_install_${sharedOrStatic}_NAME} PARENT_SCOPE)

  # Reusable location of the SimpleTPL install
  set(TribitsExampleProject2_Tpls_install_${sharedOrStatic}_DIR ${testDir}
    PARENT_SCOPE)

endfunction()


TribitsExampleProject2_Tpls_install_tests(SHARED)
TribitsExampleProject2_Tpls_install_tests(STATIC)
