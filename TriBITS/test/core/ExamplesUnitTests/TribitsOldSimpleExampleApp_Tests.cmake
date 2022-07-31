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
# TribitsOldSimpleExampleApp
########################################################################

set(
  TribitsOldSimpleExampleApp_TribitsExampleProject_TRIBITS_DIR_default
  "${${PROJECT_NAME}_TRIBITS_DIR}" )

set_default_and_from_env(
  TribitsOldSimpleExampleApp_TribitsExampleProject_TRIBITS_DIR
  "${TribitsOldSimpleExampleApp_TribitsExampleProject_TRIBITS_DIR_default}"
  )
# NOTE: The above var can be overridden to use an older version of TriBITS and
# TribitsExampleProject to build and install and test against this
# TribitsOldSimpleExampleApp.


function(TribitsOldSimpleExampleApp  sharedOrStatic  serialOrMpi  useDeprecatedTargets)

  set(appConfigurePassRegexAll "")

  if (sharedOrStatic STREQUAL "SHARED")
    set(buildSharedLibsArg -DBUILD_SHARED_LIBS=ON)
  elseif (sharedOrStatic STREQUAL "STATIC")
    set(buildSharedLibsArg -DBUILD_SHARED_LIBS=OFF)
  else()
    message(FATAL_ERROR "Invalid value sharedOrStatic='${sharedOrStatic}'!")
  endif()

  if (serialOrMpi STREQUAL "SERIAL")
    set(tplEnableMpiArg -DTPL_ENABLE_MPI=OFF)
    set(excludeIfNotTrueVarList "")
  elseif (serialOrMpi STREQUAL "MPI")
    set(tplEnableMpiArg -DTPL_ENABLE_MPI=ON)
    set(excludeIfNotTrueVarList "TriBITS_PROJECT_MPI_IS_ENABLED")
  else()
    message(FATAL_ERROR "Invalid value tplEnableMpiArg='${tplEnableMpiArg}'!")
  endif()

  if (useDeprecatedTargets STREQUAL "USE_DEPRECATED_TARGETS")
    set(useDeprecatedTargetsArg -DTribitsOldSimpleExApp_USE_DEPRECATED_TARGETS=ON)
    if (TribitsOldSimpleExampleApp_TribitsExampleProject_TRIBITS_DIR STREQUAL
      TribitsOldSimpleExampleApp_TribitsExampleProject_TRIBITS_DIR_default
      )
      list(APPEND appConfigurePassRegexAll
        "The library that is being linked to, pws_b, is marked as being deprecated"
        "WARNING: The non-namespaced target 'pws_b' is deprecated"
        "'WithSubpackagesB::pws_b'"
        "'WithSubpackagesB::all_libs'"
        "package 'WithSubpackagesB'"
        "project 'TribitsExProj'"
        "'WithSubpackagesB_LIBRARIES'"
        )
    endif()
  elseif (useDeprecatedTargets STREQUAL "USE_NEW_TARGETS")
    set(useDeprecatedTargetsArg "")
  else()
    message(FATAL_ERROR "Invalid value useDeprecatedTargets='${useDeprecatedTargets}'!")
  endif()

  set(testBaseName
    ${CMAKE_CURRENT_FUNCTION}_${sharedOrStatic}_${serialOrMpi}_${useDeprecatedTargets})
  set(testName ${PACKAGE_NAME}_${testBaseName})
  set(testDir ${CMAKE_CURRENT_BINARY_DIR}/${testName})

  tribits_add_advanced_test( ${testBaseName}
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran ${excludeIfNotTrueVarList}
    XHOSTTYPE Darwin

    TEST_0
      MESSAGE "Do the configure of TribitsExampleProject"
      WORKING_DIRECTORY BUILD
      CMND ${CMAKE_COMMAND}
      ARGS
        ${TribitsExampleProject_COMMON_CONFIG_ARGS}
        -DTribitsExProj_ENABLE_Fortran=ON
        -DTribitsExProj_ENABLE_ALL_PACKAGES=ON
        -DTribitsExProj_ENABLE_SECONDARY_TESTED_CODE=ON
        -DTribitsExProj_ENABLE_INSTALL_CMAKE_CONFIG_FILES=ON
        -DTPL_ENABLE_SimpleTpl=ON
        -DSimpleTpl_INCLUDE_DIRS=${SimpleTpl_install_${sharedOrStatic}_DIR}/install/include
        -DSimpleTpl_LIBRARY_DIRS=${SimpleTpl_install_${sharedOrStatic}_DIR}/install/lib
        ${buildSharedLibsArg}
        ${tplEnableMpiArg}
        -DCMAKE_INSTALL_PREFIX=${testDir}/install
        ${TribitsOldSimpleExampleApp_TribitsExampleProject_TRIBITS_DIR}/examples/TribitsExampleProject

    TEST_1
      MESSAGE "Build and install TribitsExampleProject locally"
      WORKING_DIRECTORY BUILD
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND make ARGS ${CTEST_BUILD_FLAGS} install

    TEST_2
      MESSAGE "Configure TribitsOldSimpleExampleApp locally"
      WORKING_DIRECTORY app_build
      CMND ${CMAKE_COMMAND} ARGS
        ${useDeprecatedTargetsArg}
        -DCMAKE_PREFIX_PATH=${testDir}/install
        ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsOldSimpleExampleApp
      PASS_REGULAR_EXPRESSION_ALL
        "${foundProjectOrPackageStr}"
        "${appConfigurePassRegexAll}"
        "-- Configuring done"
        "-- Generating done"
        "-- Build files have been written to: .*/${testName}/app_build"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_3
      MESSAGE "Build TribitsOldSimpleExampleApp"
      WORKING_DIRECTORY app_build
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND make ARGS ${CTEST_BUILD_FLAGS}
      PASS_REGULAR_EXPRESSION_ALL
        "Built target app"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_4
      MESSAGE "Test TribitsOldSimpleExampleApp"
      WORKING_DIRECTORY app_build
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
      PASS_REGULAR_EXPRESSION_ALL
        "Util Deps: B A SimpleCxx simpletpl headeronlytpl SimpleCxx simpletpl headeronlytpl"
        "Full Deps: WithSubpackages:B A SimpleCxx simpletpl headeronlytpl SimpleCxx simpletpl headeronlytpl A SimpleCxx simpletpl headeronlytpl[;] MixedLang:Mixed Language[;] SimpleCxx:simpletpl headeronlytpl"
        "util_test [.]+   Passed"
        "app_test [.]+   Passed"
        "100% tests passed, 0 tests failed out of 2"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    ${LD_LIBRARY_PATH_HACK_FOR_SIMPLETPL_${sharedOrStatic}_ENVIRONMENT_ARG}

    ADDED_TEST_NAME_OUT ${testNameBase}_NAME
    )
  # NOTE: The above test verifies 

  if (${testNameBase}_NAME)
    set_tests_properties(${${testNameBase}_NAME}
      PROPERTIES DEPENDS ${SimpleTpl_install_${sharedOrStatic}_NAME} )
  endif()

endfunction()


TribitsOldSimpleExampleApp(STATIC  MPI  USE_DEPRECATED_TARGETS)
TribitsOldSimpleExampleApp(STATIC  SERIAL  USE_DEPRECATED_TARGETS)
TribitsOldSimpleExampleApp(SHARED  SERIAL USE_NEW_TARGETS)
