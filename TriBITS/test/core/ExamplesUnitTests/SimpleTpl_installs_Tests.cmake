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
# SimpleTpl installs
########################################################################


function(SimpleTpl_install_test sharedOrStatic)

  if (sharedOrStatic STREQUAL "SHARED")
    set(buildSharedLibsArg
      -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE
      -DCMAKE_MACOSX_RPATH=TRUE)
  else()
    set(buildSharedLibsArg -DBUILD_SHARED_LIBS=OFF)
  endif()

  # A) Build and install SharedTpl

  set(testNameBase SimpleTpl_install_${sharedOrStatic})
  set(testName ${PACKAGE_NAME}_${testNameBase})
  set(testDir ${CMAKE_CURRENT_BINARY_DIR}/${testName})

  tribits_add_advanced_test( ${testNameBase}
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1

    TEST_0
      MESSAGE "Copy source for SimpleTpl"
      CMND ${CMAKE_COMMAND}
      ARGS -E copy_directory ${${PROJECT_NAME}_TRIBITS_DIR}/examples/tpls/SimpleTpl .
      WORKING_DIRECTORY SimpleTpl

    TEST_1
      MESSAGE "Configure SimpleTpl"
      WORKING_DIRECTORY BUILD
      CMND ${CMAKE_COMMAND}
      ARGS
        ${SERIAL_PASSTHROUGH_CONFIGURE_ARGS}
        ${buildSharedLibsArg}
        -DCMAKE_BUILD_TYPE=Release
	-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=ON
        -DCMAKE_INSTALL_PREFIX=${testDir}/install
        -DCMAKE_INSTALL_INCLUDEDIR=include
        -DCMAKE_INSTALL_LIBDIR=lib
        ${testDir}/SimpleTpl
      PASS_REGULAR_EXPRESSION_ALL
        "Configuring done"
        "Generating done"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_2
      MESSAGE "Build and install SimpleTpl"
      WORKING_DIRECTORY BUILD
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND ${CMAKE_COMMAND} ARGS --build . --config Release --target install
      PASS_REGULAR_EXPRESSION_ALL
        "Installing: ${testDir}/install/.*simpletpl[.]"
        "Installing: ${testDir}/install/include/SimpleTpl.hpp"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_3
      MESSAGE "Delete source and build directory for SimpleTpl"
      CMND ${CMAKE_COMMAND} ARGS -E rm -rf SimpleTpl BUILD

      ADDED_TEST_NAME_OUT SimpleTpl_install_${sharedOrStatic}_NAME
    )

  # Name of added test to use to create test dependencies
  set(SimpleTpl_install_${sharedOrStatic}_NAME
    ${SimpleTpl_install_${sharedOrStatic}_NAME} PARENT_SCOPE)

  # Reusable location of the SimpleTPL install
  set(SimpleTpl_install_${sharedOrStatic}_DIR ${testDir} PARENT_SCOPE)

  # B) Check the RPATH of SHARED SharedTpl

  if (sharedOrStatic STREQUAL "SHARED")

    if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
      set(THIS_RPATH_INSPECT_CMND "${RPATH_INSPECT_CMND}")
      set(THIS_RPATH_INSPECT_ARG "${RPATH_INSPECT_ARG}")
      set(RPATH_GREP_STR "@rpath/libsimpletpl.dylib")
    elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
      set(THIS_RPATH_INSPECT_CMND "ls")
      set(THIS_RPATH_INSPECT_ARG "")
      set(RPATH_GREP_STR "libsimpletpl")
    else()
      set(THIS_RPATH_INSPECT_CMND "")
      set(THIS_RPATH_INSPECT_ARG "")
      set(RPATH_GREP_STR "")
    endif()

    tribits_add_advanced_test( ${testNameBase}_check_RPATH
      OVERALL_WORKING_DIRECTORY TEST_NAME
      OVERALL_NUM_MPI_PROCS 1
      EXCLUDE_IF_NOT_TRUE THIS_RPATH_INSPECT_CMND

      TEST_0
        MESSAGE "Inspect the RPATH of the installed shared lib libsimpletpl"
        CMND ${THIS_RPATH_INSPECT_CMND}
        ARGS
          ${THIS_RPATH_INSPECT_ARG} ${testDir}/install/lib/libsimpletpl.${SHARED_LIB_EXT}
        PASS_REGULAR_EXPRESSION  "${THIS_RPATH_GREP_STR}"
        ALWAYS_FAIL_ON_NONZERO_RETURN

      ADDED_TEST_NAME_OUT ${testNameBase}_check_RPATH_NAME
      )

    if (${testNameBase}_check_RPATH_NAME)
      set_tests_properties(${${testNameBase}_check_RPATH_NAME}
        PROPERTIES DEPENDS ${SimpleTpl_install_${sharedOrStatic}_NAME} )
    endif()

  endif()

endfunction()


SimpleTpl_install_test(SHARED)
SimpleTpl_install_test(STATIC)
