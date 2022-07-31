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
# Test generation of package dependency and related XML and related files
#

add_subdirectory(DumpXmlDepsTests)

# In case the project has ${PROJECT_NAME}_TRIBITS_DIR pointing to a symlink,
# use the TriBITS implenetation in these test (which should be the same)
set(THIS_TRIBITS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../tribits")


# The mock test project
set(MOCK_PROJECT_NAME Trilinos)

set(TRIBITS_MOCK_TRILINOS_DIR "examples/MockTrilinos")

set(MOCK_TRILINOS_DIR ${${PROJECT_NAME}_TRIBITS_DIR}/${TRIBITS_MOCK_TRILINOS_DIR})


if (WIN23 AND NOT CYGWIN)
  set(DIFF_COMMAND_STUFF "${CMAKE_COMMAND}" ARGS -E compare_files)
else()
  set(DIFF_COMMAND_STUFF diff ARGS)
endif()


tribits_add_advanced_test( XmlHtmlCDashOutputTest
  OVERALL_WORKING_DIRECTORY TEST_NAME
  TEST_0 CMND ${CMAKE_COMMAND}
    ARGS
      -D${MOCK_PROJECT_NAME}_TRIBITS_DIR:FILEPATH=${${PROJECT_NAME}_TRIBITS_DIR}
      -D ${MOCK_PROJECT_NAME}_SHORTCIRCUIT_AFTER_DEPENDENCY_HANDLING:BOOL=ON
      -D ${MOCK_PROJECT_NAME}_ENABLE_ALL_PACKAGES:BOOL=ON
      -D ${MOCK_PROJECT_NAME}_DEPS_XML_OUTPUT_FILE:STRING=TrilinosPackageDependencies.xml
      -D ${MOCK_PROJECT_NAME}_DEPS_HTML_OUTPUT_FILE:STRING=TrilinosPackageDependenciesTable.html
      -D ${MOCK_PROJECT_NAME}_CDASH_DEPS_XML_OUTPUT_FILE:STRING=CDashSubprojectDependencies.xml
      -D ${MOCK_PROJECT_NAME}_ENABLE_C:BOOL=OFF
      -D ${MOCK_PROJECT_NAME}_ENABLE_CXX:BOOL=OFF
      -D ${MOCK_PROJECT_NAME}_ENABLE_Fortran:BOOL=OFF
      ${MOCK_TRILINOS_DIR}
    PASS_REGULAR_EXPRESSION "Shortcircuiting after dependency tracking ..."
  TEST_1 CMND ${DIFF_COMMAND_STUFF}
    ${CMAKE_CURRENT_SOURCE_DIR}/TrilinosPackageDependencies.gold.xml
    TrilinosPackageDependencies.xml
  TEST_2 CMND ${DIFF_COMMAND_STUFF}
    ${CMAKE_CURRENT_SOURCE_DIR}/TrilinosPackageDependenciesTable.gold.html
     TrilinosPackageDependenciesTable.html
  TEST_3 CMND ${DIFF_COMMAND_STUFF}
    ${CMAKE_CURRENT_SOURCE_DIR}/CDashSubprojectDependencies.gold.xml
    CDashSubprojectDependencies.xml
  )


tribits_add_advanced_test( TribitsDependencies_UnitTests
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE}
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/TribitsDependencies_UnitTests.py -v
    PASS_REGULAR_EXPRESSION "OK"
  )


tribits_add_advanced_test( TribitsPackageFilePathUtils_UnitTests
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE}
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/TribitsPackageFilePathUtils_UnitTests.py -v
    PASS_REGULAR_EXPRESSION "OK"
  )


tribits_add_advanced_test( TribitsPackageTestNameUtils_UnitTests
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE}
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/TribitsPackageTestNameUtils_UnitTests.py -v
    PASS_REGULAR_EXPRESSION "OK"
  )

#
# Test the get-tribits-packages.py script
#

tribits_add_advanced_test( get_tribits_packages_default
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0
    CMND ${PYTHON_EXECUTABLE}
    ARGS
      "${${PROJECT_NAME}_TRIBITS_DIR}/ci_support/get-tribits-packages.py"
      --deps-xml-file="${CMAKE_CURRENT_SOURCE_DIR}/TrilinosPackageDependencies.gold.xml"
    PASS_REGULAR_EXPRESSION_ALL
      "^TrilinosFramework,Teuchos,.*,Sacado,Thyra,Isorropia,.*,Phalanx,Panzer\n"
  )

tribits_add_advanced_test( get_tribits_packages_only_toplevel_packages
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0
    CMND ${PYTHON_EXECUTABLE}
    ARGS
      "${${PROJECT_NAME}_TRIBITS_DIR}/ci_support/get-tribits-packages.py"
      --deps-xml-file="${CMAKE_CURRENT_SOURCE_DIR}/TrilinosPackageDependencies.gold.xml"
      --only-top-level-packages=on
    PASS_REGULAR_EXPRESSION_ALL
      "^TrilinosFramework,Teuchos,.*,Sacado,Thyra,Isorropia,.*,Phalanx,Panzer\n"
  )

tribits_add_advanced_test( get_tribits_packages_all_se_packages
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0
    CMND ${PYTHON_EXECUTABLE}
    ARGS
      "${${PROJECT_NAME}_TRIBITS_DIR}/ci_support/get-tribits-packages.py"
      --deps-xml-file="${CMAKE_CURRENT_SOURCE_DIR}/TrilinosPackageDependencies.gold.xml"
      --only-top-level-packages=off
    PASS_REGULAR_EXPRESSION_ALL
      "^TrilinosFramework.*,Sacado,ThyraCoreLibs,ThyraGoodStuff,ThyraCrazyStuff,ThyraEpetra,ThyraEpetraExt,ThyraTpetra,Thyra,Isorropia,.*,Phalanx,Panzer\n"
  )

#
# Test the get-tribits-packages-from-last-tests-failed.py script
#

tribits_add_advanced_test( get_tribits_packages_from_last_tests_failed_1
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0
    CMND ${PYTHON_EXECUTABLE}
    ARGS
      "${${PROJECT_NAME}_TRIBITS_DIR}/ci_support/get-tribits-packages-from-last-tests-failed.py"
      --deps-xml-file="${CMAKE_CURRENT_SOURCE_DIR}/TrilinosPackageDependencies.gold.xml"
      --last-tests-failed-file="${CMAKE_CURRENT_SOURCE_DIR}/LastTestsFailed.1.log"
    PASS_REGULAR_EXPRESSION_ALL
      "^Thyra,Teuchos\n"
  )

#
# Test TribitsExampleProject/cmake/ProjectCiFileChangeLogic.py
#

tribits_add_advanced_test( TribitsExampleProject_ProjectCiFileChangeLogic_UnitTests
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE}
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/TribitsExampleProject_ProjectCiFileChangeLogic_UnitTests.py -v
    PASS_REGULAR_EXPRESSION "OK"
  )

#
# Test the get-tribits-packages-from-last-tests-failed.py script with
# TribitsExampleProject that does *NOT* have TriBITS copied into it in the
# standard location.
#

function(test_tribitsexampleproject_get_tribits_packages_from_files_list
  TEST_POSTFIX  CHANGED_FILES_NAME  MATCH_REGEX
  )

  tribits_add_advanced_test(
    TribitsExampleProject_get_tribits_packages_from_files_list_${TEST_POSTFIX}
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1

    TEST_0
      CMND "${CMAKE_COMMAND}"
      ARGS -E copy
        "${CMAKE_CURRENT_LIST_DIR}/test_data/${CHANGED_FILES_NAME}"
        changed_files.txt

    TEST_1
      CMND "${CMAKE_COMMAND}"
      ARGS
        -D PROJECT_SOURCE_DIR="${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject"
        -D TribitsExProj_DEPS_XML_OUTPUT_FILE=TribitsExProjPackageDependencies.cmake
        -P "${${PROJECT_NAME}_TRIBITS_DIR}/ci_support/TribitsDumpDepsXmlScript.cmake"

    TEST_2
      CMND ${PYTHON_EXECUTABLE}
      ARGS
        "${${PROJECT_NAME}_TRIBITS_DIR}/ci_support/get-tribits-packages-from-files-list.py"
        --deps-xml-file=TribitsExProjPackageDependencies.cmake
        --files-list-file=changed_files.txt
        --project-dir="${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject"
      PASS_REGULAR_EXPRESSION_ALL
        "${MATCH_REGEX}"

    )

endfunction()

test_tribitsexampleproject_get_tribits_packages_from_files_list(
  1  tribits-example-project-changed-files-1.txt  "^ALL_PACKAGES,SimpleCxx\n" )

test_tribitsexampleproject_get_tribits_packages_from_files_list(
  2  tribits-example-project-changed-files-2.txt  "^ALL_PACKAGES,WithSubpackagesC\n" )

test_tribitsexampleproject_get_tribits_packages_from_files_list(
  3  tribits-example-project-changed-files-3.txt  "^ALL_PACKAGES,MixedLang\n" )

test_tribitsexampleproject_get_tribits_packages_from_files_list(
  4  tribits-example-project-changed-files-4.txt  "^WithSubpackages\n" )

#
# Test the get-tribits-packages-from-last-tests-failed.py script with
# TribitsExampleProject with TriBITS copied into the standard location so that
# we can leave off the --project-dir argument.
#

function(test_tribitsexampleproject_w_tribits_get_tribits_packages_from_files_list
  TEST_POSTFIX  CHANGED_FILES_NAME  MATCH_REGEX
  )

  tribits_add_advanced_test(
    TribitsExampleProject_w_tribits_get_tribits_packages_from_files_list_${TEST_POSTFIX}
    OVERALL_WORKING_DIRECTORY TEST_NAME
    OVERALL_NUM_MPI_PROCS 1

    TEST_0
      CMND cp
      ARGS -r
        "${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject"
	TribitsExampleProject

    TEST_1
      CMND cp
      ARGS -r
        "${${PROJECT_NAME}_TRIBITS_DIR}"
	TribitsExampleProject/cmake/tribits

    TEST_2
      CMND cp
      ARGS
        "${CMAKE_CURRENT_LIST_DIR}/test_data/${CHANGED_FILES_NAME}"
        changed_files.txt

    TEST_3
      CMND "${CMAKE_COMMAND}"
      ARGS
        -D TribitsExProj_DEPS_XML_OUTPUT_FILE=TribitsExProjPackageDependencies.cmake
        -P "TribitsExampleProject/cmake/tribits/ci_support/TribitsDumpDepsXmlScript.cmake"

    TEST_4
      CMND ${PYTHON_EXECUTABLE}
      ARGS
        "TribitsExampleProject/cmake/tribits/ci_support/get-tribits-packages-from-files-list.py"
        --deps-xml-file=TribitsExProjPackageDependencies.cmake
        --files-list-file=changed_files.txt
      PASS_REGULAR_EXPRESSION_ALL
        "${MATCH_REGEX}"

    )

endfunction()

test_tribitsexampleproject_w_tribits_get_tribits_packages_from_files_list(
  1  tribits-example-project-changed-files-1.txt  "^ALL_PACKAGES,SimpleCxx\n" )

test_tribitsexampleproject_w_tribits_get_tribits_packages_from_files_list(
  2  tribits-example-project-changed-files-2.txt  "^ALL_PACKAGES,WithSubpackagesC\n" )

test_tribitsexampleproject_w_tribits_get_tribits_packages_from_files_list(
  3  tribits-example-project-changed-files-3.txt  "^ALL_PACKAGES,MixedLang\n" )

test_tribitsexampleproject_w_tribits_get_tribits_packages_from_files_list(
  4  tribits-example-project-changed-files-4.txt  "^WithSubpackages\n" )


#
# Test checkin-test.py script
#

tribits_add_advanced_test( clone_extra_repos_UnitTests
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE}
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/clone_extra_repos_UnitTests.py
      --with-cmake="${CMAKE_COMMAND}"
      -v
    PASS_REGULAR_EXPRESSION "OK"
  )

tribits_add_advanced_test( CheckinTest_UnitTests
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE}
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/CheckinTest_UnitTests.py
      --with-cmake="${CMAKE_COMMAND}"
      -v
    PASS_REGULAR_EXPRESSION "OK"
  )


#
# Test script cdash_build_testing_date.py
#


tribits_add_advanced_test( cdash_build_testing_date_UnitTests
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE}
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/cdash_build_testing_date_UnitTests.py
      -v
    PASS_REGULAR_EXPRESSION "OK"
  )


#
# tribits_strip_comments_from_cmake_cache_file()
#

tribits_add_advanced_test( TribitsStripCommentsFromCMakeCacheFile_small
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${CMAKE_COMMAND}
    ARGS
      -DPROJECT_NAME=DummyProject
      -DTRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DCMAKE_CACHE_FILE_IN=${CMAKE_CURRENT_SOURCE_DIR}/CMakeCache.small.txt
      -DCMAKE_CACHE_FILE_CLEAN_OUT=CMakeCache.small.clean.txt
      -P ${CMAKE_CURRENT_SOURCE_DIR}/TribitsStripCommentsFromCMakeCacheFile_TestDriver.cmake
  TEST_1 CMND diff
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/CMakeCache.small.clean.txt.expected
      CMakeCache.small.clean.txt
  )

tribits_add_advanced_test( TribitsStripCommentsFromCMakeCacheFile_large
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${CMAKE_COMMAND}
    ARGS
      -DPROJECT_NAME=DummyProject
      -DTRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DCMAKE_CACHE_FILE_IN=${CMAKE_CURRENT_SOURCE_DIR}/CMakeCache.large.txt
      -DCMAKE_CACHE_FILE_CLEAN_OUT=CMakeCache.large.clean.txt
      -P ${CMAKE_CURRENT_SOURCE_DIR}/TribitsStripCommentsFromCMakeCacheFile_TestDriver.cmake
  TEST_1 CMND diff
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/CMakeCache.large.clean.txt.expected
      CMakeCache.large.clean.txt
  )

tribits_add_advanced_test( CDashQueryAnalyzeReport_UnitTests
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE}
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/CDashQueryAnalyzeReport_UnitTests.py -v
    PASS_REGULAR_EXPRESSION "OK"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  )

tribits_add_advanced_test( cdash_analyze_and_report_UnitTests
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE}
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/cdash_analyze_and_report_UnitTests.py -v
    PASS_REGULAR_EXPRESSION "OK"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  )

tribits_add_advanced_test( CreateIssueTrackerFromCDashQuery_UnitTests
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${PYTHON_EXECUTABLE}
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/CreateIssueTrackerFromCDashQuery_UnitTests.py -v
    PASS_REGULAR_EXPRESSION "OK"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  )

#
# TribitsExampleProject checkin-test.py tests
#


tribits_add_advanced_test( TribitsExampleProject_checkin_test
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Copy TribitsExampleProject so we can snapshot TriBITS into it"
    CMND cp
    ARGS -r ${THIS_TRIBITS_DIR}/examples/TribitsExampleProject
      TribitsExampleProject
    # NOTE: Had to use 'cp -r' instead of 'cmake -E copy_directory' because
    # the latter failed for a development version of CMake.  But since the
    # checkin-test.py scrpt will really only run on Linux/Unix systems, using
    # 'cp -r' is not a testing portability problem.

  TEST_1
    MESSAGE "Copy tribits dir into TribitsExampleProject/cmake/"
    CMND cp
    ARGS -r ${THIS_TRIBITS_DIR}
      TribitsExampleProject/cmake/tribits

  TEST_2
    MESSAGE "Run checkin-test.py script to make sure it finds base dir"
    CMND ${PYTHON_EXECUTABLE}
    ARGS -E TribitsExampleProject/cmake/tribits/ci_support/checkin-test.py
      --enable-all-packages=on --send-email-to= --send-email-to-on-push= --allow-no-pull
    PASS_REGULAR_EXPRESSION_ALL
      "src-dir=.*/TriBITS_TribitsExampleProject_checkin_test/TribitsExampleProject/cmake/tribits/ci_support/[.][.]/[.][.]/[.][.]"
      "0[)] MPI_DEBUG => No configure, build, or test for MPI_DEBUG was requested! => Not ready to push! [(]-1.00 min[)]"
      "1[)] SERIAL_RELEASE => No configure, build, or test for SERIAL_RELEASE was requested! => Not ready to push! [(]-1.00 min[)]"
    # NOTE: That above test proves that the checkin-test.py script can find
    # the base directory when run as
    # <projectDir>/cmake/tribis/ci_support/checkin-test.py

  )


# NOTE: The fact that all testing and pushing for the checkin-test.py script
# is using the symlinked TriBITS/checkin-test.py tests and protects that use
# case.
