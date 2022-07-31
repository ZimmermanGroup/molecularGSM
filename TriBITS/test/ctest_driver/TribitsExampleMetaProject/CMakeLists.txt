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
# Strong automated testing of TribitsCTestDriverCore.cmake using real
# TribitsExampleMetaProject and sub-repos including cloning git repos.
#
###############################################################################

#
# Set up common vars/args for all direct CTest Driver tests
#

set(TribitsExMetaProj_DIR "${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleMetaProject")

set(COMMON_ENV_ARGS
  # Set back to default in case set in env
  CTEST_TEST_TYPE=
  TribitsExMetaProj_PACKAGES=
  CTEST_EXPLICITLY_ENABLE_IMPLICITLY_ENABLED_PACKAGES=
  CTEST_ENABLE_MODIFIED_PACKAGES_ONLY=OFF
  TribitsExMetaProj_PRE_REPOSITORIES=
  TribitsExMetaProj_EXTRA_REPOSITORIES=
  CTEST_BUILD_FLAGS=-j2
  CTEST_PARALLEL_LEVEL=1
  # Other args
  TribitsExMetaProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
  CTEST_DO_SUBMIT=OFF
  )

set(CTEST_S_SCRIPT_ARGS
  ${CMAKE_CTEST_COMMAND} -V -S
    ${TribitsExMetaProj_DIR}/cmake/ctest/general_gcc/ctest_serial_debug.cmake
  )

#
# Set up arguments for direct configures of TribitsExampleMetaProject to test
# the 'dashboard' target.
#

include("${CMAKE_CURRENT_LIST_DIR}/../../core/ExamplesUnitTests/GetCompilerPassthroughArgs.cmake")

set(TribitsExampleMetaProject_COMMON_CONFIG_ARGS
  ${COMMON_ENV_ARGS_PASSTHROUGH}
  -DTribitsExMetaProj_ENABLE_Fortran=${${PROJECT_NAME}_ENABLE_Fortran}
  ${CTEST_DROP_SITE_CONFIG_ARGS}
  )

##########################################################################################
#
# Test tribits_ctest_driver() using real git clones
#
# NOTE: To get these tests to work on some systems, one may need to switch
# away from 'https://github.com/' to using 'git@github.com:' using:
#
#   export TribitsExMetaProj_GIT_URL_REPO_BASE=git@github.com:tribits/
#
# before configuring.
#
##########################################################################################


set(cdash_build_url_expected_regex
    "https://testing[.]sandia[.]gov/cdash/index[.]php[?]project=TribitsExampleMetaProject&filtercount=3&showfilters=1&filtercombine=and&field1=site&compare1=61&value1=CustomSite&field2=buildname&compare2=61&value2=CTestDriver_TribitsExMetaProj_clone_default_branch_remote&field3=buildstamp&compare3=61&value3=[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]-[0-9][0-9][0-9][0-9]-Continuous" )
  # NOTE: Above, we have to repeat [0-9] 8 times and 4 times for the regex for
  # the number of digits in the buildstarttime.  CMake regex does not support
  # \d{8}-\d{4} :-(

set(cdash_revisions_builds_url_expected_regex
     "https://testing[.]sandia[.]gov/cdash/index.php[?]project=TribitsExampleMetaProject&filtercount=1&showfilters=1&field1=revision&compare1=61&value1=863461e3035d24c632e175c087761e83db28bdc3")

set(cdash_revisions_nonpassing_tests_expected_regex
    "https://testing[.]sandia[.]gov/cdash/queryTests.php[?]project=TribitsExampleMetaProject&filtercount=2&showfilters=1&filtercombine=and&field1=revision&compare1=61&value1=863461e3035d24c632e175c087761e83db28bdc3&field2=status&compare2=62&value2=passed" )


tribits_add_advanced_test( CTestDriver_TribitsExMetaProj_clone_default_branch_remote
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran

  TEST_0
    MESSAGE "Run ctest driver with initial clone of the repos on the default branch 'master' and remote 'origin'"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${COMMON_ENV_ARGS}
      TribitsExMetaProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
      CTEST_BUILD_NAME=CTestDriver_TribitsExMetaProj_clone_default_branch_remote
      CTEST_SITE=CustomSite
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "CTEST_NOTES_FILES=''"
      "First perform the initial checkout: .*/git. clone -o origin .*github.com.tribits/TribitsExampleMetaProject.git"
      "Perform checkout in directory: .*/TriBITS_CTestDriver_TribitsExMetaProj_clone_default_branch_remote"
      "Results will be submitted on CDash at the following links:"
      "Link to this build's results on CDash:"
      "${cdash_build_url_expected_regex}"
      "Link to all builds for this repo version on CDash:"
      "${cdash_revisions_builds_url_expected_regex}"
      "Link to all nonpassing tests for all builds for this repo version on CDash:"
      "${cdash_revisions_nonpassing_tests_expected_regex}"
      "Calling ctest_update[(][)] to update base source repo '.*/TriBITS_CTestDriver_TribitsExMetaProj_clone_default_branch_remote/TribitsExampleMetaProject"
      "Old revision of repository is: [a-z0-9]+"
      "New revision of repository is: [a-z0-9]+"
      "ctest_update[(][.][.][.][)] returned '0'"
      "cmake -P tribits_ctest_update_commands[.]cmake"
      "Git Update PASSED!"
      "TribitsExampleProject: Doing initial GIT clone/checkout from URL '.*github[.]com.tribits/TribitsExampleProject.git' to dir 'TribitsExampleProject' [.][.][.]"
      "Cloning into 'TribitsExampleProject'[.][.][.]"
      "TribitsExampleProjectAddons: Doing initial GIT clone/checkout from URL '.*github[.]com.tribits/TribitsExampleProjectAddons.git' to dir 'TribitsExampleProjectAddons' [.][.][.]"
      "Cloning into 'TribitsExampleProjectAddons'[.][.][.]"
      "CTEST_NOTES_FILES_WO_CACHE='.*/TriBITS_CTestDriver_TribitsExMetaProj_clone_default_branch_remote/BUILD/UpdateCommandsOutput.txt'"
      "Configure PASSED!"
       "CTEST_NOTES_FILES='.*/TriBITS_CTestDriver_TribitsExMetaProj_clone_default_branch_remote/BUILD/TribitsExMetaProjRepoVersion.txt[;].*/TriBITS_CTestDriver_TribitsExMetaProj_clone_default_branch_remote/BUILD/UpdateCommandsOutput.txt[;].*/TriBITS_CTestDriver_TribitsExMetaProj_clone_default_branch_remote/BUILD/CMakeCache.clean.txt'"
      "Build PASSED!"
      "100% tests passed, 0 tests failed out of 11"
      "Addon1 .*=.*.* sec[*]proc [(]1 test[)]"
      "MixedLang .* =.*.* sec[*]proc [(]1 test[)]"
      "SimpleCxx .*=*.* sec[*]proc [(]2 tests[)]"
      "WithSubpackages .*=*.* sec[*]proc [(]6 tests[)]"
      "WrapExternal .*=*.* sec[*]proc [(]1 test[)]"
      "File '' does NOT exist so all tests passed!"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_1
    MESSAGE "Check that base repo has correct remote"
    CMND git ARGS remote -v
    WORKING_DIRECTORY TribitsExampleMetaProject
    SKIP_CLEAN_WORKING_DIRECTORY
    PASS_REGULAR_EXPRESSION_ALL
      "origin.*.*github[.]com.tribits/TribitsExampleMetaProject[.]git"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_2
    MESSAGE "Check that base repo is on the correct branch"
    CMND git ARGS branch
    WORKING_DIRECTORY TribitsExampleMetaProject
    SKIP_CLEAN_WORKING_DIRECTORY
    PASS_REGULAR_EXPRESSION_ALL
      "[*].*master"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_3
    MESSAGE "Check that one of the extra repos has correct remote"
    CMND git ARGS remote -v
    WORKING_DIRECTORY TribitsExampleMetaProject/TribitsExampleProject
    SKIP_CLEAN_WORKING_DIRECTORY
    PASS_REGULAR_EXPRESSION_ALL
      "origin.*.*github[.]com.tribits/TribitsExampleProject[.]git"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_4
    MESSAGE "Check that one of the extra repos in on the correct branch"
    CMND git ARGS branch
    WORKING_DIRECTORY TribitsExampleMetaProject/TribitsExampleProject
    SKIP_CLEAN_WORKING_DIRECTORY
    PASS_REGULAR_EXPRESSION_ALL
      "[*].*master"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_5
    MESSAGE "Run ctest driver with an update changing to the branch 'for-testing'"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${COMMON_ENV_ARGS}
      CTEST_START_WITH_EMPTY_BINARY_DIRECTORY=FALSE
      CTEST_WIPE_CACHE=FALSE
      TribitsExMetaProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
      TribitsExMetaProj_BRANCH=for-testing
      CTEST_NOTES_FILES=${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleMetaProject/README
      CTEST_BUILD_NAME=CTestDriver_TribitsExMetaProj_clone_default_branch_remote
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "-- ENV_CTEST_NOTES_FILES='.*/tribits/examples/TribitsExampleMetaProject/README'"
      "-- CTEST_NOTES_FILES='.*/tribits/examples/TribitsExampleMetaProject/README'"
      "-- ENV_TribitsExMetaProj_BRANCH='for-testing'"
      "-- TribitsExMetaProj_BRANCH='for-testing'"
      "Calling ctest_update[(][)] to update base source repo '.*/TriBITS_CTestDriver_TribitsExMetaProj_clone_default_branch_remote/TribitsExampleMetaProject"
      "Old revision of repository is: [a-z0-9]+"
      "New revision of repository is: 863461e3035d24c632e175c087761e83db28bdc3"
      "ctest_update[(][.][.][.][)] returned '[0-9]+'"
      "cmake -P tribits_ctest_update_commands[.]cmake"
      "Running: .*/git checkout -B for-testing --track origin/for-testing"
      "Switched to a new branch 'for-testing'"
      "[Bb]ranch '*for-testing'* set up to track .*for-testing"
      "Git Update PASSED!"
      "For extra repos, doing switch to branch for-testing"
      "TribitsExampleProject: Doing GIT update from URL '.*github[.]com.tribits/TribitsExampleProject[.]git' to dir '.*/TribitsExampleProject' [.][.][.]"
      "TribitsExampleProjectAddons: Doing GIT update from URL '.*github[.]com.tribits/TribitsExampleProjectAddons[.]git' to dir '.*/TribitsExampleProjectAddons' [.][.][.]"
      "CTEST_NOTES_FILES_WO_CACHE='.*/BUILD/Updates.txt[;].*/BUILD/UpdateCommandsOutput.txt[;].*/tribits/examples/TribitsExampleMetaProject/README'"
      "Configure PASSED!"
      "CTEST_NOTES_FILES='.*/BUILD/TribitsExMetaProjRepoVersion.txt[;].*/BUILD/Updates.txt[;].*/BUILD/UpdateCommandsOutput.txt[;].*/tribits/examples/TribitsExampleMetaProject/README[;].*/BUILD/CMakeCache.clean.txt'"
      "Build PASSED!"
      "100% tests passed, 0 tests failed out of 11"
      "Addon1 .*=.*.* sec[*]proc [(]1 test[)]"
      "MixedLang .* =.*.* sec[*]proc [(]1 test[)]"
      "SimpleCxx .*=*.* sec[*]proc [(]2 tests[)]"
      "WithSubpackages .*=*.* sec[*]proc [(]6 tests[)]"
      "WrapExternal .*=*.* sec[*]proc [(]1 test[)]"
      "File '' does NOT exist so all tests passed!"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_6
    MESSAGE "Check that base repo is on the correct branch"
    CMND git ARGS branch
    WORKING_DIRECTORY TribitsExampleMetaProject
    SKIP_CLEAN_WORKING_DIRECTORY
    PASS_REGULAR_EXPRESSION_ALL
      "[*].*for-testing"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_7
    MESSAGE "Check that one of the extra repos in on the correct branch"
    CMND git ARGS branch
    WORKING_DIRECTORY TribitsExampleMetaProject/TribitsExampleProject
    SKIP_CLEAN_WORKING_DIRECTORY
    PASS_REGULAR_EXPRESSION_ALL
      "[*].*for-testing"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_8
    MESSAGE "Make sure that the file TribitsExMetaProjRepoVersion.txt is correct"
    CMND diff ARGS 
      "${CMAKE_CURRENT_LIST_DIR}/data/TribitsExMetaProjRepoVersion.for-testing.txt"
      BUILD/TribitsExMetaProjRepoVersion.txt 
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_9
    MESSAGE "Remove one of the git repos to make sure that it clones again"
    CMND rm ARGS -rf TribitsExampleMetaProject/TribitsExampleProjectAddons
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_10
    MESSAGE "Reset other repos back to an earlier version so we can simulate and update"
    CMND "${${PROJECT_NAME}_TRIBITS_DIR}/python_utils/gitdist"
    ARGS --dist-no-color --dist-repos=.,TribitsExampleProject reset --hard HEAD^
    WORKING_DIRECTORY TribitsExampleMetaProject
    SKIP_CLEAN_WORKING_DIRECTORY
    PASS_REGULAR_EXPRESSION_ALL
      "[*][*][*] Base Git Repo: TribitsExampleMetaProject"
      "HEAD is now at e12aad9 Merge branch 'tribits-github-snapshot' [(]TriBITSPub/TriBITS#309[)]"
      "[*][*][*] Git Repo: TribitsExampleProject"
      "HEAD is now at 59c7057 Merge branch 'tribits-github-snapshot'"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_11
    MESSAGE "Run ctest driver on branch 'for-testing'"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${COMMON_ENV_ARGS}
      CTEST_START_WITH_EMPTY_BINARY_DIRECTORY=FALSE
      CTEST_WIPE_CACHE=FALSE
      TribitsExMetaProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
      TribitsExMetaProj_BRANCH=for-testing
      CTEST_BUILD_NAME=CTestDriver_TribitsExMetaProj_clone_default_branch_remote
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "-- CTEST_NOTES_FILES=''"
      "-- ENV_TribitsExMetaProj_BRANCH='for-testing'"
      "-- TribitsExMetaProj_BRANCH='for-testing'"
      "Calling ctest_update[(][)] to update base source repo '.*/TriBITS_CTestDriver_TribitsExMetaProj_clone_default_branch_remote/TribitsExampleMetaProject"
      "Old revision of repository is: e12aad94c7514b42d059a7aa0488346dc0a8a9c0"
      "New revision of repository is: 863461e3035d24c632e175c087761e83db28bdc3"
      "ctest_update[(][.][.][.][)] returned '10'"
      "cmake -P tribits_ctest_update_commands[.]cmake"
      "HEAD is now at e12aad9 Merge branch 'tribits-github-snapshot' [(]TriBITSPub/TriBITS#309[)]"
      "Git Update PASSED!"
      "For extra repos, doing switch to branch for-testing"
      "TribitsExampleProject: Doing GIT update from URL '.*github[.]com.tribits/TribitsExampleProject[.]git' to dir '.*/TribitsExampleProject' [.][.][.]"
      "TribitsExampleProjectAddons: Doing initial GIT clone/checkout from URL '.*github.com.tribits/TribitsExampleProjectAddons.git' to dir 'TribitsExampleProjectAddons' [.][.][.]"
      "CTEST_NOTES_FILES_WO_CACHE='.*/BUILD/Updates.txt[;].*/BUILD/UpdateCommandsOutput.txt'"
      "Configure PASSED!"
      "CTEST_NOTES_FILES='.*/BUILD/TribitsExMetaProjRepoVersion.txt[;].*/BUILD/Updates.txt[;].*/BUILD/UpdateCommandsOutput.txt[;].*/BUILD/CMakeCache.clean.txt'"
      "Build PASSED!"
      "100% tests passed, 0 tests failed out of 11"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_12
    MESSAGE "Make sure that the file TribitsExMetaProjRepoVersion.txt is correct"
    CMND diff ARGS 
      "${CMAKE_CURRENT_LIST_DIR}/data/TribitsExMetaProjRepoVersion.for-testing.txt"
      BUILD/TribitsExMetaProjRepoVersion.txt 
    ALWAYS_FAIL_ON_NONZERO_RETURN

  )
  # The above test checks several things:
  #
  # * Tests the initial clone of all the repos from scratch on the default
  #   branch and remote name.
  #
  # * Tests that the Updates.txt file in *NOT* created or attached as a notes
  #   file on the initial clone.
  #
  # * Makes sure some git commit SHA1s are shown in 'Old/New revision of
  #   repository' printouts from ctest_update().
  #
  # * Checks that CDash URLs that get produced, including for all builds with
  # * the same repo version.  (This check has to be here since the base
  # * project repo must have a .git/ directory and we must know the exact SHA1
  # * of that repo for the test, which we don in this case.)
  #
  # NOTE: The reason that so many tests are done in a single ctest test is the
  # cost of the initial configure which checks the compilers which is very
  # expensive.  The other runs use
  # CTEST_START_WITH_EMPTY_BINARY_DIRECTORY=FALSE and CTEST_WIPE_CACHE=FALSE
  # to avoid this cost.


tribits_add_advanced_test( CTestDriver_TribitsExMetaProj_clone_default_branch_remote_CTEST_UPDATE_VERSION_ONLY
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran

  TEST_0
    MESSAGE "Run ctest driver with intial clone only of the repos on the default branch 'master' and remote 'origin'"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${COMMON_ENV_ARGS}
      TribitsExMetaProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
      CTEST_DO_CONFIGURE=OFF
      CTEST_DO_BUILD=OFF
      CTEST_DO_TEST=OFF
      CTEST_BUILD_NAME=CTestDriver_TribitsExMetaProj_clone_default_branch_remote_CTEST_UPDATE_VERSION_ONLY
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "CTEST_NOTES_FILES=''"
      "First perform the initial checkout: .*/git. clone -o origin .*github.com.tribits/TribitsExampleMetaProject.git"
      "Perform checkout in directory: .*/TriBITS_CTestDriver_TribitsExMetaProj_clone_default_branch_remote_CTEST_UPDATE_VERSION_ONLY"
      "Calling ctest_update[(][)] to update base source repo '.*/TriBITS_CTestDriver_TribitsExMetaProj_clone_default_branch_remote_CTEST_UPDATE_VERSION_ONLY/TribitsExampleMetaProject"
      "Old revision of repository is: [a-z0-9]+"
      "New revision of repository is: [a-z0-9]+"
      "ctest_update[(][.][.][.][)] returned '0'"
      "cmake -P tribits_ctest_update_commands[.]cmake"
      "Git Update PASSED!"
      "TribitsExampleProject: Doing initial GIT clone/checkout from URL '.*github[.]com.tribits/TribitsExampleProject.git' to dir 'TribitsExampleProject' [.][.][.]"
      "Cloning into 'TribitsExampleProject'[.][.][.]"
      "TribitsExampleProjectAddons: Doing initial GIT clone/checkout from URL '.*github[.]com.tribits/TribitsExampleProjectAddons.git' to dir 'TribitsExampleProjectAddons' [.][.][.]"
      "Cloning into 'TribitsExampleProjectAddons'[.][.][.]"
      "CTEST_NOTES_FILES_WO_CACHE='.*/TriBITS_CTestDriver_TribitsExMetaProj_clone_default_branch_remote_CTEST_UPDATE_VERSION_ONLY/BUILD/UpdateCommandsOutput.txt'"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_1
    MESSAGE "Run ctest driver again but this time with CTEST_UPDATE_VERSION_ONLY=ON"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${COMMON_ENV_ARGS}
      CTEST_START_WITH_EMPTY_BINARY_DIRECTORY=FALSE
      CTEST_WIPE_CACHE=FALSE
      TribitsExMetaProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
      CTEST_UPDATE_VERSION_ONLY=ON
      CTEST_BUILD_NAME=CTestDriver_TribitsExMetaProj_clone_default_branch_remote_CTEST_UPDATE_VERSION_ONLY
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "-- CTEST_NOTES_FILES=''"
      "-- ENV_CTEST_UPDATE_VERSION_ONLY='ON'"
      "-- CTEST_UPDATE_VERSION_ONLY='ON'"
      "Calling ctest_update[(][)] to update base source repo '.*/TriBITS_CTestDriver_TribitsExMetaProj_clone_default_branch_remote_CTEST_UPDATE_VERSION_ONLY/TribitsExampleMetaProject"
      "ctest_update[(][.][.][.][)] returned '0'"
      "Configure PASSED!"
      "Build PASSED!"
      "100% tests passed, 0 tests failed out of 11"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  )
  # The above test checks:
  #
  # * Tests the option CTEST_UPDATE_VERSION_ONLY=ON that requires all of the
  #   repos to be cloned and already be in the correct git state.
  #
  # NOTE: The reason that so many tests are done in a single ctest test is the
  # cost of the initial configure which checks the compilers which is very
  # expensive.  The other runs use
  # CTEST_START_WITH_EMPTY_BINARY_DIRECTORY=FALSE and CTEST_WIPE_CACHE=FALSE
  # to avoid this cost.


tribits_add_advanced_test( CTestDriver_TribitsExMetaProj_clone_custom_branch_remote
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_Fortran

  TEST_0
    MESSAGE "Run ctest driver with initial clone of the repos on the default branch 'master' and remote 'origin'"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${COMMON_ENV_ARGS}
      TribitsExMetaProj_GIT_REPOSITORY_REMOTE=github
      TribitsExMetaProj_BRANCH=for-testing
      TribitsExMetaProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
      CTEST_BUILD_NAME=CTestDriver_TribitsExMetaProj_clone_custom_branch_remote
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "CTEST_NOTES_FILES=''"
      "First perform the initial checkout: .*/git. clone -b for-testing -o github .*github.com.tribits/TribitsExampleMetaProject.git"
      "Perform checkout in directory: .*/TriBITS_CTestDriver_TribitsExMetaProj_clone_custom_branch_remote"
      "Calling ctest_update[(][)] to update base source repo '.*/TriBITS_CTestDriver_TribitsExMetaProj_clone_custom_branch_remote/TribitsExampleMetaProject"
      "Old revision of repository is: 863461e3035d24c632e175c087761e83db28bdc3"
      "New revision of repository is: 863461e3035d24c632e175c087761e83db28bdc3"
      "ctest_update[(][.][.][.][)] returned '0'"
      "cmake -P tribits_ctest_update_commands[.]cmake"
      "Git Update PASSED!"
      "TribitsExampleProject: Doing initial GIT clone/checkout from URL '.*github[.]com.tribits/TribitsExampleProject.git' to dir 'TribitsExampleProject' [.][.][.]"
      "Cloning into 'TribitsExampleProject'[.][.][.]"
      "TribitsExampleProjectAddons: Doing initial GIT clone/checkout from URL '.*github[.]com.tribits/TribitsExampleProjectAddons.git' to dir 'TribitsExampleProjectAddons' [.][.][.]"
      "Cloning into 'TribitsExampleProjectAddons'[.][.][.]"
      "CTEST_NOTES_FILES_WO_CACHE='.*/TriBITS_CTestDriver_TribitsExMetaProj_clone_custom_branch_remote/BUILD/UpdateCommandsOutput.txt'"
      "Configure PASSED!"
       "CTEST_NOTES_FILES='.*/TriBITS_CTestDriver_TribitsExMetaProj_clone_custom_branch_remote/BUILD/TribitsExMetaProjRepoVersion.txt[;].*/TriBITS_CTestDriver_TribitsExMetaProj_clone_custom_branch_remote/BUILD/UpdateCommandsOutput.txt[;].*/TriBITS_CTestDriver_TribitsExMetaProj_clone_custom_branch_remote/BUILD/CMakeCache.clean.txt'"
      "Build PASSED!"
      "100% tests passed, 0 tests failed out of 11"
      "Addon1 .*=.*.* sec[*]proc [(]1 test[)]"
      "MixedLang .* =.*.* sec[*]proc [(]1 test[)]"
      "SimpleCxx .*=*.* sec[*]proc [(]2 tests[)]"
      "WithSubpackages .*=*.* sec[*]proc [(]6 tests[)]"
      "WrapExternal .*=*.* sec[*]proc [(]1 test[)]"
      "File '' does NOT exist so all tests passed!"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_1
    MESSAGE "Check that base repo has correct remote"
    CMND git ARGS remote -v
    WORKING_DIRECTORY TribitsExampleMetaProject
    SKIP_CLEAN_WORKING_DIRECTORY
    PASS_REGULAR_EXPRESSION_ALL
      "github.*.*github[.]com.tribits/TribitsExampleMetaProject[.]git"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_2
    MESSAGE "Check that base repo is on the correct branch"
    CMND git ARGS branch
    WORKING_DIRECTORY TribitsExampleMetaProject
    SKIP_CLEAN_WORKING_DIRECTORY
    PASS_REGULAR_EXPRESSION_ALL
      "[*].*for-testing"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_3
    MESSAGE "Check that one of the extra repos has correct remote"
    CMND git ARGS remote -v
    WORKING_DIRECTORY TribitsExampleMetaProject/TribitsExampleProject
    SKIP_CLEAN_WORKING_DIRECTORY
    PASS_REGULAR_EXPRESSION_ALL
      "github.*.*github[.]com.tribits/TribitsExampleProject[.]git"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_4
    MESSAGE "Check that one of the extra repos in on the correct branch"
    CMND git ARGS branch
    WORKING_DIRECTORY TribitsExampleMetaProject/TribitsExampleProject
    SKIP_CLEAN_WORKING_DIRECTORY
    PASS_REGULAR_EXPRESSION_ALL
      "[*].*for-testing"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_5
    MESSAGE "Check out a retarded non-tracking branch"
    CMND "${${PROJECT_NAME}_TRIBITS_DIR}/python_utils/gitdist"
    ARGS --dist-no-color
      --dist-repos=.,TribitsExampleProject,TribitsExampleProjectAddons
      checkout -b non-tracking HEAD^
    WORKING_DIRECTORY TribitsExampleMetaProject
    SKIP_CLEAN_WORKING_DIRECTORY
    PASS_REGULAR_EXPRESSION_ALL
      "[*][*][*] Base Git Repo: TribitsExampleMetaProject"
      "Switched to a new branch 'non-tracking'"
      "[*][*][*] Git Repo: TribitsExampleProject"
      "[*][*][*] Git Repo: TribitsExampleProjectAddons"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_6
    MESSAGE "Check that base repo is on the correct branch"
    CMND git ARGS branch
    WORKING_DIRECTORY TribitsExampleMetaProject
    SKIP_CLEAN_WORKING_DIRECTORY
    PASS_REGULAR_EXPRESSION_ALL
      "[*].*non-tracking"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_7
    MESSAGE "Check that one of the extra repos in on the correct branch"
    CMND git ARGS branch
    WORKING_DIRECTORY TribitsExampleMetaProject/TribitsExampleProject
    SKIP_CLEAN_WORKING_DIRECTORY
    PASS_REGULAR_EXPRESSION_ALL
      "[*].*non-tracking"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_8
    MESSAGE "Remote the generated <project>RepoVersion.txt file"
    CMND rm ARGS BUILD/TribitsExMetaProjRepoVersion.txt
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_9
    MESSAGE "Run ctest driver to get back on the 'for-testing' branch"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${COMMON_ENV_ARGS}
      CTEST_START_WITH_EMPTY_BINARY_DIRECTORY=FALSE
      CTEST_WIPE_CACHE=FALSE
      TribitsExMetaProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
      TribitsExMetaProj_GIT_REPOSITORY_REMOTE=github
      TribitsExMetaProj_BRANCH=for-testing
      CTEST_BUILD_NAME=CTestDriver_TribitsExMetaProj_clone_custom_branch_remote
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "-- ENV_TribitsExMetaProj_BRANCH='for-testing'"
      "-- TribitsExMetaProj_BRANCH='for-testing'"
      "Calling ctest_update[(][)] to update base source repo '.*/TriBITS_CTestDriver_TribitsExMetaProj_clone_custom_branch_remote/TribitsExampleMetaProject"
      "Old revision of repository is: e12aad94c7514b42d059a7aa0488346dc0a8a9c0"
      "New revision of repository is: 863461e3035d24c632e175c087761e83db28bdc3"
      "ctest_update[(][.][.][.][)] returned '10'"
      "cmake -P tribits_ctest_update_commands[.]cmake"
      "HEAD is now at e12aad9 Merge branch 'tribits-github-snapshot' [(]TriBITSPub/TriBITS#309[)]"
      "Git Update PASSED!"
      "For extra repos, doing switch to branch for-testing"
      "TribitsExampleProject: Doing GIT update from URL '.*github[.]com.tribits/TribitsExampleProject[.]git' to dir '.*/TribitsExampleProject' [.][.][.]"
      "TribitsExampleProjectAddons: Doing GIT update from URL '.*github.com.tribits/TribitsExampleProjectAddons.git' to dir '.*/TribitsExampleProjectAddons' [.][.][.]"
      "CTEST_NOTES_FILES_WO_CACHE='.*/BUILD/Updates.txt[;].*/BUILD/UpdateCommandsOutput.txt'"
      "Configure PASSED!"
      "CTEST_NOTES_FILES='.*/BUILD/TribitsExMetaProjRepoVersion.txt[;].*/BUILD/Updates.txt[;].*/BUILD/UpdateCommandsOutput.txt[;].*/BUILD/CMakeCache.clean.txt'"
      "Build PASSED!"
      "100% tests passed, 0 tests failed out of 11"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  # NOTE: The above use case requires the custom git commands used by
  # ctest_update().  The default git commits used inside of ctest_update()
  # fail in that use case (see #TRIL-260 for an example).

  TEST_10
    MESSAGE "Check that base repo is on the correct branch"
    CMND git ARGS branch
    WORKING_DIRECTORY TribitsExampleMetaProject
    SKIP_CLEAN_WORKING_DIRECTORY
    PASS_REGULAR_EXPRESSION_ALL
      "[*].*for-testing"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_11
    MESSAGE "Check that one of the extra repos in on the correct branch"
    CMND git ARGS branch
    WORKING_DIRECTORY TribitsExampleMetaProject/TribitsExampleProject
    SKIP_CLEAN_WORKING_DIRECTORY
    PASS_REGULAR_EXPRESSION_ALL
      "[*].*for-testing"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_12
    MESSAGE "Make sure that the file TribitsExMetaProjRepoVersion.txt is correct"
    CMND diff ARGS 
      "${CMAKE_CURRENT_LIST_DIR}/data/TribitsExMetaProjRepoVersion.for-testing.txt"
      BUILD/TribitsExMetaProjRepoVersion.txt 
    ALWAYS_FAIL_ON_NONZERO_RETURN

  # ToDo: Do the update except change the git URLs for the base and extra
  # repos and make sure that it resets the git remote URL correctly.  (This
  # functionality is not implemented yet but it needs to be.)  But this will
  # fail the update because those remotes don't actually exist (but that is
  # okay for the purpose of this test).

  # ToDo: Check the remote name 'github' and git remote URLs and branch name
  # 'for-testing' in the cloned repos

  )
  # The above test checks a few other use cases:
  #
  # * Initial clone on non-default branch and remote name
  #
  # * 
  #
  # ???
  #


if (NOT GIT_VERSION_STRING VERSION_LESS "2.10.0")
  set(GIT_SUPPORTS_VERSION_DATE TRUE)
else()
  set(GIT_SUPPORTS_VERSION_DATE FASLE)
endif()

set(FULL_TEST_BINARY_DIR
  "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_CTestDriver_TribitsExMetaProj_version_date")

tribits_add_advanced_test( CTestDriver_TribitsExMetaProj_version_date
  OVERALL_WORKING_DIRECTORY  TEST_NAME
  OVERALL_NUM_MPI_PROCS  1
  EXCLUDE_IF_NOT_TRUE  GIT_SUPPORTS_VERSION_DATE  ${PROJECT_NAME}_ENABLE_Fortran

  TEST_0
    MESSAGE "Run ctest driver with initial clone and generate version date files and install"
    CMND env
    ARGS
      CTEST_DASHBOARD_ROOT=PWD
      ${COMMON_ENV_ARGS}
      TribitsExMetaProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR} # Need updated TriBITS!
      TribitsExMetaProj_BRANCH=for-testing
      TribitsExMetaProj_ENABLE_SECONDARY_TESTED_CODE=TRUE
      TribitsExMetaProj_GENERATE_VERSION_DATE_FILES=TRUE
      TribitsExMetaProj_TRACE_FILE_PROCESSING=ON
      TribitsExMetaProj_CMAKE_INSTALL_PREFIX=${FULL_TEST_BINARY_DIR}/BUILD/install
      CTEST_DO_INSTALL=TRUE
      CTEST_BUILD_NAME=CTestDriver_TribitsExMetaProj_version_date_header
      ${CTEST_S_SCRIPT_ARGS}
    PASS_REGULAR_EXPRESSION_ALL
      "-- ENV_TribitsExMetaProj_GENERATE_VERSION_DATE_FILES='TRUE'"
      "-- TribitsExMetaProj_GENERATE_VERSION_DATE_FILES='TRUE'"
      "CONFIGURE_OPTIONS = '.*[;]-DTribitsExMetaProj_GENERATE_VERSION_DATE_FILES:BOOL=TRUE[;].*[;]-DTribitsExMetaProj_TRACE_ADD_TEST=ON[;]-DCMAKE_INSTALL_PREFIX=.*/TriBITS_CTestDriver_TribitsExMetaProj_version_date/BUILD/install'"
      "TRIBITS_CTEST_DRIVER: OVERALL: ALL PASSED"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_1
    MESSAGE "Check that the TribitsExMetaProjec_version_date.h matches expected entire file"
    CMND diff
    ARGS
      "${CMAKE_CURRENT_SOURCE_DIR}/data/TribitsExMetaProj_version_date.for-testing.h"
      BUILD/TribitsExMetaProj_version_date.h
    # NOTE: The above test checks the entire contents of the file and
    # therefore locks it down in case it changes for any reason.  We will not
    # check other entire files like this, only the version date macro.

  TEST_2
    MESSAGE "Check that the VersionDAte matches expected entire file."
    CMND diff
    ARGS
      "${CMAKE_CURRENT_SOURCE_DIR}/data/VersionDate.TribitsExMetaProj.cmake"
      BUILD/VersionDate.cmake
    # NOTE: The above test checks the entire contents of the file and
    # therefore locks it down in case it changes for any reason.  We will not
    # check other entire files like this, only the version date macro.

  TEST_3
    MESSAGE "Check just macro in TribitsExampleProject_version_date.h"
    CMND cat
    ARGS BUILD/TribitsExampleProject/TribitsExampleProject_version_date.h
    PASS_REGULAR_EXPRESSION
      "#define TRIBITSEXAMPLEPROJECT_VERSION_DATE 2021091421"

  TEST_4
    MESSAGE "Check TribitsExampleProject/VersionDate.cmake"
    CMND cat
    ARGS BUILD/TribitsExampleProject/VersionDate.cmake
    PASS_REGULAR_EXPRESSION
      "set[(]TRIBITSEXAMPLEPROJECT_VERSION_DATE 2021091421[)]"

  TEST_5
    MESSAGE "Check just macro in TribitsExampleProjectAddons_version_date.h"
    CMND cat
    ARGS BUILD/TribitsExampleProjectAddons/TribitsExampleProjectAddons_version_date.h
    PASS_REGULAR_EXPRESSION
      "#define TRIBITSEXAMPLEPROJECTADDONS_VERSION_DATE 2021091421"

  TEST_6
    MESSAGE "Check TribitsExampleProjectAddons/VersionDate.cmake"
    CMND cat
    ARGS BUILD/TribitsExampleProjectAddons/VersionDate.cmake
    PASS_REGULAR_EXPRESSION
      "set[(]TRIBITSEXAMPLEPROJECTADDONS_VERSION_DATE 2021091421[)]"

  TEST_7
    MESSAGE "Check that installed TribitsExMetaProjec_version_date.h matches expected entire file!"
    CMND diff
    ARGS
      "${CMAKE_CURRENT_SOURCE_DIR}/data/TribitsExMetaProj_version_date.for-testing.h"
      BUILD/install/include/TribitsExMetaProj_version_date.h

  TEST_8
    MESSAGE "Check just the version date macro in intalled TribitsExampleProject_version_date.h"
    CMND cat
    ARGS BUILD/install/include/TribitsExampleProject_version_date.h
    PASS_REGULAR_EXPRESSION
      "#define TRIBITSEXAMPLEPROJECT_VERSION_DATE 2021091421"

  TEST_9
    MESSAGE "Check just the version date macro in installed TribitsExampleProjectAddons_version_date.h"
    CMND cat
    ARGS BUILD/install/include/TribitsExampleProjectAddons_version_date.h
    PASS_REGULAR_EXPRESSION
      "#define TRIBITSEXAMPLEPROJECTADDONS_VERSION_DATE 2021091421"

  TEST_10
    MESSAGE "Reconfigure to see the tracking of the includes of VersionDate.cmake files"
    WORKING_DIRECTORY BUILD
    SKIP_CLEAN_WORKING_DIRECTORY
    CMND ${CMAKE_COMMAND} ARGS -DTribitsExMetaProj_TRACE_FILE_PROCESSING=ON .
    PASS_REGULAR_EXPRESSION_ALL
      "-- File Trace: REPOSITORY INCLUDE .*/BUILD/VersionDate.cmake"
      "-- File Trace: REPOSITORY INCLUDE .*/BUILD/TribitsExampleProject/VersionDate.cmake"
      "-- File Trace: REPOSITORY INCLUDE .*/BUILD/TribitsExampleProjectAddons/VersionDate.cmake"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_11
    MESSAGE "Reconfigure with GIT_VERSION_STRING set to old to create undef macros"
    WORKING_DIRECTORY BUILD
    SKIP_CLEAN_WORKING_DIRECTORY
    CMND ${CMAKE_COMMAND} ARGS -DGIT_VERSION_STRING_OVERRIDE=2.6.1 .
    PASS_REGULAR_EXPRESSION_ALL
      "-- NOTE: Can't fill in version date files for TribitsExMetaProj since GIT_VERSION_STRING=2.6.1 < 2.10.0"
      "-- NOTE: Can't fill in version date files for TribitsExampleProject since GIT_VERSION_STRING=2.6.1 < 2.10.0"
      "-- NOTE: Can't fill in version date files for TribitsExampleProjectAddons since GIT_VERSION_STRING=2.6.1 < 2.10.0"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_12
    MESSAGE "Check that the TribitsExMetaProjec_version_date.h for undef macro"
    CMND cat
    ARGS BUILD/TribitsExMetaProj_version_date.h
    PASS_REGULAR_EXPRESSION
      "#undef TRIBITSEXMETAPROJ_VERSION_DATE"

  TEST_13
    MESSAGE "Check TribitsExampleProject_version_date.h for undef macro"
    CMND cat
    ARGS BUILD/TribitsExampleProject/TribitsExampleProject_version_date.h
    PASS_REGULAR_EXPRESSION
      "#undef TRIBITSEXAMPLEPROJECT_VERSION_DATE"

  TEST_14
    MESSAGE "Check TribitsExampleProjectAddons_version_date.h for undef macro"
    CMND cat
    ARGS BUILD/TribitsExampleProjectAddons/TribitsExampleProjectAddons_version_date.h
    PASS_REGULAR_EXPRESSION
      "#undef TRIBITSEXAMPLEPROJECTADDONS_VERSION_DATE"

  # ToD: Implement support for and verify that that the <Project>Config.cmake
  # file contains <REPO_NAME>_VERSION_DATE vars for each of the included repos
  # and that they are set to the correct values ...

  # ToDo: Go into the build directory and generate the source tarball to be
  # tested in a new configure below ...

  )
  # The above test checks several things:
  #
  # * Tests the generation of the VersionDate.cmake and
  #   <RepoName>_version_date.h files for each included repo.
  #
  # * Tests the install of the <RepoName>_version_date.h files.
  #
  # * Tests that if GIT_VERSION_STRING is too old, then it will generate
  #   <RepoName>_version_date.h files that have undef macros in them.


# ToDo: Untar the source tarball, verify that it contains the generated
# VersionDate.cmake files, configure to generate the correct
# <RepoVersion>_version_date.h files ...







# ToDo: Move the extra repo cloning and updating code into being called by
# cmake -P script inside of ctest_update() so that any update failure can be
# reported to CDash!  Also, don't do the configure, build, or tests if there
# is an update failure.


# ToDo: Add a test that does a bad clone using a bad repo URL for the base repo


# ToDo: Add a test that does a bad clone using a bad extra repo branch (but an
# okay base repo branch)


# ToDo: Add a test does a bad checkout of a branch that does not exist in an
# extra repo and ensure it reports the update failure.



# ToDo: Define the proper behavior for doing and update where the name
# ${TribitsExMetaProj_GIT_REPOSITORY_REMOTE} does not exist but the git repos
# already exist.  What should be the behavior?  NOTE: This should likely just
# error out as this should be an invalid use case.  If you want to use
# different remote, add it external to the ctest -S script invocation.
