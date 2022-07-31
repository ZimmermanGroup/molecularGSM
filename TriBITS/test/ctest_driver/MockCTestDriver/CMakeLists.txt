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

#
# These tests check the dependency handling behavior of the
# tribits_ctest_driver() function to see what packages will get processed and
# tested and which will not for a number of different scenarios.  This uses
# the mock TriBITS project MockTrilinos which does not have any code to build.
# It just has Dependencies.make files.
#


include(CMakeParseArguments)
include(AdvancedSet)


advanced_set(Trilinos_CTEST_COMMAND ctest CACHE STRING
  "CTest executable used to drive unit tests")

# The mock test project 
set(MOCK_PROJECT_NAME Trilinos)

set(TRIBITS_MOCK_TRILINOS_DIR "examples/MockTrilinos")

set(MOCK_TRILINOS_DIR ${${PROJECT_NAME}_TRIBITS_DIR}/${TRIBITS_MOCK_TRILINOS_DIR})


function(create_ctest_dependency_handling_test_case TEST_NAME)

  #message("CREATE_CTEST_DEPENDENCY_HANDLING_TEST_CASE: ${TEST_CASE} ${ARGN}")

  cmake_parse_arguments(
     #prefix
     PARSE
     #options
     "SKIP_STANDARD_DEPS_CHECK"
     # one_value_keywords
     ""
     # multi_value_keywords
     "COPY_FILES;ARGS;PASS_REGULAR_EXPRESSION_ALL;COMPARE_DEPS_FILE"
     ${ARGN}
     )
  
  set(TEST_CORE_NAME MockCTestDriver_${TEST_NAME})
  set(TEST_FULL_NAME ${PACKAGE_NAME}_${TEST_CORE_NAME})

  set(TEST_BLOCKS "")
  set(TEST_IDX 0)

  if (PARSE_COPY_FILES)

    list(LENGTH  PARSE_COPY_FILES  COPY_FILES_LEN)
    math(EXPR NUM_FILE_COPIES "${COPY_FILES_LEN}/2")
    math(EXPR COPY_FILES_LAST_IDX "${NUM_FILE_COPIES}-1")

    set(NUM_COPY_FILE_ARGS 2)
    set(FILE_FROM_OFFSET 0)
    set(FILE_TO_OFFSET 1)

    foreach(COPY_FILE_IDX RANGE ${COPY_FILES_LAST_IDX})

      # FILE_FROM
      math(EXPR FILE_FROM_IDX "${COPY_FILE_IDX}*${NUM_COPY_FILE_ARGS}+${FILE_FROM_OFFSET}")
      list(GET PARSE_COPY_FILES ${FILE_FROM_IDX} FILE_FROM )
      #print_var(FILE_FROM)
 
      # FILE_TO
      math(EXPR FILE_TO_IDX "${COPY_FILE_IDX}*${NUM_COPY_FILE_ARGS}+${FILE_TO_OFFSET}")
      list(GET PARSE_COPY_FILES ${FILE_TO_IDX} FILE_TO )
      #print_var(FILE_TO)

      set(TEST_BLOCKS ${TEST_BLOCKS}
        TEST_${TEST_IDX} CMND ${CMAKE_COMMAND}
          ARGS -E copy ${FILE_FROM} ${CMAKE_CURRENT_BINARY_DIR}/${TEST_FULL_NAME}/${FILE_TO}
        )

      math(EXPR TEST_IDX "${TEST_IDX}+1")

    endforeach()

  endif()

  set(TEST_BLOCKS ${TEST_BLOCKS}
    TEST_${TEST_IDX} CMND env
      ARGS
        ${MOCK_PROJECT_NAME}_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
        CTEST_DEPENDENCY_HANDLING_UNIT_TESTING=ON  # Put into unit test mode
        ${MOCK_PROJECT_NAME}_CTEST_COMMAND=${CMAKE_CTEST_COMMAND}
        ${MOCK_PROJECT_NAME}_PACKAGES=  # Set back to default
        CTEST_EXPLICITLY_ENABLE_IMPLICITLY_ENABLED_PACKAGES= # Set back to default
        CTEST_ENABLE_MODIFIED_PACKAGES_ONLY=OFF # Set back to default
        ${MOCK_PROJECT_NAME}_PRE_REPOSITORIES= # Set back to default
        ${MOCK_PROJECT_NAME}_EXTRA_REPOSITORIES= # Set back to default
        CTEST_TEST_TYPE= # Set back to default
        CTEST_DO_UPDATES=ON # Set back to default
        CTEST_DASHBOARD_ROOT=  # Must be empty
        CTEST_DO_SUBMIT=OFF # Must be off
        CTEST_START_WITH_EMPTY_BINARY_DIRECTORY=OFF  # Must be off!
        ${PARSE_ARGS}
        ${CMAKE_CTEST_COMMAND} -S ${CMAKE_CURRENT_SOURCE_DIR}/unit_test_driver.cmake
      PASS_REGULAR_EXPRESSION_ALL ${PARSE_PASS_REGULAR_EXPRESSION_ALL}
    )

  math(EXPR TEST_IDX "${TEST_IDX}+1")

  if (PARSE_COMPARE_DEPS_FILE)
    set(TEST_BLOCKS ${TEST_BLOCKS}
      TEST_${TEST_IDX} CMND ${CMAKE_COMMAND}
        ARGS -E compare_files
          "${PARSE_COMPARE_DEPS_FILE}"
          "${CMAKE_CURRENT_BINARY_DIR}/${TEST_FULL_NAME}/TrilinosPackageDependencies.xml"
      )
  endif()

  tribits_add_advanced_test( ${TEST_CORE_NAME}
    OVERALL_WORKING_DIRECTORY TEST_NAME
    ${TEST_BLOCKS}
    OVERALL_NUM_MPI_PROCS 1
    )

endfunction()


#
# Test package enables/disables
#


create_ctest_dependency_handling_test_case(
  EnableAllDefault
  PASS_REGULAR_EXPRESSION_ALL
    "Enabling all SE packages that are not currently disabled because of Trilinos_ENABLE_ALL_PACKAGES=ON"
    "Final set of enabled packages:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Stratimikos RBGen 19"
    "Final set of non-enabled packages:  Stokhos Sacado Belos Phalanx Panzer 5"
  COMPARE_DEPS_FILE
     "${CMAKE_CURRENT_SOURCE_DIR}/TrilinosPackageDependencies.Trilinos.xml"
  )


create_ctest_dependency_handling_test_case(
  EnableAllSecondaryTested
  ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Enabling all SE packages that are not currently disabled because of Trilinos_ENABLE_ALL_PACKAGES=ON"
    "Final set of enabled packages:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer 23"
    "Final set of non-enabled packages:  Stokhos 1"
  )


create_ctest_dependency_handling_test_case(
  EnableTeuchos
  ARGS Trilinos_PACKAGES=Teuchos Trilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Experimental
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled packages:  Teuchos 1"
    "Final set of non-enabled packages:  TrilinosFramework RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Stokhos Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer 23"
    "Processing current package Teuchos: libs='ON', tests='ON'"
    "CONFIGURE_OPTIONS = '-DTrilinos_TRIBITS_DIR=.+[;]-DCTEST_USE_LAUNCHERS:BOOL=1[;]-DTrilinos_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=ON[;]-DTrilinos_WARNINGS_AS_ERRORS_FLAGS:STRING=-DummyErrFlags[;]-DTrilinos_ALLOW_NO_PACKAGES:BOOL=ON[;]-DTrilinos_DISABLE_ENABLED_FORWARD_DEP_PACKAGES=ON[;]-DTrilinos_ENABLE_SECONDARY_TESTED_CODE:BOOL=OFF[;]-DTrilinos_EXTRAREPOS_FILE:STRING=.*./MockTrilinos/cmake.ExtraRepositoriesList.cmake[;]-DTrilinos_IGNORE_MISSING_EXTRA_REPOSITORIES:BOOL=ON[;]-DTrilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE:STRING=Experimental[;]-DTrilinos_ENABLE_TESTS:BOOL=ON[;]-DTrilinos_DEFINE_MISSING_PACKAGE_LIBS_TARGETS=ON[;]-DTrilinos_ENABLE_Teuchos:BOOL=ON'"
  )
# NOTE: The above test pins down the form of the configure options passed to
# different package configures to make sure everything in there.


create_ctest_dependency_handling_test_case(
  EnableRTOp_DisableTeuchos
  ARGS Trilinos_PACKAGES=RTOp Trilinos_EXCLUDE_PACKAGES=Teuchos
  PASS_REGULAR_EXPRESSION_ALL
    "Explicitly enabled packages on input .by user.:  RTOp 1"
    "NOTE: Setting Trilinos_ENABLE_RTOp=OFF which was 'ON' because RTOp has a required library dependence on disabled package Teuchos but Trilinos_DISABLE_ENABLED_FORWARD_DEP_PACKAGES=ON!"
    "Final set of enabled packages:  0"
    "Trilinos_NUM_ENABLED_PACKAGES=0:  Exiting the script!"
  )
# NOTE: The above test ensures that disabled packages turn off explicitly
#  enabled downstream packages.


create_ctest_dependency_handling_test_case(
  EnableTeuchosRTOp_DisableRTOp
  ARGS Trilinos_PACKAGES="Teuchos,RTOp" Trilinos_EXCLUDE_PACKAGES=RTOp
  PASS_REGULAR_EXPRESSION_ALL
    "Disabling excluded package RTOp ..."
    "Enabling explicitly set package Teuchos ..."
    "NOT enabling explicitly set package RTOp since it was explicitly excluded"
    "Explicitly enabled packages on input .by user.:  Teuchos 1"
    "Final set of enabled packages:  Teuchos 1"
    "Trilinos_NUM_ENABLED_PACKAGES=1:  Configuring packages!"
  )
# NOTE: The above test ensures that disabled packages turn off explicitly
#  enabled downstream packages.


create_ctest_dependency_handling_test_case(
  EnableTeuchos_DisableTPLBLAS
  ARGS Trilinos_PACKAGES=Teuchos Trilinos_EXCLUDE_TPLS=BLAS
  PASS_REGULAR_EXPRESSION_ALL
    "Explicitly enabled packages on input .by user.:  Teuchos 1"
    "Explicitly disabled TPLs on input .by user or by default.:  BLAS 1"
    "NOTE: Setting Trilinos_ENABLE_Teuchos=OFF which was 'ON' because Teuchos has a required library dependence on disabled TPL BLAS but Trilinos_DISABLE_ENABLED_FORWARD_DEP_PACKAGES=ON!"
    "Final set of enabled packages:  0"
    "Trilinos_NUM_ENABLED_PACKAGES=0:  Exiting the script!"
  )
# NOTE: The above test ensures that disabled TPLS turn off explicitly
#  enabled downstream packages.


create_ctest_dependency_handling_test_case(
  EnableThyra
  ARGS Trilinos_PACKAGES=Thyra Trilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Experimental
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Triutils Tpetra EpetraExt Thyra 8"
    "Final set of non-enabled packages:  TrilinosFramework Shards Stokhos Sacado Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer 16"
    "Processing current package Teuchos: libs='ON', tests=''"
    "CONFIGURE_OPTIONS = '-DTrilinos_TRIBITS_DIR=.+-DCTEST_USE_LAUNCHERS:BOOL=1[;]-DTrilinos_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=ON[;]-DTrilinos_WARNINGS_AS_ERRORS_FLAGS:STRING=-DummyErrFlags[;]-DTrilinos_ALLOW_NO_PACKAGES:BOOL=ON[;]-DTrilinos_DISABLE_ENABLED_FORWARD_DEP_PACKAGES=ON[;]-DTrilinos_ENABLE_SECONDARY_TESTED_CODE:BOOL=OFF[;]-DTrilinos_EXTRAREPOS_FILE:STRING=.+/examples/MockTrilinos/cmake/ExtraRepositoriesList.cmake[;]-DTrilinos_IGNORE_MISSING_EXTRA_REPOSITORIES:BOOL=ON[;]-DTrilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE:STRING=Experimental[;]-DTrilinos_ENABLE_TESTS:BOOL=[;]-DTrilinos_DEFINE_MISSING_PACKAGE_LIBS_TARGETS=ON[;]-DTrilinos_ENABLE_Teuchos:BOOL=ON'"
    "Processing current package Epetra: libs='ON', tests=''"
    "Processing current package Thyra: libs='ON', tests='ON'"
    "CONFIGURE_OPTIONS = '-DTrilinos_TRIBITS_DIR=.+-DCTEST_USE_LAUNCHERS:BOOL=1[;]-DTrilinos_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=ON[;]-DTrilinos_WARNINGS_AS_ERRORS_FLAGS:STRING=-DummyErrFlags[;]-DTrilinos_ALLOW_NO_PACKAGES:BOOL=ON[;]-DTrilinos_DISABLE_ENABLED_FORWARD_DEP_PACKAGES=ON[;]-DTrilinos_ENABLE_SECONDARY_TESTED_CODE:BOOL=OFF[;]-DTrilinos_EXTRAREPOS_FILE:STRING=.+/examples/MockTrilinos/cmake/ExtraRepositoriesList.cmake[;]-DTrilinos_IGNORE_MISSING_EXTRA_REPOSITORIES:BOOL=ON[;]-DTrilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE:STRING=Experimental[;]-DTrilinos_ENABLE_TESTS:BOOL=ON[;]-DTrilinos_ENABLE_EpetraExt:BOOL=[;]-DTrilinos_DEFINE_MISSING_PACKAGE_LIBS_TARGETS=ON[;]-DTrilinos_ENABLE_Thyra:BOOL=ON'"
  )
# NOTE: The above test pins down the form of the configure options passed to
# different package configures to make sure everything in there.


create_ctest_dependency_handling_test_case(
  EnableThyra_EnableAllForwardDepPackages
  ARGS Trilinos_PACKAGES=Thyra Trilinos_ENABLE_ALL_FORWARD_DEP_PACKAGES=TRUE
    Trilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Experimental
    CTEST_EXPLICITLY_ENABLE_IMPLICITLY_ENABLED_PACKAGES=FALSE
  PASS_REGULAR_EXPRESSION_ALL
    "-- ENV_Trilinos_ENABLE_ALL_FORWARD_DEP_PACKAGES='TRUE'"
    "-- Trilinos_ENABLE_ALL_FORWARD_DEP_PACKAGES='TRUE'"
    "Explicitly enabled SE packages on input [(]by user[)]:  Thyra 1"
    "Sweep forward enabling all forward library dependent packages because Trilinos_ENABLE_ALL_FORWARD_DEP_PACKAGES=ON"
    "-- Setting Trilinos_ENABLE_Stratimikos=ON because Trilinos_ENABLE_ThyraCoreLibs=ON"
    "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Triutils Tpetra EpetraExt Thyra Isorropia AztecOO Galeri Amesos Ifpack ML Stratimikos 15"
    "Final set of enabled SE packages:  Teuchos RTOp Epetra Zoltan Triutils Tpetra EpetraExt ThyraCoreLibs ThyraEpetra ThyraEpetraExt ThyraTpetra Thyra Isorropia AztecOO Galeri Amesos Ifpack ML Stratimikos 19"
    "Final set of packages to be explicitly processed by CTest/CDash:  Thyra Stratimikos 2"
    "Processing current package Thyra: libs='ON', tests='ON'"
    "CONFIGURE_OPTIONS = '.*-DTrilinos_ENABLE_Thyra:BOOL=ON'"
    "Processing current package Stratimikos: libs='ON', tests='ON'"
    "CONFIGURE_OPTIONS = '.*-DTrilinos_ENABLE_Stratimikos:BOOL=ON'"
  )
# NOTE: The above test makes sure that
# Trilinos_ENABLE_ALL_FORWARD_DEP_PACKAGES is read from the env and has the
# intended impact.


create_ctest_dependency_handling_test_case(
  EnableEpetraThyra_NoProcessImplicitlyEnabled
  ARGS Trilinos_PACKAGES=Epetra,Thyra
    CTEST_EXPLICITLY_ENABLE_IMPLICITLY_ENABLED_PACKAGES=OFF
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Triutils Tpetra EpetraExt Thyra 8"
    "Final set of packages to be explicitly processed by CTest/CDash:  Epetra Thyra 2"
    "0. Processing current package Epetra: libs=.ON., tests=.ON."
    "1. Processing current package Thyra: libs=.ON., tests=.ON."
  )
# NOTE: The above test shows that only 


create_ctest_dependency_handling_test_case(
  EnableThyra_ExcludeEpetra
  ARGS Trilinos_PACKAGES=Thyra Trilinos_EXCLUDE_PACKAGES=Epetra
  PASS_REGULAR_EXPRESSION_ALL
    "Explicitly enabled packages on input .* Thyra 1"
    "Explicitly disabled packages on input .*  Epetra Stokhos 2"
    "Final set of enabled packages:  Teuchos RTOp Tpetra Thyra 4"
    "Final set of non-enabled packages:  TrilinosFramework Epetra Zoltan Shards Triutils EpetraExt Stokhos Sacado Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer 20"
    "Processing current package Teuchos: libs='ON', tests=''"
    "Processing current package Thyra: libs='ON', tests='ON'"
  )


create_ctest_dependency_handling_test_case(
  EnableAll_EnablePhalanx
  ARGS Trilinos_ADDITIONAL_PACKAGES=Phalanx
  PASS_REGULAR_EXPRESSION_ALL
    "Enabling explicitly set package Phalanx"
    "Explicitly enabled packages on input .*  Phalanx 1"
    "Explicitly disabled packages on input .*  Stokhos 1"
    "Enabling all SE packages that are not currently disabled because of Trilinos_ENABLE_ALL_PACKAGES=ON"
    "Final set of enabled packages:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Stratimikos RBGen Phalanx 21"
    "Final set of non-enabled packages:  Stokhos Belos Panzer 3"
    "Processing current package RBGen: libs='ON', tests='ON'"
    "Processing current package Phalanx: libs='ON', tests='ON'"
  )


create_ctest_dependency_handling_test_case(
  EnableAll_EnableStokhos
  ARGS Trilinos_ADDITIONAL_PACKAGES=Stokhos
  PASS_REGULAR_EXPRESSION_ALL
    "Enabling explicitly set package Stokhos which was default or otherwise disabed!"
    "Explicitly enabled packages on input .*  Stokhos 1"
    "Explicitly disabled packages on input .*  0"
    "Enabling all SE packages that are not currently disabled because of Trilinos_ENABLE_ALL_PACKAGES=ON"
    "Final set of enabled packages:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Stokhos Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Stratimikos RBGen 20"
    "Final set of non-enabled packages:  Sacado Belos Phalanx Panzer 4"
    "Processing current package Stokhos: libs='ON', tests='ON'"
    "Processing current package RBGen: libs='ON', tests='ON'"
  )


create_ctest_dependency_handling_test_case(
  EnableAllSecondaryTested_EnableStokhos
  ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON Trilinos_ADDITIONAL_PACKAGES=Stokhos
  PASS_REGULAR_EXPRESSION_ALL
    "Enabling explicitly set package Stokhos"
    "Explicitly enabled packages on input .*  Stokhos 1"
    "Explicitly disabled packages on input .*  0"
    "Enabling all SE packages that are not currently disabled because of Trilinos_ENABLE_ALL_PACKAGES=ON"
    "Final set of enabled packages:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Stokhos Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer 24"
    "Final set of non-enabled packages:  0"
    "Processing current package Stokhos: libs='ON', tests='ON'"
    "Processing current package RBGen: libs='ON', tests='ON'"
    "Processing current package Phalanx: libs='ON', tests='ON'"
  )


create_ctest_dependency_handling_test_case(
  CI_EnableAllSecondaryTestedOff
  ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=OFF Trilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Continuous
  PASS_REGULAR_EXPRESSION_ALL
    "execute_process.+git.+preCopyrightTrilinos.+preCopyrightTrilinos.fetch.out"
    "Explicitly enabled packages on input .*  0"
    "Explicitly disabled packages on input .*  Stokhos Stalix 2"
    "Enabling all SE packages that are not currently disabled because of Trilinos_ENABLE_ALL_PACKAGES=ON"
    "NOTE: Not Setting Trilinos_ENABLE_Belos=ON even though Stratimikos has an optional dependence on Belos because Trilinos_ENABLE_SECONDARY_TESTED_CODE=OFF"
    "Final set of enabled packages:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Stratimikos RBGen 19"
    "Final set of packages to be explicitly processed by CTest/CDash:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Stratimikos RBGen 19"
    "Final set of non-enabled packages:  Stokhos Sacado Belos Phalanx Panzer Teko Stalix 7"
    "CONFIGURE_OPTIONS.+Trilinos_ENABLE_SECONDARY_TESTED_CODE:BOOL=OFF.+-DTrilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE:STRING=Continuous.+"
    "Processing current package Teuchos: libs='ON', tests='ON'"
    "CONFIGURE_OPTIONS.+Trilinos_ENABLE_Teuchos:BOOL=ON"
  COMPARE_DEPS_FILE
     "${CMAKE_CURRENT_SOURCE_DIR}/TrilinosPackageDependencies.Continuous.xml"
  )


create_ctest_dependency_handling_test_case(
  CI_EnableAllSecondaryTested
  ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON Trilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Continuous
  PASS_REGULAR_EXPRESSION_ALL
    "execute_process.+git.+preCopyrightTrilinos.+preCopyrightTrilinos.fetch.out"
    "Explicitly enabled packages on input .*  0"
    "Explicitly disabled packages on input .*  Stokhos Stalix 2"
    "Enabling all SE packages that are not currently disabled because of Trilinos_ENABLE_ALL_PACKAGES=ON"
    "Final set of enabled packages:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer Teko 24"
    "Final set of packages to be explicitly processed by CTest/CDash:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer Teko 24"
    "Final set of non-enabled packages:  Stokhos Stalix 2"
    "CONFIGURE_OPTIONS.+Trilinos_ENABLE_SECONDARY_TESTED_CODE:BOOL=ON.+-DTrilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE:STRING=Continuous.+"
    "Processing current package Teuchos: libs='ON', tests='ON'"
    "CONFIGURE_OPTIONS.+Trilinos_ENABLE_Teuchos:BOOL=ON"
    "Processing current package Phalanx: libs='ON', tests='ON'"
  COMPARE_DEPS_FILE
     "${CMAKE_CURRENT_SOURCE_DIR}/TrilinosPackageDependencies.Continuous.xml"
  )


# Check with an extra repo enabled and assert all of the internal logic
create_ctest_dependency_handling_test_case(
  CI_EnableAllSecondaryTested_EnableStalix
  ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON Trilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Continuous
    Trilinos_ADDITIONAL_PACKAGES=Stalix
  PASS_REGULAR_EXPRESSION_ALL
    "Explicitly enabled packages on input .*  Stalix 1"
    "Explicitly disabled packages on input .*  Stokhos 1"
    "Enabling all SE packages that are not currently disabled because of Trilinos_ENABLE_ALL_PACKAGES=ON"
    "Final set of enabled packages:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer Teko Stalix 25"
    "Final set of non-enabled packages:  Stokhos 1"
    "Processing current package Phalanx: libs='ON', tests='ON'"
    "Processing current package Stalix: libs='ON', tests='ON'"
  )


# Check repo operations for an update
create_ctest_dependency_handling_test_case(
  CI_TestGitUpdateCommands
  ARGS
    Trilinos_REPOSITORY_LOCATION=https://some-git-url/Trilinos.git
    Trilinos_GIT_REPOSITORY_REMOTE=other-remote
    CTEST_UPDATE_ARGS="--reference <some-base-dir>/Trilinos"
    Trilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Continuous
    Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
    Trilinos_ADDITIONAL_PACKAGES=Stalix
  PASS_REGULAR_EXPRESSION_ALL
    "CTEST_CHECKOUT_COMMAND=./somebasedir/git. clone -o other-remote --reference <some-base-dir>/Trilinos https://some-git-url/Trilinos[.]git"
    "preCopyrightTrilinos: Doing GIT update from URL 'url1:/git/preCopyrightTrilinos' to dir '.*/MockTrilinos/preCopyrightTrilinos'"
     "CTEST_GIT_UPDATE_CUSTOM=${CMAKE_COMMAND}[;]-DGIT_EXE=/somebasedir/git[;]-DREMOTE_NAME=other-remote[;]-DBRANCH=[;]-DUNIT_TEST_MODE=ON[;]-DOUTPUT_FILE=.*/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands/UpdateCommandsOutput.txt[;]-P[;].*/tribits/ctest_driver/tribits_ctest_update_commands_wrapper.cmake"
     "execute_process.COMMAND./somebasedir/git[;]fetch[;]other-remote[;]TIMEOUT[;]600[;]WORKING_DIRECTORY[;].*/MockTrilinos/preCopyrightTrilinos.OUTPUT_FILE[;].*/test/ctest_driver/MockCTestDriver/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands/preCopyrightTrilinos.fetch.out."
     "execute_process.COMMAND./somebasedir/git[;]clean[;]-fdx[;]WORKING_DIRECTORY[;].*/MockTrilinos/preCopyrightTrilinos.OUTPUT_FILE[;].*/test/ctest_driver/MockCTestDriver/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands/preCopyrightTrilinos.clean.out."
    "execute_process.COMMAND./somebasedir/git[;]reset[;]--hard[;]HEAD[;]WORKING_DIRECTORY[;].*/MockTrilinos/preCopyrightTrilinos.OUTPUT_FILE[;].*/test/ctest_driver/MockCTestDriver/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands/preCopyrightTrilinos.reset.out."
     "execute_process.COMMAND./somebasedir/git[;]reset[;]--hard[;]@{u}[;]WORKING_DIRECTORY[;].*/MockTrilinos/preCopyrightTrilinos.OUTPUT_FILE[;].*/test/ctest_driver/MockCTestDriver/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands/preCopyrightTrilinos.set_branch.out."
    "Dakota: Doing initial GIT clone/checkout from URL 'url3:/git/Dakota' to dir 'packages/TriKota/Dakota' ..."
    "execute_process.COMMAND[;]/somebasedir/git[;]clone[;]-o[;]other-remote[;]url3:/git/Dakota[;]packages/TriKota/Dakota[;]WORKING_DIRECTORY[;].*/tribits/examples/MockTrilinos[;]OUTPUT_FILE[;].*/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands/Dakota.clone.out"
     "execute_process.COMMAND[;]/somebasedir/git[;]fetch[;]other-remote[;]TIMEOUT[;]600[;]WORKING_DIRECTORY[;].*/MockTrilinos/packages/TriKota/Dakota[;]OUTPUT_FILE[;].*/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands/Dakota.fetch.out"
    "execute_process.COMMAND[;]/somebasedir/git[;]clean[;]-fdx[;]WORKING_DIRECTORY[;].*/MockTrilinos/packages/TriKota/Dakota[;]OUTPUT_FILE[;].*/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands/Dakota.clean.out"
    "execute_process.COMMAND[;]/somebasedir/git[;]reset[;]--hard[;]HEAD[;]WORKING_DIRECTORY[;].*/MockTrilinos/packages/TriKota/Dakota[;]OUTPUT_FILE[;].*/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands/Dakota.reset.out"
    "execute_process.COMMAND[;]/somebasedir/git[;]reset[;]--hard[;]@{u}[;]WORKING_DIRECTORY[;].*/MockTrilinos/packages/TriKota/Dakota[;]OUTPUT_FILE[;].*/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands/Dakota.set_branch.out"
    "Dakota: Doing initial GIT clone/checkout from URL 'url3:/git/Dakota' to dir 'packages/TriKota/Dakota' ..."
    "execute_process.COMMAND[;].*/python.*[;].*/python_utils/gitdist[;]--dist-no-color[;]log[;]--pretty=format.*[;]--name-status[;]-C[;]ORIG_HEAD..HEAD[;]WORKING_DIRECTORY[;].*/examples/MockTrilinos[;]OUTPUT_FILE[;].*/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands/Updates.txt"
    "Adding POST extra Continuous repository preCopyrightTrilinos"
    "Adding POST extra Continuous repository Dakota"
    "Trilinos_NUM_PACKAGES='24'"
    "Trilinos_NUM_TPLS='19'"
    "Reading list of POST extra packages from .*/MockTrilinos/preCopyrightTrilinos/PackagesList.cmake"
    "Trilinos_NUM_PACKAGES='26'"
    "Trilinos_NUM_TPLS='20'"
    "Skipping reading packages and TPLs for POST extra repo Dakota because marked NOPACKAGES"
    )


# Test repo operations for Trilinos_BRANCH=other-branch (#130)
create_ctest_dependency_handling_test_case(
  CI_TestGitUpdateCommands_Branch
  ARGS
    Trilinos_REPOSITORY_LOCATION=https://some-git-url/Trilinos.git
    CTEST_UPDATE_ARGS="--reference <some-base-dir>/Trilinos"
    Trilinos_BRANCH=other-branch
    Trilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Continuous
    Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
    Trilinos_ADDITIONAL_PACKAGES=Stalix
  PASS_REGULAR_EXPRESSION_ALL
    "CTEST_CHECKOUT_COMMAND=./somebasedir/git. clone -b other-branch -o origin --reference <some-base-dir>/Trilinos https://some-git-url/Trilinos[.]git"
    "CTEST_GIT_UPDATE_CUSTOM=${CMAKE_COMMAND}[;]-DGIT_EXE=/somebasedir/git[;]-DREMOTE_NAME=origin[;]-DBRANCH=other-branch[;]-DUNIT_TEST_MODE=ON[;]-DOUTPUT_FILE=.*/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch/UpdateCommandsOutput.txt[;]-P[;].*/tribits/ctest_driver/tribits_ctest_update_commands_wrapper.cmake"
    
    "For extra repos, doing switch to branch other-branch"
    "preCopyrightTrilinos: Doing GIT update from URL 'url1:/git/preCopyrightTrilinos' to dir '.*/MockTrilinos/preCopyrightTrilinos'"
    "execute_process.COMMAND./somebasedir/git[;]fetch[;]origin[;]TIMEOUT[;]600[;]WORKING_DIRECTORY[;].*/MockTrilinos/preCopyrightTrilinos.OUTPUT_FILE[;].*/test/ctest_driver/MockCTestDriver/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch/preCopyrightTrilinos.fetch.out."
    "execute_process.COMMAND./somebasedir/git[;]clean[;]-fdx[;]WORKING_DIRECTORY[;].*/MockTrilinos/preCopyrightTrilinos.OUTPUT_FILE[;].*/test/ctest_driver/MockCTestDriver/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch/preCopyrightTrilinos.clean.out."
    "execute_process.COMMAND./somebasedir/git[;]reset[;]--hard[;]HEAD[;]WORKING_DIRECTORY[;].*/MockTrilinos/preCopyrightTrilinos.OUTPUT_FILE[;].*/test/ctest_driver/MockCTestDriver/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch/preCopyrightTrilinos.reset.out."
    "execute_process.COMMAND./somebasedir/git[;]checkout[;]-B[;]other-branch[;]--track[;]origin/other-branch[;]WORKING_DIRECTORY[;].*/MockTrilinos/preCopyrightTrilinos.OUTPUT_FILE[;].*/test/ctest_driver/MockCTestDriver/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch/preCopyrightTrilinos.set_branch.out."
    "Dakota: Doing initial GIT clone/checkout from URL 'url3:/git/Dakota' to dir 'packages/TriKota/Dakota' ..."
    "execute_process.COMMAND[;]/somebasedir/git[;]clone[;]-b[;]other-branch[;]-o[;]origin[;]url3:/git/Dakota[;]packages/TriKota/Dakota[;]WORKING_DIRECTORY[;].*/tribits/examples/MockTrilinos[;]OUTPUT_FILE[;].*/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch/Dakota.clone.out"
    "execute_process.COMMAND[;]/somebasedir/git[;]clean[;]-fdx[;]WORKING_DIRECTORY[;].*/MockTrilinos/packages/TriKota/Dakota[;]OUTPUT_FILE[;].*/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch/Dakota.clean.out"
    "execute_process.COMMAND[;]/somebasedir/git[;]reset[;]--hard[;]HEAD[;]WORKING_DIRECTORY[;].*/MockTrilinos/packages/TriKota/Dakota[;]OUTPUT_FILE[;].*/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch/Dakota.reset.out"
    "execute_process.COMMAND[;]/somebasedir/git[;]fetch[;]origin[;]TIMEOUT[;]600[;]WORKING_DIRECTORY[;].*/MockTrilinos/packages/TriKota/Dakota[;]OUTPUT_FILE[;].*/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch/Dakota.fetch.out"
    "execute_process.COMMAND[;]/somebasedir/git[;]checkout[;]-B[;]other-branch[;]--track[;]origin/other-branch[;]WORKING_DIRECTORY[;].*/MockTrilinos/packages/TriKota/Dakota[;]OUTPUT_FILE[;].*/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch/Dakota.set_branch.out"
    "Adding POST extra Continuous repository preCopyrightTrilinos"
    "Adding POST extra Continuous repository Dakota"
    "Trilinos_NUM_PACKAGES='24'"
    "Trilinos_NUM_TPLS='19'"
    "Reading list of POST extra packages from .*/MockTrilinos/preCopyrightTrilinos/PackagesList.cmake"
    "Trilinos_NUM_PACKAGES='26'"
    "Trilinos_NUM_TPLS='20'"
    "Skipping reading packages and TPLs for POST extra repo Dakota because marked NOPACKAGES"
    )
  # NOTE: Above tets with default Trilinos_GIT_REPOSITORY_REMOTE=origin which
  # is important to test!


# Test repo operations Trilinos_BRANCH=other-branch, Trilinos_EXTRAREPOS_BRANCH=yet-other-branch
create_ctest_dependency_handling_test_case(
  CI_TestGitUpdateCommands_Branch_ExtraReposBranch
  ARGS
    Trilinos_REPOSITORY_LOCATION=https://some-git-url/Trilinos.git
    Trilinos_BRANCH=other-branch
    Trilinos_EXTRAREPOS_BRANCH=yet-other-branch
    Trilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Continuous
    Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
    Trilinos_ADDITIONAL_PACKAGES=Stalix
  PASS_REGULAR_EXPRESSION_ALL

    "CTEST_CHECKOUT_COMMAND=./somebasedir/git. clone -b other-branch -o origin  https://some-git-url/Trilinos[.]git"
    "CTEST_GIT_UPDATE_CUSTOM=${CMAKE_COMMAND}[;]-DGIT_EXE=/somebasedir/git[;]-DREMOTE_NAME=origin[;]-DBRANCH=other-branch[;]-DUNIT_TEST_MODE=ON[;]-DOUTPUT_FILE=.*/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch_ExtraReposBranch/UpdateCommandsOutput.txt[;]-P[;].*/tribits/ctest_driver/tribits_ctest_update_commands_wrapper.cmake"
    
    "For extra repos, doing switch to branch yet-other-branch"
    "preCopyrightTrilinos: Doing GIT update from URL 'url1:/git/preCopyrightTrilinos' to dir '.*/MockTrilinos/preCopyrightTrilinos'"
    "execute_process.COMMAND./somebasedir/git[;]clean[;]-fdx[;]WORKING_DIRECTORY[;].*/MockTrilinos/preCopyrightTrilinos.OUTPUT_FILE[;].*/test/ctest_driver/MockCTestDriver/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch_ExtraReposBranch/preCopyrightTrilinos.clean.out."
    "execute_process.COMMAND./somebasedir/git[;]reset[;]--hard[;]HEAD[;]WORKING_DIRECTORY[;].*/MockTrilinos/preCopyrightTrilinos.OUTPUT_FILE[;].*/test/ctest_driver/MockCTestDriver/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch_ExtraReposBranch/preCopyrightTrilinos.reset.out."
    "execute_process.COMMAND./somebasedir/git[;]fetch[;]origin[;]TIMEOUT[;]600[;]WORKING_DIRECTORY[;].*/MockTrilinos/preCopyrightTrilinos.OUTPUT_FILE[;].*/test/ctest_driver/MockCTestDriver/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch_ExtraReposBranch/preCopyrightTrilinos.fetch.out."
    "execute_process.COMMAND./somebasedir/git[;]checkout[;]-B[;]yet-other-branch[;]--track[;]origin/yet-other-branch[;]WORKING_DIRECTORY[;].*/MockTrilinos/preCopyrightTrilinos.OUTPUT_FILE[;].*/test/ctest_driver/MockCTestDriver/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch_ExtraReposBranch/preCopyrightTrilinos.set_branch.out."
    "Dakota: Doing initial GIT clone/checkout from URL 'url3:/git/Dakota' to dir 'packages/TriKota/Dakota' ..."
    "execute_process.COMMAND[;]/somebasedir/git[;]clone[;]-b[;]yet-other-branch[;]-o[;]origin[;]url3:/git/Dakota[;]packages/TriKota/Dakota[;]WORKING_DIRECTORY[;].*/tribits/examples/MockTrilinos[;]OUTPUT_FILE[;].*/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch_ExtraReposBranch/Dakota.clone.out"
    "execute_process.COMMAND[;]/somebasedir/git[;]clean[;]-fdx[;]WORKING_DIRECTORY[;].*/MockTrilinos/packages/TriKota/Dakota[;]OUTPUT_FILE[;].*/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch_ExtraReposBranch/Dakota.clean.out"
    "execute_process.COMMAND[;]/somebasedir/git[;]reset[;]--hard[;]HEAD[;]WORKING_DIRECTORY[;].*/MockTrilinos/packages/TriKota/Dakota[;]OUTPUT_FILE[;].*/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch_ExtraReposBranch/Dakota.reset.out"
    "execute_process.COMMAND[;]/somebasedir/git[;]fetch[;]origin[;]TIMEOUT[;]600[;]WORKING_DIRECTORY[;].*/MockTrilinos/packages/TriKota/Dakota[;]OUTPUT_FILE[;].*/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch_ExtraReposBranch/Dakota.fetch.out"
    "execute_process.COMMAND[;]/somebasedir/git[;]checkout[;]-B[;]yet-other-branch[;]--track[;]origin/yet-other-branch[;]WORKING_DIRECTORY[;].*/MockTrilinos/packages/TriKota/Dakota[;]OUTPUT_FILE[;].*/TriBITS_MockCTestDriver_CI_TestGitUpdateCommands_Branch_ExtraReposBranch/Dakota.set_branch.out"
    "Adding POST extra Continuous repository preCopyrightTrilinos"
    "Adding POST extra Continuous repository Dakota"
    "Trilinos_NUM_PACKAGES='24'"
    "Trilinos_NUM_TPLS='19'"
    "Reading list of POST extra packages from .*/MockTrilinos/preCopyrightTrilinos/PackagesList.cmake"
    "Trilinos_NUM_PACKAGES='26'"
    "Trilinos_NUM_TPLS='20'"
    "Skipping reading packages and TPLs for POST extra repo Dakota because marked NOPACKAGES"
    )


create_ctest_dependency_handling_test_case(
  Nightly_EnableAllSecondaryTested
  ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON Trilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Nightly
  PASS_REGULAR_EXPRESSION_ALL
    "execute_process.+git.+preCopyrightTrilinos.+preCopyrightTrilinos.fetch.out"
    "execute_process.+git.+extraTrilinosRepo.+extraTrilinosRepo.fetch.out"
    "Explicitly enabled packages on input .*  0"
    "Explicitly disabled packages on input .*  Stokhos Stalix 2"
    "Enabling all SE packages that are not currently disabled because of Trilinos_ENABLE_ALL_PACKAGES=ON"
    "Final set of enabled packages:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer Teko ExtraPack 25"
    "Final set of non-enabled packages:  Stokhos Stalix 2"
    "Trilinos_NUM_ENABLED_PACKAGES=25:  Configuring packages!"
    "Processing current package ExtraPack: libs='ON', tests='ON'"
    "Configuring TRIBITS_PACKAGE='ExtraPack'"
  )


create_ctest_dependency_handling_test_case(
  PreExtraRepos_ExtraReposFile
  ARGS
    Trilinos_EXTRAREPOS_FILE="${CMAKE_CURRENT_SOURCE_DIR}/../../core/DependencyUnitTests/PreAndExtraRepositoriesList.cmake"
    Trilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Continuous
    Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
  PASS_REGULAR_EXPRESSION_ALL
    "-- Adding PRE extra Continuous repository preRepoOnePackage"
    "-- Adding POST extra Continuous repository preCopyrightTrilinos"
    "-- Adding POST extra Continuous repository Dakota"
    "preRepoOnePackage: Doing GIT update from URL .url1:/git/preRepoOnePackage. to dir ..*/MockTrilinos/preRepoOnePackage."
    "-- Trilinos_PRE_REPOSITORIES=.preRepoOnePackage."
    "-- Trilinos_EXTRA_REPOSITORIES=.preCopyrightTrilinos[;]Dakota."
    "Reading list of PRE extra packages from .*/MockTrilinos/preRepoOnePackage/PackagesList.cmake"
    "Final set of enabled packages:  preRepoOnePackage TrilinosFramework Teuchos .* Teko"
    "Processing current package preRepoOnePackage: libs=.ON., tests=.ON."
    "CONFIGURE_OPTIONS = ..*-DTrilinos_EXTRAREPOS_FILE:STRING=.*/DependencyUnitTests/PreAndExtraRepositoriesList.cmake.*-DTrilinos_ENABLE_preRepoOnePackage:BOOL=ON."
  )


create_ctest_dependency_handling_test_case(
  PreExtraRepos_ExtraReposFile_ExtraReposList
  ARGS
    Trilinos_EXTRAREPOS_FILE="${CMAKE_CURRENT_SOURCE_DIR}/../../core/DependencyUnitTests/PreAndExtraRepositoriesList.cmake"
    Trilinos_PRE_REPOSITORIES=preRepoOnePackage
    Trilinos_EXTRA_REPOSITORIES=preCopyrightTrilinos,extraTrilinosRepo,Dakota
    Trilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Nightly
    Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
  PASS_REGULAR_EXPRESSION_ALL
    "-- Adding PRE extra Continuous repository preRepoOnePackage"
    "-- Adding POST extra Continuous repository preCopyrightTrilinos"
    "-- Adding POST extra Nightly repository extraTrilinosRepo"
    "-- Adding POST extra Continuous repository Dakota"
    "-- Trilinos_PRE_REPOSITORIES=.preRepoOnePackage."
    "-- Trilinos_EXTRA_REPOSITORIES=.preCopyrightTrilinos,extraTrilinosRepo,Dakota."
    "Reading list of PRE extra packages from .*/MockTrilinos/preRepoOnePackage/PackagesList.cmake"
    "Reading list of native packages from .*/MockTrilinos/PackagesList.cmake"
    "Reading list of POST extra packages from .*/preCopyrightTrilinos/PackagesList.cmake"
    "Reading list of POST extra packages from .*/extraTrilinosRepo/PackagesList.cmake"
    "Skipping reading packages and TPLs for POST extra repo Dakota because marked NOPACKAGES"
    "Final set of enabled packages:  preRepoOnePackage TrilinosFramework Teuchos .* Teko"
    "Processing current package preRepoOnePackage: libs=.ON., tests=.ON."
    "CONFIGURE_OPTIONS = ..*-DTrilinos_EXTRAREPOS_FILE:STRING=.*/DependencyUnitTests/PreAndExtraRepositoriesList.cmake.*-DTrilinos_ENABLE_preRepoOnePackage:BOOL=ON."
  )


create_ctest_dependency_handling_test_case(
  PreExtraRepos_ExtraReposFile_ExtraReposListPartial
  ARGS
    Trilinos_EXTRAREPOS_FILE="${CMAKE_CURRENT_SOURCE_DIR}/../../core/DependencyUnitTests/PreAndExtraRepositoriesList.cmake"
    Trilinos_PRE_REPOSITORIES=
    Trilinos_EXTRA_REPOSITORIES=preCopyrightTrilinos,Dakota
    Trilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Nightly
    Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
  PASS_REGULAR_EXPRESSION_ALL
    "-- Adding PRE extra Continuous repository preRepoOnePackage"
    "-- Adding POST extra Continuous repository preCopyrightTrilinos"
    "-- Adding POST extra Continuous repository Dakota"
    "-- Trilinos_PRE_REPOSITORIES=.preRepoOnePackage."
    "-- Trilinos_EXTRA_REPOSITORIES=.preCopyrightTrilinos,Dakota."
    "Reading list of PRE extra packages from .*/MockTrilinos/preRepoOnePackage/PackagesList.cmake"
    "Reading list of native packages from .*/MockTrilinos/PackagesList.cmake"
    "Reading list of POST extra packages from .*/preCopyrightTrilinos/PackagesList.cmake"
    "Skipping reading packages and TPLs for POST extra repo Dakota because marked NOPACKAGES"
    "Final set of enabled packages:  preRepoOnePackage TrilinosFramework Teuchos .* Teko"
    "Processing current package preRepoOnePackage: libs=.ON., tests=.ON."
    "CONFIGURE_OPTIONS = ..*-DTrilinos_EXTRAREPOS_FILE:STRING=.*/DependencyUnitTests/PreAndExtraRepositoriesList.cmake.*-DTrilinos_ENABLE_preRepoOnePackage:BOOL=ON."
  )


create_ctest_dependency_handling_test_case(
  PackageEnablesFile_ThyraEpetra
  ARGS
    Trilinos_PACKAGE_ENABLES_FILE=${CMAKE_CURRENT_LIST_DIR}/enableThyraEpetra.cmake
    Trilinos_CTEST_DO_ALL_AT_ONCE=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Explicitly enabled SE packages on input [(]by user[)]:  ThyraEpetra 1"
    "-- Setting ThyraEpetra_ENABLE_TESTS=ON"
    "-- Setting ThyraEpetra_ENABLE_EXAMPLES=ON"
    "-- Setting Trilinos_ENABLE_Thyra=ON because Trilinos_ENABLE_ThyraCoreLibs=ON"
    "-- Setting Thyra_ENABLE_TESTS=ON because ThyraEpetra_ENABLE_TESTS=ON"
    "-- Setting Thyra_ENABLE_EXAMPLES=ON because ThyraEpetra_ENABLE_EXAMPLES=ON"
    "CONFIGURE_OPTIONS = .*[;]-DTrilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE:STRING=Experimental[;]-DTrilinos_ENABLE_TESTS:BOOL=ON'"
    "Final set of enabled packages:  Teuchos RTOp Epetra Thyra 4"
    "Final set of enabled SE packages:  Teuchos RTOp Epetra ThyraCoreLibs ThyraEpetra Thyra 6"
    "Final set of packages to be explicitly processed by CTest/CDash:  Teuchos RTOp Epetra Thyra 4"
  )
  # NOTE: The above is the same use case for the STK enable for the SPARC
  # Trilinos configuration for the bug described in ATDV-399.  This test
  # proves this is fixed.


create_ctest_dependency_handling_test_case(
  PackageEnablesFile_ThyraEpetra_and_packages_Teuchos_error
  ARGS
    Trilinos_PACKAGES=Teuchos
    Trilinos_PACKAGE_ENABLES_FILE=${CMAKE_CURRENT_LIST_DIR}/enableThyraEpetra.cmake
  PASS_REGULAR_EXPRESSION_ALL
    "CMake Error at .*/tribits/ctest_driver/TribitsCTestDriverCore.cmake"
    "ERROR: Both Trilinos_PACKAGES and Trilinos_PACKAGE_ENABLES_FILE cannot"
  )


#
# Test enable logic based on modified files/packages
#


if (PYTHON_EXECUTABLE)

  create_ctest_dependency_handling_test_case(
    CI_EnableOnlyModified_ModifiedNone
    COPY_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.preCopyrightTrilinos.txt
    ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
      CTEST_TEST_TYPE=Continuous CTEST_ENABLE_MODIFIED_PACKAGES_ONLY=ON
    PASS_REGULAR_EXPRESSION_ALL
      "Final set of enabled packages:  0"
      "Trilinos_NUM_ENABLED_PACKAGES=0:  Exiting the script"
    SKIP_STANDARD_DEPS_CHECK
    )


  create_ctest_dependency_handling_test_case(
    CI_EnableOnlyModified_ModifiedTeuchos
    COPY_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.Teuchos.txt modifiedFiles.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.preCopyrightTrilinos.txt
    ARGS  Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
      CTEST_TEST_TYPE=Continuous  CTEST_ENABLE_MODIFIED_PACKAGES_ONLY=ON
      CTEST_UPDATE_RETURN_VAL=1
    PASS_REGULAR_EXPRESSION_ALL
      "Directly modified or failing non-disabled packages that need to be tested:  Teuchos 1"
      "Explicitly enabled packages on input .*  Teuchos 1"
      "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer Teko 23"
      "Final set of non-enabled packages:  TrilinosFramework Stokhos Stalix 3"
      "ctest_update.+ returned '1'"
      "Processing current package Teuchos: libs='ON', tests='ON'"
      "Processing current package Teko: libs='ON', tests='ON'"
    )


  create_ctest_dependency_handling_test_case(
    CI_Nightly_EnableOnlyModified_ModifiedTeuchos
    COPY_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.Teuchos.txt modifiedFiles.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.preCopyrightTrilinos.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.extraTrilinosRepo.txt
    ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
      CTEST_TEST_TYPE=Continuous CTEST_ENABLE_MODIFIED_PACKAGES_ONLY=ON
      CTEST_START_WITH_EMPTY_BINARY_DIRECTORY=TRUE
      Trilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Nightly
      CTEST_UPDATE_RETURN_VAL=1
    PASS_REGULAR_EXPRESSION_ALL
      "Directly modified or failing non-disabled packages that need to be tested:  Teuchos 1"
      "Explicitly enabled packages on input .*  Teuchos 1"
      "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer Teko ExtraPack 24"
      "Final set of non-enabled packages:  TrilinosFramework Stokhos Stalix 3"
      "ctest_update.+ returned '1'"
      "Processing current package Teuchos: libs='ON', tests='ON'"
      "CONFIGURE_OPTIONS.+Trilinos_ENABLE_Teuchos:BOOL=ON"
      "Processing current package Zoltan: libs='ON', tests=''"
      "Processing current package Teko: libs='ON', tests='ON'"
      "Processing current package ExtraPack: libs='ON', tests='ON'"
    )


  create_ctest_dependency_handling_test_case(
    CI_EnableOnlyModified_ModifiedThyra_NoEnableImplictlyEnabled
    COPY_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.Thyra.txt modifiedFiles.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.preCopyrightTrilinos.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.extraTrilinosRepo.txt
    ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
      CTEST_TEST_TYPE=Continuous CTEST_ENABLE_MODIFIED_PACKAGES_ONLY=ON
      CTEST_START_WITH_EMPTY_BINARY_DIRECTORY=FALSE
      CTEST_UPDATE_RETURN_VAL=1
    PASS_REGULAR_EXPRESSION_ALL
      "Directly modified or failing non-disabled packages that need to be tested:  Thyra 1"
      "Explicitly enabled packages on input .*  Thyra 1"
      "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos Phalanx Panzer Teko 22"
      "Final set of non-enabled packages:  TrilinosFramework Stokhos RBGen Stalix 4"
      "Final set of packages to be explicitly processed by CTest/CDash:  Thyra Stratimikos Panzer Teko 4"
      "ctest_update.+ returned '1'"
      "Processing current package Thyra: libs='ON', tests='ON'"
      "CONFIGURE_OPTIONS.+Trilinos_ENABLE_Thyra:BOOL=ON"
      "Processing current package Teko: libs='ON', tests='ON'"
    )


  create_ctest_dependency_handling_test_case(
    CI_EnableOnlyModified_ModifiedThyraCoreLibs_NoEnableImplictlyEnabled
    COPY_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.ThyraCoreLibs.txt modifiedFiles.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.preCopyrightTrilinos.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.extraTrilinosRepo.txt
    ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
      CTEST_TEST_TYPE=Continuous CTEST_ENABLE_MODIFIED_PACKAGES_ONLY=ON
      CTEST_START_WITH_EMPTY_BINARY_DIRECTORY=FALSE
      CTEST_UPDATE_RETURN_VAL=1
    PASS_REGULAR_EXPRESSION_ALL
      "Directly modified or failing non-disabled packages that need to be tested:  ThyraCoreLibs 1"
      "Explicitly enabled packages on input .*  0"
      "Explicitly enabled SE packages on input .*  ThyraCoreLibs 1"
      "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos Phalanx Panzer Teko 22"
      "Final set of non-enabled packages:  TrilinosFramework Stokhos RBGen Stalix 4"
      "Final set of packages to be explicitly processed by CTest/CDash:  Thyra Stratimikos Panzer Teko 4"
      "ctest_update.+ returned '1'"
      "Processing current package Thyra: libs='ON', tests='ON'"
      "CONFIGURE_OPTIONS.+Trilinos_ENABLE_Thyra:BOOL=ON"
      "Processing current package Teko: libs='ON', tests='ON'"
    )


  create_ctest_dependency_handling_test_case(
    CI_EnableOnlyModified_ModifiedTeko
    COPY_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.Teko.txt modifiedFiles.preCopyrightTrilinos.txt
    ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
      CTEST_TEST_TYPE=Continuous CTEST_ENABLE_MODIFIED_PACKAGES_ONLY=ON
      CTEST_START_WITH_EMPTY_BINARY_DIRECTORY=TRUE
      CTEST_UPDATE_RETURN_VAL=0 # Main update returns nothing for the main Trilinos repo!
    PASS_REGULAR_EXPRESSION_ALL
      "Directly modified or failing non-disabled packages that need to be tested:  Teko 1"
      "Explicitly enabled packages on input .*  Teko 1"
      "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Triutils Tpetra EpetraExt Thyra Isorropia AztecOO Galeri Amesos Ifpack ML Belos Stratimikos Teko 17"
      "Final set of non-enabled packages:  TrilinosFramework Shards Stokhos Sacado Intrepid RBGen Phalanx Panzer Stalix 9"
      "ctest_update.+ returned '0'"
      "Processing current package Teuchos: libs='ON', tests=''"
      "CONFIGURE_OPTIONS.+Trilinos_ENABLE_Teuchos:BOOL=ON"
      "Processing current package Teko: libs='ON', tests='ON'"
    )
  # NOTE: The above unit test is critical because it shows that Teko will get
  # enabled and processed even though the main git update of the main Trilinos
  # repo returns no updated files.  This ensures that if only extra repo
  # packages get modified then they will get picked up in CI testing and
  # processed correctly.


  create_ctest_dependency_handling_test_case(
    CI_EnableOnlyModified_ModifiedTeko_OnlyPrimaryTestedCode
    COPY_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.Teko.txt modifiedFiles.preCopyrightTrilinos.txt
    ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=OFF
      CTEST_TEST_TYPE=Continuous CTEST_ENABLE_MODIFIED_PACKAGES_ONLY=ON
      CTEST_START_WITH_EMPTY_BINARY_DIRECTORY=TRUE
      CTEST_UPDATE_RETURN_VAL=0 # Main update returns nothing for the main Trilinos repo!
    PASS_REGULAR_EXPRESSION_ALL
      "-- MODIFIED_PACKAGES_LIST='Teko'"
      "NOT enabling modified ST package: Teko"
      "Directly modified or failing non-disabled packages that need to be tested:  0"
      "Explicitly enabled packages on input .by user.:  0"
      "Final set of enabled packages:  0"
      "Directly modified or failing non-disabled packages that need to be tested:  0"
      "Trilinos_NUM_ENABLED_PACKAGES=0:  Exiting the script"
    )
  # NOTE: The above test ensures that if an ST package is changed but only PT
  # packages are enabled, then it will not be enabled.


  create_ctest_dependency_handling_test_case(
    CI_EnableOnlyModified_ModifiedTeuchosTeko
    COPY_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.Teuchos.txt modifiedFiles.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.Teko.txt modifiedFiles.preCopyrightTrilinos.txt
    ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
      CTEST_TEST_TYPE=Continuous CTEST_ENABLE_MODIFIED_PACKAGES_ONLY=ON
      CTEST_UPDATE_RETURN_VAL=2
    PASS_REGULAR_EXPRESSION_ALL
      "MODIFIED_PACKAGES_LIST='Teuchos.Teko'"
      "Enabling modified package: Teuchos"
      "Enabling modified package: Teko"
      "Directly modified or failing non-disabled packages that need to be tested:  Teuchos Teko 2"
      "Explicitly enabled packages on input .*  Teuchos Teko 2"
      "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer Teko 23"
      "Final set of non-enabled packages:  TrilinosFramework Stokhos Stalix 3"
      "ctest_update.+ returned '2'"
      "Processing current package Teuchos: libs='ON', tests='ON'"
      "CONFIGURE_OPTIONS.+Trilinos_ENABLE_Teuchos:BOOL=ON"
      "Processing current package Teko: libs='ON', tests='ON'"
      "CONFIGURE_OPTIONS.+Trilinos_ENABLE_Teko:BOOL=ON"
    )


  create_ctest_dependency_handling_test_case(
    CI_EnableOnlyModified_ModifiedStokhosStalix_FailedNone
    COPY_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.Stokhos.txt modifiedFiles.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.Stalix.txt modifiedFiles.preCopyrightTrilinos.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/failedPackages.none.txt failedPackages.txt
    ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
      CTEST_TEST_TYPE=Continuous CTEST_ENABLE_MODIFIED_PACKAGES_ONLY=ON
      CTEST_UPDATE_RETURN_VAL=1
    PASS_REGULAR_EXPRESSION_ALL
      "MODIFIED_PACKAGES_LIST='Stokhos.Stalix'"
      "FAILING_PACKAGES_LIST=''"
      "Not enabling explicitly disabled modified package: Stokhos"
      "Not enabling explicitly disabled modified package: Stalix"
      "Directly modified or failing non-disabled packages that need to be tested:  0"
      "Explicitly enabled packages on input .*  0"
      "Explicitly disabled packages on input .*  Stokhos Stalix 2"
      "Final set of enabled packages:  0"
      "Final set of non-enabled packages:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Stokhos Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer Teko Stalix 26"
      "Trilinos_NUM_ENABLED_PACKAGES=0:  Exiting the script!"
    )


  create_ctest_dependency_handling_test_case(
    CI_EnableOnlyModified_ModifiedTeko_FailedThyraRBGen
    COPY_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.Teko.txt modifiedFiles.preCopyrightTrilinos.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/failedPackages.Thyra.RBGen.txt failedPackages.txt
    ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
      CTEST_TEST_TYPE=Continuous CTEST_ENABLE_MODIFIED_PACKAGES_ONLY=ON
      CTEST_UPDATE_RETURN_VAL=0 # Main update returns nothing for the main Trilinos repo!
    PASS_REGULAR_EXPRESSION_ALL
      "MODIFIED_PACKAGES_LIST='Teko'"
      "FAILING_PACKAGES_LIST='Thyra.RBGen'"
      "Enabling modified package: Teko"
      "Enabling previously failing package: Thyra"
      "Enabling previously failing package: RBGen"
      "Directly modified or failing non-disabled packages that need to be tested:  Thyra RBGen Teko 3"
      "Explicitly enabled packages on input .*  Thyra RBGen Teko 3"
      "Explicitly disabled packages on input .*  Stokhos Stalix 2"
      "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer Teko 23"
      "Final set of non-enabled packages:  TrilinosFramework Stokhos Stalix 3"
      "Final set of packages to be explicitly processed by CTest/CDash:  Thyra Stratimikos RBGen Panzer Teko 5"
      "ctest_update.+ returned '0'"
      "Processing current package Thyra: libs='ON', tests='ON'"
      "Processing current package RBGen: libs='ON', tests='ON'"
      "Processing current package Teko: libs='ON', tests='ON'"
    )


  create_ctest_dependency_handling_test_case(
    CI_AAO_EnableOnlyModified_ModifiedTeko_FailedAllPackages
    COPY_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.Teko.txt modifiedFiles.preCopyrightTrilinos.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/failedPackages.ALL_PACKAGES.txt failedPackages.txt
    ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
      Trilinos_CTEST_DO_ALL_AT_ONCE=TRUE
      CTEST_TEST_TYPE=Continuous
      CTEST_ENABLE_MODIFIED_PACKAGES_ONLY=ON
      CTEST_UPDATE_RETURN_VAL=0 # Main update returns nothing for the main Trilinos repo!
    PASS_REGULAR_EXPRESSION_ALL
      "MODIFIED_PACKAGES_LIST='Teko'"
      "FAILING_PACKAGES_LIST='ALL_PACKAGES'"
      "Enabling modified package: Teko"
      "Enabling previously failing ALL_PACKAGES"
      "Directly modified or failing non-disabled packages that need to be tested:  ALL_PACKAGES"
      "Explicitly enabled packages on input .*  Teko 1"
      "Explicitly disabled packages on input .*  Stokhos Stalix 2"
      "Enabling all SE packages that are not currently disabled because of Trilinos_ENABLE_ALL_PACKAGES=ON .Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON."
      "Final set of non-enabled packages:  Stokhos Stalix 2"
      "Final set of packages to be explicitly processed by CTest/CDash:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer Teko 24"
      "ctest_update.[.][.][.]. returned '0'"
      "Configure, build, test, and submit results for Trilinos packages"
      "CONFIGURE_OPTIONS = '-DTrilinos_TRIBITS_DIR=.*[;]-DTrilinos_ENABLE_ALL_PACKAGES=ON[;]-DTrilinos_ENABLE_TESTS:BOOL=ON'"
      "Skipping actual ctest_configure.. because CTEST_DEPENDENCY_HANDLING_UNIT_TESTING='ON'"
      "Skipping build because CTEST_DEPENDENCY_HANDLING_UNIT_TESTING='ON'"
      "Skipping testing because CTEST_DEPENDENCY_HANDLING_UNIT_TESTING='ON'"
      "Done with the all-at-once configure, build, test, and submit of Trilinos packages"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
    )
  # NOTE: The above test is for the all-at-once appraoch that checks for the
  # ALL_PACKAGES value in failedPackages.txt.  That value can be returned from
  # a configure failure in a previous all-at-once CI build.  This test makes
  # sure that all packages will get enabled in the follow-up CI iteration in
  # that case.


  create_ctest_dependency_handling_test_case(
    CI_AAO_EnableAllPackages_ExcludePackges
    ARGS
      Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
      Trilinos_EXCLUDE_PACKAGES=RBGen,Panzer
      Trilinos_CTEST_DO_ALL_AT_ONCE=TRUE
      CTEST_TEST_TYPE=Nightly
    PASS_REGULAR_EXPRESSION_ALL
      "Trilinos_EXCLUDE_PACKAGES='RBGen,Panzer'"
      "Disabling excluded package RBGen [.][.][.]"
      "Disabling excluded package Panzer [.][.][.]"
      "Explicitly disabled packages on input .by user or by default.:  Stokhos RBGen Panzer Stalix 4"
      "Enabling all SE packages that are not currently disabled because of Trilinos_ENABLE_ALL_PACKAGES=ON .Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON."
      "Final set of non-enabled packages:  Stokhos RBGen Panzer Stalix 4"
      "Final set of non-enabled SE packages:  Stokhos ThyraCrazyStuff RBGen Panzer Stalix 5"
      "Configure, build, test, and submit results for Trilinos packages"
      "CONFIGURE_OPTIONS = '.+[;]-DTrilinos_ENABLE_ALL_PACKAGES=ON[;]-DTrilinos_ENABLE_RBGen=OFF[;]-DTrilinos_ENABLE_Panzer=OFF[;]-DTrilinos_ENABLE_TESTS:BOOL=ON'"
      "Skipping actual ctest_configure.. because CTEST_DEPENDENCY_HANDLING_UNIT_TESTING='ON'"
      "Skipping build because CTEST_DEPENDENCY_HANDLING_UNIT_TESTING='ON'"
      "Skipping testing because CTEST_DEPENDENCY_HANDLING_UNIT_TESTING='ON'"
      "Done with the all-at-once configure, build, test, and submit of Trilinos packages"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
    )
  # NOTE: The above test is for the all-at-once appraoch where
  # <Project>_ENABLE_ALL_PACKAGES=ON is set (implicitly) but some packages are
  # disabled through setting <Project>_EXCLUDE_PACKAGES.  This makes sure that
  # these disables are passed into the inner configure.


  create_ctest_dependency_handling_test_case(
    CI_EnableOnlyModified_ModifiedNone_FailedTeko_OnlyPrimaryStableCode
    COPY_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.preCopyrightTrilinos.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/failedPackages.Teko.txt failedPackages.txt
    ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=OFF
      CTEST_TEST_TYPE=Continuous CTEST_ENABLE_MODIFIED_PACKAGES_ONLY=ON
      CTEST_UPDATE_RETURN_VAL=0 # Main update returns nothing for the main Trilinos repo!
    PASS_REGULAR_EXPRESSION_ALL
      "MODIFIED_PACKAGES_LIST=''"
      "FAILING_PACKAGES_LIST='Teko'"
      "NOT enabling previously failing ST package: Teko"
      "Directly modified or failing non-disabled packages that need to be tested:  0"
      "Explicitly enabled packages on input .*  0"
      "Final set of enabled packages:  0"
      "Final set of packages to be explicitly processed by CTest/CDash:  0"
      "Trilinos_NUM_ENABLED_PACKAGES=0:  Exiting the script"
    )


  create_ctest_dependency_handling_test_case(
    CI_EnableOnlyModified_ModifiedNone_FailedThyraRBGen
    COPY_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.preCopyrightTrilinos.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/failedPackages.Thyra.RBGen.txt failedPackages.txt
    ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
      CTEST_TEST_TYPE=Continuous CTEST_ENABLE_MODIFIED_PACKAGES_ONLY=ON
      CTEST_UPDATE_RETURN_VAL=0 # Main update returns nothing for the main Trilinos repo!
    PASS_REGULAR_EXPRESSION_ALL
      "ctest_update.+ returned '0'"
      "MODIFIED_PACKAGES_LIST=''"
      "FAILING_PACKAGES_LIST='Thyra.RBGen'"
      "Enabling previously failing package: Thyra"
      "Enabling previously failing package: RBGen"
      "Directly modified or failing non-disabled packages that need to be tested:  Thyra RBGen 2"
      "Explicitly enabled packages on input .*  Thyra RBGen 2"
      "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer Teko 23"
    )


  create_ctest_dependency_handling_test_case(
    CI_EnableOnlyModified_ModifiedNone_FailedStokhosStalix
    COPY_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.preCopyrightTrilinos.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/failedPackages.Stokhos.Stalix.txt failedPackages.txt
    ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
      CTEST_TEST_TYPE=Continuous CTEST_ENABLE_MODIFIED_PACKAGES_ONLY=ON
      CTEST_UPDATE_RETURN_VAL=0 # Main update returns nothing for the main Trilinos repo!
    PASS_REGULAR_EXPRESSION_ALL
      "ctest_update.+ returned '0'"
      "MODIFIED_PACKAGES_LIST=''"
      "FAILING_PACKAGES_LIST='Stokhos.Stalix'"
      "Not enabling explicitly disabled previously failing package: Stokhos"
      "Not enabling explicitly disabled previously failing package: Stalix"
      "Directly modified or failing non-disabled packages that need to be tested:  0"
      "Explicitly enabled packages on input .*  0"
      "Explicitly disabled packages on input .*  Stokhos Stalix 2"
      "Final set of enabled packages:  0"
      "Trilinos_NUM_ENABLED_PACKAGES=0:  Exiting the script"
    )


  create_ctest_dependency_handling_test_case(
    CI_EnableOnlyModified_GlobalModified_PT_AAO
    COPY_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.Global.txt modifiedFiles.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.preCopyrightTrilinos.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt failedPackages.txt
    ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=OFF
      CTEST_TEST_TYPE=Continuous
      CTEST_ENABLE_MODIFIED_PACKAGES_ONLY=ON
      CTEST_UPDATE_RETURN_VAL=0 # Main update returns nothing for the main Trilinos repo!
      Trilinos_CTEST_DO_ALL_AT_ONCE=ON
    PASS_REGULAR_EXPRESSION_ALL
      "ctest_update.+ returned '0'"
      "MODIFIED_PACKAGES_LIST='ALL_PACKAGES'"
      "FAILING_PACKAGES_LIST=''"
      "Directly modified or failing non-disabled packages that need to be tested:  ALL_PACKAGES"
      "Explicitly enabled packages on input .*  0"
      "Explicitly disabled packages on input .*  Stokhos Stalix 2"
      "Enabling all SE packages that are not currently disabled because of Trilinos_ENABLE_ALL_PACKAGES=ON [(]Trilinos_ENABLE_SECONDARY_TESTED_CODE=OFF[)] [.][.][.]"
      "Final set of enabled packages:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Stratimikos RBGen 19"
      "Final set of non-enabled packages:  Stokhos Sacado Belos Phalanx Panzer Teko Stalix 7"
      "MODIFIED_PACKAGES_LIST='ALL_PACKAGES':  Found modified packages, processing enabled packages!"
      "CONFIGURE_OPTIONS = '.+[;]-DTrilinos_ENABLE_ALL_PACKAGES=ON[;].+'"
    )


  create_ctest_dependency_handling_test_case(
    CI_EnableOnlyModified_GlobalModified_ST_AAO
    COPY_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.Global.txt modifiedFiles.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt modifiedFiles.preCopyrightTrilinos.txt
      ${CMAKE_CURRENT_SOURCE_DIR}/modifiedFiles.empty.txt failedPackages.txt
    ARGS Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON
      CTEST_TEST_TYPE=Continuous
      CTEST_ENABLE_MODIFIED_PACKAGES_ONLY=ON
      CTEST_UPDATE_RETURN_VAL=0 # Main update returns nothing for the main Trilinos repo!
      Trilinos_CTEST_DO_ALL_AT_ONCE=ON
    PASS_REGULAR_EXPRESSION_ALL
      "ctest_update.+ returned '0'"
      "MODIFIED_PACKAGES_LIST='ALL_PACKAGES'"
      "FAILING_PACKAGES_LIST=''"
      "Directly modified or failing non-disabled packages that need to be tested:  ALL_PACKAGES"
      "Explicitly enabled packages on input .*  0"
      "Explicitly disabled packages on input .*  Stokhos Stalix 2"
      "Enabling all SE packages that are not currently disabled because of Trilinos_ENABLE_ALL_PACKAGES=ON [(]Trilinos_ENABLE_SECONDARY_TESTED_CODE=ON[)] [.][.][.]"
      "Final set of enabled packages:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer Teko 24"
      "Final set of non-enabled packages:  Stokhos Stalix 2"
      "MODIFIED_PACKAGES_LIST='ALL_PACKAGES':  Found modified packages, processing enabled packages!"
      "CONFIGURE_OPTIONS = '.+[;]-DTrilinos_ENABLE_ALL_PACKAGES=ON[;].+'"
    )


  # ToDo: Test modified package not to be implicitly enabled to not be enabled!

endif(PYTHON_EXECUTABLE)


#
# Test other behavior of TribitsCTestDriverCore
#

create_ctest_dependency_handling_test_case(
  Override_MPI_EXEC_MAX_NUMPROCSS
  ARGS Trilinos_PACKAGES=Teuchos MPI_EXEC_MAX_NUMPROCS=6
  PASS_REGULAR_EXPRESSION_ALL
    "Processing current package Teuchos: libs='ON', tests='ON'"
    "CONFIGURE_OPTIONS = '.*-DMPI_EXEC_MAX_NUMPROCS:STRING=6.*'"
  )


#
# Test tribits_ctest_update_commands_wrapper.cmake
#


tribits_add_advanced_test( CTestDriver_tribits_ctest_update_commands_wrapper_named_branch
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    CMND "${CMAKE_COMMAND}"
    ARGS
      -DGIT_EXE=/some-dir/git
      -DREMOTE_NAME=my-remote
      -DBRANCH=my-branch
      -DUNIT_TEST_MODE=ON
      -DOUTPUT_FILE=UpdateCommandsOutput.txt
      -P ${${PROJECT_NAME}_TRIBITS_DIR}/ctest_driver/tribits_ctest_update_commands_wrapper.cmake
    ALWAYS_FAIL_ON_NONZERO_RETURN
    PASS_REGULAR_EXPRESSION_ALL
      "cmake -P tribits_ctest_update_commands_wrapper.cmake"
      "-- OUTPUT_FILE=UpdateCommandsOutput.txt"
      "tribits_ctest_update_commands_wrapper.cmake return: 0"

  TEST_1
    CMND cat ARGS UpdateCommandsOutput.txt
    ALWAYS_FAIL_ON_NONZERO_RETURN
    PASS_REGULAR_EXPRESSION_ALL
      "cmake -P tribits_ctest_update_commands.cmake:"
      "-- GIT_EXE=/some-dir/git"
      "-- REMOTE_NAME=my-remote"
      "-- BRANCH=my-branch"
      "-- UNIT_TEST_MODE=ON"
      "Running: /some-dir/git fetch my-remote"
      "execute_process[(]COMMAND[;]/some-dir/git[;]fetch[;]my-remote[)]"
      "Running: /some-dir/git clean -fdx"
      "execute_process[(]COMMAND[;]/some-dir/git[;]clean[;]-fdx[)]"
      "Running: /some-dir/git reset --hard HEAD"
      "execute_process[(]COMMAND[;]/some-dir/git[;]reset[;]--hard[;]HEAD[)]"
      "Running: /some-dir/git checkout -B my-branch --track my-remote/my-branch"
      "execute_process[(]COMMAND[;]/some-dir/git[;]checkout[;]-B[;]my-branch[;]--track[;]my-remote/my-branch[)]"
      "Git Update PASSED!"

   )


tribits_add_advanced_test( CTestDriver_tribits_ctest_update_commands_wrapper_default_branch
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    CMND "${CMAKE_COMMAND}"
    ARGS
      -DGIT_EXE=/some-dir/git
      -DREMOTE_NAME=origin
      -DBRANCH=
      -DUNIT_TEST_MODE=ON
      -DOUTPUT_FILE=UpdateCommandsOutput.txt
      -P ${${PROJECT_NAME}_TRIBITS_DIR}/ctest_driver/tribits_ctest_update_commands_wrapper.cmake
    ALWAYS_FAIL_ON_NONZERO_RETURN
    PASS_REGULAR_EXPRESSION_ALL
      "cmake -P tribits_ctest_update_commands_wrapper.cmake"
      "-- OUTPUT_FILE=UpdateCommandsOutput.txt"
      "tribits_ctest_update_commands_wrapper.cmake return: 0"

  TEST_1
    CMND cat ARGS UpdateCommandsOutput.txt
    ALWAYS_FAIL_ON_NONZERO_RETURN
    PASS_REGULAR_EXPRESSION_ALL
      "cmake -P tribits_ctest_update_commands.cmake:"
      "-- GIT_EXE=/some-dir/git"
      "-- REMOTE_NAME=origin"
      "-- BRANCH="
      "-- UNIT_TEST_MODE=ON"
      "Running: /some-dir/git fetch origin"
      "execute_process[(]COMMAND[;]/some-dir/git[;]fetch[;]origin[)]"
      "Running: /some-dir/git clean -fdx"
      "execute_process[(]COMMAND[;]/some-dir/git[;]clean[;]-fdx[)]"
      "Running: /some-dir/git reset --hard HEAD"
      "execute_process[(]COMMAND[;]/some-dir/git[;]reset[;]--hard[;]HEAD[)]"
      "Running: /some-dir/git reset --hard @{u}"
      "execute_process[(]COMMAND[;]/some-dir/git[;]reset[;]--hard[;]@{u}[)]"
      "Git Update PASSED!"

   )
