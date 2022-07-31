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

set(TRIBITS_MOCK_TRILINOS_DIR "examples/MockTrilinos")


# MockTrilinos project
set(MOCK_PROJECT_NAME Trilinos)
set(MOCK_TRILINOS_DIR
  ${${PROJECT_NAME}_TRIBITS_DIR}/${TRIBITS_MOCK_TRILINOS_DIR})


#
# Check for invalid input and other basic behaviors before checking generated
# XML files
#
# NOTE: In the below error message regextes, '[ \t\r\n]+' are used to match
# whitespace chars instead of just a space ' ' since CMake wraps error
# messages from message(FATAL_ERROR ...) on multiple lines and we can't
# predict the link breaks.  Also, we can't use the REGEX '\s' since CMake does
# not allow it.
#


tribits_add_advanced_test( DepXmlDumpTests_NoProjectSourceDir
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Copy tribits dir so that it will not appear in the standard location under any TriBITS project"
    CMND cp
    ARGS -r ${${PROJECT_NAME}_TRIBITS_DIR} tribits

  TEST_1 CMND "${CMAKE_COMMAND}"
    ARGS
      -P tribits/ci_support/TribitsDumpDepsXmlScript.cmake
    PASS_REGULAR_EXPRESSION_ALL
      "ERROR:[ \t\r\n]+Cannot[ \t\r\n]+determine[ \t\r\n]+a[ \t\r\n]+default[ \t\r\n]+PROJECT_SOURCE_DIR[ \t\r\n]+location,[ \t\r\n]+please[ \t\r\n]+set[ \t\r\n]+PROJECT_SOURCE_DIR!"
     ALWAYS_FAIL_ON_ZERO_RETURN

  )


tribits_add_advanced_test( DepXmlDumpTests_BadProjectSourceDir
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND "${CMAKE_COMMAND}"
    ARGS
      -D PROJECT_SOURCE_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -P ${${PROJECT_NAME}_TRIBITS_DIR}/ci_support/TribitsDumpDepsXmlScript.cmake
    PASS_REGULAR_EXPRESSION_ALL
       "ERROR:[ \t\r\n]+PROJECT_SOURCE_DIR='.*/tribits'[ \t\r\n]+is[ \t\r\n]+not[ \t\r\n]+a[ \t\r\n]+TriBITS[ \t\r\n]+project[ \t\r\n]+base[ \t\r\n]+dir[ \t\r\n]+since[ \t\r\n]+it[ \t\r\n]+is[ \t\r\n]+missing[ \t\r\n]+the[ \t\r\n]+file[ \t\r\n]+ProjectName.cmake"
    ALWAYS_FAIL_ON_ZERO_RETURN
  )


tribits_add_advanced_test( DepXmlDumpTests_InputProjectSourceDir
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0 CMND "${CMAKE_COMMAND}"
    ARGS
      -D PROJECT_SOURCE_DIR=${MOCK_TRILINOS_DIR}
      -D ${MOCK_PROJECT_NAME}_DEPS_XML_OUTPUT_FILE=${MOCK_PROJECT_NAME}PackageDependencies.xml
      -P ${${PROJECT_NAME}_TRIBITS_DIR}/ci_support/TribitsDumpDepsXmlScript.cmake
    PASS_REGULAR_EXPRESSION_ALL
      "Input: PROJECT_SOURCE_DIR = '${MOCK_TRILINOS_DIR}'"
      "-- Setting Trilinos_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}"
      "Dumping the XML dependencies file .*/TriBITS_DepXmlDumpTests_InputProjectSourceDir/${MOCK_PROJECT_NAME}PackageDependencies.xml"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_1 CMND "${CMAKE_COMMAND}"
    ARGS
      -E compare_files
      ${MOCK_PROJECT_NAME}PackageDependencies.xml
      ${${PACKAGE_NAME}_SOURCE_DIR}/test/ci_support/TrilinosPackageDependencies.gold.xml

  )
  # NOTE: In the above invocation, the project does not have TriBITS it its
  # standard location and this shows that such a case is well supported.



tribits_add_advanced_test( DepXmlDumpTests_MissingDepsXmlOutputFile
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND "${CMAKE_COMMAND}"
    ARGS
      -D PROJECT_SOURCE_DIR=${MOCK_TRILINOS_DIR}
      -P ${${PROJECT_NAME}_TRIBITS_DIR}/ci_support/TribitsDumpDepsXmlScript.cmake
    PASS_REGULAR_EXPRESSION_ALL
      "ERROR:[ \t\r\n]+Trilinos_DEPS_XML_OUTPUT_FILE[ \t\r\n]+cannot[ \t\r\n]+be[ \t\r\n]+empty.[ \t\r\n]+Please[ \t\r\n]+set[ \t\r\n]+Trilinos_DEPS_XML_OUTPUT_FILE!"
    ALWAYS_FAIL_ON_ZERO_RETURN
  )


tribits_add_advanced_test( DepXmlDumpTests_DefaultProjectSourceDir
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    CMND cp
    ARGS -r ${MOCK_TRILINOS_DIR} MockTrilinos

  TEST_1
    CMND cp
    ARGS -r ${${PROJECT_NAME}_TRIBITS_DIR} MockTrilinos/cmake/tribits

  TEST_2 CMND "${CMAKE_COMMAND}"
    ARGS
      -D${MOCK_PROJECT_NAME}_DEPS_XML_OUTPUT_FILE=${MOCK_PROJECT_NAME}PackageDependencies.xml
      -P MockTrilinos/cmake/tribits/ci_support/TribitsDumpDepsXmlScript.cmake
    PASS_REGULAR_EXPRESSION_ALL
      "Input: PROJECT_SOURCE_DIR = ''"
      "-- Setting default PROJECT_SOURCE_DIR=/.*/TriBITS_DepXmlDumpTests_DefaultProjectSourceDir/MockTrilinos"
      "-- Setting Trilinos_TRIBITS_DIR=/.*/TriBITS_DepXmlDumpTests_DefaultProjectSourceDir/MockTrilinos/cmake/tribits"
      "Dumping the XML dependencies file .*/TriBITS_DepXmlDumpTests_DefaultProjectSourceDir/${MOCK_PROJECT_NAME}PackageDependencies.xml"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_3 CMND "${CMAKE_COMMAND}"
    ARGS
      -E compare_files
      ${MOCK_PROJECT_NAME}PackageDependencies.xml
      ${${PACKAGE_NAME}_SOURCE_DIR}/test/ci_support/TrilinosPackageDependencies.gold.xml

  )
  # NOTE: The above test shows that if TriBITS is present in the default
  # project location, then you don't need to specify the PROJECT_SOURCE_DIR
  # var.


#####################################################################
#
# Unit tests for just creating the dependency XML files
#
#####################################################################


function(create_dependency_xml_dump_test_case TEST_NAME)

  cmake_parse_arguments(
     #prefix
     PARSE
     #options
     ""
     #one_value_keywords
     ""
     #mulit_value_keywords
     "ARGS;PASS_REGULAR_EXPRESSION_ALL;COMPARE_XML_FILE"
     ${ARGN}
     )

  set(EXTENDED_TEST_NAME DepXmlDumpTests_${TEST_NAME})
  set(FULL_TEST_NAME ${PACKAGE_NAME}_${EXTENDED_TEST_NAME})

  set(TEST_OUTPUT_REL_DIR ${FULL_TEST_NAME})
  set(TEST_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${TEST_OUTPUT_REL_DIR})

  tribits_add_advanced_test( ${EXTENDED_TEST_NAME}
    OVERALL_WORKING_DIRECTORY ${TEST_OUTPUT_REL_DIR}
    OVERALL_NUM_MPI_PROCS 1
    TEST_0 CMND "${CMAKE_COMMAND}"
      ARGS
        -DPROJECT_SOURCE_DIR:PATH=${MOCK_TRILINOS_DIR}
        -D${MOCK_PROJECT_NAME}_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
        -D${MOCK_PROJECT_NAME}_NATIVE_REPOSITORIES=.
        -D${MOCK_PROJECT_NAME}_DEPS_XML_OUTPUT_FILE=${TEST_OUTPUT_DIR}/${MOCK_PROJECT_NAME}PackageDependencies.xml
        -D${MOCK_PROJECT_NAME}_CDASH_DEPS_XML_OUTPUT_FILE=CDashSubprojectDependencies.xml
        -D${MOCK_PROJECT_NAME}_DEPS_HTML_OUTPUT_FILE=${MOCK_PROJECT_NAME}PackageDependenciesTable.html
        ${PARSE_ARGS}
        -P ${${PROJECT_NAME}_TRIBITS_DIR}/ci_support/TribitsDumpDepsXmlScript.cmake
      PASS_REGULAR_EXPRESSION_ALL ${PARSE_PASS_REGULAR_EXPRESSION_ALL}
      ALWAYS_FAIL_ON_NONZERO_RETURN
    TEST_1 CMND "${CMAKE_COMMAND}"
      ARGS
        -E compare_files
        ${TEST_OUTPUT_DIR}/${MOCK_PROJECT_NAME}PackageDependencies.xml
        ${${PACKAGE_NAME}_SOURCE_DIR}/test/ci_support/${PARSE_COMPARE_XML_FILE}
    XHOSTTYPE Windows
    )

endfunction()


create_dependency_xml_dump_test_case(
  BasePackagesTpls
  PASS_REGULAR_EXPRESSION_ALL
    "Trilinos_NUM_PACKAGES='24'"
    "Trilinos_NUM_TPLS='19'"
  COMPARE_XML_FILE
    TrilinosPackageDependencies.gold.xml
  )


create_dependency_xml_dump_test_case(
  WithPreCopyrightTrilinos
  ARGS
    -DTrilinos_EXTRA_REPOSITORIES=preCopyrightTrilinos
    -DLIST_ALWAYS_MISSING_PACKAGE=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Trilinos_NUM_PACKAGES='26'"
    "Trilinos_NUM_TPLS='20'"
  COMPARE_XML_FILE
    TrilinosPackageDependencies.preCopyrightTrilinos.gold.xml
  )
# The above test also ensures that the TribitsDumpDepsXmlScript.cmake behaves
# correctly when missing packages are listed but are marked as such and
# therefore should be ignored.  I have another test for
# TriBITSExProj/TribitsExampleProject that verifies that such a package is
# processed correctly when it does exist.  It is just that this was not
# testing this dump script.


create_dependency_xml_dump_test_case(
  WithPreCopyrightTrilinosExtraTrilinosRepo
  ARGS
    -DTrilinos_EXTRA_REPOSITORIES=preCopyrightTrilinos,extraTrilinosRepo
  PASS_REGULAR_EXPRESSION_ALL
    "Trilinos_NUM_PACKAGES='26'"
    "Trilinos_NUM_TPLS='22'"
  COMPARE_XML_FILE
    TrilinosPackageDependencies.preCopyrightTrilinos.extraTrilinosRepo.gold.xml
  )


create_dependency_xml_dump_test_case(
  WithExtraRepoOnePackage
  ARGS
    -DTrilinos_EXTRA_REPOSITORIES=extraRepoOnePackage
  PASS_REGULAR_EXPRESSION_ALL
    "Trilinos_NUM_PACKAGES='24'"
    "Trilinos_NUM_TPLS='19'"
  COMPARE_XML_FILE
    TrilinosPackageDependencies.extraRepoOnePackage.gold.xml
  )


create_dependency_xml_dump_test_case(
  WithPreRepoOnePackagePreCopyrightTrilinos
  ARGS
    -DTrilinos_PRE_REPOSITORIES=preRepoOnePackage
    -DTrilinos_EXTRA_REPOSITORIES=preCopyrightTrilinos
    -DLIST_ALWAYS_MISSING_PACKAGE=ON
  PASS_REGULAR_EXPRESSION_ALL
    "Trilinos_NUM_PACKAGES='27'"
    "Trilinos_NUM_TPLS='20'"
  COMPARE_XML_FILE
    TrilinosPackageDependencies.preRepoOnePackage.preCopyrightTrilinos.gold.xml
  )
