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
# These tests test the basic dependency adjustment logic in TriBITS for
# enabling and disabling packages, subpackages, tests/examples, TPLs, etc.
# These run very fast because they don't need to actually configure any code.
#

set(TRIBITS_REDUCED_MOCK_TRILINOS_DIR "examples/ReducedMockTrilinos")
set(TRIBITS_MOCK_TRILINOS_DIR "examples/MockTrilinos")


# MockTrilinos project
set(MOCK_PROJECT_NAME Trilinos)
set(MOCK_TRILINOS_DIR
  ${${PROJECT_NAME}_TRIBITS_DIR}/${TRIBITS_MOCK_TRILINOS_DIR})


#####################################################################
#
# Unit tests for dependency handling for reduced set of packages
#
#####################################################################

# Reduced MockTrilinos project
set(REDUCED_MOCK_PROJECT_NAME Trilinos)
set(REDUCED_MOCK_TRILINOS_DIR
  ${${PROJECT_NAME}_TRIBITS_DIR}/${TRIBITS_REDUCED_MOCK_TRILINOS_DIR})

# Expected output files directory
set(REDUCED_MOCK_TRILINOS_OUTPUT_DIR
  "${${PROJECT_NAME}_TRIBITS_DIR}/doc/guides/ReducedMockTrilinosOutput")

#
# NOTE: These tests compare against expected output stored in files under the
# in the guides/ReducedMockTrilinosOuptut/ directory and are used in the
# TribitsUsersGuide.rst document.  Therefore, if the behavior of TriBITS
# changes in such a way as to break these tests, then if any of the contents
# of the files under this directory need to change, then one must also look at
# the TribitsUsersGuilde.rst document and update it for the new behavior of
# TriBITS!
#


function(create_reduced_dependency_handling_test_case TEST_NAME)

  cmake_parse_arguments(
     #prefix
     PARSE
     #options
     ""
     #one_value_keywords
     ""
     #multi_value_keywords
     "ARGS;PASS_REGULAR_EXPRESSION_ALL"
     ${ARGN}
     )

  tribits_add_advanced_test( DepTestReduced_${TEST_NAME}
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    TEST_0 CMND "${CMAKE_COMMAND}"
      ARGS
        ${PARSE_ARGS}
        ${REDUCED_MOCK_TRILINOS_DIR}
      PASS_REGULAR_EXPRESSION_ALL ${PARSE_PASS_REGULAR_EXPRESSION_ALL}
    )

endfunction()


#
# Read in an expected output file and create a REGEX array to be used in
# checking test results.
#

function(read_expected_output_file_into_regex_array  EXPECTED_OUTPUT_FILE
  EXPECTED_OUTPUT_REGEX_ARRAY_OUT
  )
  file(READ ${EXPECTED_OUTPUT_FILE} EXPECTED_RAW_STR_ALL)
  #print_var(EXPECTED_RAW_STR_ALL)
  split("${EXPECTED_RAW_STR_ALL}" "\n" EXPECTED_RAW_STR_ARRAY)
  #print_var(EXPECTED_RAW_STR_ARRAY)
  set(EXPECTED_REGEX_ARRAY)
  foreach(EXPECTED_RAW_STR ${EXPECTED_RAW_STR_ARRAY})
    #print_var(EXPECTED_RAW_STR)
    if (NOT "${EXPECTED_RAW_STR}" STREQUAL "")
      set(EXPECTED_REGEX_STR "${EXPECTED_RAW_STR}")
      string(REPLACE "(" "." EXPECTED_REGEX_STR "${EXPECTED_REGEX_STR}") 
      string(REPLACE ")" "." EXPECTED_REGEX_STR "${EXPECTED_REGEX_STR}") 
      string(REPLACE "[" "." EXPECTED_REGEX_STR "${EXPECTED_REGEX_STR}") 
      string(REPLACE "]" "." EXPECTED_REGEX_STR "${EXPECTED_REGEX_STR}") 
      string(REPLACE "*" "." EXPECTED_REGEX_STR "${EXPECTED_REGEX_STR}") 
      list(APPEND EXPECTED_REGEX_ARRAY "${EXPECTED_REGEX_STR}")
      #print_var(EXPECTED_REGEX_ARRAY)
    endif()
  endforeach()
  set_property(DIRECTORY APPEND
    PROPERTY CMAKE_CONFIGURE_DEPENDS "${EXPECTED_OUTPUT_FILE}")
  set(${EXPECTED_OUTPUT_REGEX_ARRAY_OUT} ${EXPECTED_REGEX_ARRAY} PARENT_SCOPE)
endfunction()



#
# NoEnables
#

read_expected_output_file_into_regex_array(
  "${REDUCED_MOCK_TRILINOS_OUTPUT_DIR}/NoEnables.txt"
  RMT_NoEnables_REGEX_ARRAY
  )
#print_var(REDUCED_EXPECTED_DEPENDENCIES_REGEX_ARRAY)


create_reduced_dependency_handling_test_case(
  NoEnables
  ARGS
  PASS_REGULAR_EXPRESSION_ALL
    ${RMT_NoEnables_REGEX_ARRAY}
  )


#
# EnableThyra_EnableTests
#

read_expected_output_file_into_regex_array(
  "${REDUCED_MOCK_TRILINOS_OUTPUT_DIR}/EnableThyra_EnableTests.txt"
  EnableThyra_EnableTests_REGEX_ARRAY
  )

create_reduced_dependency_handling_test_case(
  EnableThyra_EnableTests
  ARGS
    -DTrilinos_ENABLE_Thyra:BOOL=ON
    -DTrilinos_ENABLE_TESTS:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    ${EnableThyra_EnableTests_REGEX_ARRAY}
  )


#
# EnableThyra_EnableTests_EnableBoost_ST
#

read_expected_output_file_into_regex_array(
  "${REDUCED_MOCK_TRILINOS_OUTPUT_DIR}/EnableThyra_EnableTests_EnableBoost_ST.txt"
  EnableThyra_EnableTests_EnableBoost_ST_REGEX_ARRAY
  )

create_reduced_dependency_handling_test_case(
  EnableThyra_EnableTests_EnableBoost_ST
  ARGS
    -DTPL_ENABLE_Boost:BOOL=ON
    -DTrilinos_ENABLE_Thyra:BOOL=ON
    -DTrilinos_ENABLE_TESTS:BOOL=ON
    -DTrilinos_ENABLE_SECONDARY_TESTED_CODE:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    ${EnableThyra_EnableTests_EnableBoost_ST_REGEX_ARRAY}
  )


#
# EnableThyra_EnableTests_EnableBoost_ST_RequiredOnly
#

read_expected_output_file_into_regex_array(
  "${REDUCED_MOCK_TRILINOS_OUTPUT_DIR}/EnableThyra_EnableTests_EnableBoost_ST_RequiredOnly.txt"
  EnableThyra_EnableTests_EnableBoost_ST_RequiredOnly_REGEX_ARRAY
  )

create_reduced_dependency_handling_test_case(
  EnableThyra_EnableTests_EnableBoost_ST_RequiredOnly
  ARGS
    -DTPL_ENABLE_Boost:BOOL=ON
    -DTrilinos_ENABLE_Thyra:BOOL=ON
    -DTrilinos_ENABLE_TESTS:BOOL=ON
    -DTrilinos_ENABLE_SECONDARY_TESTED_CODE:BOOL=ON
    -DTrilinos_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=OFF
  PASS_REGULAR_EXPRESSION_ALL
    ${EnableThyra_EnableTests_EnableBoost_ST_RequiredOnly_REGEX_ARRAY}
  )


#
# EnableThyra_DisableEpetra_EnableTests
#

read_expected_output_file_into_regex_array(
  "${REDUCED_MOCK_TRILINOS_OUTPUT_DIR}/EnableThyra_DisableEpetra_EnableTests.txt"
  EnableThyra_DisableEpetra_EnableTests_REGEX_ARRAY
  )

create_reduced_dependency_handling_test_case(
  EnableThyra_DisableEpetra_EnableTests
  ARGS
    -DTrilinos_ENABLE_Thyra:BOOL=ON
    -DTrilinos_ENABLE_Epetra:BOOL=OFF
    -DTrilinos_ENABLE_TESTS:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    ${EnableThyra_DisableEpetra_EnableTests_REGEX_ARRAY}
  )

#
# EnableEpetra_EnableRTOp_DisableTeuchos
#

set(TEST_NAME "EnableEpetra_EnableRTOp_DisableTeuchos")

read_expected_output_file_into_regex_array(
  "${REDUCED_MOCK_TRILINOS_OUTPUT_DIR}/${TEST_NAME}.txt"
  ${TEST_NAME}_REGEX_ARRAY
  )

create_reduced_dependency_handling_test_case(
  ${TEST_NAME}
  ARGS
    -DTrilinos_ENABLE_Epetra:BOOL=ON
    -DTrilinos_ENABLE_RTOp:BOOL=ON
    -DTrilinos_ENABLE_Teuchos:BOOL=OFF
   PASS_REGULAR_EXPRESSION_ALL
    ${${TEST_NAME}_REGEX_ARRAY}
  )


#
# EnableEpetra_EnableRTOp_DisableTeuchos_DisableEnabledFwdDepPackages
#

set(TEST_NAME "EnableEpetra_EnableRTOp_DisableTeuchos_DisableEnabledFwdDepPackages")

read_expected_output_file_into_regex_array(
  "${REDUCED_MOCK_TRILINOS_OUTPUT_DIR}/${TEST_NAME}.txt"
  ${TEST_NAME}_REGEX_ARRAY
  )

create_reduced_dependency_handling_test_case(
  ${TEST_NAME}
  ARGS
    -DTrilinos_ENABLE_Epetra:BOOL=ON
    -DTrilinos_ENABLE_RTOp:BOOL=ON
    -DTrilinos_ENABLE_Teuchos:BOOL=OFF
    -DTrilinos_DISABLE_ENABLED_FORWARD_DEP_PACKAGES=ON
   PASS_REGULAR_EXPRESSION_ALL
    ${${TEST_NAME}_REGEX_ARRAY}
  )


#
# EnableThyraCoreLibs_EnableTests
#

set(TEST_NAME "EnableThyraCoreLibs_EnableTests")

read_expected_output_file_into_regex_array(
  "${REDUCED_MOCK_TRILINOS_OUTPUT_DIR}/${TEST_NAME}.txt"
  ${TEST_NAME}_REGEX_ARRAY
  )

create_reduced_dependency_handling_test_case(
  ${TEST_NAME}
  ARGS
    -DTrilinos_ENABLE_ThyraCoreLibs:BOOL=ON
    -DTrilinos_ENABLE_TESTS:BOOL=ON
   PASS_REGULAR_EXPRESSION_ALL
    ${${TEST_NAME}_REGEX_ARRAY}
    "-- Setting Trilinos_ENABLE_Thyra=ON because Trilinos_ENABLE_ThyraCoreLibs=ON"
    "-- Setting Thyra_ENABLE_TESTS=ON because ThyraCoreLibs_ENABLE_TESTS=ON"
    "-- Setting Thyra_ENABLE_EXAMPLES=ON because ThyraCoreLibs_ENABLE_EXAMPLES=ON"
    # NOTE: These greps are repeated because they are the major purpose of
    # this test!
  )
# NOTE: The above test verifies that enabling just a subpackage and tests will
# enable both the parent package (Thyra in this case) and the parent package's
# tests and examples.


#
# EnableThyraEpetra_EnableTests
#

set(TEST_NAME "EnableThyraEpetra_EnableTests")

read_expected_output_file_into_regex_array(
  "${REDUCED_MOCK_TRILINOS_OUTPUT_DIR}/${TEST_NAME}.txt"
  ${TEST_NAME}_REGEX_ARRAY
  )

create_reduced_dependency_handling_test_case(
  ${TEST_NAME}
  ARGS
    -DTrilinos_ENABLE_ThyraEpetra:BOOL=ON
    -DTrilinos_ENABLE_TESTS:BOOL=ON
   PASS_REGULAR_EXPRESSION_ALL
    ${${TEST_NAME}_REGEX_ARRAY}
    "-- Setting Trilinos_ENABLE_Thyra=ON because Trilinos_ENABLE_ThyraCoreLibs=ON"
    "-- Setting Thyra_ENABLE_TESTS=ON because ThyraEpetra_ENABLE_TESTS=ON"
    "-- Setting Thyra_ENABLE_EXAMPLES=ON because ThyraEpetra_ENABLE_EXAMPLES=ON"
    # NOTE: These greps are repeated because they are the major purpose of
    # this test!
  )
# NOTE: The above test verifies that enabling a mid-level subpackage that
# triggers the enable of an upstream subpackage in the same parent package
# (but not that upstream subpackage's tests) will trigger the enable both the
# parent package (Thyra in this case) and the parent package's tests and
# examples.


#
# EnableAllPackages_DumpDependencies
#

read_expected_output_file_into_regex_array(
  "${REDUCED_MOCK_TRILINOS_OUTPUT_DIR}/ExpectedDependencies.txt"
  REDUCED_EXPECTED_DEPENDENCIES_REGEX_ARRAY
  )
#print_var(REDUCED_EXPECTED_DEPENDENCIES_REGEX_ARRAY)

read_expected_output_file_into_regex_array(
  "${REDUCED_MOCK_TRILINOS_OUTPUT_DIR}/EnableAllPackages.txt"
  REDUCED_EnableAllPackages_REGEX_ARRAY
  )
#print_var(REDUCED_EnableAllPackages_REGEX_ARRAY)

create_reduced_dependency_handling_test_case(
  EnableAllPackages_DumpDependencies
  ARGS
    -DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON
    -DTrilinos_DUMP_PACKAGE_DEPENDENCIES:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    ${REDUCED_EXPECTED_DEPENDENCIES_REGEX_ARRAY}
    ${REDUCED_EnableAllPackages_REGEX_ARRAY}
    "Final set of enabled packages:  Teuchos RTOp Epetra Thyra 4"
    "Final set of enabled SE packages:  Teuchos RTOp Epetra ThyraCoreLibs ThyraEpetra Thyra 6"
    "Final set of non-enabled packages:  Triutils EpetraExt 2"
    "Final set of non-enabled SE packages:  Triutils EpetraExt ThyraGoodStuff ThyraCrazyStuff ThyraEpetraExt 5"
  )

create_reduced_dependency_handling_test_case(
  EnableAllPackages_VerboseConfigure
  ARGS
    -DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON
    -DTrilinos_VERBOSE_CONFIGURE:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "MPI_FINDMOD='.*/ReducedMockTrilinos/../../core/std_tpls/FindTPLMPI.cmake'"
    "MPI_TESTGROUP='PT'"
    "MPI_TPLS_LIST_FILE='.*/ReducedMockTrilinos/TPLsList.cmake'"
    "AMD_FINDMOD='cmake/TPLs/FindTPLAMD.cmake'"
    "AMD_TESTGROUP='EX'"
    "AMD_TPLS_LIST_FILE='.*/ReducedMockTrilinos/TPLsList.cmake'"
    "PETSC_FINDMOD='.*/ReducedMockTrilinos/../../common_tpls/FindTPLPETSC.cmake'"
    "PETSC_TESTGROUP='ST'"
    "PETSC_TPLS_LIST_FILE='.*/ReducedMockTrilinos/TPLsList.cmake'"
  )


#
# Other ReducedMockTrilinos tests
#


create_reduced_dependency_handling_test_case(
  EnableAllPackages_RequiredOnly
  ARGS
    -DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON
    -DTrilinos_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=OFF
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled packages:  Teuchos RTOp Epetra Thyra 4"
    "Final set of enabled SE packages:  Teuchos RTOp Epetra ThyraCoreLibs ThyraEpetra Thyra 6"
    "Final set of non-enabled packages:  Triutils EpetraExt 2"
    "Final set of non-enabled SE packages:  Triutils EpetraExt ThyraGoodStuff ThyraCrazyStuff ThyraEpetraExt "
  )


create_reduced_dependency_handling_test_case(
  EnableAllPackages_ST_TestRequiredTpl_DisableAMD
  ARGS
    -DSHOW_TEST_REQUIRED_TPL_DEP=ON
    -DTrilinos_ENABLE_ALL_PACKAGES=ON
    -DTrilinos_ENABLE_SECONDARY_TESTED_CODE=ON
    -DTrilinos_ENABLE_TESTS=ON
    -DTPL_ENABLE_AMD=OFF
  PASS_REGULAR_EXPRESSION_ALL
    "-- Setting EpetraExt_ENABLE_TESTS=OFF because EpetraExt has a required test/example dependence on disabled TPL AMD"
    "-- Setting EpetraExt_ENABLE_EXAMPLES=OFF because EpetraExt has a required test/example dependence on disabled TPL AMD"
    "-- Setting Trilinos_ENABLE_ThyraEpetraExt=ON"
    "Processing enabled package: Triutils [(]Libs, Tests, Examples[)]"
    "Processing enabled package: EpetraExt [(]Libs[)]"
    "Finished configuring Trilinos!"
    "Configuring done"
  )


create_reduced_dependency_handling_test_case(
  EnableAllPackages_ST_TestRequiredPackage_DisableTriutils
  ARGS
    -DSHOW_TEST_REQUIRED_PACKAGE_DEP=ON
    -DTrilinos_ENABLE_ALL_PACKAGES=ON
    -DTrilinos_ENABLE_SECONDARY_TESTED_CODE=ON
    -DTrilinos_ENABLE_TESTS=ON
    -DTrilinos_ENABLE_Triutils=OFF
  PASS_REGULAR_EXPRESSION_ALL
    "-- Setting EpetraExt_ENABLE_TESTS=OFF because EpetraExt has a required test/example dependence on disabled package Triutils"
    "-- Setting EpetraExt_ENABLE_EXAMPLES=OFF because EpetraExt has a required test/example dependence on disabled package Triutils"
    "-- Setting Trilinos_ENABLE_ThyraEpetraExt=ON"
    "Final set of non-enabled packages:  Triutils 1"
    "Processing enabled package: Epetra [(]Libs, Tests, Examples[)]"
    "Processing enabled package: EpetraExt [(]Libs[)]"
    "Finished configuring Trilinos!"
    "Configuring done"
  )


create_reduced_dependency_handling_test_case(
  EnableAllPackages_ST_EpetraExtEnableTriutilsOff
  ARGS
    -DTrilinos_ENABLE_ALL_PACKAGES=ON
    -DTrilinos_ENABLE_SECONDARY_TESTED_CODE=ON
    -DTrilinos_ENABLE_TESTS=ON
    -DEpetraExt_ENABLE_Triutils=OFF
  PASS_REGULAR_EXPRESSION_ALL
    "-- NOTE: EpetraExt_ENABLE_Triutils=OFF is already set so not enabling even though Trilinos_ENABLE_Triutils=ON is set!"
  )


create_reduced_dependency_handling_test_case(
  EnableAllPackages_ST_EpetraExtEnableTriutilsOn
  ARGS
    -DTrilinos_ENABLE_ALL_PACKAGES=ON
    -DTrilinos_ENABLE_SECONDARY_TESTED_CODE=ON
    -DTrilinos_ENABLE_TESTS=ON
    -DEpetraExt_ENABLE_Triutils=ON
  PASS_REGULAR_EXPRESSION_ALL
    "-- NOTE: EpetraExt_ENABLE_Triutils=ON is already set!"
  )


create_reduced_dependency_handling_test_case(
 ST_EnableEpetraExt_DisableTriutils_EpetraExtEnableTriutilsOn
  ARGS
    -DTrilinos_ENABLE_SECONDARY_TESTED_CODE=ON
    -DTrilinos_ENABLE_TESTS=ON
    -DTrilinos_ENABLE_EpetraExt=ON
    -DTrilinos_ENABLE_Triutils=OFF
    -DEpetraExt_ENABLE_Triutils=ON
  PASS_REGULAR_EXPRESSION_ALL
    "-- NOTE: Setting EpetraExt_ENABLE_Triutils=OFF which was ON because EpetraExt has an optional library dependence on disabled package Triutils"
  )


#####################################################################
#
# Unit tests for dependency handling for full set of packages
#
#####################################################################


function(create_dependency_handling_test_case TEST_NAME)

  cmake_parse_arguments(
     #prefix
     PARSE
     #options
     ""
     #one_value_keywords
     ""
     #multi_value_keywords
     "ARGS;PASS_REGULAR_EXPRESSION_ALL"
     ${ARGN}
     )

  tribits_add_advanced_test( DepTests_${TEST_NAME}
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1
    TEST_0 CMND "${CMAKE_COMMAND}"
      ARGS
        -D${MOCK_PROJECT_NAME}_TRIBITS_DIR:FILEPATH=${${PROJECT_NAME}_TRIBITS_DIR}
        -D${MOCK_PROJECT_NAME}_SHORTCIRCUIT_AFTER_DEPENDENCY_HANDLING:BOOL=ON
        -D${MOCK_PROJECT_NAME}_DEPS_XML_OUTPUT_FILE:STRING=
        -D${MOCK_PROJECT_NAME}_DEPS_HTML_OUTPUT_FILE:STRING=
        -D${MOCK_PROJECT_NAME}_ENABLE_C:BOOL=OFF
        -D${MOCK_PROJECT_NAME}_ENABLE_CXX:BOOL=OFF
        -D${MOCK_PROJECT_NAME}_ENABLE_Fortran:BOOL=OFF
        ${PARSE_ARGS}
        ${MOCK_TRILINOS_DIR}
      PASS_REGULAR_EXPRESSION_ALL ${PARSE_PASS_REGULAR_EXPRESSION_ALL}
    )

endfunction()


create_dependency_handling_test_case(
  EnableAllPackages
  ARGS -DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON -DTrilinos_WARN_ABOUT_MISSING_EXTERNAL_PACKAGES=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled packages:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Stratimikos RBGen 19"
    "NOTE: MissingPackage is being ignored since its directory is missing and MissingPackage_ALLOW_MISSING_EXTERNAL_PACKAGE = TRUE"
  )


read_expected_output_file_into_regex_array(
  "${CMAKE_CURRENT_LIST_DIR}/DepTests_EnableAllPackages_DumpDependencies.txt"
  EnableAllPackages_DumpDependencies_REGEX_ARRAY
  )
#print_var(EnableAllPackages_DumpDependencies_REGEX_ARRAY)

create_dependency_handling_test_case(
  EnableAllPackages_DumpDependencies
  ARGS
    -DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON
    -DTrilinos_DUMP_PACKAGE_DEPENDENCIES:BOOL=ON
-DTrilinos_WARN_ABOUT_MISSING_EXTERNAL_PACKAGES=ON
  PASS_REGULAR_EXPRESSION_ALL
    "${EnableAllPackages_DumpDependencies_REGEX_ARRAY}"
    "Final set of enabled packages:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Stratimikos RBGen 19"
    "NOTE: MissingPackage is being ignored since its directory is missing and MissingPackage_ALLOW_MISSING_EXTERNAL_PACKAGE = TRUE"
  )


create_dependency_handling_test_case(
  EnableAllPackages_EnableTests
  ARGS
    -DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON
     -DTrilinos_ENABLE_TESTS:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled packages:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Stratimikos RBGen 19"
    "Setting Teuchos_ENABLE_TESTS=ON"
    "Setting RTOp_ENABLE_TESTS=ON"
    "Setting Epetra_ENABLE_TESTS=ON"
    "Setting ML_ENABLE_TESTS=ON"
    "Setting Stratimikos_ENABLE_TESTS=ON"
    "Setting RBGen_ENABLE_TESTS=ON"
  )


read_expected_output_file_into_regex_array(
  "${CMAKE_CURRENT_LIST_DIR}/DepTests_EnableAllPackages_EnableTests_DumpDependencies.txt"
  EnableAllPackages_EnableTests_DumpDependencies_REGEX_ARRAY
  )
#print_var(EnableAllPackages_DumpDependencies_REGEX_ARRAY)

create_dependency_handling_test_case(
  EnableAllPackages_EnableTests_DumpDependencies
  ARGS
    -DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON
    -DTrilinos_ENABLE_TESTS:BOOL=ON
    -DTrilinos_DUMP_PACKAGE_DEPENDENCIES:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled packages:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Stratimikos RBGen 19"
    "Setting Teuchos_ENABLE_TESTS=ON"
    "Setting RTOp_ENABLE_TESTS=ON"
    "Setting Epetra_ENABLE_TESTS=ON"
    "Setting ML_ENABLE_TESTS=ON"
    "Setting Stratimikos_ENABLE_TESTS=ON"
    "Setting RBGen_ENABLE_TESTS=ON"
  )


create_dependency_handling_test_case(
  DisableEpetra_EnableAllPackages
  ARGS -DTrilinos_ENABLE_Epetra:BOOL=OFF -DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled packages:  TrilinosFramework Teuchos RTOp Zoltan Shards Tpetra Thyra Intrepid ML RBGen 10"
  )


create_dependency_handling_test_case(
  TestTrueFalse
  ARGS -DTrilinos_ENABLE_ALL_PACKAGES=TRUE -DTrilinos_ENABLE_Zoltan=TRUE
    -DTrilinos_ENABLE_TEST=TRUE -DTPL_ENABLE_BLAS=FALSE
#    -DTrilinos_VERBOSE_CONFIGURE=TRUE
  PASS_REGULAR_EXPRESSION_ALL
    "Explicitly enabled packages on input .by user.:  Zoltan 1"
    "Explicitly disabled packages on input .by user or by default.:  Stokhos 1"
    "Explicitly disabled TPLs on input .by user or by default.:  MPI BLAS 2"
    "Setting Trilinos_ENABLE_Teuchos=OFF because Teuchos has a required library dependence on disabled TPL BLAS"
    "Setting Trilinos_ENABLE_TrilinosFramework=ON"
    "Final set of enabled packages:  TrilinosFramework Zoltan Shards 3"
    "Final set of enabled SE packages:  TrilinosFramework Zoltan Shards 3"
    "Final set of non-enabled packages:  Teuchos RTOp Epetra Triutils Tpetra EpetraExt Stokhos Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer 21"
    "Final set of non-enabled SE packages:  Teuchos RTOp Epetra Triutils Tpetra EpetraExt Stokhos Sacado ThyraCoreLibs ThyraGoodStuff ThyraCrazyStuff ThyraEpetra ThyraEpetraExt ThyraTpetra Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer 27"
  )
# Above test makes sure that TRUE/FALSE also processed correctly for enable
# vars.


create_dependency_handling_test_case(
  EnableEpetra_DisableTeuchos_EnableAllForwardPackages
  ARGS -DTrilinos_ENABLE_Epetra:BOOL=ON -DTrilinos_ENABLE_Teuchos:BOOL=OFF
    -DTrilinos_ENABLE_ALL_FORWARD_DEP_PACKAGES:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Setting Trilinos_ENABLE_RTOp=OFF because RTOp has a required library dependence on disabled package Teuchos"
    "Final set of enabled packages:  Epetra Zoltan Triutils AztecOO ML 5"
  )


create_dependency_handling_test_case(
  DisableEpetra_DisableTeuchos_EnableAllPackages
  ARGS -DTrilinos_ENABLE_Epetra:BOOL=OFF -DTrilinos_ENABLE_Teuchos:BOOL=OFF -DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled packages:  TrilinosFramework Zoltan Shards ML 4"
  )


create_dependency_handling_test_case(
  EnableTeuchos_EnableAllForwardPackages
  ARGS -DTrilinos_ENABLE_Teuchos:BOOL=ON -DTrilinos_ENABLE_ALL_FORWARD_DEP_PACKAGES:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Stratimikos RBGen 18"
  )


create_dependency_handling_test_case(
  EnableEpetraExt_EnableAllForwardPackages
  ARGS -DTrilinos_ENABLE_EpetraExt:BOOL=ON -DTrilinos_ENABLE_ALL_FORWARD_DEP_PACKAGES:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Stratimikos RBGen 18"
  )


create_dependency_handling_test_case(
  EnableEpetraExt
  ARGS -DTrilinos_ENABLE_EpetraExt:BOOL=ON -DTrilinos_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=OFF
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled packages:  Teuchos Epetra EpetraExt 3"
  )


create_dependency_handling_test_case(
  EnableEpetraExt_EnableAllOptionalPackages
  ARGS -DTrilinos_ENABLE_EpetraExt:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled packages:  Teuchos Epetra Zoltan Triutils EpetraExt 5"
  )


create_dependency_handling_test_case(
  EnableEpetraExt_DisableAztecOO_EnableAllForwardPackages
  ARGS -DTrilinos_ENABLE_EpetraExt:BOOL=ON -DTrilinos_ENABLE_AztecOO:BOOL=OFF
    -DTrilinos_ENABLE_ALL_FORWARD_DEP_PACKAGES:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "NOTE: Not Setting Trilinos_ENABLE_Belos=ON even though Stratimikos has an optional dependence on Belos because Trilinos_ENABLE_SECONDARY_TESTED_CODE=OFF"
    "NOTE: Not Setting Trilinos_ENABLE_Sacado=ON even though Intrepid has an optional dependence on Sacado because Trilinos_ENABLE_SECONDARY_TESTED_CODE=OFF"
    "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Thyra Isorropia Galeri Amesos Intrepid Ifpack ML Stratimikos RBGen 17"
  )


create_dependency_handling_test_case(
  EnableAmesos_DisableZoltan_EnableAllForwardPackages
  ARGS -DTrilinos_ENABLE_Amesos:BOOL=ON -DTrilinos_ENABLE_Zoltan:BOOL=OFF
    -DTrilinos_ENABLE_ALL_FORWARD_DEP_PACKAGES:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled packages:  Teuchos RTOp Epetra Shards Triutils EpetraExt Thyra AztecOO Galeri Amesos Intrepid Ifpack ML Stratimikos 14"
  )


create_dependency_handling_test_case(
  EnableThyraCoreLibs_EnableTests
  ARGS -DTrilinos_ENABLE_ThyraCoreLibs:BOOL=ON -DTrilinos_ENABLE_TESTS:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Setting Trilinos_ENABLE_Thyra=ON because Trilinos_ENABLE_ThyraCoreLibs=ON"
    "Setting Thyra_ENABLE_TESTS=ON because ThyraCoreLibs_ENABLE_TESTS=ON"
    "Setting Thyra_ENABLE_EXAMPLES=ON because ThyraCoreLibs_ENABLE_EXAMPLES=ON"
    "Final set of enabled packages:  Teuchos RTOp Thyra 3"
    "Final set of enabled SE packages:  Teuchos RTOp ThyraCoreLibs Thyra 4"
  )


create_dependency_handling_test_case(
  EnableThyra_EnableAllForwardPackages
  ARGS -DTrilinos_ENABLE_Thyra:BOOL=ON -DTrilinos_ENABLE_ALL_FORWARD_DEP_PACKAGES:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Explicitly enabled packages on input .by user.:  Thyra 1"
    "Enabling subpackages for hard enables of parent packages due to Trilinos_ENABLE_<PARENT_PACKAGE>=ON ..."
    "Setting subpackage enable Trilinos_ENABLE_ThyraCoreLibs=ON because parent package Trilinos_ENABLE_Thyra=ON"
    "Setting subpackage enable Trilinos_ENABLE_ThyraEpetra=ON because parent package Trilinos_ENABLE_Thyra=ON"
    "Setting subpackage enable Trilinos_ENABLE_ThyraEpetraExt=ON because parent package Trilinos_ENABLE_Thyra=ON"
    "Setting subpackage enable Trilinos_ENABLE_ThyraTpetra=ON because parent package Trilinos_ENABLE_Thyra=ON"
    "Sweep forward enabling all forward library dependent packages because Trilinos_ENABLE_ALL_FORWARD_DEP_PACKAGES=ON ..."
    "Setting Trilinos_ENABLE_Stratimikos=ON because Trilinos_ENABLE_ThyraCoreLibs=ON"
    "Final set of enabled packages:  .+ Thyra .+ Stratimikos"
    "Final set of non-enabled SE packages:  .+ ThyraGoodStuff ThyraCrazyStuff"
  )


create_dependency_handling_test_case(
  EnableThyra_DisableThyraEpetraExt
  ARGS -DTrilinos_ENABLE_Thyra:BOOL=ON -DTrilinos_ENABLE_ThyraEpetraExt:BOOL=OFF
  PASS_REGULAR_EXPRESSION_ALL
    "Setting subpackage enable Trilinos_ENABLE_ThyraCoreLibs=ON because parent package Trilinos_ENABLE_Thyra=ON"
    "Setting subpackage enable Trilinos_ENABLE_ThyraEpetra=ON because parent package Trilinos_ENABLE_Thyra=ON"
    "Setting subpackage enable Trilinos_ENABLE_ThyraTpetra=ON because parent package Trilinos_ENABLE_Thyra=ON"
    "Setting Thyra_ENABLE_ThyraCrazyStuff=OFF because Thyra has an optional library dependence on disabled package ThyraCrazyStuff"
    "Setting Trilinos_ENABLE_Stratimikos=OFF because Stratimikos has a required library dependence on disabled package ThyraEpetraExt"
    "Setting Thyra_ENABLE_ThyraEpetraExt=OFF because Thyra has an optional library dependence on disabled package ThyraEpetraExt"
    "Final set of enabled packages:  Teuchos RTOp Epetra Triutils Tpetra Thyra 6"
    "Final set of enabled SE packages:  Teuchos RTOp Epetra Triutils Tpetra ThyraCoreLibs ThyraEpetra ThyraTpetra Thyra 9"
    "Final set of non-enabled packages:  TrilinosFramework Zoltan Shards EpetraExt Stokhos Sacado Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer 18"
    "Final set of non-enabled SE packages:  TrilinosFramework Zoltan Shards EpetraExt Stokhos Sacado ThyraGoodStuff ThyraCrazyStuff ThyraEpetraExt Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer 21"
  )


create_dependency_handling_test_case(
  EnableThyraEpetraExt
  ARGS -DTrilinos_ENABLE_ThyraEpetraExt:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Explicitly enabled packages on input .by user.:  0"
    "Explicitly enabled SE packages on input .by user.:  ThyraEpetraExt 1"
    "Setting Trilinos_ENABLE_ThyraEpetra=ON because ThyraEpetraExt has a required dependence on ThyraEpetra"
    "Setting Trilinos_ENABLE_ThyraCoreLibs=ON because ThyraEpetra has a required dependence on ThyraCoreLibs"
    "Setting Trilinos_ENABLE_Teuchos=ON because ThyraCoreLibs has a required dependence on Teuchos"
    "Setting Trilinos_ENABLE_Thyra=ON because Trilinos_ENABLE_ThyraCoreLibs=ON"
    "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Triutils EpetraExt Thyra 7"
    "Final set of non-enabled SE packages:  .+ ThyraGoodStuff ThyraCrazyStuff ThyraTpetra .+"
  )


create_dependency_handling_test_case(
  DisableThyra_EnableThyraEpetraExt
  ARGS -DTrilinos_ENABLE_Thyra:BOOL=OFF -DTrilinos_ENABLE_ThyraEpetraExt:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Setting subpackage enable Trilinos_ENABLE_ThyraEpetraExt=OFF because parent package Trilinos_ENABLE_Thyra=OFF"
    "Final set of enabled packages:  0"
    "Final set of enabled SE packages:  0"
  )


create_dependency_handling_test_case(
  EnableThyra_DisableThyraEpetra_EnableThyraEpetraExt_DisableEnabledDownstreamPackages
  ARGS -DTrilinos_ENABLE_Thyra:BOOL=ON -DTrilinos_ENABLE_ThyraEpetra:BOOL=OFF
    -DTrilinos_ENABLE_ThyraEpetraExt:BOOL=ON
    -DTrilinos_DISABLE_ENABLED_FORWARD_DEP_PACKAGES=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Explicitly enabled packages on input .by user.:  Thyra 1"
    "Explicitly enabled SE packages on input .by user.:  ThyraEpetraExt Thyra 2"
    "Explicitly disabled SE packages on input .by user or by default.:  .+ ThyraEpetra"
    "NOTE: Setting Trilinos_ENABLE_ThyraEpetraExt=OFF which was 'ON' because ThyraEpetraExt has a required library dependence on disabled package ThyraEpetra but Trilinos_DISABLE_ENABLED_FORWARD_DEP_PACKAGES=ON!"
    "Setting Thyra_ENABLE_ThyraEpetra=OFF because Thyra has an optional library dependence on disabled package ThyraEpetra"
    "Final set of enabled packages:  Teuchos RTOp Epetra Triutils Tpetra Thyra 6"
    "Final set of enabled SE packages:  Teuchos RTOp Epetra Triutils Tpetra ThyraCoreLibs ThyraTpetra Thyra 8"
  )


create_dependency_handling_test_case(
  EnableThyra_DisableEpetra_EnableThyraEpetraExt_DisableEnabledDownstreamPackages
  ARGS -DTrilinos_ENABLE_Thyra:BOOL=ON -DTrilinos_ENABLE_Epetra:BOOL=OFF
    -DTrilinos_ENABLE_ThyraEpetraExt:BOOL=ON 
    -DTrilinos_DISABLE_ENABLED_FORWARD_DEP_PACKAGES=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Explicitly enabled packages on input .by user.:  Thyra 1"
    "Explicitly enabled SE packages on input .by user.:  ThyraEpetraExt Thyra 2"
    "Explicitly disabled packages on input .by user or by default.:  Epetra"
    "Setting Trilinos_ENABLE_ThyraEpetra=OFF because ThyraEpetra has a required library dependence on disabled package Epetra"
    "NOTE: Setting Trilinos_ENABLE_ThyraEpetraExt=OFF which was 'ON' because ThyraEpetraExt has a required library dependence on disabled package EpetraExt but Trilinos_DISABLE_ENABLED_FORWARD_DEP_PACKAGES=ON!"
    "Final set of enabled packages:  Teuchos RTOp Tpetra Thyra 4"
    "Final set of enabled SE packages:  Teuchos RTOp Tpetra ThyraCoreLibs ThyraTpetra Thyra 6"
  )


create_dependency_handling_test_case(
  EnableStratimikos_DisableThyra
  ARGS -DTrilinos_ENABLE_Stratimikos=ON -DTrilinos_ENABLE_Thyra=OFF
  PASS_REGULAR_EXPRESSION_ALL
    "Setting subpackage enable Trilinos_ENABLE_ThyraCoreLibs=OFF because parent package Trilinos_ENABLE_Thyra=OFF"
    "Setting subpackage enable Trilinos_ENABLE_ThyraGoodStuff=OFF because parent package Trilinos_ENABLE_Thyra=OFF" 
    "Setting subpackage enable Trilinos_ENABLE_ThyraEpetra=OFF because parent package Trilinos_ENABLE_Thyra=OFF"
    "Setting subpackage enable Trilinos_ENABLE_ThyraEpetraExt=OFF because parent package Trilinos_ENABLE_Thyra=OFF"
    "Setting subpackage enable Trilinos_ENABLE_ThyraTpetra=OFF because parent package Trilinos_ENABLE_Thyra=OFF"
    "ERROR: Setting Trilinos_ENABLE_Stratimikos=OFF which was 'ON' because Stratimikos has a required library dependence on disabled package ThyraCoreLibs"
  )


create_dependency_handling_test_case(
  EnableStratimikos_DisableThyra_DisableEnabledDownstreamPackages
  ARGS -DTrilinos_ENABLE_Stratimikos=ON -DTrilinos_ENABLE_Thyra=OFF
    -DTrilinos_DISABLE_ENABLED_FORWARD_DEP_PACKAGES=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Setting subpackage enable Trilinos_ENABLE_ThyraCoreLibs=OFF because parent package Trilinos_ENABLE_Thyra=OFF"
    "Setting subpackage enable Trilinos_ENABLE_ThyraGoodStuff=OFF because parent package Trilinos_ENABLE_Thyra=OFF" 
    "Setting subpackage enable Trilinos_ENABLE_ThyraEpetra=OFF because parent package Trilinos_ENABLE_Thyra=OFF"
    "Setting subpackage enable Trilinos_ENABLE_ThyraEpetraExt=OFF because parent package Trilinos_ENABLE_Thyra=OFF"
    "Setting subpackage enable Trilinos_ENABLE_ThyraTpetra=OFF because parent package Trilinos_ENABLE_Thyra=OFF"
    "NOTE: Setting Trilinos_ENABLE_Stratimikos=OFF which was 'ON' because Stratimikos has a required library dependence on disabled package ThyraCoreLibs but Trilinos_DISABLE_ENABLED_FORWARD_DEP_PACKAGES=ON"
    "Final set of enabled packages:  0"
    "Final set of non-enabled packages:  .+ Teuchos .+ Thyra .+ Stratimikos .+"
  )


create_dependency_handling_test_case(
  EnableEpetra_DisableThyraCoreLibs_EnableAllForwardPackages
  ARGS -DTrilinos_ENABLE_Epetra:BOOL=ON -DTrilinos_ENABLE_ThyraCoreLibs:BOOL=OFF
    -DTrilinos_ENABLE_ALL_FORWARD_DEP_PACKAGES:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled packages:  Teuchos Epetra Zoltan Shards Triutils Tpetra EpetraExt Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML RBGen 15"
  )


create_dependency_handling_test_case(
  EnableAllPackages_DefaultTPLs
  ARGS -DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled TPLs:  BLAS LAPACK DUMMY 3"
    "Final set of non-enabled TPLs:  MPI Boost Scotch METIS ParMETIS CppUnit ADOLC ADIC TVMET y12m SuperLUDist SuperLU UMFPACK AMD PETSC MUMPS 16"
  )


create_dependency_handling_test_case(
  EnableAllPackages_EnableBoost
  ARGS -DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON -DTPL_ENABLE_Boost:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled TPLs:  BLAS LAPACK Boost DUMMY 4"
  )


create_dependency_handling_test_case(
  EnableAllPackages_EnableBoostParMETIS
  ARGS -DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON -DTPL_ENABLE_Boost:BOOL=ON
    -DTPL_ENABLE_ParMETIS:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled TPLs:  BLAS LAPACK Boost ParMETIS DUMMY 5"
  )


create_dependency_handling_test_case(
  EnableAllPackages_EnableBoost_TeuchosEnableBoostOff
  ARGS -DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON -DTPL_ENABLE_Boost:BOOL=ON
    -DTeuchos_ENABLE_Boost=OFF
  PASS_REGULAR_EXPRESSION_ALL
    "Enabling all optional package TPL support <TRIBITS_PACKAGE>_ENABLE_<DEPTPL> not currently disabled for enabled TPLs"
    "-- NOTE: Teuchos_ENABLE_Boost=OFF is already set so not enabling even though TPL_ENABLE_Boost=ON is set!"
  )


create_dependency_handling_test_case(
  EnableAllPackages_EnableBoost_TeuchosEnableBoostOn
  ARGS -DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON -DTPL_ENABLE_Boost:BOOL=ON
    -DTeuchos_ENABLE_Boost=ON
  PASS_REGULAR_EXPRESSION_ALL
    "-- NOTE: Teuchos_ENABLE_Boost=ON is already set!"
  )


create_dependency_handling_test_case(
  EnableZoltan_ZoltanEnableParMETIS
  ARGS  -DTrilinos_ENABLE_Zoltan=ON -DZoltan_ENABLE_ParMETIS=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Setting TPL_ENABLE_ParMETIS=ON because Zoltan_ENABLE_ParMETIS=ON"
    "Final set of enabled TPLs:  ParMETIS 1"
  )


create_dependency_handling_test_case(
  EnableZoltan_ZoltanEnableParMETIS_TplDisableParMETIS
  ARGS -DTrilinos_ENABLE_Zoltan=ON -DZoltan_ENABLE_ParMETIS=ON -DTPL_ENABLE_ParMETIS=OFF
  PASS_REGULAR_EXPRESSION_ALL
    "NOTE: Setting Zoltan_ENABLE_ParMETIS=OFF which was ON since TPL_ENABLE_ParMETIS=OFF"
    "Final set of enabled TPLs:  0"
  )


create_dependency_handling_test_case(
  EnablePanzer_DefaultMPI
  ARGS  -DTrilinos_ENABLE_Panzer=ON
    -DTrilinos_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=OFF
  PASS_REGULAR_EXPRESSION_ALL
    "Setting Trilinos_ENABLE_Panzer=OFF which was 'ON' because Panzer has a required library dependence on disabled TPL MPI!"
  )


create_dependency_handling_test_case(
  EnablePanzer_EnableMPI
  ARGS  -DTrilinos_ENABLE_Panzer=ON
    -DTPL_ENABLE_MPI:BOOL=ON
    -DTrilinos_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=OFF
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled packages:  .*Panzer"
  )


create_dependency_handling_test_case(
  EnablePanzer_DisableMPI
  ARGS  -DTrilinos_ENABLE_Panzer=ON
    -DTPL_ENABLE_MPI:BOOL=OFF
    -DTrilinos_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=OFF
  PASS_REGULAR_EXPRESSION_ALL
    "ERROR: Setting Trilinos_ENABLE_Panzer=OFF which was 'ON' because Panzer has a required library dependence on disabled TPL MPI!"
    "Configuring incomplete, errors occurred!"
  )


create_dependency_handling_test_case(
  EnablePanzer_DisableMPI_DisableEnabledDownstreamPackages
  ARGS  -DTrilinos_ENABLE_Panzer=ON
    -DTPL_ENABLE_MPI:BOOL=OFF
    -DTrilinos_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=OFF
    -DTrilinos_DISABLE_ENABLED_FORWARD_DEP_PACKAGES=ON
  PASS_REGULAR_EXPRESSION_ALL
    "NOTE: Setting Trilinos_ENABLE_Panzer=OFF which was 'ON' because Panzer has a required library dependence on disabled TPL MPI but Trilinos_DISABLE_ENABLED_FORWARD_DEP_PACKAGES=ON"
    "Final set of enabled packages:  0"
    "Final set of non-enabled packages:  .+ Panzer .+"
  )


create_dependency_handling_test_case(
  NoOptionalPackages_EnableSubpackagesOnly
  ARGS
    -DTrilinos_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=OFF
    -DTrilinos_ENABLE_ThyraEpetra=ON
    -DTrilinos_ENABLE_ThyraGoodStuff=ON
  PASS_REGULAR_EXPRESSION_ALL
    "-- Setting Trilinos_ENABLE_Thyra=ON because Trilinos_ENABLE_ThyraCoreLibs=ON"
    "-- Setting Thyra_ENABLE_ThyraCoreLibs=ON because Trilinos_ENABLE_ThyraCoreLibs=ON"
    "-- Setting Thyra_ENABLE_ThyraGoodStuff=ON because Trilinos_ENABLE_ThyraGoodStuff=ON"
    "-- Setting Thyra_ENABLE_ThyraEpetra=ON because Trilinos_ENABLE_ThyraEpetra=ON"
    "Final set of enabled packages:  Teuchos RTOp Epetra Thyra 4"
    "Final set of enabled SE packages:  Teuchos RTOp Epetra ThyraCoreLibs ThyraGoodStuff ThyraEpetra Thyra 7"
  )


create_dependency_handling_test_case(
  EnableTpetra_EnableAllForwardPackages
  ARGS  -DTrilinos_ENABLE_Tpetra=ON
    -DTrilinos_ENABLE_ALL_FORWARD_DEP_PACKAGES:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Triutils Tpetra EpetraExt Thyra 8"
    "Final set of enabled SE packages:  Teuchos RTOp Epetra Zoltan Triutils Tpetra EpetraExt ThyraCoreLibs ThyraEpetra ThyraEpetraExt ThyraTpetra Thyra 12"
  )


create_dependency_handling_test_case(
  EnableAllPackages_EnableSecondaryTestedCode_EnableAllOptionalPackges
  ARGS  -DTrilinos_ENABLE_ALL_PACKAGES=ON -DTrilinos_ENABLE_SECONDARY_TESTED_CODE=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Final set of enabled packages:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx 22"
    "Final set of enabled TPLs:  BLAS LAPACK Boost DUMMY 4"
  )


create_dependency_handling_test_case(
  EnableStratimikos_EnableAllOptionalPackages_BadSystem1
  ARGS  -DTrilinos_ENABLE_Stratimikos=ON
    -DTrilinos_HOSTTYPE:STRING=BadSystem1
  PASS_REGULAR_EXPRESSION_ALL
    "Explicitly disabled packages on input .+:  Stokhos Ifpack ML 3"
    "Setting Stratimikos_ENABLE_Ifpack=OFF because Stratimikos has an optional library dependence on disabled package Ifpack"
    "Setting Stratimikos_ENABLE_ML=OFF because Stratimikos has an optional library dependence on disabled package ML"
    "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Triutils EpetraExt Thyra AztecOO Galeri Amesos Stratimikos 11"
    "Final set of non-enabled packages:  TrilinosFramework Shards Tpetra Stokhos Sacado Isorropia Intrepid Ifpack ML Belos RBGen Phalanx Panzer 13"
  )


create_dependency_handling_test_case(
  EnableStratimikos_EnableAllOptionalPackages_BadSystem2
  ARGS  -DTrilinos_ENABLE_Stratimikos=ON
    -DTrilinos_HOSTTYPE:STRING=BadSystem2
  PASS_REGULAR_EXPRESSION_ALL
    "Explicitly disabled packages on input .+:  Stokhos Ifpack 2"
    "Setting Stratimikos_ENABLE_Ifpack=OFF because Stratimikos has an optional library dependence on disabled package Ifpack"
    "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Triutils EpetraExt Thyra Isorropia AztecOO Galeri Amesos ML Stratimikos 13"
    "Final set of enabled SE packages:  Teuchos RTOp Epetra Zoltan Triutils EpetraExt ThyraCoreLibs ThyraEpetra ThyraEpetraExt Thyra Isorropia AztecOO Galeri Amesos ML Stratimikos 16"
    "Final set of non-enabled packages:  TrilinosFramework Shards Tpetra Stokhos Sacado Intrepid Ifpack Belos RBGen Phalanx Panzer 11"
    "Final set of non-enabled SE packages:  TrilinosFramework Shards Tpetra Stokhos Sacado ThyraGoodStuff ThyraCrazyStuff ThyraTpetra Intrepid Ifpack Belos RBGen Phalanx Panzer 14"
  )


create_dependency_handling_test_case(
  EnableStratimikos_EnableIfpack_EnableAllOptionalPackages_BadSystem2
  ARGS  -DTrilinos_ENABLE_Stratimikos=ON -DTrilinos_ENABLE_Ifpack=ON
    -DTrilinos_HOSTTYPE:STRING=BadSystem2
  PASS_REGULAR_EXPRESSION_ALL
    "NOTE: User has set Trilinos_ENABLE_Ifpack=ON but the" 
    "package Ifpack is not supported on this platform type 'BadSystem2'"
    "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Triutils EpetraExt Thyra Isorropia AztecOO Galeri Amesos Ifpack ML Stratimikos 14"
    "Final set of enabled SE packages:  Teuchos RTOp Epetra Zoltan Triutils EpetraExt ThyraCoreLibs ThyraEpetra ThyraEpetraExt Thyra Isorropia AztecOO Galeri Amesos Ifpack ML Stratimikos 17"
  )


create_dependency_handling_test_case(
  preCopyrightTrilinos_EnableTeko
  ARGS -DTrilinos_EXTRA_REPOSITORIES=preCopyrightTrilinos
    -DTrilinos_ENABLE_Teko=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Trilinos_NUM_PACKAGES='26'"
    "Trilinos_NUM_TPLS='20'"
    "Explicitly enabled packages on input .* Teko 1"
    "Explicitly disabled packages on input .* Stokhos Stalix 2"
    "Setting Trilinos_ENABLE_Teuchos=ON because Teko has a required dependence on Teuchos"
    "Setting Trilinos_ENABLE_Epetra=ON because Teko has a required dependence on Epetra"
    "Setting Trilinos_ENABLE_Thyra=ON because Teko has a required dependence on Thyra"
    "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Triutils Tpetra EpetraExt Thyra Isorropia AztecOO Galeri Amesos Ifpack ML Stratimikos Teko 16"
    "Final set of non-enabled packages:  TrilinosFramework Shards Stokhos Sacado Intrepid Belos RBGen Phalanx Panzer Stalix 10"
    "Final set of enabled TPLs:  BLAS LAPACK TekoDepTPL 3"
  )


create_dependency_handling_test_case(
  preCopyrightTrilinos_EnableTeko_InconsistentExtraReposFile
  ARGS -DTrilinos_EXTRA_REPOSITORIES=preCopyrightTrilinos
    -DTrilinos_EXTRAREPOS_FILE=${CMAKE_CURRENT_SOURCE_DIR}/NoPackagesExtraReposFile.cmake
    -DTrilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Nightly
    -DTrilinos_IGNORE_MISSING_EXTRA_REPOSITORIES=ON
    #-DTrilinos_VERBOSE_CONFIGURE=ON
    #-DTRIBITS_PROCESS_EXTRAREPOS_LISTS_DEBUG=ON
    -DTrilinos_ENABLE_Teko=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Trilinos_NUM_PACKAGES='26'"
    "Trilinos_NUM_TPLS='20'"
    "Explicitly enabled packages on input .* Teko 1"
  )


create_dependency_handling_test_case(
  DefaultExtraReposFile_EnableAllPackages_TraceFileProcessing
  ARGS 
    -DTrilinos_EXTRAREPOS_FILE=${MOCK_TRILINOS_DIR}/cmake/ExtraRepositoriesList.cmake
    -DTrilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Nightly
    -DTrilinos_IGNORE_MISSING_EXTRA_REPOSITORIES=TRUE
    -DTrilinos_TRACE_FILE_PROCESSING=ON
    -DTrilinos_ENABLE_ALL_PACKAGES=ON
  PASS_REGULAR_EXPRESSION_ALL
    "-- File Trace: PROJECT    INCLUDE    .*/MockTrilinos/Version.cmake"
    "-- File Trace: REPOSITORY INCLUDE    .*/MockTrilinos/cmake/CallbackSetupExtraOptions.cmake"
    "-- File Trace: REPOSITORY INCLUDE    .*/MockTrilinos/PackagesList.cmake"
    "-- File Trace: REPOSITORY INCLUDE    .*/MockTrilinos/TPLsList.cmake"
    "-- File Trace: REPOSITORY INCLUDE    .*/MockTrilinos/preCopyrightTrilinos/PackagesList.cmake"
    "-- File Trace: REPOSITORY INCLUDE    .*/MockTrilinos/preCopyrightTrilinos/TPLsList.cmake"
    "-- File Trace: REPOSITORY INCLUDE    .*/MockTrilinos/extraTrilinosRepo/PackagesList.cmake"
    "-- File Trace: REPOSITORY INCLUDE    .*/MockTrilinos/extraTrilinosRepo/TPLsList.cmake"
    "-- File Trace: REPOSITORY INCLUDE    .*/MockTrilinos/cmake/RepositoryDependenciesSetup.cmake"
    "-- File Trace: PROJECT    INCLUDE    .*/MockTrilinos/cmake/ProjectDependenciesSetup.cmake"
    "-- File Trace: PACKAGE    INCLUDE    .*/MockTrilinos/cmake/cmake/Dependencies.cmake"
    "-- File Trace: PACKAGE    INCLUDE    .*/MockTrilinos/packages/teuchos/cmake/Dependencies.cmake"
    "-- File Trace: PACKAGE    INCLUDE    .*/MockTrilinos/packages/thyra/cmake/Dependencies.cmake"
    "-- File Trace: PACKAGE    INCLUDE    .*/MockTrilinos/packages/thyra/src/cmake/Dependencies.cmake"
    "-- File Trace: PACKAGE    INCLUDE    .*/MockTrilinos/packages/thyra/src/good_stuff/cmake/Dependencies.cmake"
    "-- File Trace: PACKAGE    INCLUDE    .*/MockTrilinos/packages/thyra/src/crazy_stuff/cmake/Dependencies.cmake"
    "-- File Trace: PACKAGE    INCLUDE    .*/MockTrilinos/packages/thyra/adapters/epetra/cmake/Dependencies.cmake"
    "-- File Trace: PACKAGE    INCLUDE    .*/MockTrilinos/packages/thyra/adapters/epetraext/cmake/Dependencies.cmake"
    "-- File Trace: PACKAGE    INCLUDE    .*/MockTrilinos/packages/thyra/adapters/tpetra/cmake/Dependencies.cmake"
    "-- File Trace: PACKAGE    INCLUDE    .*/MockTrilinos/preCopyrightTrilinos/teko/cmake/Dependencies.cmake"
    "-- File Trace: PACKAGE    INCLUDE    .*/MockTrilinos/preCopyrightTrilinos/stalix/cmake/Dependencies.cmake"
    "-- File Trace: PACKAGE    INCLUDE    .*/MockTrilinos/extraTrilinosRepo/extrapack/cmake/Dependencies.cmake"
  )


create_dependency_handling_test_case(
  PreAndExtraReposFile_Nightly_EnableAllPackages
  ARGS 
    -DTrilinos_EXTRAREPOS_FILE=${CMAKE_CURRENT_SOURCE_DIR}/PreAndExtraRepositoriesList.cmake
    -DTrilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Nightly
    -DTrilinos_ENABLE_SECONDARY_TESTED_CODE:BOOL=ON
    -DTrilinos_IGNORE_MISSING_EXTRA_REPOSITORIES=TRUE
    -DTrilinos_ENABLE_ALL_PACKAGES=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Selecting the set of .Nightly. extra repos .ignoring missing repos. [.][.][.]"
    "-- Adding PRE extra Continuous repository preRepoOnePackage"
    "-- Adding POST extra Continuous repository preCopyrightTrilinos"
    "-- Adding POST extra Nightly repository extraTrilinosRepo"
    "NOTE: Ignoring missing extra repo 'Dakota' as requested since .*/examples/MockTrilinos/packages/TriKota/Dakota does not exist"
    "Reading list of PRE extra packages from .*/MockTrilinos/preRepoOnePackage/PackagesList.cmake"
    "Reading list of PRE extra TPLs from .*/MockTrilinos/preRepoOnePackage/TPLsList.cmake"
    "Final set of enabled SE packages:  preRepoOnePackage TrilinosFramework Teuchos .* Stratimikos RBGen Phalanx Teko ExtraPack [0-9][0-9]"
  )
# NOTE: The above test shows that PRE extra repos are processed correctly and
# their packages are listed before packages in the native project.


create_dependency_handling_test_case(
  PreAndExtraReposFile_Continuous_EnableAllPackages
  ARGS 
    -DTrilinos_EXTRAREPOS_FILE=${CMAKE_CURRENT_SOURCE_DIR}/PreAndExtraRepositoriesList.cmake
    -DTrilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Continuous
    -DTrilinos_ENABLE_SECONDARY_TESTED_CODE:BOOL=ON
    -DTrilinos_IGNORE_MISSING_EXTRA_REPOSITORIES=TRUE
    -DTrilinos_ENABLE_ALL_PACKAGES=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Selecting the set of .Continuous. extra repos .ignoring missing repos. [.][.][.]"
    "-- Adding PRE extra Continuous repository preRepoOnePackage"
    "-- Adding POST extra Continuous repository preCopyrightTrilinos"
    "-- .NOT. adding POST extra Nightly repository extraTrilinosRepo"
    "NOTE: Ignoring missing extra repo 'Dakota' as requested since .*/examples/MockTrilinos/packages/TriKota/Dakota does not exist"
    "Reading list of PRE extra packages from .*/MockTrilinos/preRepoOnePackage/PackagesList.cmake"
    "Reading list of PRE extra TPLs from .*/MockTrilinos/preRepoOnePackage/TPLsList.cmake"
    "Final set of enabled SE packages:  preRepoOnePackage TrilinosFramework Teuchos .* Stratimikos RBGen Phalanx Teko [0-9][0-9]"
  )
# NOTE: The above test shows that Nightly repos are not processed but are
# printed out as not being processed!


create_dependency_handling_test_case(
  PreAndExtraReposFile_Continuous_AssertMissing_EnableAllPackages
  ARGS 
    -DTrilinos_EXTRAREPOS_FILE=${CMAKE_CURRENT_SOURCE_DIR}/PreAndExtraRepositoriesList.cmake
    -DTrilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Continuous
    -DTrilinos_ENABLE_SECONDARY_TESTED_CODE:BOOL=ON
    -DTrilinos_IGNORE_MISSING_EXTRA_REPOSITORIES=FALSE
    -DTrilinos_ENABLE_ALL_PACKAGES=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Selecting the set of .Continuous. extra repos .asserting all selected repos exist. [.][.][.]"
    "-- Adding PRE extra Continuous repository preRepoOnePackage"
    "-- Adding POST extra Continuous repository preCopyrightTrilinos"
    "-- .NOT. adding POST extra Nightly repository extraTrilinosRepo"
    "CMake Error at .*/TribitsProcessExtraRepositoriesList.cmake"
    "ERROR. Skipping missing extra repo .Dakota. since"
    "Configuring incomplete, errors occurred"
  )
# NOTE: The above test shows that a missing repo is flagged and aborts
# configuration and that it prints the right header message!


create_dependency_handling_test_case(
  preCopyrightTrilinos_EnableStalix
  ARGS -DTrilinos_EXTRA_REPOSITORIES=preCopyrightTrilinos
    -DTrilinos_ENABLE_Stalix=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Explicitly enabled packages on input .* Stalix 1"
    "Explicitly disabled packages on input .* Stokhos 1"
    "NOTE: Not Setting Trilinos_ENABLE_ThyraGoodStuff=ON even though Thyra has an optional dependence on ThyraGoodStuff because Trilinos_ENABLE_SECONDARY_TESTED_CODE=OFF"
    "Setting Trilinos_ENABLE_Tpetra=ON because Stalix has a required dependence on Tpetra"
    "Setting Trilinos_ENABLE_Thyra=ON because Stalix has a required dependence on Thyra"
    "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Triutils Tpetra EpetraExt Sacado Thyra Stalix 10"
    "Final set of non-enabled packages:  TrilinosFramework Shards Stokhos Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Panzer Teko 16"
  )


create_dependency_handling_test_case(
  preCopyrightTrilinos_ShowInvalidPackageNameError
  ARGS -DTrilinos_EXTRA_REPOSITORIES=preCopyrightTrilinos
    -DSHOW_INVALID_PACKAGE_NAME_ERROR=ON
    -DTrilinos_ASSERT_MISSING_PACKAGES=ON
  PASS_REGULAR_EXPRESSION_ALL
    "CMake Error at .*/TribitsReadDepsFilesCreateDepsGraph.cmake:.+ [(]message[)]:"
    "Error, the package 'Anasazi' is listed as a dependency of the package"
    "'Teko' is in the list 'Trilinos_SE_PACKAGES' but the package 'Anasazi' is"
    "either not defined or is listed later in the package order[.]  This may also"
    "be an attempt to create a circular dependency between the packages"
    "'Anasazi' and 'Teko' [(]which is not allowed[)][.]  Check the spelling of"
    "'Anasazi' or see how it is listed in a call to"
    "tribits_repository_define_packages[(][)] in relation to 'Teko'[.]"
  )


create_dependency_handling_test_case(
  preCopyrightTrilinos_ShowInvalidPackageSelfDependency
  ARGS -DTrilinos_EXTRA_REPOSITORIES=preCopyrightTrilinos
    -DSHOW_SELF_PACKAGE_DEPENDENCY_ERROR=ON
    -DTrilinos_ASSERT_MISSING_PACKAGES=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Error, the package 'Teko' is listed as a dependency of itself in the list"
    " 'LIB_REQUIRED_DEP_PACKAGES'!"
  )


create_dependency_handling_test_case(
  preCopyrightTrilinos_ShowInvalidSubpackageSelfDependency
  ARGS
    -DSHOW_SELF_SUBPACKAGE_DEPENDENCY_ERROR=ON
    -DTrilinos_ASSERT_MISSING_PACKAGES=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Error, the package 'ThyraTpetra' is listed as a dependency of itself in the"
     " list 'LIB_REQUIRED_DEP_PACKAGES'!"
  )


create_dependency_handling_test_case(
  MissingExtraRepositoryFail
  ARGS -DTrilinos_EXTRA_REPOSITORIES=MissingExtraRepo
    -DTrilinos_IGNORE_MISSING_EXTRA_REPOSITORIES=OFF
  PASS_REGULAR_EXPRESSION_ALL
    "ERROR: Skipping missing POST extra repo 'MissingExtraRepo' packages list"
    "ERROR: Skipping missing POST extra repo 'MissingExtraRepo' TPLs list"
    "Configuring incomplete, errors occurred"
  )


create_dependency_handling_test_case(
  MissingExtraRepositoryIgnore
  ARGS -DTrilinos_EXTRA_REPOSITORIES=MissingExtraRepo
    -DTrilinos_IGNORE_MISSING_EXTRA_REPOSITORIES=ON
    -DTrilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=
    -DTRIBITS_PROCESS_EXTRAREPOS_LISTS_DEBUG=TRUE
  PASS_REGULAR_EXPRESSION_ALL
    "NOTE: Ignoring missing POST extra repo 'MissingExtraRepo' packages list file ..+/examples/MockTrilinos/MissingExtraRepo/PackagesList.cmake. on request!"
    "NOTE: Ignoring missing POST extra repo 'MissingExtraRepo' TPLs list file ..+/examples/MockTrilinos/MissingExtraRepo/TPLsList.cmake. on request!"
    "Configuring done"
    "Generating done"
  )


create_dependency_handling_test_case(
  NativeRepositoriesList1_EnableAllPackages_ST
  ARGS -DTrilinos_NATIVE_REPO_FILE_OVERRRIDE=cmake/NativeRepositoriesList1.cmake
    -DTrilinos_ENABLE_SECONDARY_TESTED_CODE:BOOL=ON
    -DTrilinos_ENABLE_ALL_PACKAGES:BOOL=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Reading list of native packages from .*/MockTrilinos/PackagesList.cmake"
    "Reading list of native packages from .*/MockTrilinos/preCopyrightTrilinos/PackagesList.cmake"
    "Reading list of native packages from .*/MockTrilinos/extraTrilinosRepo/PackagesList.cmake"
    "Final set of enabled packages:  TrilinosFramework Teuchos RTOp Epetra Zoltan Shards Triutils Tpetra EpetraExt Sacado Thyra Isorropia AztecOO Galeri Amesos Intrepid Ifpack ML Belos Stratimikos RBGen Phalanx Teko ExtraPack 24"
    "Configuring done"
    "Generating done"
  )


create_dependency_handling_test_case(
  extraTrilinosRepo_EnableExtraPack
  ARGS "-DTrilinos_EXTRA_REPOSITORIES=preCopyrightTrilinos,extraTrilinosRepo"
    -DTrilinos_ENABLE_ExtraPack=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Trilinos_NUM_TPLS='22'"
    "Explicitly enabled packages on input .* ExtraPack 1"
    "Explicitly disabled packages on input .* Stokhos Stalix 2"
    "Setting Trilinos_ENABLE_Teuchos=ON because ExtraPack has a required dependence on Teuchos"
    "Setting Trilinos_ENABLE_Teko=ON because ExtraPack has a required dependence on Teko"
    "NOTE: Not Setting Trilinos_ENABLE_Sacado=ON even though ExtraPack has an optional dependence on Sacado because Trilinos_ENABLE_SECONDARY_TESTED_CODE=OFF"
    "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Triutils Tpetra EpetraExt Thyra Isorropia AztecOO Galeri Amesos Ifpack ML Stratimikos Teko ExtraPack 17"
    "Final set of non-enabled packages:  TrilinosFramework Shards Stokhos Sacado Intrepid Belos RBGen Phalanx Panzer Stalix 10"
    "Final set of enabled TPLs:  BLAS LAPACK TekoDepTPL ExtraTPL2 4"
  )


create_dependency_handling_test_case(
  extraTrilinosRepo_EnableExtraPack_EnableExtraTPL1
  ARGS "-DTrilinos_EXTRA_REPOSITORIES=preCopyrightTrilinos,extraTrilinosRepo"
    -DTrilinos_ENABLE_ExtraPack=ON -DTPL_ENABLE_ExtraTPL1=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Trilinos_NUM_TPLS='22'"
    "Explicitly enabled packages on input .* ExtraPack 1"
    "Final set of enabled TPLs:  BLAS LAPACK TekoDepTPL ExtraTPL1 ExtraTPL2 5"
  )


create_dependency_handling_test_case(
  extraTrilinosRepo_ReplacePackagesList
  ARGS -DTrilinos_EXTRA_REPOSITORIES=preCopyrightTrilinos
    -DpreCopyrightTrilinos_PACKAGES_LIST_FILE=cmake/Reduced_preCopyrightTrilinos_PackagesList.cmake
  PASS_REGULAR_EXPRESSION_ALL
    "Reading list of POST extra packages from .*/MockTrilinos/cmake/Reduced_preCopyrightTrilinos_PackagesList.cmake"
    "Trilinos_NUM_PACKAGES='24'"
    "Final set of non-enabled SE packages:  .* Phalanx Panzer Stalix 31"
  )


create_dependency_handling_test_case(
  ENABLE_KNOWN_EXTERNAL_REPOS_TYPE_CI_EnableTeko
  ARGS -DTrilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE:STRING=Continuous
   -DTrilinos_IGNORE_MISSING_EXTRA_REPOSITORIES=ON
   -DTrilinos_ENABLE_Teko=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Explicitly enabled packages on input .* Teko 1"
    "Explicitly disabled packages on input .* Stokhos Stalix 2"
    "Setting Trilinos_ENABLE_Teuchos=ON because Teko has a required dependence on Teuchos"
    "Setting Trilinos_ENABLE_Epetra=ON because Teko has a required dependence on Epetra"
    "Setting Trilinos_ENABLE_Thyra=ON because Teko has a required dependence on Thyra"
    "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Triutils Tpetra EpetraExt Thyra Isorropia AztecOO Galeri Amesos Ifpack ML Stratimikos Teko 16"
    "Final set of non-enabled packages:  TrilinosFramework Shards Stokhos Sacado Intrepid Belos RBGen Phalanx Panzer Stalix 10"
  )


create_dependency_handling_test_case(
  ENABLE_KNOWN_EXTERNAL_REPOS_TYPE_Nightly_EnableExtraPack
  ARGS  -DTrilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE:STRING=Nightly
    -DTrilinos_IGNORE_MISSING_EXTRA_REPOSITORIES=ON
    -DTrilinos_ENABLE_ExtraPack=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Explicitly enabled packages on input .* ExtraPack 1"
    "Explicitly disabled packages on input .* Stokhos Stalix 2"
    "Setting Trilinos_ENABLE_Teuchos=ON because ExtraPack has a required dependence on Teuchos"
    "Setting Trilinos_ENABLE_Teko=ON because ExtraPack has a required dependence on Teko"
    "Final set of enabled packages:  Teuchos RTOp Epetra Zoltan Triutils Tpetra EpetraExt Thyra Isorropia AztecOO Galeri Amesos Ifpack ML Stratimikos Teko ExtraPack 17"
    "Final set of non-enabled packages:  TrilinosFramework Shards Stokhos Sacado Intrepid Belos RBGen Phalanx Panzer Stalix 10"
  )
