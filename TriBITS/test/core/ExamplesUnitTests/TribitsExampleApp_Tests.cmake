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


################################################################################
#
# TribitsExampleApp Testing
#
################################################################################


################################################################################
# TribitsExampleApp helper functions
################################################################################


if (NOT "$ENV{TRIBITS_ADD_LD_LIBRARY_PATH_HACK_FOR_SIMPLETPL}" STREQUAL "")
  set(TRIBITS_ADD_LD_LIBRARY_PATH_HACK_FOR_SIMPLETPL_DEFAULT
    $ENV{TRIBITS_ADD_LD_LIBRARY_PATH_HACK_FOR_SIMPLETPL})
else()
  set($ENV{TRIBITS_ADD_LD_LIBRARY_PATH_HACK_FOR_SIMPLETPL} OFF)
endif()
advanced_set(TRIBITS_ADD_LD_LIBRARY_PATH_HACK_FOR_SIMPLETPL
  ${TRIBITS_ADD_LD_LIBRARY_PATH_HACK_FOR_SIMPLETPL_DEFAULT} CACHE BOOL
  "Set to TRUE to add LD_LIBRARY_PATH to libsimpletpl.so for platforms where RPATH not working")

function(set_ENV_HACK_FOR_SIMPLETPL_ENVIRONMENT_ARG sharedOrStatic)
  set(ENV_HACK_FOR_SIMPLETPL_${sharedOrStatic}_ENVIRONMENT_ARG_ON
    ENVIRONMENT LD_LIBRARY_PATH=${SimpleTpl_install_${sharedOrStatic}_DIR}/install/lib)
  if (TRIBITS_ADD_LD_LIBRARY_PATH_HACK_FOR_SIMPLETPL)
    set(ENV_HACK_FOR_SIMPLETPL_${sharedOrStatic}_ENVIRONMENT_ARG
      ${ENV_HACK_FOR_SIMPLETPL_${sharedOrStatic}_ENVIRONMENT_ARG_ON})
  else()
    set(ENV_HACK_FOR_SIMPLETPL_${sharedOrStatic}_ENVIRONMENT_ARG "")
  endif()
  set(ENV_HACK_FOR_SIMPLETPL_${sharedOrStatic}_ENVIRONMENT_ARG_ON
    ${ENV_HACK_FOR_SIMPLETPL_${sharedOrStatic}_ENVIRONMENT_ARG_ON}
    PARENT_SCOPE)
  set(ENV_HACK_FOR_SIMPLETPL_${sharedOrStatic}_ENVIRONMENT_ARG
    ${ENV_HACK_FOR_SIMPLETPL_${sharedOrStatic}_ENVIRONMENT_ARG}
    PARENT_SCOPE)
endfunction()
set_ENV_HACK_FOR_SIMPLETPL_ENVIRONMENT_ARG(STATIC)
set_ENV_HACK_FOR_SIMPLETPL_ENVIRONMENT_ARG(SHARED)
# NOTE: Above, we have to set LD_LIBRARY_PATH to pick up the
# libsimpletpl.so because CMake 3.17.5 and 3.21.2 with the GitHub Actions
# Umbuntu build is refusing to put in the RPATH for libsimpletpl.so into
# libsimplecxx.so even through CMAKE_INSTALL_RPATH_USE_LINK_PATH=ON is
# set.  This is not needed for the RHEL 7 builds that I have tried where
# CMake is behaving correctly and putting in RPATH correctly.  But because
# I can't log into this system, it is very hard and time consuming to
# debug this so I am just giving up at this point.


# Macro to set up ENVIRONMENT arg as var 'TEST_ENV_ARG' for
# tribits_add_advanced_test() for below TribitsExampleApp tests so that
# upstream shared libs can be found in variety of platforms.
#
# Usage:
#
#   TribitsExampleApp_set_test_env_var([ALWAYS_SET_ENV_VARS])
#
# Must be alled after 'testDir' is defined!
#
macro(TribitsExampleApp_set_test_env_var)

  cmake_parse_arguments(
     PARSE  #prefix
     "ALWAYS_SET_ENV_VARS"  #options
     ""  #one_value_keywords
     ""  #multi_value_keywords
     ${ARGN}
     )
  tribits_check_for_unparsed_arguments()

  if (WIN32)
    # Set extra paths and convert to native Windows paths
    set(extraPathsCMake
      "${testDir}/install/bin"
      "${SimpleTpl_install_${sharedOrStatic}_DIR}/install/bin"
      )
    convertCMakePathsToNativePaths("${extraPathsCMake}" extraPaths)
    set(PATH_VAL "${extraPaths};$ENV{PATH}")
    string(REPLACE ";" "\\;" PATH_VAL "${PATH_VAL}")
    # Prepend Windows PATH
    set(TEST_ENV_ARG
      ENVIRONMENT "PATH=${PATH_VAL}")
  elseif (CYGWIN)
    set(TEST_ENV_ARG
      ENVIRONMENT
      "PATH=${testDir}/install/bin:${SimpleTpl_install_${sharedOrStatic}_DIR}/install/bin:$ENV{PATH}")
  else()
    if (PARSE_ALWAYS_SET_ENV_VARS)
      set(TEST_ENV_ARG
        ${ENV_HACK_FOR_SIMPLETPL_${sharedOrStatic}_ENVIRONMENT_ARG_ON})
    else()
      set(TEST_ENV_ARG
        ${ENV_HACK_FOR_SIMPLETPL_${sharedOrStatic}_ENVIRONMENT_ARG})
    endif()
  endif()

endmacro()


function(convertCMakePathsToNativePaths  pathsListIn  pathsListVarOut)
  set(pathsListOut)
  foreach (pathIn "${pathsListIn}")
    file(TO_NATIVE_PATH "${pathIn}" pathOut)
    list(APPEND pathsListOut "${pathOut}")
  endforeach()
  set(${pathsListVarOut} "${pathsListOut}" PARENT_SCOPE)
endfunction()


# Macro to handle the 'sharedOrStatic' arguemnt
#
macro(TribitsExampleApp_process_sharedOrStatic_arg)

  if (sharedOrStatic STREQUAL "SHARED")
    set(buildSharedLibsArg -DBUILD_SHARED_LIBS=ON)
  elseif (sharedOrStatic STREQUAL "STATIC")
    set(buildSharedLibsArg -DBUILD_SHARED_LIBS=OFF)
  else()
    message(FATAL_ERROR "Invalid value of buildSharedLibsArg='${buildSharedLibsArg}'!")
  endif()

endmacro()


# Create the expected string of full dependencies given a list of enabled TPLs
# and packages.
#
# Usage:
#
#   TribitsExampleApp_GetExpectedAppFullDeps( <fullDepsStrOut>
#     <pkg1> <pkg2> ...)
#
function(TribitsExampleApp_GetExpectedAppFullDeps  fullDepsStrOut)

  if ("SimpleTpl" IN_LIST ARGN)
    set(simpletplText "simpletpl ")
  else()
    set(simpletplText)
  endif()

  if ("SimpleCxx" IN_LIST ARGN)
    set(EXPECTED_SIMPLECXX_DEPS
      "${simpletplText}headeronlytpl")
    set(EXPECTED_SIMPLECXX_AND_DEPS
      "SimpleCxx ${EXPECTED_SIMPLECXX_DEPS}")
  else()
    set(EXPECTED_SIMPLECXX_DEPS "")
    set(EXPECTED_SIMPLECXX_AND_DEPS "")
  endif()

  if ("WithSubpackagesA" IN_LIST ARGN)
    set(EXPECTED_A_DEPS "${EXPECTED_SIMPLECXX_AND_DEPS}")
    set(EXPECTED_A_AND_DEPS "A ${EXPECTED_A_DEPS}")
    set(EXPECTED_A_AND_DEPS_STR "${EXPECTED_A_AND_DEPS} ")
  else()
    set(EXPECTED_A_DEPS "")
    set(EXPECTED_A_AND_DEPS "")
    set(EXPECTED_A_AND_DEPS_STR "")
  endif()

  if ("WithSubpackagesB" IN_LIST ARGN)
    set(EXPECTED_B_DEPS
      "${EXPECTED_A_AND_DEPS_STR}${EXPECTED_SIMPLECXX_AND_DEPS}")
    set(EXPECTED_B_AND_DEPS
      "B ${EXPECTED_B_DEPS}")
    set(EXPECTED_B_AND_DEPS_STR
      "${EXPECTED_B_AND_DEPS} ")
  else()
    set(EXPECTED_B_DEPS "")
    set(EXPECTED_B_AND_DEPS "")
    set(EXPECTED_B_AND_DEPS_STR "")
  endif()

  if ("WithSubpackagesC" IN_LIST ARGN)
    set(EXPECTED_C_DEPS
      "${EXPECTED_B_AND_DEPS_STR}${EXPECTED_A_AND_DEPS}")
  else()
    set(EXPECTED_C_DEPS "")
  endif()

  set(fullDepsStr "")
  if (EXPECTED_C_DEPS)
    appendStrWithGlue(fullDepsStr "[;] "
      "WithSubpackagesC:${EXPECTED_C_DEPS}")
  endif()
  if (EXPECTED_B_DEPS)
    appendStrWithGlue(fullDepsStr "[;] "
      "WithSubpackagesB:${EXPECTED_B_DEPS}")
  endif()
  if (EXPECTED_A_DEPS)
    appendStrWithGlue(fullDepsStr "[;] "
      "WithSubpackagesA:${EXPECTED_A_DEPS}")
  endif()
  if ("MixedLang" IN_LIST ARGN)
    appendStrWithGlue(fullDepsStr "[;] "
      "MixedLang:Mixed Language")
  endif()
  if (EXPECTED_SIMPLECXX_DEPS)
    appendStrWithGlue(fullDepsStr "[;] "
      "SimpleCxx:${EXPECTED_SIMPLECXX_DEPS}")
  endif()

  set(${fullDepsStrOut} "${fullDepsStr}" PARENT_SCOPE)

endfunction()


function(appendStrWithGlue  strVarNameInOut   glueStr  str)
  set(strVar "${${strVarNameInOut}}")
  if (strVar)
    string(APPEND strVar "${glueStr}${str}")
  else()
    set(strVar "${str}")
  endif()
  set(${strVarNameInOut} "${strVar}" PARENT_SCOPE)
endfunction()


################################################################################


function(TribitsExampleApp_NoFortran fullOrComponents sharedOrStatic)

  if (fullOrComponents STREQUAL "FULL")
    set(tribitsExProjUseComponentsArg "")
  elseif (fullOrComponents STREQUAL "COMPONENTS")
    set(tribitsExProjUseComponentsArg
      -DTribitsExApp_USE_COMPONENTS=SimpleCxx,WithSubpackages)
  else()
    message(FATAL_ERROR "Invalid value of fullOrComponents='${fullOrComponents}'!")
  endif()

  TribitsExampleApp_process_sharedOrStatic_arg()

  set(testBaseName ${CMAKE_CURRENT_FUNCTION}_${fullOrComponents}_${sharedOrStatic})
  set(testName ${PACKAGE_NAME}_${testBaseName})
  set(testDir ${CMAKE_CURRENT_BINARY_DIR}/${testName})

  TribitsExampleApp_set_test_env_var()

  if (WIN32 AND sharedOrStatic STREQUAL "SHARED")
    set(copyDllsCmndArgs
      CMND ${CMAKE_COMMAND}
      ARGS
        -D FROM_DIRS="${testDir}/install/bin,${SimpleTpl_install_${sharedOrStatic}_DIR}/install/bin"
        -D GLOB_EXPR="*.dll"
        -D TO_DIR="app_build/Release"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/copy_files_glob.cmake"
      )
  else()
    set(copyDllsCmndArgs 
      CMND ${CMAKE_COMMAND} ARGS -E echo "skipped")
  endif()

  TribitsExampleApp_GetExpectedAppFullDeps(fullDepsStr
    SimpleTpl SimpleCxx WithSubpackagesA WithSubpackagesB WithSubpackagesC)

  tribits_add_advanced_test( ${testBaseName}
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    XHOSTTYPE Darwin

    TEST_0
      MESSAGE "Copy source for TribitsExampleProject"
      CMND ${CMAKE_COMMAND}
      ARGS -E copy_directory
        ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject .
      WORKING_DIRECTORY TribitsExampleProject

    TEST_1
      MESSAGE "Do the configure of TribitsExampleProject"
      WORKING_DIRECTORY BUILD
      CMND ${CMAKE_COMMAND}
      ARGS
        ${TribitsExampleProject_COMMON_CONFIG_ARGS}
        -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
        -DCMAKE_BUILD_TYPE=Release
	-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=ON
        -DTribitsExProj_ENABLE_Fortran=OFF
        -DTribitsExProj_ENABLE_ALL_PACKAGES=ON
        -DTribitsExProj_ENABLE_SECONDARY_TESTED_CODE=ON
        -DTribitsExProj_ENABLE_INSTALL_CMAKE_CONFIG_FILES=ON
        -DTPL_ENABLE_SimpleTpl=ON
        -DSimpleTpl_INCLUDE_DIRS=${SimpleTpl_install_${sharedOrStatic}_DIR}/install/include
        -DSimpleTpl_LIBRARY_DIRS=${SimpleTpl_install_${sharedOrStatic}_DIR}/install/lib
        ${buildSharedLibsArg}
        -DCMAKE_INSTALL_PREFIX=${testDir}/install
        ${testDir}/TribitsExampleProject

    TEST_2
      MESSAGE "Build and install TribitsExampleProject locally"
      WORKING_DIRECTORY BUILD
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND ${CMAKE_COMMAND} ARGS --build . --config Release --target install

    TEST_3
      MESSAGE "Delete source and build directory for TribitsExampleProject"
      CMND ${CMAKE_COMMAND} ARGS -E rm -rf TribitsExampleProject BUILD

    TEST_4
      MESSAGE "Configure TribitsExampleApp locally"
      WORKING_DIRECTORY app_build
      CMND ${CMAKE_COMMAND} ARGS
        -DCMAKE_PREFIX_PATH=${testDir}/install
        -DCMAKE_BUILD_TYPE=Release
        ${tribitsExProjUseComponentsArg}
        ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleApp
      PASS_REGULAR_EXPRESSION_ALL
        "-- Configuring done"
        "-- Generating done"
        "-- Build files have been written to: .*/${testName}/app_build"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_5
      MESSAGE "Build TribitsExampleApp"
      WORKING_DIRECTORY app_build
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND  ${CMAKE_COMMAND} ARGS --build . --config Release
      
    TEST_6
      MESSAGE "Copy dlls on Windows platforms (only)"
      ${copyDllsCmndArgs}
      
    TEST_7
      MESSAGE "Test TribitsExampleApp"
      WORKING_DIRECTORY app_build
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
      PASS_REGULAR_EXPRESSION_ALL
        "Full Deps: ${fullDepsStr}"
        "app_test [.]+   Passed"
        "100% tests passed, 0 tests failed out of 1"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    ${TEST_ENV_ARG}

    ADDED_TEST_NAME_OUT ${testNameBase}_NAME
    )
  # NOTE: Above test deletes the source and build dir for
  # TribitsExampleProject after the install to ensure that the install dir is
  # stand-alone.

  if (${testNameBase}_NAME)
    set_tests_properties(${${testNameBase}_NAME}
      PROPERTIES DEPENDS ${SimpleTpl_install_${sharedOrStatic}_NAME} )
  endif()

endfunction()


TribitsExampleApp_NoFortran(FULL STATIC)
TribitsExampleApp_NoFortran(COMPONENTS STATIC)
TribitsExampleApp_NoFortran(COMPONENTS SHARED)
# NOTE: We don't need to test the permutation SHARED FULL as well.  That does
# not really test anything new.


################################################################################


function(TribitsExampleApp_EnableSingleSubpackage fullOrComponents sharedOrStatic)

  if (fullOrComponents STREQUAL "FULL")
    set(tribitsExProjUseComponentsArg "")
  elseif (fullOrComponents STREQUAL "COMPONENTS")
    set(tribitsExProjUseComponentsArg
      -DTribitsExApp_USE_COMPONENTS=SimpleCxx,WithSubpackages)
  else()
    message(FATAL_ERROR "Invalid value of fullOrComponents='${fullOrComponents}'!")
  endif()

  TribitsExampleApp_process_sharedOrStatic_arg()

  set(testBaseName
    ${CMAKE_CURRENT_FUNCTION}_${fullOrComponents}_${sharedOrStatic})
  set(testName ${PACKAGE_NAME}_${testBaseName})
  set(testDir ${CMAKE_CURRENT_BINARY_DIR}/${testName})

  TribitsExampleApp_set_test_env_var()

  TribitsExampleApp_GetExpectedAppFullDeps(fullDepsStr
    SimpleTpl SimpleCxx WithSubpackagesA WithSubpackagesB)

  tribits_add_advanced_test( ${testBaseName}
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    XHOSTTYPE Darwin

    TEST_0
      MESSAGE "Copy source for TribitsExampleProject"
      CMND ${CMAKE_COMMAND}
      ARGS -E copy_directory
        ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject .
      WORKING_DIRECTORY TribitsExampleProject

    TEST_1
      MESSAGE "Make Withsubpackages OPTIONAL subpackages REQUIRED"
      CMND ${CMAKE_COMMAND}
      ARGS
        -D FILE="TribitsExampleProject/packages/with_subpackages/cmake/Dependencies.cmake"
        -D STRING_TO_REPLACE=OPTIONAL
        -D REPLACEMENT_STRING=REQUIRED
        -P ${CMAKE_CURRENT_SOURCE_DIR}/replace_string.cmake

    TEST_2
      MESSAGE "Do the configure of TribitsExampleProject with just one subpackage"
      WORKING_DIRECTORY BUILD
      CMND ${CMAKE_COMMAND}
      ARGS
        ${TribitsExampleProject_COMMON_CONFIG_ARGS}
        -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
        -DCMAKE_BUILD_TYPE=Release
	-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=ON
        -DTribitsExProj_ENABLE_Fortran=OFF
        -DTribitsExProj_ENABLE_WithSubpackagesB=ON
        -DTribitsExProj_ENABLE_SECONDARY_TESTED_CODE=ON
        -DTribitsExProj_ENABLE_INSTALL_CMAKE_CONFIG_FILES=ON
        -DTPL_ENABLE_SimpleTpl=ON
        -DSimpleTpl_INCLUDE_DIRS=${SimpleTpl_install_${sharedOrStatic}_DIR}/install/include
        -DSimpleTpl_LIBRARY_DIRS=${SimpleTpl_install_${sharedOrStatic}_DIR}/install/lib
        ${buildSharedLibsArg}
        -DCMAKE_INSTALL_PREFIX=${testDir}/install
        ${testDir}/TribitsExampleProject
      PASS_REGULAR_EXPRESSION_ALL
        "Final set of enabled packages:  SimpleCxx WithSubpackages 2"
        "Final set of enabled SE packages:  SimpleCxx WithSubpackagesA WithSubpackagesB WithSubpackages 4"
        "Final set of non-enabled packages:  MixedLang WrapExternal 2"
        "Final set of non-enabled SE packages:  MixedLang WithSubpackagesC WrapExternal 3"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_3
      MESSAGE "Build and install TribitsExampleProject locally"
      WORKING_DIRECTORY BUILD
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND ${CMAKE_COMMAND} ARGS --build . --config Release --target install

    TEST_4
      MESSAGE "Configure TribitsExampleApp locally"
      WORKING_DIRECTORY app_build
      CMND ${CMAKE_COMMAND} ARGS
        -DCMAKE_PREFIX_PATH=${testDir}/install
        -DCMAKE_BUILD_TYPE=Release
        ${tribitsExProjUseComponentsArg}
        ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleApp
      PASS_REGULAR_EXPRESSION_ALL
        "-- Configuring done"
        "-- Generating done"
        "-- Build files have been written to: .*/${testName}/app_build"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_5
      MESSAGE "Build TribitsExampleApp"
      WORKING_DIRECTORY app_build
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND  ${CMAKE_COMMAND} ARGS --build . --config Release

    TEST_6
      MESSAGE "Test TribitsExampleApp"
      WORKING_DIRECTORY app_build
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
      PASS_REGULAR_EXPRESSION_ALL
        "Full Deps: ${fullDepsStr}"
        "app_test [.]+   Passed"
        "100% tests passed, 0 tests failed out of 1"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    ${TEST_ENV_ARG}

    ADDED_TEST_NAME_OUT ${testNameBase}_NAME
    )
  # NOTE: The above test changes all of the subpackages in WithSubpackages to
  # be 'REQUIRED' and then only enables a subset of its required subpackages
  # and checks that the <Package>Config.cmake files get created correctly.

  if (${testNameBase}_NAME)
    set_tests_properties(${${testNameBase}_NAME}
      PROPERTIES DEPENDS ${SimpleTpl_install_${sharedOrStatic}_NAME} )
  endif()

endfunction()


TribitsExampleApp_EnableSingleSubpackage(FULL STATIC)


################################################################################


function(TribitsExampleApp_ALL_ST  byProjectOrPackage  sharedOrStatic  serialOrMpi)

  if (byProjectOrPackage STREQUAL "ByProject")
    set(findByProjectOrPackageArg -DTribitsExApp_FIND_INDIVIDUAL_PACKAGES=OFF)
    set(foundProjectOrPackageStr "Found TribitsExProj")
  elseif (byProjectOrPackage STREQUAL "ByPackage")
    set(findByProjectOrPackageArg -DTribitsExApp_FIND_INDIVIDUAL_PACKAGES=ON)
    set(foundProjectOrPackageStr "Found SimpleCxx")
  else()
    message(FATAL_ERROR
      "Invalid value findByProjectOrPackageArg='${findByProjectOrPackageArg}'!")
  endif()

  TribitsExampleApp_process_sharedOrStatic_arg()

  if (serialOrMpi STREQUAL "SERIAL")
    set(tplEnableMpiArg -DTPL_ENABLE_MPI=OFF)
  elseif (serialOrMpi STREQUAL "MPI")
    set(tplEnableMpiArg -DTPL_ENABLE_MPI=ON)
  else()
    message(FATAL_ERROR "Invalid value tplEnableMpiArg='${tplEnableMpiArg}'!")
  endif()

  set(testBaseName ${CMAKE_CURRENT_FUNCTION}_${byProjectOrPackage}_${sharedOrStatic}_${serialOrMpi})
  set(testName ${PACKAGE_NAME}_${testBaseName})
  set(testDir ${CMAKE_CURRENT_BINARY_DIR}/${testName})

  TribitsExampleApp_set_test_env_var()

  TribitsExampleApp_GetExpectedAppFullDeps(fullDepsStr
    SimpleTpl SimpleCxx MixedLang WithSubpackagesA WithSubpackagesB WithSubpackagesC)

  tribits_add_advanced_test( ${testBaseName}
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
    XHOSTTYPE Darwin

    TEST_0
      MESSAGE "Copy source for TribitsExampleProject"
      CMND ${CMAKE_COMMAND}
      ARGS -E copy_directory
        ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject .
      WORKING_DIRECTORY TribitsExampleProject

    TEST_1
      MESSAGE "Do the configure of TribitsExampleProject"
      WORKING_DIRECTORY BUILD
      CMND ${CMAKE_COMMAND}
      ARGS
        ${TribitsExampleProject_COMMON_CONFIG_ARGS}
        -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
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
        ${testDir}/TribitsExampleProject

    TEST_2
      MESSAGE "Build and install TribitsExampleProject locally"
      WORKING_DIRECTORY BUILD
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND make ARGS ${CTEST_BUILD_FLAGS} install

    TEST_3
      MESSAGE "Delete source and build directory for TribitsExampleProject"
      CMND ${CMAKE_COMMAND} ARGS -E rm -rf TribitsExampleProject BUILD

    TEST_4
      MESSAGE "Configure TribitsExampleApp locally"
      WORKING_DIRECTORY app_build
      CMND ${CMAKE_COMMAND} ARGS
        -DCMAKE_PREFIX_PATH=${testDir}/install
        -DTribitsExApp_USE_COMPONENTS=SimpleCxx,MixedLang,WithSubpackages
        ${findByProjectOrPackageArg}
        ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleApp
      PASS_REGULAR_EXPRESSION_ALL
        "${foundProjectOrPackageStr}"
        "-- Configuring done"
        "-- Generating done"
        "-- Build files have been written to: .*/${testName}/app_build"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_5
      MESSAGE "Build TribitsExampleApp"
      WORKING_DIRECTORY app_build
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND make ARGS ${CTEST_BUILD_FLAGS}
      PASS_REGULAR_EXPRESSION_ALL
        "Built target app"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_6
      MESSAGE "Test TribitsExampleApp"
      WORKING_DIRECTORY app_build
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
      PASS_REGULAR_EXPRESSION_ALL
        "Full Deps: ${fullDepsStr}"
        "app_test [.]+   Passed"
        "100% tests passed, 0 tests failed out of 1"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    ${TEST_ENV_ARG}

    ADDED_TEST_NAME_OUT ${testNameBase}_NAME
    )
  # NOTE: Above test deletes the source and build dir for
  # TribitsExampleProject after the install to ensure that the install dir is
  # stand-alone.

  if (${testNameBase}_NAME)
    set_tests_properties(${${testNameBase}_NAME}
      PROPERTIES DEPENDS ${SimpleTpl_install_${sharedOrStatic}_NAME} )
  endif()

endfunction()


TribitsExampleApp_ALL_ST(ByProject  STATIC  SERIAL)
TribitsExampleApp_ALL_ST(ByProject  SHARED  MPI)
TribitsExampleApp_ALL_ST(ByPackage  STATIC  MPI)
TribitsExampleApp_ALL_ST(ByPackage  SHARED  SERIAL)


################################################################################


function(TribitsExampleApp_INCLUDE byProjectOrPackage sharedOrStatic importedNoSystem)

  if (byProjectOrPackage STREQUAL "ByProject")
    set(findByProjectOrPackageArg -DTribitsExApp_FIND_INDIVIDUAL_PACKAGES=OFF)
    set(foundProjectOrPackageStr "Found TribitsExProj")
  elseif (byProjectOrPackage STREQUAL "ByPackage")
    set(findByProjectOrPackageArg -DTribitsExApp_FIND_INDIVIDUAL_PACKAGES=ON)
    set(foundProjectOrPackageStr "Found SimpleCxx")
  else()
    message(FATAL_ERROR "Invalid value for findByProjectOrPackageArg='${findByProjectOrPackageArg}'!")
  endif()

  TribitsExampleApp_process_sharedOrStatic_arg()

  if (importedNoSystem STREQUAL "IMPORTED_NO_SYSTEM")
    set(importedNoSystemArg -DTribitsExProj_IMPORTED_NO_SYSTEM=ON)
    set(importedNoSystemNameSuffix "_${importedNoSystem}")
  elseif (importedNoSystem STREQUAL "")
    set(importedNoSystemArg "")
    set(importedNoSystemNameSuffix "")
  else()
    message(FATAL_ERROR "Invalid value for importedNoSystem='${importedNoSystem}'!")
  endif()

  set(testBaseName ${CMAKE_CURRENT_FUNCTION}_${byProjectOrPackage}_${sharedOrStatic}${importedNoSystemNameSuffix})
  set(testName ${PACKAGE_NAME}_${testBaseName})
  set(testDir ${CMAKE_CURRENT_BINARY_DIR}/${testName})

  TribitsExampleApp_set_test_env_var()

  TribitsExampleApp_GetExpectedAppFullDeps(fullDepsStr
    SimpleTpl SimpleCxx MixedLang WithSubpackagesA WithSubpackagesB WithSubpackagesC)

  if (importedNoSystem STREQUAL "IMPORTED_NO_SYSTEM")
    set(tribitExProjIncludeRegex "[-]I${testDir}/install")
  elseif (importedNoSystem STREQUAL "")
    set(tribitExProjIncludeRegex "[-]isystem ${testDir}/install")
  else()
    message(FATAL_ERROR "Invalid value for importedNoSystem='${importedNoSystem}'!")
  endif()

  if (CMAKE_VERSION  VERSION_GREATER_EQUAL  3.23)
    set(CMAKE_VERSION_2_23 TRUE)
  else()
    set(CMAKE_VERSION_2_23 FALSE)
  endif()

  tribits_add_advanced_test( ${testBaseName}
    OVERALL_WORKING_DIRECTORY  TEST_NAME
    OVERALL_NUM_MPI_PROCS  1
    EXCLUDE_IF_NOT_TRUE  ${PROJECT_NAME}_ENABLE_Fortran  COMPILER_IS_GNU
      CMAKE_VERSION_2_23  NINJA_EXE
    XHOSTTYPE  Darwin

    TEST_0
      MESSAGE "Do the configure of TribitsExampleProject"
      WORKING_DIRECTORY BUILD
      CMND ${CMAKE_COMMAND}
      ARGS
        ${TribitsExampleProject_COMMON_CONFIG_ARGS}
        -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
        -DTribitsExProj_ENABLE_Fortran=ON
        -DTribitsExProj_ENABLE_ALL_PACKAGES=ON
        -DTribitsExProj_ENABLE_SECONDARY_TESTED_CODE=ON
        -DTribitsExProj_ENABLE_INSTALL_CMAKE_CONFIG_FILES=ON
        -DTPL_ENABLE_SimpleTpl=ON
        -DSimpleTpl_INCLUDE_DIRS=${SimpleTpl_install_${sharedOrStatic}_DIR}/install/include
        -DSimpleTpl_LIBRARY_DIRS=${SimpleTpl_install_${sharedOrStatic}_DIR}/install/lib
        ${buildSharedLibsArg}
        ${importedNoSystemArg}
        -DCMAKE_INSTALL_PREFIX=${testDir}/install
        ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject

    TEST_1
      MESSAGE "Build and install TribitsExampleProject locally"
      WORKING_DIRECTORY BUILD
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND ${CMAKE_COMMAND} ARGS --build . --target install

    TEST_2
      MESSAGE "Configure TribitsExampleApp locally"
      WORKING_DIRECTORY app_build
      CMND ${CMAKE_COMMAND} ARGS
        -GNinja
        -DCMAKE_PREFIX_PATH=${testDir}/install
        -DTribitsExApp_USE_COMPONENTS=SimpleCxx,MixedLang,WithSubpackages
        ${findByProjectOrPackageArg}
        ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleApp
      PASS_REGULAR_EXPRESSION_ALL
        "${foundProjectOrPackageStr}"
        "-- Configuring done"
        "-- Generating done"
        "-- Build files have been written to: .*/${testName}/app_build"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_3
      MESSAGE "Build TribitsExampleApp verbose to see include dirs"
      WORKING_DIRECTORY app_build
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND ${CMAKE_COMMAND} ARGS --build . -v
      PASS_REGULAR_EXPRESSION_ALL
        "${tribitExProjIncludeRegex}"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_4
      MESSAGE "Test TribitsExampleApp"
      WORKING_DIRECTORY app_build
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
      PASS_REGULAR_EXPRESSION_ALL
        "Full Deps: ${fullDepsStr}"
        "app_test [.]+   Passed"
        "100% tests passed, 0 tests failed out of 1"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    ${TEST_ENV_ARG}

    ADDED_TEST_NAME_OUT ${testNameBase}_NAME
    )
  # NOTE: Above test checks that the IMPORTED_NO_SYSTEM property is set
  # correctly and CMake is handling it correctly.  NOTE: Above, we had to
  # switch to Ninja and 'cmake --build . -v [--target <target>] in order to
  # get verbose output when run inside of a cmake -S script with CMake
  # 3.23-rc2.  Not sure why this is but this is better anyway.

  if (${testNameBase}_NAME)
    set_tests_properties(${${testNameBase}_NAME}
      PROPERTIES DEPENDS ${SimpleTpl_install_${sharedOrStatic}_NAME} )
  endif()

endfunction()


TribitsExampleApp_INCLUDE(ByProject STATIC "")
TribitsExampleApp_INCLUDE(ByProject SHARED IMPORTED_NO_SYSTEM)
TribitsExampleApp_INCLUDE(ByProject STATIC IMPORTED_NO_SYSTEM)
TribitsExampleApp_INCLUDE(ByPackage SHARED IMPORTED_NO_SYSTEM)
TribitsExampleApp_INCLUDE(ByPackage STATIC IMPORTED_NO_SYSTEM)
# NOTE: Above, we are checking that all of the targets defined by the
# <Project>Config.cmake and <Package>Config.cmake files all result in -I
# includes for TribitsExProj when setting -D<Project>_IMPORTED_NO_SYSTEM=ON.


################################################################################


function(TribitsExampleApp_NoOptionalPackages byProjectOrPackage sharedOrStatic)

  if (byProjectOrPackage STREQUAL "ByProject")
    set(findByProjectOrPackageArg -DTribitsExApp_FIND_INDIVIDUAL_PACKAGES=OFF)
    set(foundProjectOrPackageStr "Found TribitsExProj")
  elseif (byProjectOrPackage STREQUAL "ByPackage")
    set(findByProjectOrPackageArg -DTribitsExApp_FIND_INDIVIDUAL_PACKAGES=ON)
    set(foundProjectOrPackageStr "Found SimpleCxx")
  else()
    message(FATAL_ERROR "Invalid value for findByProjectOrPackageArg='${findByProjectOrPackageArg}'!")
  endif()

  TribitsExampleApp_process_sharedOrStatic_arg()

  set(testBaseName ${CMAKE_CURRENT_FUNCTION}_${byProjectOrPackage}_${sharedOrStatic})
  set(testName ${PACKAGE_NAME}_${testBaseName})
  set(testDir ${CMAKE_CURRENT_BINARY_DIR}/${testName})

  TribitsExampleApp_set_test_env_var()

  TribitsExampleApp_GetExpectedAppFullDeps(fullDepsStr
    SimpleTpl SimpleCxx MixedLang WithSubpackagesA WithSubpackagesB WithSubpackagesC)

  tribits_add_advanced_test( ${testBaseName}
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
    XHOSTTYPE Darwin

    TEST_0
      MESSAGE "Do the configure of TribitsExampleProject"
      WORKING_DIRECTORY BUILD
      CMND ${CMAKE_COMMAND}
      ARGS
        ${TribitsExampleProject_COMMON_CONFIG_ARGS}
        -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
        -DTribitsExProj_ENABLE_Fortran=ON
        -DTribitsExProj_ENABLE_ALL_OPTIONAL_PACKAGES=OFF
        -DTribitsExProj_ENABLE_SECONDARY_TESTED_CODE=ON
        -DTribitsExProj_ENABLE_MixedLang=ON
        -DTribitsExProj_ENABLE_WithSubpackagesA=ON
        -DTribitsExProj_ENABLE_WithSubpackagesB=ON
        -DTribitsExProj_ENABLE_WithSubpackagesC=ON
        -DTribitsExProj_ENABLE_INSTALL_CMAKE_CONFIG_FILES=ON
        -DTPL_ENABLE_SimpleTpl=ON
        -DSimpleTpl_INCLUDE_DIRS=${SimpleTpl_install_${sharedOrStatic}_DIR}/install/include
        -DSimpleTpl_LIBRARY_DIRS=${SimpleTpl_install_${sharedOrStatic}_DIR}/install/lib
        ${buildSharedLibsArg}
        -DCMAKE_INSTALL_PREFIX=${testDir}/install
        ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject
      PASS_REGULAR_EXPRESSION_ALL
        "-- Setting TribitsExProj_ENABLE_WithSubpackages=ON because TribitsExProj_ENABLE_WithSubpackagesA=ON"
        "-- Setting WithSubpackages_ENABLE_WithSubpackagesA=ON because TribitsExProj_ENABLE_WithSubpackagesA=ON"
        "-- Setting WithSubpackages_ENABLE_WithSubpackagesB=ON because TribitsExProj_ENABLE_WithSubpackagesB=ON"
        "-- Setting WithSubpackages_ENABLE_WithSubpackagesC=ON because TribitsExProj_ENABLE_WithSubpackagesC=ON"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_1
      MESSAGE "Build and install TribitsExampleProject locally"
      WORKING_DIRECTORY BUILD
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND make ARGS ${CTEST_BUILD_FLAGS} install

    TEST_2
      MESSAGE "Configure TribitsExampleApp locally"
      WORKING_DIRECTORY app_build
      CMND ${CMAKE_COMMAND} ARGS
        -DCMAKE_PREFIX_PATH=${testDir}/install
        -DTribitsExApp_USE_COMPONENTS=SimpleCxx,MixedLang,WithSubpackages
        ${findByProjectOrPackageArg}
        ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleApp
      PASS_REGULAR_EXPRESSION_ALL
        "${foundProjectOrPackageStr}"
        "-- Configuring done"
        "-- Generating done"
        "-- Build files have been written to: .*/${testName}/app_build"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_3
      MESSAGE "Build TribitsExampleApp"
      WORKING_DIRECTORY app_build
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND make ARGS ${CTEST_BUILD_FLAGS}
      PASS_REGULAR_EXPRESSION_ALL
        "Built target app"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_4
      MESSAGE "Test TribitsExampleApp"
      WORKING_DIRECTORY app_build
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
      PASS_REGULAR_EXPRESSION_ALL
        "Full Deps: ${fullDepsStr}"
        "app_test [.]+   Passed"
        "100% tests passed, 0 tests failed out of 1"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    ${TEST_ENV_ARG}

    ADDED_TEST_NAME_OUT ${testNameBase}_NAME
    )
  # NOTE: The above test ensures that the <ParentPackage>Config.cmake file for
  # a parent package with subpackages gets constructed correctly when optional
  # packages are disabled and when only the subpackages are explicitly enabled
  # (see trilinos/Trilinos#9972 and trilinos/Trilinos#9973).

  if (${testNameBase}_NAME)
    set_tests_properties(${${testNameBase}_NAME}
      PROPERTIES DEPENDS ${SimpleTpl_install_${sharedOrStatic}_NAME} )
  endif()

endfunction()


TribitsExampleApp_NoOptionalPackages(ByProject STATIC)
TribitsExampleApp_NoOptionalPackages(ByPackage SHARED)
#  Don't need to test all the permulations here


################################################################################


function(TribitsExampleApp_ALL_ST_tpl_link_options byProjectOrPackage sharedOrStatic)

  if (byProjectOrPackage STREQUAL "ByProject")
    set(findByProjectOrPackageArg -DTribitsExApp_FIND_INDIVIDUAL_PACKAGES=OFF)
    set(foundProjectOrPackageStr "Found TribitsExProj")
  elseif (byProjectOrPackage STREQUAL "ByPackage")
    set(findByProjectOrPackageArg -DTribitsExApp_FIND_INDIVIDUAL_PACKAGES=ON)
    set(foundProjectOrPackageStr "Found SimpleCxx")
  else()
    message(FATAL_ERROR "Invalid value for findByProjectOrPackageArg='${findByProjectOrPackageArg}'!")
  endif()

  TribitsExampleApp_process_sharedOrStatic_arg()

  set(testBaseName ${CMAKE_CURRENT_FUNCTION}_${byProjectOrPackage}_${sharedOrStatic})
  set(testName ${PACKAGE_NAME}_${testBaseName})
  set(testDir ${CMAKE_CURRENT_BINARY_DIR}/${testName})

  TribitsExampleApp_set_test_env_var(ALWAYS_SET_ENV_VARS)
  # Above, must always set up runtime paths to find upstream TPL since RPATH
  # will not be set with -L<dir> option!

  TribitsExampleApp_GetExpectedAppFullDeps(fullDepsStr
    SimpleTpl SimpleCxx MixedLang WithSubpackagesA WithSubpackagesB WithSubpackagesC)

  tribits_add_advanced_test( ${testBaseName}
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran
    XHOSTTYPE Darwin

    TEST_0
      MESSAGE "Copy source for TribitsExampleProject"
      CMND ${CMAKE_COMMAND}
      ARGS -E copy_directory
        ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject .
      WORKING_DIRECTORY TribitsExampleProject

    TEST_1
      MESSAGE "Write configuration fragment file to deal with semi-colon problem"
      CMND ${CMAKE_COMMAND}
      ARGS
        -DSIMPLE_TPL_INSTALL_BASE=${SimpleTpl_install_${sharedOrStatic}_DIR}/install
        -DLIBDIR_NAME=lib
        -DOUTPUT_CMAKE_FRAG_FILE="${testDir}/SimpleTplOpts.cmake"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/write_simple_tpl_link_options_spec.cmake"

    TEST_2
      MESSAGE "Do the configure of TribitsExampleProject"
      WORKING_DIRECTORY BUILD
      CMND ${CMAKE_COMMAND}
      ARGS
        -C "${testDir}/SimpleTplOpts.cmake"
        ${TribitsExampleProject_COMMON_CONFIG_ARGS}
        -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
        -DTribitsExProj_ENABLE_Fortran=ON
        -DTribitsExProj_ENABLE_ALL_PACKAGES=ON
        -DTribitsExProj_ENABLE_SECONDARY_TESTED_CODE=ON
        -DTribitsExProj_ENABLE_INSTALL_CMAKE_CONFIG_FILES=ON
        ${buildSharedLibsArg}
        -DCMAKE_INSTALL_PREFIX=${testDir}/install
        ${testDir}/TribitsExampleProject

    TEST_3
      MESSAGE "Build and install TribitsExampleProject locally"
      WORKING_DIRECTORY BUILD
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND make ARGS ${CTEST_BUILD_FLAGS} install

    TEST_4
      MESSAGE "Delete source and build directory for TribitsExampleProject"
      CMND ${CMAKE_COMMAND} ARGS -E rm -rf TribitsExampleProject BUILD

    TEST_5
      MESSAGE "Configure TribitsExampleApp locally"
      WORKING_DIRECTORY app_build
      CMND ${CMAKE_COMMAND} ARGS
        -DCMAKE_PREFIX_PATH=${testDir}/install
        -DTribitsExApp_USE_COMPONENTS=SimpleCxx,MixedLang,WithSubpackages
        ${findByProjectOrPackageArg}
        ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleApp
      PASS_REGULAR_EXPRESSION_ALL
        "${foundProjectOrPackageStr}"
        "-- Configuring done"
        "-- Generating done"
        "-- Build files have been written to: .*/${testName}/app_build"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_6
      MESSAGE "Build TribitsExampleApp"
      WORKING_DIRECTORY app_build
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND make ARGS ${CTEST_BUILD_FLAGS}
      PASS_REGULAR_EXPRESSION_ALL
        "Built target app"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_7
      MESSAGE "Test TribitsExampleApp"
      WORKING_DIRECTORY app_build
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
      PASS_REGULAR_EXPRESSION_ALL
        "Full Deps: ${fullDepsStr}"
        "app_test [.]+   Passed"
        "100% tests passed, 0 tests failed out of 1"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    ${TEST_ENV_ARG}

    ADDED_TEST_NAME_OUT ${testNameBase}_NAME
    )

  if (${testNameBase}_NAME)
    set_tests_properties(${${testNameBase}_NAME}
      PROPERTIES DEPENDS ${SimpleTpl_install_${sharedOrStatic}_NAME} )
  endif()

endfunction()
# NOTE: Above, it seems you always have to set LD_LIBRARY_PATH because CMake
# does not put in RPATH if you only specifiy the TPL directory using -L<dir>.
# If you use the entire library file, CMake will put in RPATH correctly.


TribitsExampleApp_ALL_ST_tpl_link_options(ByProject STATIC)
TribitsExampleApp_ALL_ST_tpl_link_options(ByProject SHARED)
TribitsExampleApp_ALL_ST_tpl_link_options(ByPackage STATIC)
TribitsExampleApp_ALL_ST_tpl_link_options(ByPackage SHARED)


################################################################################


function(TribitsExampleApp_ALL_ST_buildtree sharedOrStatic)

  TribitsExampleApp_process_sharedOrStatic_arg()

  if ( (CYGWIN OR WIN32) AND sharedOrStatic STREQUAL "SHARED")
    set(NOT_CYGWIN_OR_WIN32_SHARED FALSE)
  else()
    set(NOT_CYGWIN_OR_WIN32_SHARED TRUE)
  endif()
  # NOTE: It is just too hard and hard to maintain to prepend all the
  # directories to PATH for all of the upstream TribitsExProj libraries
  # scattered around the build tree when you are on Windows and have DLLs.

  set(testBaseName ${CMAKE_CURRENT_FUNCTION}_${sharedOrStatic})
  set(testName ${PACKAGE_NAME}_${testBaseName})
  set(testDir ${CMAKE_CURRENT_BINARY_DIR}/${testName})

  TribitsExampleApp_GetExpectedAppFullDeps(fullDepsStr
    SimpleCxx MixedLang WithSubpackagesA WithSubpackagesB WithSubpackagesC)

  tribits_add_advanced_test( ${testBaseName}
    OVERALL_WORKING_DIRECTORY  TEST_NAME
    OVERALL_NUM_MPI_PROCS  1
    EXCLUDE_IF_NOT_TRUE  ${PROJECT_NAME}_ENABLE_Fortran  NOT_CYGWIN_OR_WIN32_SHARED
    XHOSTTYPE  Darwin

    TEST_0
      MESSAGE "Do the configure of TribitsExampleProject"
      CMND ${CMAKE_COMMAND}
      ARGS
        ${TribitsExampleProject_COMMON_CONFIG_ARGS}
        -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
        -DTribitsExProj_ENABLE_Fortran=ON
        -DTribitsExProj_ENABLE_ALL_PACKAGES=ON
        -DTribitsExProj_ENABLE_SECONDARY_TESTED_CODE=ON
        -DTribitsExProj_ENABLE_INSTALL_CMAKE_CONFIG_FILES=ON
        ${buildSharedLibsArg}
        ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject

    TEST_1
      MESSAGE "Build TribitsExampleProject only (no install)"
      CMND make ARGS ${CTEST_BUILD_FLAGS}

    TEST_2
      MESSAGE "Configure TribitsExampleApp locally"
      WORKING_DIRECTORY app_build
      CMND ${CMAKE_COMMAND} ARGS
        -DCMAKE_PREFIX_PATH=${testDir}/cmake_packages
        -DTribitsExApp_FIND_INDIVIDUAL_PACKAGES=TRUE
        -DTribitsExApp_USE_COMPONENTS=SimpleCxx,MixedLang,WithSubpackages
        ${findByProjectOrPackageArg}
        ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleApp
      PASS_REGULAR_EXPRESSION_ALL
        "Found SimpleCxx"
        "Found MixedLang"
        "-- Configuring done"
        "-- Generating done"
        "-- Build files have been written to: .*/${testName}/app_build"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_3
      MESSAGE "Build TribitsExampleApp"
      WORKING_DIRECTORY app_build
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND make ARGS ${CTEST_BUILD_FLAGS}
      PASS_REGULAR_EXPRESSION_ALL
        "Built target app"
      ALWAYS_FAIL_ON_NONZERO_RETURN

    TEST_4
      MESSAGE "Test TribitsExampleApp"
      WORKING_DIRECTORY app_build
      SKIP_CLEAN_WORKING_DIRECTORY
      CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
      PASS_REGULAR_EXPRESSION_ALL
        "Full Deps: ${fullDepsStr}"
        "app_test [.]+   Passed"
        "100% tests passed, 0 tests failed out of 1"
      ALWAYS_FAIL_ON_NONZERO_RETURN

      ${ENV_HACK_FOR_SIMPLETPL_${sharedOrStatic}_ENVIRONMENT_ARG}

      )

endfunction()
#
# NOTE: The test above validates that <Package>Config.cmake files work from
# the build tree.  But we don't run the test for SHARED builds on Cygwin
# because having to append all of the paths to the libraries in the build tree
# of TribitsExProj2 is just too much work.  The testing on Linux systems is
# enough I think.


TribitsExampleApp_ALL_ST_buildtree(STATIC)
TribitsExampleApp_ALL_ST_buildtree(SHARED)
