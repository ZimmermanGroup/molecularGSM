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

cmake_minimum_required(VERSION 3.17.0 FATAL_ERROR)

message("PROJECT_NAME = ${PROJECT_NAME}")
message("${PROJECT_NAME}_TRIBITS_DIR = ${${PROJECT_NAME}_TRIBITS_DIR}")

set( CMAKE_MODULE_PATH
  "${${PROJECT_NAME}_TRIBITS_DIR}/core/utils"
  "${${PROJECT_NAME}_TRIBITS_DIR}/core/package_arch"
  )

set(TRIBITS_ADD_EXECUTABLE_UNIT_TESTING ON)

include(MessageWrapper)
include(TribitsTestCategories)
include(TribitsAddTest)
include(TribitsAddAdvancedTest)
include(TribitsAddExecutableAndTest)
include(TribitsETISupport)
include(TribitsFindPythonInterp)
include(TribitsStripQuotesFromStr)
include(TribitsStandardizePaths)
include(TribitsFilepathHelpers)
include(TribitsGetVersionDate)
include(TribitsTplFindIncludeDirsAndLibraries)
include(TribitsReportInvalidTribitsUsage)
include(TribitsGitRepoVersionInfo)
include(UnitTestHelpers)
include(GlobalSet)
include(GlobalNullSet)
include(AppendStringVar)


################################################################################
#
# Unit tests for a collection of TriBITS Core CMake code
#
# These unit tests are written in CMake itself.  This is not a very advanced
# unit testing system and it not that easy to work with.  However, it does
# perform some pretty strong testing and is much better than doing nothing.
# Each set of tests is in a function scope so as not to impact other tests.
#
################################################################################


################################################################################
#
# Testing misc functions and macros
#
################################################################################


function(unittest_append_string_var)

  message("\n***")
  message("*** Testing append_string_var()")
  message("***\n")

  message("append_string_var(): Testing simple concatenation")
  set(SOME_STRING_VAR "")
  append_string_var(SOME_STRING_VAR
     "begin\n" )
  append_string_var(SOME_STRING_VAR
     "middle1" " middile2" " middle3\n" )
  append_string_var(SOME_STRING_VAR
     "end\n" )
  unittest_compare_const(SOME_STRING_VAR
    "begin\nmiddle1 middile2 middle3\nend\n")

  message("append_string_var(): Testing with [] and {} which messes up ;")
  set(SOME_STRING_VAR "")
  append_string_var(SOME_STRING_VAR
     "[\n" )
  append_string_var(SOME_STRING_VAR
     "{middle1" " middile2" " middle3}\n" )
  append_string_var(SOME_STRING_VAR
     "]\n" )
  unittest_compare_const(SOME_STRING_VAR
    "[\n;{middle1; middile2; middle3}\n;]\n")

  message("append_string_var_ext(): Testing with [] and {} which ignores ;")
  set(SOME_STRING_VAR "")
  append_string_var_ext(SOME_STRING_VAR
     "[\n" )
  append_string_var_ext(SOME_STRING_VAR
     "{middle1 middile2 middle3}\n" )
  append_string_var_ext(SOME_STRING_VAR
     "]\n" )
  unittest_compare_const(SOME_STRING_VAR
    "[\n{middle1 middile2 middle3}\n]\n")

endfunction()


function(unittest_tribits_find_python_interp)

  message("\n***")
  message("*** Testing tribits_find_python_interp()")
  message("***\n")

  set(MESSAGE_WRAPPER_UNIT_TEST_MODE  TRUE)
  set(TRIBITS_FIND_PYTHON_UNITTEST  TRUE)

  message("tribits_find_python_interp(): ${PROJECT_NAME}_USES_PYTHON=FALSE")
  set(${PROJECT_NAME}_USES_PYTHON  FALSE)
  global_set(MESSAGE_WRAPPER_INPUT)
  tribits_find_python_interp()
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ;NOTE: Skipping check for Python because; ${PROJECT_NAME}_USES_PYTHON='FALSE'")
  unittest_compare_const(FIND_PythonInterp_ARGS
    "")

  message("tribits_find_python_interp(): ${PROJECT_NAME}_USES_PYTHON=")
  global_set(MESSAGE_WRAPPER_INPUT)
  set(${PROJECT_NAME}_USES_PYTHON)
  set(PYTHON_EXECUTABLE_UNITTEST_VAL /path/to/python2.4)
  tribits_find_python_interp()
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ;PYTHON_EXECUTABLE='/path/to/python2.4'")

  message("tribits_find_python_interp(): ${PROJECT_NAME}_USES_PYTHON=TRUE")
  global_set(MESSAGE_WRAPPER_INPUT)
  set(${PROJECT_NAME}_USES_PYTHON TRUE)
  global_set(PYTHON_EXECUTABLE_UNITTEST_VAL /path/to/python2.4)
  tribits_find_python_interp()
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ;PYTHON_EXECUTABLE='/path/to/python2.4'")
  unittest_compare_const(FIND_PythonInterp_ARGS
    "PythonInterp")

  message("tribits_find_python_interp(): ${PROJECT_NAME}_REQUIRES_PYTHON=TRUE")
  global_set(MESSAGE_WRAPPER_INPUT)
  set(${PROJECT_NAME}_USES_PYTHON FALSE)
  set(${PROJECT_NAME}_REQUIRES_PYTHON TRUE)
  set(PYTHON_EXECUTABLE_UNITTEST_VAL /path/to/python2.4)
  tribits_find_python_interp()
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ;PYTHON_EXECUTABLE='/path/to/python2.4'")
  unittest_compare_const(FIND_PythonInterp_ARGS
    "PythonInterp;REQUIRED")

  message("tribits_find_python_interp(): PythonInterp_FIND_VERSION=2.3")
  global_set(MESSAGE_WRAPPER_INPUT)
  set(PythonInterp_FIND_VERSION 2.3)
  set(PYTHON_EXECUTABLE_UNITTEST_VAL /dummy)
  tribits_find_python_interp()
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;Error,; PythonInterp_FIND_VERSION=2.3 < 2.6; is not allowed!;-- ;PYTHON_EXECUTABLE='/dummy'")

endfunction()


function(unittest_tribits_standardize_abs_paths)

  message("\n***")
  message("*** Testing tribits_standardize_abs_paths()")
  message("***\n")

  tribits_standardize_abs_paths(STANDARDIZED_ABS_PATHS
    "/okay/abs/path"
    "/abs/rel/path/../../other/path"
    "/final/okay/path"
    )
  unittest_compare_const(STANDARDIZED_ABS_PATHS
    "/okay/abs/path;/abs/other/path;/final/okay/path")

endfunction()


function(tribits_dir_is_basedir_test_case  absBaseDir  absFullDir  expectedIsBaseDir)
  message("\ntribits_dir_is_basedir(\"${absBaseDir}\" \"${absFullDir}\" isBaseDir)")
  tribits_dir_is_basedir("${absBaseDir}" "${absFullDir}" isBaseDir)
  unittest_compare_const(isBaseDir ${expectedIsBaseDir})
endfunction()


function(unittest_tribits_dir_is_basedir)

  message("\n***")
  message("*** Testing tribits_dir_is_basedir()")
  message("***\n")

  tribits_dir_is_basedir_test_case(
    "/some/base/path" "/some/base/path" TRUE)

  tribits_dir_is_basedir_test_case(
    "/some/base/path" "/some/base/path/more" TRUE)

  tribits_dir_is_basedir_test_case(
    "/some/base/path" "/some/base/paths" FALSE)

  tribits_dir_is_basedir_test_case(
    "/some/base/path/more" "/some/base/path" FALSE)

  tribits_dir_is_basedir_test_case(
    "/some/base/path" "/some/other/path" FALSE)

endfunction()


function(tribits_get_dir_array_below_base_dir_test_case  absBaseDir  absFullDir
  expectedTrailingDirArrayVar
  )
  message("\ntribits_get_dir_array_below_base_dir(\"${absBaseDir}\" \"${absFullDir}\" trailingDirArray)")
  tribits_get_dir_array_below_base_dir("${absBaseDir}" "${absFullDir}" trailingDirArray)
  unittest_compare_const(trailingDirArray "${expectedTrailingDirArrayVar}")
endfunction()


function(unittest_tribits_get_dir_array_below_base_dir)

  message("\n***")
  message("*** Testing tribits_get_dir_array_below_base_dir()")
  message("***\n")

  tribits_get_dir_array_below_base_dir_test_case(
    "/some/base/path" "/some/base/path" "")

  tribits_get_dir_array_below_base_dir_test_case(
    "/some/base/path" "/some/base/path/subdir" "subdir")

  tribits_get_dir_array_below_base_dir_test_case(
    "/some/base/path" "/some/base/path/subdir1/subdir2" "subdir1;subdir2")

endfunction()


function(unittest_tribits_misc)

  message("\n***")
  message("*** Testing miscellaneous TriBITS macro functionality")
  message("***\n")

  message("Testing message_wrapper(...) without capture")
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE FALSE)
  global_set(MESSAGE_WRAPPER_INPUT "Dummy")
  message_wrapper("Some message that should get printed and not intercepted")
  unittest_compare_const(MESSAGE_WRAPPER_INPUT "Dummy")

  message("Testing message_wrapper(...) with capture")
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)
  global_set(MESSAGE_WRAPPER_INPUT "Dummy")
  message_wrapper("Some message that should get intercepted")
  unittest_compare_const(MESSAGE_WRAPPER_INPUT "Dummy;Some message that should get intercepted")

  message("Testing find_list_element(${PROJECT_NAME}_VALID_CATEGORIES BASIC ELEMENT_FOUND)")
  find_list_element(${PROJECT_NAME}_VALID_CATEGORIES BASIC ELEMENT_FOUND)
  unittest_compare_const(ELEMENT_FOUND "TRUE")

  message("Testing find_list_element(${PROJECT_NAME}_VALID_CATEGORIES BADCAT ELEMENT_FOUND)")
  find_list_element(${PROJECT_NAME}_VALID_CATEGORIES BADCAT ELEMENT_FOUND)
  unittest_compare_const(ELEMENT_FOUND "FALSE")

  message("Testing tribits_get_invalid_categories( ... BADCAT)")
  tribits_get_invalid_categories(INVALID_CATEGORIES BADCAT)
  unittest_compare_const( INVALID_CATEGORIES "BADCAT" )

  message("Testing tribits_get_invalid_categories( ... BADCAT BASIC)")
  tribits_get_invalid_categories(INVALID_CATEGORIES BADCAT BASIC)
  unittest_compare_const( INVALID_CATEGORIES "BADCAT" )

  message("Testing tribits_get_invalid_categories( ... BASIC BADCAT)")
  tribits_get_invalid_categories(INVALID_CATEGORIES BASIC BADCAT)
  unittest_compare_const( INVALID_CATEGORIES "BADCAT" )

  message("Testing tribits_get_invalid_categories( ... BADCAT1 BADCAT2)")
  tribits_get_invalid_categories(INVALID_CATEGORIES BADCAT1 BADCAT2)
  unittest_compare_const( INVALID_CATEGORIES "BADCAT1;BADCAT2" )

  message("Testing tribits_get_invalid_categories( ... BASIC BADCAT1 NIGHTLY BADCAT2 PERFORMANCE)")
  tribits_get_invalid_categories(INVALID_CATEGORIES BASIC BADCAT1 NIGHTLY BADCAT2 PERFORMANCE)
  unittest_compare_const( INVALID_CATEGORIES "BADCAT1;BADCAT2" )

  message("Testing tribits_filter_and_assert_categories( ... BADCAT1 BASIC BADCAT2)")
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)
  global_set(MESSAGE_WRAPPER_INPUT)
  set(CATEGORIES BADCAT1 BASIC BADCAT2)
  tribits_filter_and_assert_categories(CATEGORIES)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE FALSE)
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "SEND_ERROR;Error: The categories 'BADCAT1;BADCAT2' are not; in the list of valid categories '${${PROJECT_NAME}_VALID_CATEGORIES_STR}'!")
  unittest_compare_const(CATEGORIES "BADCAT1;BASIC;BADCAT2")

  message("Testing tribits_filter_and_assert_categories( ... BASIC)")
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)
  global_set(MESSAGE_WRAPPER_INPUT "Dummy")
  set(CATEGORIES BASIC)
  tribits_filter_and_assert_categories(CATEGORIES)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE FALSE)
  unittest_compare_const(MESSAGE_WRAPPER_INPUT "Dummy")
  unittest_compare_const(CATEGORIES "BASIC")

  message("Testing tribits_filter_and_assert_categories( ... BASIC NIGHTLY)")
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)
  global_set(MESSAGE_WRAPPER_INPUT "Dummy")
  set(CATEGORIES BASIC NIGHTLY)
  tribits_filter_and_assert_categories(CATEGORIES)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE FALSE)
  unittest_compare_const(MESSAGE_WRAPPER_INPUT "Dummy")
  unittest_compare_const(CATEGORIES "BASIC;NIGHTLY")

  message("Testing tribits_filter_and_assert_categories( ... BASIC WEEKLY NIGHTLY)")
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)
  global_set(MESSAGE_WRAPPER_INPUT "")
  set(CATEGORIES BASIC WEEKLY NIGHTLY)
  tribits_filter_and_assert_categories(CATEGORIES)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE FALSE)
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "WARNING;Warning: The test category 'WEEKLY' is deprecated; and is replaced with 'HEAVY'.  Please change to use 'HEAVY' instead.")
  unittest_compare_const(CATEGORIES "BASIC;HEAVY;NIGHTLY")

  message("Testing tribits_filter_and_assert_categories( ... HEAVY)")
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)
  global_set(MESSAGE_WRAPPER_INPUT "Dummy")
  set(CATEGORIES HEAVY)
  tribits_filter_and_assert_categories(CATEGORIES)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE FALSE)
  unittest_compare_const(MESSAGE_WRAPPER_INPUT "Dummy")
  unittest_compare_const(CATEGORIES "HEAVY")

  message("Testing tribits_filter_and_assert_categories( ... BASIC HEAVY)")
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)
  global_set(MESSAGE_WRAPPER_INPUT "Dummy")
  set(CATEGORIES BASIC HEAVY)
  tribits_filter_and_assert_categories(CATEGORIES)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE FALSE)
  unittest_compare_const(MESSAGE_WRAPPER_INPUT "Dummy")
  unittest_compare_const(CATEGORIES "BASIC;HEAVY")

endfunction()


macro(unittest_tribits_tpl_allow_pre_find_package_unset_vars  TPL_NAME)
  set(${TPL_NAME}_INCLUDE_DIRS "")
  set(${TPL_NAME}_LIBRARY_NAMES "")
  set(${TPL_NAME}_LIBRARY_DIRS "")
  set(${TPL_NAME}_FORCE_PRE_FIND_PACKAGE FALSE)
  set(${TPL_NAME}_ALLOW_PACKAGE_PREFIND TRUE)
  set(TPL_${TPL_NAME}_INCLUDE_DIRS "")
  set(TPL_${TPL_NAME}_LIBRARIES "")
  set(TPL_${TPL_NAME}_LIBRARY_DIRS "")
endmacro()


function(unittest_tribits_strip_quotes_from_str)

  message("\n***")
  message("*** Testing tribits_strip_quotes_from_str()")
  message("***\n")

  message("Testing tribits_strip_quotes_from_str() with str with quotes")
  tribits_strip_quotes_from_str("\"some string in quotes\"" STR_OUT)
  unittest_compare_const(STR_OUT "some string in quotes")

  message("Testing tribits_strip_quotes_from_str() with just '\"'")
  tribits_strip_quotes_from_str("\"" STR_OUT)
  unittest_compare_const(STR_OUT "\"")

  message("Testing tribits_strip_quotes_from_str() with just '\"\"'")
  tribits_strip_quotes_from_str("\"\"" STR_OUT)
  unittest_compare_const(STR_OUT "")

  message("Testing tribits_strip_quotes_from_str() with str with quote only on front")
  tribits_strip_quotes_from_str("\"some string in quotes" STR_OUT)
  unittest_compare_const(STR_OUT "\"some string in quotes")

  message("Testing tribits_strip_quotes_from_str() with str with quote only on back")
  tribits_strip_quotes_from_str("some string in quotes\"" STR_OUT)
  unittest_compare_const(STR_OUT "some string in quotes\"")

endfunction()


function(unittest_tribits_get_raw_git_commit_utc_time)

  message("\n***")
  message("*** Testing tribits_get_raw_git_commit_utc_time()")
  message("***\n")

  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)
  set(TRIBITS_GET_RAW_GIT_COMMIT_UTC_TIME_UNIT_TEST_MODE TRUE)

  message("Testing tribits_get_raw_git_commit_utc_time() no GIT_EXECUTABLE")
  global_null_set(MESSAGE_WRAPPER_INPUT)
  tribits_get_raw_git_commit_utc_time( dummy_base_repo dummy_commit_ref
    git_commit_utc_time )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;Error, GIT_EXECUTABLE not set!;FATAL_ERROR;Error, GIT_VERSION_STRING not set!;FATAL_ERROR;Error, GIT_VERSION_STRING= < 2.10.0!"
    )

  set(GIT_EXECUTABLE dummy_git)

  message("Testing tribits_get_raw_git_commit_utc_time() no GIT_VERSION_STRING")
  global_null_set(MESSAGE_WRAPPER_INPUT)
  tribits_get_raw_git_commit_utc_time( dummy_base_repo dummy_commit_ref
    git_commit_utc_time )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;Error, GIT_VERSION_STRING not set!;FATAL_ERROR;Error, GIT_VERSION_STRING= < 2.10.0!"
    )

  set(GIT_VERSION_STRING "2.6.1")

  message("Testing tribits_get_raw_git_commit_utc_time() GIT_VERSION_STRING too low")
  global_null_set(MESSAGE_WRAPPER_INPUT)
  tribits_get_raw_git_commit_utc_time( dummy_base_repo dummy_commit_ref
    git_commit_utc_time )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;Error, GIT_VERSION_STRING=2.6.1 < 2.10.0!"
    )

  set(MESSAGE_WRAPPER_UNIT_TEST_MODE FALSE)

  # NOTE: The actual git commit and post-processing behavior in
  # tribits_get_raw_git_commit_utc_time() is tested in
  # tribits_get_version_date/CMakeLists.txt.

endfunction()


function(unittest_tribits_get_version_date_from_raw_git_commit_utc_time)

  message("\n***")
  message("*** Testing tribits_get_version_date_from_raw_git_commit_utc_time()")
  message("***\n")

  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)

  message("Testing tribits_get_version_date_from_raw_git_commit_utc_time() good input 1")
  global_null_set(MESSAGE_WRAPPER_INPUT)
  tribits_get_version_date_from_raw_git_commit_utc_time(
    "2019-03-02 02:34:15 +0000" VERSION_DATE)
  unittest_compare_const(VERSION_DATE "2019030202")
  unittest_compare_const(MESSAGE_WRAPPER_INPUT "")

  message("Testing tribits_get_version_date_from_raw_git_commit_utc_time() good input 2")
  global_null_set(MESSAGE_WRAPPER_INPUT)
  tribits_get_version_date_from_raw_git_commit_utc_time(
    "2024-12-13 12:10:15 +0000" VERSION_DATE)
  unittest_compare_const(VERSION_DATE "2024121312")
  unittest_compare_const(MESSAGE_WRAPPER_INPUT "")

  message("Testing tribits_get_version_date_from_raw_git_commit_utc_time() bad offset")
  global_null_set(MESSAGE_WRAPPER_INPUT)
  tribits_get_version_date_from_raw_git_commit_utc_time(
    "2024-12-13 12:10:15 -0600" VERSION_DATE)
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;ERROR, '2024-12-13 12:10:15 -0600' is NOT; in UTC which would have offset '+0000'!"
    )

  set(MESSAGE_WRAPPER_UNIT_TEST_MODE FALSE)

endfunction()


function(unittest_tribits_git_repo_sha1)

  message("\n***")
  message("*** Testing tribits_git_repo_sha1()")
  message("***\n")

  find_package(Git REQUIRED)

  set(tribitsProjDir "${${PROJECT_NAME}_TRIBITS_DIR}/..")

  # Matches any 40-char SHA1
  set(anySha1Regex "")
  foreach(i RANGE 0 39)
    string(APPEND anySha1Regex "[a-z0-9]")
  endforeach()

  if (IS_DIRECTORY "${tribitsProjDir}/.git")

    message("Testing tribits_git_repo_sha1() with the base TriBITS project (without FAILURE_MESSAGE_OUT)\n")
    tribits_git_repo_sha1("${tribitsProjDir}" gitRepoSha1)
    unittest_string_var_regex(gitRepoSha1 REGEX_STRINGS "${anySha1Regex}")

    message("Testing tribits_git_repo_sha1() with the base TriBITS project (with FAILURE_MESSAGE_OUT)\n")
    tribits_git_repo_sha1("${tribitsProjDir}" gitRepoSha1
      FAILURE_MESSAGE_OUT  failureMsg)
    unittest_string_var_regex(gitRepoSha1 REGEX_STRINGS "${anySha1Regex}")
    unittest_compare_const(failureMsg "")

  endif()

   message("Testing tribits_git_repo_sha1(): No GIT_EXECUTABLE (with FAILURE_MESSAGE_OUT)\n")
  set(GIT_EXECUTABLE_SAVED "${GIT_EXECUTABLE}")
  set(GIT_EXECUTABLE "")
  tribits_git_repo_sha1("${tribitsProjDir}" gitRepoSha1
    FAILURE_MESSAGE_OUT  failureMsg)
  unittest_compare_const(gitRepoSha1 "")
  unittest_compare_const(failureMsg "ERROR: The program 'git' could not be found!")
  set(GIT_EXECUTABLE "${GIT_EXECUTABLE_SAVED}")
  unset(GIT_EXECUTABLE_SAVED)

   message("Testing tribits_git_repo_sha1(): Invalid repo dir (with FAILURE_MESSAGE_OUT)\n")
  tribits_git_repo_sha1("/repo/does/not/exist" gitRepoSha1
    FAILURE_MESSAGE_OUT  failureMsg)
  unittest_compare_const(gitRepoSha1 "")
  unittest_compare_const(failureMsg "ERROR: The directory /repo/does/not/exist/.git does not exist!")

endfunction()


function(unittest_tribits_tpl_allow_pre_find_package)

  message("\n***")
  message("*** Testing tribits_tpl_allow_pre_find_package()")
  message("***\n")

  message("tribits_tpl_allow_pre_find_package(): No vars set")
  unittest_tribits_tpl_allow_pre_find_package_unset_vars(SomeTpl)
  tribits_tpl_allow_pre_find_package(SomeTpl  SomeTpl_ALLOW_PACKAGE_PREFIND)
  unittest_compare_const(SomeTpl_ALLOW_PACKAGE_PREFIND  "TRUE")

  message("tribits_tpl_allow_pre_find_package(): Set SomeTpl_INCLUDE_DIRS")
  unittest_tribits_tpl_allow_pre_find_package_unset_vars(SomeTpl)
  set(SomeTpl_INCLUDE_DIRS "/some/include/dir")
  tribits_tpl_allow_pre_find_package(SomeTpl  SomeTpl_ALLOW_PACKAGE_PREFIND)
  unittest_compare_const(SomeTpl_ALLOW_PACKAGE_PREFIND  "FALSE")

  message("tribits_tpl_allow_pre_find_package(): Set SomeTpl_LIBRARY_NAMES")
  unittest_tribits_tpl_allow_pre_find_package_unset_vars(SomeTpl)
  set(SomeTpl_LIBRARY_NAMES "lib1;lib2")
  tribits_tpl_allow_pre_find_package(SomeTpl  SomeTpl_ALLOW_PACKAGE_PREFIND)
  unittest_compare_const(SomeTpl_ALLOW_PACKAGE_PREFIND  "FALSE")

  message("tribits_tpl_allow_pre_find_package(): Set SomeTpl_LIBRARY_DIRS")
  unittest_tribits_tpl_allow_pre_find_package_unset_vars(SomeTpl)
  set(SomeTpl_LIBRARY_DIRS "/some/lib/dir")
  tribits_tpl_allow_pre_find_package(SomeTpl  SomeTpl_ALLOW_PACKAGE_PREFIND)
  unittest_compare_const(SomeTpl_ALLOW_PACKAGE_PREFIND  "FALSE")

  message("tribits_tpl_allow_pre_find_package(): Set SomeTpl_[INCLUDE_DIRS,LIBRARY_NAMES,LIBRARY_DIRS]")
  unittest_tribits_tpl_allow_pre_find_package_unset_vars(SomeTpl)
  set(SomeTpl_INCLUDE_DIRS "/some/include/dir")
  set(SomeTpl_LIBRARY_NAMES "lib1;lib2")
  set(SomeTpl_LIBRARY_DIRS "/some/lib/dir")
  tribits_tpl_allow_pre_find_package(SomeTpl  SomeTpl_ALLOW_PACKAGE_PREFIND)
  unittest_compare_const(SomeTpl_ALLOW_PACKAGE_PREFIND  "FALSE")

  #set(TRIBITS_TPL_ALLOW_PRE_FIND_PACKAGE_DEBUG TRUE)

  message("tribits_tpl_allow_pre_find_package(): No vars set (2)")
  unittest_tribits_tpl_allow_pre_find_package_unset_vars(SomeTpl)
  tribits_tpl_allow_pre_find_package(SomeTpl  SomeTpl_ALLOW_PACKAGE_PREFIND)
  unittest_compare_const(SomeTpl_ALLOW_PACKAGE_PREFIND  "TRUE")

  message("tribits_tpl_allow_pre_find_package(): Set SomeTpl_INCLUDE_DIRS (force prefind)")
  unittest_tribits_tpl_allow_pre_find_package_unset_vars(SomeTpl)
  set(SomeTpl_INCLUDE_DIRS "/some/include/dir")
  set(SomeTpl_FORCE_PRE_FIND_PACKAGE TRUE)
  tribits_tpl_allow_pre_find_package(SomeTpl  SomeTpl_ALLOW_PACKAGE_PREFIND)
  unittest_compare_const(SomeTpl_ALLOW_PACKAGE_PREFIND  "TRUE")

  message("tribits_tpl_allow_pre_find_package(): Set SomeTpl_LIBRARY_NAMES (force prefind)")
  unittest_tribits_tpl_allow_pre_find_package_unset_vars(SomeTpl)
  set(SomeTpl_LIBRARY_NAMES "lib1;lib2")
  set(SomeTpl_FORCE_PRE_FIND_PACKAGE TRUE)
  tribits_tpl_allow_pre_find_package(SomeTpl  SomeTpl_ALLOW_PACKAGE_PREFIND)
  unittest_compare_const(SomeTpl_ALLOW_PACKAGE_PREFIND  "TRUE")

  message("tribits_tpl_allow_pre_find_package(): Set SomeTpl_LIBRARY_DIRS (force prefind)")
  unittest_tribits_tpl_allow_pre_find_package_unset_vars(SomeTpl)
  set(SomeTpl_LIBRARY_DIRS "/some/lib/dir")
  set(SomeTpl_FORCE_PRE_FIND_PACKAGE TRUE)
  tribits_tpl_allow_pre_find_package(SomeTpl  SomeTpl_ALLOW_PACKAGE_PREFIND)
  unittest_compare_const(SomeTpl_ALLOW_PACKAGE_PREFIND  "TRUE")

  message("tribits_tpl_allow_pre_find_package(): Set SomeTpl_[INCLUDE_DIRS,LIBRARY_NAMES,LIBRARY_DIRS] (force prefind)")
  unittest_tribits_tpl_allow_pre_find_package_unset_vars(SomeTpl)
  set(SomeTpl_INCLUDE_DIRS "/some/include/dir")
  set(SomeTpl_LIBRARY_NAMES "lib1;lib2")
  set(SomeTpl_LIBRARY_DIRS "/some/lib/dir")
  set(SomeTpl_FORCE_PRE_FIND_PACKAGE TRUE)
  tribits_tpl_allow_pre_find_package(SomeTpl  SomeTpl_ALLOW_PACKAGE_PREFIND)
  unittest_compare_const(SomeTpl_ALLOW_PACKAGE_PREFIND  "TRUE")

  message("tribits_tpl_allow_pre_find_package(): No vars set (3)")
  unittest_tribits_tpl_allow_pre_find_package_unset_vars(SomeTpl)
  tribits_tpl_allow_pre_find_package(SomeTpl  SomeTpl_ALLOW_PACKAGE_PREFIND)
  unittest_compare_const(SomeTpl_ALLOW_PACKAGE_PREFIND  "TRUE")

  message("tribits_tpl_allow_pre_find_package(): Set TPL_SomeTpl_INCLUDE_DIRS")
  unittest_tribits_tpl_allow_pre_find_package_unset_vars(SomeTpl)
  set(TPL_SomeTpl_INCLUDE_DIRS "/some/include/dir")
  tribits_tpl_allow_pre_find_package(SomeTpl  SomeTpl_ALLOW_PACKAGE_PREFIND)
  unittest_compare_const(SomeTpl_ALLOW_PACKAGE_PREFIND  "FALSE")

  message("tribits_tpl_allow_pre_find_package(): Set TPL_SomeTpl_LIBRARIES")
  unittest_tribits_tpl_allow_pre_find_package_unset_vars(SomeTpl)
  set(TPL_SomeTpl_LIBRARIES "/some/include/dir")
  tribits_tpl_allow_pre_find_package(SomeTpl  SomeTpl_ALLOW_PACKAGE_PREFIND)
  unittest_compare_const(SomeTpl_ALLOW_PACKAGE_PREFIND  "FALSE")

  message("tribits_tpl_allow_pre_find_package(): Set TPL_SomeTpl_LIBRARY_DIRS")
  unittest_tribits_tpl_allow_pre_find_package_unset_vars(SomeTpl)
  set(TPL_SomeTpl_LIBRARY_DIRS "/some/lib/dir")
  tribits_tpl_allow_pre_find_package(SomeTpl  SomeTpl_ALLOW_PACKAGE_PREFIND)
  unittest_compare_const(SomeTpl_ALLOW_PACKAGE_PREFIND  "FALSE")

  message("tribits_tpl_allow_pre_find_package(): Set TPL_SomeTpl_[INCLUDE_DIRS,LIBRARIES,LIBRARY_DIRS]")
  unittest_tribits_tpl_allow_pre_find_package_unset_vars(SomeTpl)
  set(TPL_SomeTpl_INCLUDE_DIRS "/some/include/dir")
  set(TPL_SomeTpl_LIBRARIES "/some/include/dir")
  set(TPL_SomeTpl_LIBRARY_DIRS "/some/lib/dir")
  tribits_tpl_allow_pre_find_package(SomeTpl  SomeTpl_ALLOW_PACKAGE_PREFIND)
  unittest_compare_const(SomeTpl_ALLOW_PACKAGE_PREFIND  "FALSE")

  message("tribits_tpl_allow_pre_find_package(): Set SomeTpl_INCLUDE_DIRS and TPL_SomeTpl_INCLUDE_DIRS (force update)")
  unittest_tribits_tpl_allow_pre_find_package_unset_vars(SomeTpl)
  set(SomeTpl_INCLUDE_DIRS "/some/include/dir")
  set(TPL_SomeTpl_INCLUDE_DIRS "/some/include/dir")
  set(SomeTpl_FORCE_PRE_FIND_PACKAGE TRUE)
  tribits_tpl_allow_pre_find_package(SomeTpl  SomeTpl_ALLOW_PACKAGE_PREFIND)
  unittest_compare_const(SomeTpl_ALLOW_PACKAGE_PREFIND  "FALSE")

  #set(TRIBITS_TPL_ALLOW_PRE_FIND_PACKAGE_DEBUG TRUE)

  # ToDo: Test that "force prefind" does not affect TPL_${TPL_NAME}_XXX logic

  message("tribits_tpl_allow_pre_find_package(): No vars set (4)")
  unittest_tribits_tpl_allow_pre_find_package_unset_vars(SomeTpl)
  tribits_tpl_allow_pre_find_package(SomeTpl  SomeTpl_ALLOW_PACKAGE_PREFIND)
  unittest_compare_const(SomeTpl_ALLOW_PACKAGE_PREFIND  "TRUE")

endfunction()


function(unittest_tribits_report_invalid_tribits_usage)

  message("\n***")
  message("*** Testing tribits_report_invalid_tribits_usage()")
  message("***\n")

  set(MESSAGE_WRAPPER_UNIT_TEST_MODE  TRUE)
  set(PROJECT_NAME TRITU_PROJECT)

  message("tribits_report_invalid_tribits_usage(): Default (FATAL_ERROR)")
  global_set(MESSAGE_WRAPPER_INPUT)
  tribits_report_invalid_tribits_usage("Something went" " very wrong" " I think 1!")
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;Something went; very wrong; I think 1!")

  message("tribits_report_invalid_tribits_usage(): FATAL_ERROR")
  global_set(MESSAGE_WRAPPER_INPUT)
  set(${PROJECT_NAME}_ASSERT_CORRECT_TRIBITS_USAGE FATAL_ERROR)
  tribits_report_invalid_tribits_usage("Something went" " very wrong" " I think 2!")
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;Something went; very wrong; I think 2!")

  message("tribits_report_invalid_tribits_usage(): SEND_ERROR")
  global_set(MESSAGE_WRAPPER_INPUT)
  set(${PROJECT_NAME}_ASSERT_CORRECT_TRIBITS_USAGE SEND_ERROR)
  tribits_report_invalid_tribits_usage("Something went very wrong I think 3!")
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "SEND_ERROR;Something went very wrong I think 3!")

  message("tribits_report_invalid_tribits_usage(): WARNING")
  global_set(MESSAGE_WRAPPER_INPUT)
  set(${PROJECT_NAME}_ASSERT_CORRECT_TRIBITS_USAGE WARNING)
  tribits_report_invalid_tribits_usage("Something went very wrong I think 4!")
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "WARNING;Something went very wrong I think 4!")

  message("tribits_report_invalid_tribits_usage(): IGNORE")
  global_set(MESSAGE_WRAPPER_INPUT)
  set(${PROJECT_NAME}_ASSERT_CORRECT_TRIBITS_USAGE IGNORE)
  tribits_report_invalid_tribits_usage("Something went very wrong I think 5!")
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "")

  message("tribits_report_invalid_tribits_usage(): INVALID_ARGUMENT")
  global_set(MESSAGE_WRAPPER_INPUT)
  set(${PROJECT_NAME}_ASSERT_CORRECT_TRIBITS_USAGE INVALID_ARGUMENT)
  tribits_report_invalid_tribits_usage("Something went very wrong I think 6!")
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;Error, invalid value for; TRITU_PROJECT_ASSERT_CORRECT_TRIBITS_USAGE =; 'INVALID_ARGUMENT'!;  Value values include 'FATAL_ERROR', 'SEND_ERROR', 'WARNING', and 'IGNORE'!")

endfunction()


################################################################################
#
# Testing tribits_add_test()
#
################################################################################


function(unittest_tribits_add_test_basic)

  message("\n***")
  message("*** Testing basic functionality of tribits_add_test(...)")
  message("***\n")

  # Needed by tribits_add_test(...)
  set(PACKAGE_NAME PackageA)
  set(PARENT_PACKAGE_NAME PackageA)
  set(${PACKAGE_NAME}_ENABLE_TESTS ON)

  # Used locally
  set(EXEN SomeExec)
  set(PACKEXEN ${PACKAGE_NAME}_${EXEN})
  set(PACKEXEN_PATH "${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe")

  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)

  #
  # Add basic test with and without tracing
  #

  message("Unconditionally add test (no tracing)")
  tribits_add_test( ${EXEN} ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH}"
    )
  unittest_compare_const( # Don't trace by default
    MESSAGE_WRAPPER_INPUT
    ""
    )
  unittest_compare_const( # Don't capture unless I want to
    TRIBITS_SET_TEST_PROPERTIES_INPUT
    ""
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "${PACKEXEN}" )

  # Turn on tracing for the rest of the tests!
  set(${PROJECT_NAME}_TRACE_ADD_TEST ON)

  message("Unconditionally add test (with tracing)")
  tribits_add_test( ${EXEN} ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH}"
    )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (BASIC, PROCESSORS=1)!"
    )
  unittest_compare_const( # Don't capture unless I want to
    TRIBITS_SET_TEST_PROPERTIES_INPUT
    ""
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "${PACKEXEN}" )

  #
  # <Package>_ENABLE_TESTS=OFF
  #

  message("Tests not enabled")
  set(${PACKAGE_NAME}_ENABLE_TESTS OFF)
  tribits_add_test( ${EXEN} ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- PackageA_SomeExec: NOT added test because PackageA_ENABLE_TESTS='OFF'."
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "" )
  set(${PACKAGE_NAME}_ENABLE_TESTS ON)

  #
  # Test different numbers and types of arguments
  #

  message("Add a single basic test with no arguments")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH}"
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "${PACKEXEN}" )

  message("Add a single basic test with a single argument")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ARGS arg1 )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH};arg1"
    )

  message("Add a single basic test with a single argument that is '0'")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ARGS 0 )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH};0"
    )

  message("Add a single basic test with a single argument that is 'N'")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ARGS N )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH};N"
    )

  message("Add a single basic test with two arguments")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ARGS "arg1 arg2" ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH};arg1;arg2"
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "${PACKEXEN}" )

  message("Add two tests with simple arguments")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ARGS "arg1" "arg2 arg3"
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_0;COMMAND;${PACKEXEN_PATH};arg1;NAME;${PACKEXEN}_1;COMMAND;${PACKEXEN_PATH};arg2;arg3"
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "${PACKEXEN}_0;${PACKEXEN}_1" )

  message("Add a double quoted input argument")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ARGS "--arg1=\"bob and cats\"" )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH};--arg1=\"bob and cats\""
    )

  message("Add a double quoted with single quotes input argument")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ARGS "--arg1=\"'bob' and 'cats'\"" )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH};--arg1=\"'bob' and 'cats'\""
    )

  message("Add one test using POSTFIX_AND_ARGS_0 with empty postfix")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN}
    POSTFIX_AND_ARGS_0  ""  arg1
    ADDED_TESTS_NAMES_OUT  ${EXEN}_TEST_NAMES
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH};arg1"
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "${PACKEXEN}" )

  message("Add two tests using POSTFIX_AND_ARGS_0 with spaces and semi-colons")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN}
    POSTFIX_AND_ARGS_0  "test0"  "has some<sep>spaces" "--and semi<sep>colons too"
    POSTFIX_AND_ARGS_1  "test1"  "has2 some<sep>spaces" "--and2 semi<sep>colons too"
    LIST_SEPARATOR "<sep>"
    ADDED_TESTS_NAMES_OUT  ${EXEN}_TEST_NAMES
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;PackageA_SomeExec_test0;COMMAND;${PACKEXEN_PATH};has some;spaces;--and semi;colons too;NAME;PackageA_SomeExec_test1;COMMAND;${PACKEXEN_PATH};has2 some;spaces;--and2 semi;colons too"
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "${PACKEXEN}_test0;${PACKEXEN}_test1" )
  # NOTE: There is a test under CTestScriptsUnitTests/ that actaully runs an
  # real test and command that verifies the correct handling of semi-colons.
  # The mashing that CMake above is doing to these replaced semicolons does
  # not do them justice.

  message("Add two tests with different postfixes and arguments")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN}
    POSTFIX_AND_ARGS_0  pf_arg1  arg1
    POSTFIX_AND_ARGS_1  pf_arg23  arg2  arg3
    ADDED_TESTS_NAMES_OUT  ${EXEN}_TEST_NAMES
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_pf_arg1;COMMAND;${PACKEXEN_PATH};arg1;NAME;${PACKEXEN}_pf_arg23;COMMAND;${PACKEXEN_PATH};arg2;arg3"
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "${PACKEXEN}_pf_arg1;${PACKEXEN}_pf_arg23" )

  message("Add two tests with different postfixes and arguments with '0' and 'N'")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN}
    POSTFIX_AND_ARGS_0  pf_arg1  0
    POSTFIX_AND_ARGS_1  pf_arg23  N  0
    ADDED_TESTS_NAMES_OUT  ${EXEN}_TEST_NAMES
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_pf_arg1;COMMAND;${PACKEXEN_PATH};0;NAME;${PACKEXEN}_pf_arg23;COMMAND;${PACKEXEN_PATH};N;0"
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "${PACKEXEN}_pf_arg1;${PACKEXEN}_pf_arg23" )

  message("Add an executable with no prefix")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} NOEXEPREFIX ARGS arg1 )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${CMAKE_CURRENT_BINARY_DIR}/${EXEN}.exe;arg1"
    )

  message("Add an executable with no suffix")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} NOEXESUFFIX ARGS arg1 )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN};arg1"
    )

  message("Add an executable with no prefix and no suffix")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} NOEXEPREFIX NOEXESUFFIX ARGS arg1 )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${CMAKE_CURRENT_BINARY_DIR}/${EXEN};arg1"
    )

  message("Add a test with a different name from the executable")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} NAME SomeOtherName ARGS arg1 )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKAGE_NAME}_SomeOtherName;COMMAND;${PACKEXEN_PATH};arg1"
    )

  message("Add a test with with a postfix appended to the executable name")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} NAME_POSTFIX somePostfix ARGS arg1
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_somePostfix;COMMAND;${PACKEXEN_PATH};arg1"
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "${PACKEXEN}_somePostfix" )

  #
  # HOST, XHOST, HOSTTYPE, XHOSTTYPE
  #

  message("Test in HOST")
  set(${PROJECT_NAME}_HOSTNAME MyHost)
  tribits_add_test( ${EXEN} HOST MyHost ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (BASIC, PROCESSORS=1)!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH}"
    )
  unittest_compare_const( # Don't capture unless I want to
    TRIBITS_SET_TEST_PROPERTIES_INPUT
    ""
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "${PACKEXEN}" )

  message("Test not in HOST")
  set(${PROJECT_NAME}_HOSTNAME TheHost)
  tribits_add_test( ${EXEN} HOST MyHost ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because ${PROJECT_NAME}_HOSTNAME='TheHost' does not match list HOST='MyHost'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "" )

  message("Test in XHOST")
  set(${PROJECT_NAME}_HOSTNAME MyHost)
  tribits_add_test( ${EXEN} XHOST MyHost ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because ${PROJECT_NAME}_HOSTNAME='MyHost' matches list XHOST='MyHost'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "" )

  message("Test not in XHOST")
  set(${PROJECT_NAME}_HOSTNAME TheHost)
  tribits_add_test( ${EXEN} XHOST MyHost ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (BASIC, PROCESSORS=1)!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH}"
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "${PACKEXEN}" )

  message("Test in HOSTTYPE")
  set(CMAKE_HOST_SYSTEM_NAME MyHostType)
  tribits_add_test( ${EXEN} HOSTTYPE MyHostType ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (BASIC, PROCESSORS=1)!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH}"
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "${PACKEXEN}" )

  message("Test not in HOSTTYPE")
  set(CMAKE_HOST_SYSTEM_NAME TheHostType)
  tribits_add_test( ${EXEN} HOSTTYPE MyHostType ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because CMAKE_HOST_SYSTEM_NAME='TheHostType' does not match list HOSTTYPE='MyHostType'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "" )

  message("Test in XHOSTTYPE")
  set(CMAKE_HOST_SYSTEM_NAME MyHostType)
  tribits_add_test( ${EXEN} XHOSTTYPE MyHostType ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because CMAKE_HOST_SYSTEM_NAME='MyHostType' matches list XHOSTTYPE='MyHostType'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "" )

  message("Test not in XHOSTTYPE")
  set(CMAKE_HOST_SYSTEM_NAME TheHostType)
  tribits_add_test( ${EXEN} XHOSTTYPE MyHostType ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (BASIC, PROCESSORS=1)!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH}"
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "${PACKEXEN}" )

  #
  # EXCLUDE_IF_NOT_TRUE
  #

  message("EXCLUDE_IF_NOT_TRUE <true>")
  set(VAR_THAT_IS_TRUE TRUE)
  tribits_add_test( ${EXEN} EXCLUDE_IF_NOT_TRUE  VAR_THAT_IS_TRUE
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (BASIC, PROCESSORS=1)!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH}"
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "${PACKEXEN}" )

  message("EXCLUDE_IF_NOT_TRUE <true> <true>")
  set(VAR_THAT_IS_TRUE1 TRUE)
  set(VAR_THAT_IS_TRUE2 TRUE)
  tribits_add_test( ${EXEN} EXCLUDE_IF_NOT_TRUE  VAR_THAT_IS_TRUE1 VAR_THAT_IS_TRUE2
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (BASIC, PROCESSORS=1)!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH}"
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "${PACKEXEN}" )

  message("EXCLUDE_IF_NOT_TRUE <false>")
  set(VAR_THAT_IS_FALSE FALSE)
  tribits_add_test( ${EXEN} EXCLUDE_IF_NOT_TRUE  VAR_THAT_IS_FALSE
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because EXCLUDE_IF_NOT_TRUE VAR_THAT_IS_FALSE='FALSE'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "" )

  message("EXCLUDE_IF_NOT_TRUE <false> <true>")
  set(VAR_THAT_IS_TRUE TRUE)
  set(VAR_THAT_IS_FALSE FALSE)
  tribits_add_test( ${EXEN} EXCLUDE_IF_NOT_TRUE  VAR_THAT_IS_FALSE  VAR_THAT_IS_TRUE
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because EXCLUDE_IF_NOT_TRUE VAR_THAT_IS_FALSE='FALSE'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "" )

  message("EXCLUDE_IF_NOT_TRUE <true> <false>")
  set(VAR_THAT_IS_TRUE TRUE)
  set(VAR_THAT_IS_FALSE FALSE)
  tribits_add_test( ${EXEN} EXCLUDE_IF_NOT_TRUE  VAR_THAT_IS_TRUE  VAR_THAT_IS_FALSE
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because EXCLUDE_IF_NOT_TRUE VAR_THAT_IS_FALSE='FALSE'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "" )

  #
  # DISABLED, DISABLED_AND_MSG
  #

  message("DISABLED <msg> (trace add test)")
  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT ON) 
  tribits_add_test( ${EXEN}
    DISABLED "Disabled because of B and C"
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const( ${EXEN}_TEST_NAMES
    "PackageA_SomeExec" )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH}"
    )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- PackageA_SomeExec: Added test (BASIC, PROCESSORS=1, DISABLED)!;--  => Reason DISABLED: Disabled because of B and C" )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "DISABLED;ON")

  message("DISABLED <msg> (no trace add test)")
  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT ON) 
  set(${PROJECT_NAME}_TRACE_ADD_TEST OFF)
  tribits_add_test( ${EXEN}
    DISABLED "Disabled because of C and D"
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const( ${EXEN}_TEST_NAMES
    "PackageA_SomeExec" )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH}"
    )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT "" )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "DISABLED;ON")
  set(${PROJECT_NAME}_TRACE_ADD_TEST ON)

  message("DISABLED FALSE (trace add test)")
  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT ON)
  tribits_add_test( ${EXEN}
    DISABLED FALSE
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const( ${EXEN}_TEST_NAMES
    "PackageA_SomeExec" )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- PackageA_SomeExec: Added test (BASIC, PROCESSORS=1)!" )
  unittest_not_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "DISABLED;ON")

  message("DISABLED no (trace add test)")
  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT ON)
  tribits_add_test( ${EXEN}
    DISABLED no
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const( ${EXEN}_TEST_NAMES
    "PackageA_SomeExec" )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- PackageA_SomeExec: Added test (BASIC, PROCESSORS=1)!" )
  unittest_not_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "DISABLED;ON")

  message("<fullTestName>_SET_DISABLED_AND_MSG=<msg>")
  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT ON)
  set(${PACKEXEN}_SET_DISABLED_AND_MSG "Disabled because of D and E")
  tribits_add_test( ${EXEN}
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const( ${EXEN}_TEST_NAMES
    "PackageA_SomeExec" )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH}"
    )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- PackageA_SomeExec: Added test (BASIC, PROCESSORS=1, DISABLED)!;--  => Reason DISABLED: Disabled because of D and E" )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "DISABLED;ON")

  message("<fullTestName>_SET_DISABLED_AND_MSG=<msg> with NAME_POSTFIX and POSTFIX_AND_ARGS_<IDX>")
  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT ON)
  set(THIS_TEST_NAME ${PACKEXEN}_mypostfix_argpostfix0)
  set(${THIS_TEST_NAME}_SET_DISABLED_AND_MSG "Disabled because of F and G")
  tribits_add_test( ${EXEN} NAME_POSTFIX mypostfix POSTFIX_AND_ARGS_0 argpostfix0 arg0
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const( ${EXEN}_TEST_NAMES
    "${THIS_TEST_NAME}" )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${THIS_TEST_NAME};COMMAND;${PACKEXEN_PATH};arg0"
    )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- ${THIS_TEST_NAME}: Added test (BASIC, PROCESSORS=1, DISABLED)!;--  => Reason DISABLED: Disabled because of F and G")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "DISABLED;ON")

  message("DISABLED <msg> input arg and <fullTestName>_SET_DISABLED_AND_MSG=false")
  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT ON)
  set(${PACKEXEN}_SET_DISABLED_AND_MSG false) # Ovesrrides DISABLED option to tat()!
  tribits_add_test( ${EXEN}
    DISABLED "Disabled by default (but this will be re-enabled by var)"
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const( ${EXEN}_TEST_NAMES
    "PackageA_SomeExec" )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH}"
    )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- PackageA_SomeExec: Added test (BASIC, PROCESSORS=1)!" )
  unittest_not_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "DISABLED")  # Make sure DISABLED prop not even set!

  message("<fullTestName>_SET_DISABLED_AND_MSG with NAME_POSTFIX and POSTFIX_AND_ARGS_<IDX>")
  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT ON)
  set(THIS_TEST_NAME ${PACKEXEN}_mypostfix_argpostfix0)
  set(${THIS_TEST_NAME}_SET_DISABLED_AND_MSG "Disabled because of H and I")
  tribits_add_test( ${EXEN} NAME_POSTFIX mypostfix POSTFIX_AND_ARGS_0 argpostfix0 arg0
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const( ${EXEN}_TEST_NAMES
    "${THIS_TEST_NAME}" )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${THIS_TEST_NAME};COMMAND;${PACKEXEN_PATH};arg0"
    )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- ${THIS_TEST_NAME}: Added test (BASIC, PROCESSORS=1, DISABLED)!;--  => Reason DISABLED: Disabled because of H and I" )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "DISABLED;ON")

  #
  # RUN_SERIAL
  #

  message("<fullTestName>_SET_RUN_SERIAL=ON")
  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT ON)
  set(${PACKEXEN}_SET_RUN_SERIAL ON)
  tribits_add_test( ${EXEN}
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const( ${EXEN}_TEST_NAMES
    "PackageA_SomeExec" )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH}"
    )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- PackageA_SomeExec: Added test (BASIC, PROCESSORS=1, RUN_SERIAL)!" )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "RUN_SERIAL;ON")

  message("Set RUN_SERIAL input option")
  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT ON)
  set(${PACKEXEN}_SET_RUN_SERIAL "")
  tribits_add_test( ${EXEN} RUN_SERIAL
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const( ${EXEN}_TEST_NAMES
    "PackageA_SomeExec" )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH}"
    )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- PackageA_SomeExec: Added test (BASIC, PROCESSORS=1, RUN_SERIAL)!" )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "RUN_SERIAL;ON")

  message("Set RUN_SERIAL input option but set <fullTestName>_SET_RUN_SERIAL=OFF")
  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT ON)
  set(${PACKEXEN}_SET_RUN_SERIAL OFF)  # Overrides the input option RUN_SERIAL!
  tribits_add_test( ${EXEN} RUN_SERIAL
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const( ${EXEN}_TEST_NAMES
    "PackageA_SomeExec" )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH}"
    )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- PackageA_SomeExec: Added test (BASIC, PROCESSORS=1)!" )
  unittest_not_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "RUN_SERIAL") # Make sure that RUN_SERIAL prop not even set!

  #
  # DIRECTORY
  #

  message("Add a test with the relative directory overridden")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} DIRECTORY "../somedir" ARGS arg1 )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${CMAKE_CURRENT_BINARY_DIR}/../somedir/${PACKEXEN}.exe;arg1"
    )

  message("Add a test with the absolute directory overridden")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} DIRECTORY "/some/abs/path" ARGS arg1 )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;/some/abs/path/${PACKEXEN}.exe;arg1"
    )


  #
  # ADD_DIR_TO_NAME
  #

  message("Add a test with ADD_DIR_TO_NAME")
  set(PACKAGE_SOURCE_DIR "/base/project/package/mypackage")
  set(CMAKE_CURRENT_SOURCE_DIR "/base/project/package/mypackage/tests/unit-tests/test1")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ADD_DIR_TO_NAME ARGS arg1 )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKAGE_NAME}_tests_unit-tests_test1_${EXEN};COMMAND;${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_tests_unit-tests_test1_${EXEN}.exe;arg1"
    )
  unset(PACKAGE_SOURCE_DIR)
  unset(CMAKE_CURRENT_SOURCE_DIR)

  #
  # <fullTestName>_EXTRA_ARGS
  #

  message("Test <TestName>_EXTRA_ARGS with on ARGS")
  set(${PACKEXEN}_EXTRA_ARGS "--extra_arg1;something;-extra_arg2;-R")
  tribits_add_test( ${EXEN} )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH};--extra_arg1;something;-extra_arg2;-R"
    )

  message("Add arbitrary command-line arguments in <TestName>_EXTRA_ARGS with one ARG")
  set(${PACKEXEN}_EXTRA_ARGS "--extra_arg1;something")
  tribits_add_test( ${EXEN} ARGS "orig_arg orig_val" )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${PACKEXEN_PATH};orig_arg;orig_val;--extra_arg1;something"
    )

  message("Test <TestName>_EXTRA_ARGS with two ARGS")
  set(${PACKEXEN}_0_EXTRA_ARGS "--extra_arg1;something1")
  set(${PACKEXEN}_1_EXTRA_ARGS "--extra_arg2;something2")
  tribits_add_test( ${EXEN} ARGS "--orig_arg1 orig_val1" "--orig_arg2" )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_0;COMMAND;${PACKEXEN_PATH};--orig_arg1;orig_val1;--extra_arg1;something1;NAME;${PACKEXEN}_1;COMMAND;${PACKEXEN_PATH};--orig_arg2;--extra_arg2;something2"
    )

  message("Test <TestName>_EXTRA_ARGS for POSTFIX_AND_ARGS_0 ")
  set(${PACKEXEN}_extra_postfix_EXTRA_ARGS "--extra_arg1;something")
  tribits_add_test( ${EXEN} POSTFIX_AND_ARGS_0 extra_postfix --orig_arg orig_val )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_extra_postfix;COMMAND;${PACKEXEN_PATH};--orig_arg;orig_val;--extra_arg1;something"
    )

  message("Test <TestName>_EXTRA_ARGS for POSTFIX_AND_ARGS_0 and POSTFIX_AND_ARGS_1 ")
  set(${PACKEXEN}_extra_postfix1_EXTRA_ARGS "--extra_arg1;something1")
  set(${PACKEXEN}_extra_postfix2_EXTRA_ARGS "--extra_arg2;something2")
  tribits_add_test( ${EXEN}
    POSTFIX_AND_ARGS_0 extra_postfix1 --orig_arg1 orig_val1
    POSTFIX_AND_ARGS_1 extra_postfix2 --orig_arg2 orig_val2 )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_extra_postfix1;COMMAND;${PACKEXEN_PATH};--orig_arg1;orig_val1;--extra_arg1;something1;NAME;${PACKEXEN}_extra_postfix2;COMMAND;${PACKEXEN_PATH};--orig_arg2;orig_val2;--extra_arg2;something2"
    )

endfunction()


function(unittest_tribits_add_test_disable)

  message("\n***")
  message("*** Testing test-by-test disable of tribits_add_test(...)")
  message("***\n")

  # Needed by tribits_add_test(...)
  set(PACKAGE_NAME PackageA)
  set(PARENT_PACKAGE_NAME PackageA)
  set(${PACKAGE_NAME}_ENABLE_TESTS ON)

  # Used locally
  set(EXEN SomeExec)
  set(PACKEXEN ${PACKAGE_NAME}_${EXEN})

  message("Check that tribits_add_test(...) adds test")
  tribits_add_test( ${EXEN} )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )

  message("Check that tribits_add_advanced_test(...) adds test (no tracing)")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( SomeCmnd
    TEST_0 CMND someCmnd
    ADDED_TEST_NAME_OUT  SomeCmnd_TEST_NAME
    )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT "")
  unittest_compare_const(SomeCmnd_TEST_NAME "${PACKAGE_NAME}_SomeCmnd")
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    "\"someCmnd\""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    1
    )

  message("Check that PackageA_SomeExec_DISABLE=ON disables tribits_add_test(...) (no tracing)")
  set(PackageA_SomeExec_DISABLE ON)
  tribits_add_test( ${EXEN} )
  set(PackageA_SomeExec_DISABLE OFF)
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )

  message("Check that PackageA_SomeCmnd_DISABLE=ON disables tribits_add_advanced_test(...) (no tracing)")
  set(PackageA_SomeCmnd_DISABLE ON)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( SomeCmnd
    TEST_0 CMND someCmnd
    ADDED_TEST_NAME_OUT  SomeCmnd_TEST_NAME
    )
  set(PackageA_SomeCmnd_DISABLE OFF)
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    ""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )
  unittest_compare_const(SomeCmnd_TEST_NAME "")

  # Turn on tracing
  set(${PROJECT_NAME}_TRACE_ADD_TEST ON)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)

  message("Check that tribits_add_advanced_test(...) adds test (with tracing)")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( SomeCmnd
    TEST_0 CMND someCmnd
    ADDED_TEST_NAME_OUT  SomeCmnd_TEST_NAME
    )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_SomeCmnd: Added test (BASIC, PROCESSORS=1)!")
  unittest_compare_const(SomeCmnd_TEST_NAME "${PACKAGE_NAME}_SomeCmnd")
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    "\"someCmnd\""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    1
    )

  message("Check that tribits_add_advanced_test(...) produces FATAL_ERROR with unparsed args")
  tribits_add_advanced_test_unittest_reset()
  set(${PROJECT_NAME}_CHECK_FOR_UNPARSED_ARGUMENTS CUSTOM_FAILURE_MODE)
  tribits_add_advanced_test( SomeCmnd
     "unparsedarg1" "unparsedarg2"
    TEST_0 "unparsedarg3" "unparsedarg4" CMND someCmnd ARGS "arg1<semicol>arg2"
    ADDED_TEST_NAME_OUT  SomeCmnd_TEST_NAME
    )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "CUSTOM_FAILURE_MODE;Arguments passed in unrecognized.  PARSE_UNPARSED_ARGUMENTS = 'unparsedarg1;unparsedarg2';CUSTOM_FAILURE_MODE;Arguments passed in unrecognized.  PARSE_UNPARSED_ARGUMENTS = 'unparsedarg3;unparsedarg4';-- PackageA_SomeCmnd: Added test (BASIC, PROCESSORS=1)!"
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    "\"someCmnd\" \"arg1<semicol>arg2\""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    1
    )

  message("Check that PackageA_SomeExec_DISABLE=ON disables tribits_add_test(...)")
  set(PackageA_SomeExec_DISABLE ON)
  tribits_add_test( ${EXEN} )
  set(PackageA_SomeExec_DISABLE OFF)
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because PackageA_SomeExec_DISABLE='ON'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )

  message("Check that PackageA_SomeCmnd_DISABLE=ON disables tribits_add_advanced_test(...)")
  set(PackageA_SomeCmnd_DISABLE ON)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( SomeCmnd
    TEST_0 CMND someCmnd
    ADDED_TEST_NAME_OUT  SomeCmnd_TEST_NAME
    )
  set(PackageA_SomeCmnd_DISABLE OFF)
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_SomeCmnd: NOT added test because PackageA_SomeCmnd_DISABLE='ON'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    ""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )
  unittest_compare_const(SomeCmnd_TEST_NAME "")

  # Test PackageA_SKIP_CTEST_ADD_TEST

  set(PackageA_SKIP_CTEST_ADD_TEST ON)

  message("Check that PackageA_SKIP_CTEST_ADD_TEST=ON disables tribits_add_test(...)")
  tribits_add_test(${EXEN})
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because PackageA_SKIP_CTEST_ADD_TEST='ON'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )

  message("Check that PackageA_SKIP_CTEST_ADD_TEST=ON disables tribits_add_advanced_test(...)")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( SomeCmnd
    TEST_0 CMND someCmnd
    ADDED_TEST_NAME_OUT  SomeCmnd_TEST_NAME
    )
  set(PackageA_SomeCmnd_DISABLE OFF)
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_SomeCmnd: NOT added test because PackageA_SKIP_CTEST_ADD_TEST='ON'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    ""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )
  unittest_compare_const(SomeCmnd_TEST_NAME "")

  unset(PackageA_SKIP_CTEST_ADD_TEST)

  # Test ParentPackage_SKIP_CTEST_ADD_TEST

  set(PARENT_PACKAGE_NAME ParentPackage)
  set(ParentPackage_SKIP_CTEST_ADD_TEST ON)

  message("Check that ParentPackage_SKIP_CTEST_ADD_TEST=ON disables tribits_add_test(...)")
  tribits_add_test(${EXEN})
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because ParentPackage_SKIP_CTEST_ADD_TEST='ON'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )

  message("Check that ParentPackage_SKIP_CTEST_ADD_TEST=ON disables tribits_add_advanced_test(...)")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( SomeCmnd
    TEST_0 CMND someCmnd
    ADDED_TEST_NAME_OUT  SomeCmnd_TEST_NAME
    )
  set(PackageA_SomeCmnd_DISABLE OFF)
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_SomeCmnd: NOT added test because ParentPackage_SKIP_CTEST_ADD_TEST='ON'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    ""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )
  unittest_compare_const(SomeCmnd_TEST_NAME "")

  set(PARENT_PACKAGE_NAME ${PACKAGE_NAME})
  unset(ParentPackage_SKIP_CTEST_ADD_TEST)

endfunction()


function(unittest_tribits_add_test_categories)

  message("\n***")
  message("*** Testing tribits_add_test( ... CATEGORIES ... )")
  message("***\n")

  # Needed by tribits_add_test(...)
  set(PACKAGE_NAME PackageA)
  set(${PACKAGE_NAME}_ENABLE_TESTS ON)

  set(${PROJECT_NAME}_TEST_CATEGORIES "")

  # Used locally
  set(EXEN SomeExec)
  set(PACKEXEN ${PACKAGE_NAME}_${EXEN})

  # Turn on tracing for the rest of the tests!
  set(${PROJECT_NAME}_TRACE_ADD_TEST ON)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)

  message("Test no category matching NIGHTLY category set by client")
  set(${PROJECT_NAME}_TEST_CATEGORIES NIGHTLY)
  tribits_add_test( ${EXEN} )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (BASIC, PROCESSORS=1)!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )

  message("Test no category matching BASIC category set by client")
  set(${PROJECT_NAME}_TEST_CATEGORIES BASIC)
  tribits_add_test( ${EXEN} )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (BASIC, PROCESSORS=1)!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )

  message("Test NIGHTLY category matching NIGHTLY category set by client")
  set(${PROJECT_NAME}_TEST_CATEGORIES NIGHTLY)
  tribits_add_test( ${EXEN} CATEGORIES NIGHTLY )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (NIGHTLY, PROCESSORS=1)!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )

  message("Test HEAVY category matching HEAVY category set by client")
  set(${PROJECT_NAME}_TEST_CATEGORIES HEAVY)
  tribits_add_test( ${EXEN} CATEGORIES HEAVY )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (HEAVY, PROCESSORS=1)!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )

  message("Test WEEKLY category matching HEAVY category set by client")
  set(${PROJECT_NAME}_TEST_CATEGORIES HEAVY)
  tribits_add_test( ${EXEN} CATEGORIES WEEKLY )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "WARNING;Warning: The test category 'WEEKLY' is deprecated; and is replaced with 'HEAVY'.  Please change to use 'HEAVY' instead.;-- PackageA_SomeExec: Added test (HEAVY, PROCESSORS=1)!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )

  message("Test NIGHTLY category *not* matching BASIC category set by client")
  set(${PROJECT_NAME}_TEST_CATEGORIES BASIC)
  tribits_add_test( ${EXEN} CATEGORIES NIGHTLY )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because ${PROJECT_NAME}_TEST_CATEGORIES='${${PROJECT_NAME}_TEST_CATEGORIES}' does not match this test's CATEGORIES='NIGHTLY'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )

  message("Test BASIC category matching BASIC category set by client")
  set(${PROJECT_NAME}_TEST_CATEGORIES BASIC)
  tribits_add_test( ${EXEN} CATEGORIES BASIC )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )

  message("Test no category *not* matching PERFORMANCE category set by client")
  set(${PROJECT_NAME}_TEST_CATEGORIES PERFORMANCE)
  tribits_add_test( ${EXEN} )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because ${PROJECT_NAME}_TEST_CATEGORIES='${${PROJECT_NAME}_TEST_CATEGORIES}' does not match this test's CATEGORIES='BASIC'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )

  message("Test PERFORMANCE category matching PERFORMANCE category set by client")
  set(${PROJECT_NAME}_TEST_CATEGORIES PERFORMANCE)
  tribits_add_test( ${EXEN} CATEGORIES PERFORMANCE )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (PERFORMANCE, PROCESSORS=1)!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )

  message("Test NIGHTLY, PERFORMANCE category matching PERFORMANCE category set by client")
  set(${PROJECT_NAME}_TEST_CATEGORIES PERFORMANCE)
  tribits_add_test( ${EXEN} CATEGORIES NIGHTLY PERFORMANCE )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (NIGHTLY, PERFORMANCE, PROCESSORS=1)!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )

  message("Test invalid BADCAT category not matching anything and resulting in error")
  set(${PROJECT_NAME}_TEST_CATEGORIES NIGHTLY)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)
  global_set(MESSAGE_WRAPPER_INPUT "")
  tribits_add_test( ${EXEN} CATEGORIES BADCAT )
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE FALSE)
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "SEND_ERROR;Error: The categories 'BADCAT' are not; in the list of valid categories '${${PROJECT_NAME}_VALID_CATEGORIES_STR}'!;-- PackageA_SomeExec: NOT added test because ${PROJECT_NAME}_TEST_CATEGORIES='NIGHTLY' does not match this test's CATEGORIES='BADCAT'!")
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "")

endfunction()


function(unittest_tribits_add_test_comm)

  message("\n***")
  message("*** Testing tribits_add_test( ... COMM ... )")
  message("***\n")

  # Needed by tribits_add_test(...)
  set(PACKAGE_NAME PackageB)
  set(${PACKAGE_NAME}_ENABLE_TESTS ON)

  #
  # A) Doing default serial mode
  #

  # In serial mode, these vars are not even set!
  unset(MPI_EXEC_MAX_NUMPROCS)
  unset(MPI_EXEC_DEFAULT_NUMPROCS)

  # Used locally
  set(EXEN SomeExec)
  set(PACKEXEN ${PACKAGE_NAME}_${EXEN})

  # Turn on tracing for the rest of the tests!
  set(${PROJECT_NAME}_TRACE_ADD_TEST ON)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)

  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT ON)

  message("Add a test for serial with no COMM input")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (BASIC, PROCESSORS=1)!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )

  message("Try to add a serial test but with MPI_EXEC_DEFAULT_NUMPROCS=3 > 1")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  set(MPI_EXEC_DEFAULT_NUMPROCS 3)
  tribits_add_test( ${EXEN} )
  unset(MPI_EXEC_DEFAULT_NUMPROCS)
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (BASIC, PROCESSORS=1)!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )

  message("Do not add serial with COMM mpi")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} COMM mpi )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because TPL_ENABLE_MPI='' and COMM='mpi'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )

  message("Add a serial test with NUM_MPI_PROCS=2 > 1")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} COMM serial mpi NUM_MPI_PROCS 2 )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because NUM_MPI_PROCS='2' > MPI_EXEC_MAX_NUMPROCS='1'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )

  # Add serial tests where MPI_EXEC_MAX_NUMPROCS  > 1 is sets by the user!
  set(MPI_EXEC_MAX_NUMPROCS 5)

  message("Add a serial test with NUM_MPI_PROCS=2 > 1 and MPI_EXEC_MAX_NUMPROCS=5 set by user!")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} COMM serial mpi NUM_MPI_PROCS 2 )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because NUM_MPI_PROCS='2' > MPI_EXEC_MAX_NUMPROCS='1'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )

  message("Add a serial test with NUM_TOTAL_CORES_USED > MPI_EXEC_MAX_NUMPROCS")
   global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} NUM_TOTAL_CORES_USED 6 )
   unittest_compare_const(
     MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because NUM_TOTAL_CORES_USED='6' > MPI_EXEC_MAX_NUMPROCS='5'!"
     )
   unittest_compare_const(
     TRIBITS_ADD_TEST_ADD_TEST_INPUT
     ""
     )

  #
  # B) Doing MPI mode
  #

  set(TPL_ENABLE_MPI ON)
  set(MPI_EXEC_MAX_NUMPROCS 5)
  set(MPI_EXEC_DEFAULT_NUMPROCS 3)
  set(MPI_EXEC mpiexec)
  set(MPI_EXEC_PRE_NUMPROCS_FLAGS "--pre-num-procs-flags1;--pre-num-procs-flags2")
  set(MPI_EXEC_NUMPROCS_FLAG --num-procs)
  set(MPI_EXEC_POST_NUMPROCS_FLAGS "--post-num-procs-flags1;--post-num-procs-flags2")

  message("Add a test for MPI with no COMM input")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}_MPI_${MPI_EXEC_DEFAULT_NUMPROCS}: Added test (BASIC, NUM_MPI_PROCS=3, PROCESSORS=3)!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_MPI_${MPI_EXEC_DEFAULT_NUMPROCS};COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};${MPI_EXEC_DEFAULT_NUMPROCS};${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )

  message("Add a test for MPI with no COMM input but with some args")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ARGS "arg1 arg2" )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}_MPI_${MPI_EXEC_DEFAULT_NUMPROCS}: Added test (BASIC, NUM_MPI_PROCS=3, PROCESSORS=3)!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_MPI_${MPI_EXEC_DEFAULT_NUMPROCS};COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};${MPI_EXEC_DEFAULT_NUMPROCS};${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe;arg1;arg2"
    )

  message("Add a serial-only in an MPI-only build (adds no test)")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} COMM serial )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because TPL_ENABLE_MPI='ON' and COMM='serial'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )

  message("Add a test for MPI with 'COMM mpi'")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ARGS "arg1 arg2" COMM mpi )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}_MPI_${MPI_EXEC_DEFAULT_NUMPROCS}: Added test (BASIC, NUM_MPI_PROCS=3, PROCESSORS=3)!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_MPI_${MPI_EXEC_DEFAULT_NUMPROCS};COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};${MPI_EXEC_DEFAULT_NUMPROCS};${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe;arg1;arg2"
    )

  message("Add an MPI test with 2 procs")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ARGS "arg1 arg2" COMM mpi NUM_MPI_PROCS 2 )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_MPI_2;COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};2;${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe;arg1;arg2"
    )

  message("Add an MPI test with 4 procs (greater than default, less than max)")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ARGS "arg1 arg2" COMM mpi NUM_MPI_PROCS 4 )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_MPI_4;COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};4;${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe;arg1;arg2"
    )

  message("Add an MPI test with the exact number of max processes")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ARGS "arg1 arg2" COMM mpi NUM_MPI_PROCS 5 )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_MPI_5;COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};5;${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe;arg1;arg2"
    )

  message("Add an MPI test with one more than the number of allowed processors (will not be added)")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ARGS "arg1 arg2" COMM mpi NUM_MPI_PROCS 6 )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because NUM_MPI_PROCS='6' > MPI_EXEC_MAX_NUMPROCS='5'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )

  message("Add an MPI test with NUM_PROCS 1-10 (will be max num procs)")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ARGS "arg1 arg2" COMM mpi NUM_MPI_PROCS 1-10 )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_MPI_${MPI_EXEC_MAX_NUMPROCS};COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};${MPI_EXEC_MAX_NUMPROCS};${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe;arg1;arg2"
    )

  message("Add an MPI test with NUM_PROCS 3-10 (will be max num procs)")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ARGS "arg1 arg2" COMM mpi NUM_MPI_PROCS 3-10 )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_MPI_${MPI_EXEC_MAX_NUMPROCS};COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};${MPI_EXEC_MAX_NUMPROCS};${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe;arg1;arg2"
    )

  message("Add an MPI test with NUM_PROCS ${MPI_EXEC_MAX_NUMPROCS}-10 (will be max num procs)")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ARGS "arg1 arg2" COMM mpi NUM_MPI_PROCS ${MPI_EXEC_MAX_NUMPROCS}-10 )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_MPI_${MPI_EXEC_MAX_NUMPROCS};COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};${MPI_EXEC_MAX_NUMPROCS};${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe;arg1;arg2"
    )

  message("Add an MPI test where the default num processes is same as of max num processes")
  set(MPI_EXEC_DEFAULT_NUMPROCS 5)
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ARGS "arg1 arg2" COMM mpi)
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_MPI_5;COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};5;${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe;arg1;arg2"
    )

  message("Add an MPI test where the default num processes is one more than of max num processes (will not added test)")
  set(MPI_EXEC_DEFAULT_NUMPROCS 6)
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} ARGS "arg1 arg2" COMM mpi)
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because MPI_EXEC_DEFAULT_NUMPROCS='6' > MPI_EXEC_MAX_NUMPROCS='5'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )

  message("Add an MPI test with NUM_TOTAL_CORES_USED < MPI_EXEC_MAX_NUMPROCS")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} NUM_MPI_PROCS 1 NUM_TOTAL_CORES_USED 4 )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}_MPI_1: Added test (BASIC, NUM_MPI_PROCS=1, PROCESSORS=4)!"
    )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKEXEN}_MPI_1;PROPERTIES;PROCESSORS;4")
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_MPI_1;COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};1;${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )

  message("Add an MPI test with NUM_TOTAL_CORES_USED == MPI_EXEC_MAX_NUMPROCS")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} NUM_MPI_PROCS 1 NUM_TOTAL_CORES_USED 5 )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}_MPI_1: Added test (BASIC, NUM_MPI_PROCS=1, PROCESSORS=5)!"
    )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKEXEN}_MPI_1;PROPERTIES;PROCESSORS;5")
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_MPI_1;COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};1;${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )

  message("Add an MPI test with NUM_TOTAL_CORES_USED > MPI_EXEC_MAX_NUMPROCS")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} NUM_MPI_PROCS 1 NUM_TOTAL_CORES_USED 6 )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}_MPI_1: NOT added test because NUM_TOTAL_CORES_USED='6' > MPI_EXEC_MAX_NUMPROCS='5'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )

  message("Add a test with NUM_MPI_PROCS > NUM_TOTAL_CORES_USED")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} NUM_MPI_PROCS 3 NUM_TOTAL_CORES_USED 2 )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;ERROR: ${PACKEXEN}_MPI_3: NUM_MPI_PROCS='3' > NUM_TOTAL_CORES_USED='2' not allowed!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )

  set(MPI_EXEC_DEFAULT_NUMPROCS 3)

  #
  # Doing serial mode
  #

  set(TPL_ENABLE_MPI OFF)

  message("Add a test for serial mode with 'COMM serial'")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} COMM serial ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "${PACKEXEN}" )

  message("Add a test for serial mode with 'COMM mpi (adds no test)")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} COMM mpi ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: NOT added test because TPL_ENABLE_MPI='OFF' and COMM='mpi'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    ""
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES  "" )

  message("Add a test with MPI and NAME_POSTFIX")
  set(TPL_ENABLE_MPI ON)
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} NAME_POSTFIX mypostfix1
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_mypostfix1_MPI_${MPI_EXEC_DEFAULT_NUMPROCS};COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};${MPI_EXEC_DEFAULT_NUMPROCS};${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES
    "${PACKEXEN}_mypostfix1_MPI_${MPI_EXEC_DEFAULT_NUMPROCS}" )

  message("Add a test with MPI and NAME")
  set(TPL_ENABLE_MPI ON)
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} NAME ${EXEN}_mypostfix2
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_mypostfix2_MPI_${MPI_EXEC_DEFAULT_NUMPROCS};COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};${MPI_EXEC_DEFAULT_NUMPROCS};${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES
    "${PACKEXEN}_mypostfix2_MPI_${MPI_EXEC_DEFAULT_NUMPROCS}" )

  message("Add a test with MPI, two arguments, and NAME_POSTFIX")
  set(TPL_ENABLE_MPI ON)
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} NAME_POSTFIX mypostfix3 ARGS "arg1" "arg2"
    ADDED_TESTS_NAMES_OUT ${EXEN}_TEST_NAMES )
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_mypostfix3_0_MPI_${MPI_EXEC_DEFAULT_NUMPROCS};COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};${MPI_EXEC_DEFAULT_NUMPROCS};${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe;arg1;NAME;${PACKEXEN}_mypostfix3_1_MPI_${MPI_EXEC_DEFAULT_NUMPROCS};COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};${MPI_EXEC_DEFAULT_NUMPROCS};${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe;arg2"
    )
  unittest_compare_const( ${EXEN}_TEST_NAMES
    "${PACKEXEN}_mypostfix3_0_MPI_${MPI_EXEC_DEFAULT_NUMPROCS};${PACKEXEN}_mypostfix3_1_MPI_${MPI_EXEC_DEFAULT_NUMPROCS}" )

  #
  # RUN_SERIAL
  #

  message("<fullTestName>_SET_RUN_SERIAL=ON with MPI enabled")
  set(${PACKEXEN}_MPI_1_SET_RUN_SERIAL ON)
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} NUM_MPI_PROCS 1)
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}_MPI_1: Added test (BASIC, NUM_MPI_PROCS=1, PROCESSORS=1, RUN_SERIAL)!"
    )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKEXEN}_MPI_1;PROPERTIES;PROCESSORS;1")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "RUN_SERIAL;ON")
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_MPI_1;COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};1;${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )

  message("<fullTestName>_SET_RUN_SERIAL=ON with NAME_POSTFIX, POSTFIX_AND_ARGS_<IDX> and MPI enabled")
  set(THIS_TEST_NAME ${PACKEXEN}_mypostfix3_argpostfix0_MPI_1)
  set(${THIS_TEST_NAME}_SET_RUN_SERIAL ON)
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} NUM_MPI_PROCS 1
     NAME_POSTFIX mypostfix3 POSTFIX_AND_ARGS_0 argpostfix0 arg0)
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${THIS_TEST_NAME}: Added test (BASIC, NUM_MPI_PROCS=1, PROCESSORS=1, RUN_SERIAL)!"
    )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${THIS_TEST_NAME};PROPERTIES;PROCESSORS;1")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "RUN_SERIAL;ON")
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${THIS_TEST_NAME};COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};1;${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe;arg0"
    )

  #
  # DISABLED, DISABLED_AND_MSG
  #

  message("<fullTestName>_SET_DISABLED_AND_MSG with MPI enabled")
  set(THIS_TEST_NAME ${PACKEXEN}_MPI_1)
  set(${THIS_TEST_NAME}_SET_RUN_SERIAL "")  # Turn off for DISABLED tests
  set(${THIS_TEST_NAME}_SET_DISABLED_AND_MSG "Disabled because of DAM and MPI")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} NUM_MPI_PROCS 1)
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${THIS_TEST_NAME}: Added test (BASIC, NUM_MPI_PROCS=1, PROCESSORS=1, DISABLED)!;--  => Reason DISABLED: Disabled because of DAM and MPI"
    )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${THIS_TEST_NAME};PROPERTIES;PROCESSORS;1")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "DISABLED;ON")
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${THIS_TEST_NAME};COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};1;${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )

  message("<fullTestName>_SET_DISABLED_AND_MSG with NAME_POSTFIX, POSTFIX_AND_ARGS_<IDX> and MPI enabled")
  set(THIS_TEST_NAME ${PACKEXEN}_mypostfix4_argpostfix0_MPI_1)
  set(${THIS_TEST_NAME}_SET_DISABLED_AND_MSG "Disabled because of DAM, NP, PAA, and MPI")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} NUM_MPI_PROCS 1
     NAME_POSTFIX mypostfix4 POSTFIX_AND_ARGS_0 argpostfix0 arg0)
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${THIS_TEST_NAME}: Added test (BASIC, NUM_MPI_PROCS=1, PROCESSORS=1, DISABLED)!;--  => Reason DISABLED: Disabled because of DAM, NP, PAA, and MPI"
    )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${THIS_TEST_NAME};PROPERTIES;PROCESSORS;1")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "DISABLED;ON")
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${THIS_TEST_NAME};COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};1;${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe;arg0"
    )

endfunction()


function(unittest_tribits_add_test_properties)

  message("\n***")
  message("*** Testing the setting of test properties with tribits_add_test(...)")
  message("***\n")

  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT ON)

  # Needed by tribits_add_test(...)
  set(PACKAGE_NAME PackageA)
  set(${PACKAGE_NAME}_ENABLE_TESTS ON)

  # Used locally
  set(EXEN SomeExec)
  set(PACKEXEN ${PACKAGE_NAME}_${EXEN})

  # Turn on tracing for the rest of the tests!
  set(${PROJECT_NAME}_TRACE_ADD_TEST ON)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)

  message("Test setting of default properties")
  tribits_add_test(${EXEN})
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (BASIC, PROCESSORS=1)!" )
  unittest_compare_const(TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN};COMMAND;${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )
  unittest_compare_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_SomeExec;PROPERTIES;REQUIRED_FILES;${CMAKE_CURRENT_BINARY_DIR}/PackageA_SomeExec.exe;PackageA_SomeExec;PROPERTIES;PROCESSORS;1;PackageA_SomeExec;APPEND;PROPERTY;LABELS"
    )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_SomeExec;APPEND;PROPERTY;LABELS")

  message("Test setting PASS_REGULAR_EXPRESSION")
  tribits_add_test(${EXEN} PASS_REGULAR_EXPRESSION
     "[^a-zA-Z0-9_]My first pass Regex" "*/second pass regex")
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (BASIC, PROCESSORS=1)!" )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_SomeExec;PROPERTIES;PASS_REGULAR_EXPRESSION;[^a-zA-Z0-9_]My first pass Regex;*/second pass regex;")

  message("Test setting FAIL_REGULAR_EXPRESSION")
  tribits_add_test(${EXEN} FAIL_REGULAR_EXPRESSION
     "[^a-zA-Z0-9_]My first fail Regex" "*/second fail regex")
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (BASIC, PROCESSORS=1)!" )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_SomeExec;APPEND;PROPERTY;FAIL_REGULAR_EXPRESSION;[^a-zA-Z0-9_]My first fail Regex;*/second fail regex;")

  message("Test setting WILL_FAIL")
  tribits_add_test(${EXEN} WILL_FAIL FAIL_REGULAR_EXPRESSION
     "[^a-zA-Z0-9_]1ST fail Regex" "*/2nd fail regex")
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (BASIC, PROCESSORS=1)!" )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_SomeExec;PROPERTIES;WILL_FAIL;ON;")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_SomeExec;APPEND;PROPERTY;FAIL_REGULAR_EXPRESSION;[^a-zA-Z0-9_]1ST fail Regex;*/2nd fail regex;")

  message("Test setting integer TIMEOUT with no scaling (not even defined)")
  tribits_add_test(${EXEN} TIMEOUT 200)
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (BASIC, PROCESSORS=1, TIMEOUT=200)!" )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_SomeExec;PROPERTIES;TIMEOUT;200")

  message("Test setting non-integer TIMEOUT with no scaling (not even defined)")
  tribits_add_test(${EXEN} TIMEOUT 200.50)
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}: Added test (BASIC, PROCESSORS=1, TIMEOUT=200.50)!" )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_SomeExec;PROPERTIES;TIMEOUT;200.50")
  # NOTE: No truncation in TIMEOUT in this case!

  message("Test setting integer TIMEOUT with no scaling (default 1.0)")
  set(${PROJECT_NAME}_SCALE_TEST_TIMEOUT 1.0)
  tribits_add_test(${EXEN} TIMEOUT 200)
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_SomeExec;PROPERTIES;TIMEOUT;200")

  message("Test setting non-integer TIMEOUT with no scaling (default 1.0)")
  set(${PROJECT_NAME}_SCALE_TEST_TIMEOUT 1.0)
  tribits_add_test(${EXEN} TIMEOUT 200.50)
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_SomeExec;PROPERTIES;TIMEOUT;200.50")

  message("Test setting integer TIMEOUT with no scaling (non-default integral 1")
  set(${PROJECT_NAME}_SCALE_TEST_TIMEOUT 1)
  tribits_add_test(${EXEN} TIMEOUT 200)
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_SomeExec;PROPERTIES;TIMEOUT;200")
  set(${PROJECT_NAME}_SCALE_TEST_TIMEOUT 1.0)

  message("Test setting non-integer TIMEOUT with no scaling (non-default integral 1)")
  set(${PROJECT_NAME}_SCALE_TEST_TIMEOUT 1)
  tribits_add_test(${EXEN} TIMEOUT 200.50)
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_SomeExec;PROPERTIES;TIMEOUT;200")
  set(${PROJECT_NAME}_SCALE_TEST_TIMEOUT 1.0)
  # NOTE: TIMEOUT is truncated in this case!

  message("Test integral scaling 2 of non-integer TIMEOUT")
  set(${PROJECT_NAME}_SCALE_TEST_TIMEOUT 2)
  tribits_add_test(${EXEN} TIMEOUT 300.0)
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_SomeExec;PROPERTIES;TIMEOUT;600")
  set(${PROJECT_NAME}_SCALE_TEST_TIMEOUT 1.0)

  message("Test non-integral scaling 1.5 of non-integer TIMEOUT")
  set(${PROJECT_NAME}_SCALE_TEST_TIMEOUT 1.5)
  tribits_add_test(${EXEN} TIMEOUT 300.0)
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_SomeExec;PROPERTIES;TIMEOUT;450")
  set(${PROJECT_NAME}_SCALE_TEST_TIMEOUT 1.0)

  message("Test non-integral scaling 1.57 of non-integer TIMEOUT")
  set(${PROJECT_NAME}_SCALE_TEST_TIMEOUT 1.57)
  tribits_add_test(${EXEN} TIMEOUT 300.0)
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_SomeExec;PROPERTIES;TIMEOUT;450")
  # NOTE: 1.57 is truncated to 1.5 as part of the scaling algorithm
  set(${PROJECT_NAME}_SCALE_TEST_TIMEOUT 1.0)

  message("Test setting ENVIRONMENT")
  tribits_add_test(${EXEN} ENVIRONMENT var1=val1 var2=val2 )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_SomeExec;PROPERTY;ENVIRONMENT;var1=val1;var2=val2")

  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT OFF)

endfunction()


################################################################################
#
# Testing tribits_add_advanced_test()
#
################################################################################


function(unittest_tribits_add_advanced_test_basic)

  message("\n***")
  message("*** Testing basic functionality of tribits_add_advanced_test(...)")
  message("***\n")

  # Needed by tribits_add_advanced_test(...)
  set(PACKAGE_NAME PackageA)
  set(PARENT_PACKAGE_NAME PackageA)
  set(${PACKAGE_NAME}_ENABLE_TESTS ON)

  # Used locally
  set(EXEN SomeExec)
  set(PACKEXEN ${PACKAGE_NAME}_${EXEN}.exe)
  set(CMNDN ls)

  message("***\n*** Add a single basic command with no arguments (and check other parts)\n***")
  set(${PROJECT_NAME}_SHOW_TEST_START_END_DATE_TIME ON)
  set(${PROJECT_NAME}_SHOW_MACHINE_LOAD_IN_TEST OFF)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_basic_cmnd_1_args_0
    OVERALL_NUM_TOTAL_CORES_USED 4
    TIMEOUT 333.2
    TEST_0 CMND ${CMNDN}
      WORKING_DIRECTORY "someSubdir"
    ADDED_TEST_NAME_OUT  TAAT_basic_cmnd_1_args_0_TEST_NAME
    )
  unittest_compare_const(TAAT_basic_cmnd_1_args_0_TEST_NAME
    "${PACKAGE_NAME}_TAAT_basic_cmnd_1_args_0")
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    "\"${CMNDN}\""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    1
    )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_TAAT_basic_cmnd_1_args_0.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"ls\""
      "NUM_CMNDS 1"
      "set[(]SKIP_CLEAN_OVERALL_WORKING_DIRECTORY .FALSE.[)]"
      "set[(]SHOW_START_END_DATE_TIME ON[)]"
      "set[(]SHOW_MACHINE_LOAD OFF[)]"
      "set[(]CATEGORIES [)]"
      "set[(]PROCESSORS 4[)]"
      "set[(]TIMEOUT 333.2[)]"
      "set[(] TEST_0_WORKING_DIRECTORY .someSubdir. [)]"
      "set[(] TEST_0_SKIP_CLEAN_WORKING_DIRECTORY FALSE [)]"
    )

  message("***\n*** Add a single package executable with no arguments (and check other stuff)\n***")
  tribits_add_advanced_test_unittest_reset()
  set(${PROJECT_NAME}_SHOW_TEST_START_END_DATE_TIME OFF) # Above test was ON
  set(${PROJECT_NAME}_SHOW_MACHINE_LOAD_IN_TEST ON) # Above test was OFF
  tribits_add_advanced_test( TAAT_basic_exec_1_args_0
    TEST_0 EXEC ${EXEN}
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    "\"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    1
    )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_TAAT_basic_exec_1_args_0.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 1"
      "set[(]SHOW_START_END_DATE_TIME OFF[)]"
      "set[(]SHOW_MACHINE_LOAD ON[)]"
    )

  message("***\n*** Use a test name with '/' in the name\n***")
  tribits_add_advanced_test_unittest_reset()
  set(${PROJECT_NAME}_SHOW_TEST_START_END_DATE_TIME OFF) # Above test was ON
  set(${PROJECT_NAME}_SHOW_MACHINE_LOAD_IN_TEST ON) # Above test was OFF
  tribits_add_advanced_test( TAAT_basic/exec/1_args_0
    OVERALL_WORKING_DIRECTORY TEST_NAME
    TEST_0 EXEC ${EXEN}
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    "\"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    1
    )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_TAAT_basic__exec__1_args_0.cmake"
    REGEX_STRINGS
      "OVERALL_WORKING_DIRECTORY \"${PACKAGE_NAME}_TAAT_basic__exec__1_args_0\""
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 1"
      "set[(]SHOW_START_END_DATE_TIME OFF[)]"
      "set[(]SHOW_MACHINE_LOAD ON[)]"
    )

  message("***\n*** Add a single basic command with two arguments\n***")
  tribits_add_advanced_test_unittest_reset()
  set(${PROJECT_NAME}_SHOW_TEST_START_END_DATE_TIME OFF)
  tribits_add_advanced_test( TAAT_basic_cmnd_1_args_2
    OVERALL_WORKING_DIRECTORY  TEST_NAME
    SKIP_CLEAN_OVERALL_WORKING_DIRECTORY
    TEST_0 CMND ${CMNDN} ARGS CMakeLists.txt CMakeFiles
      WORKING_DIRECTORY "someSubdir"
      SKIP_CLEAN_WORKING_DIRECTORY
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    "\"${CMNDN}\" \"CMakeLists.txt\" \"CMakeFiles\""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    1
    )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_TAAT_basic_cmnd_1_args_2.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"ls\" \"CMakeLists.txt\" \"CMakeFiles\""
      "NUM_CMNDS 1"
      "set[(]OVERALL_WORKING_DIRECTORY .PackageA_TAAT_basic_cmnd_1_args_2.[)]"
      "set[(]SKIP_CLEAN_OVERALL_WORKING_DIRECTORY .TRUE.[)]"
      "CMAKE_MODULE_PATH"
      "set[(]SHOW_START_END_DATE_TIME OFF[)]"
      "set[(] TEST_0_WORKING_DIRECTORY .someSubdir. [)]"
      "set[(] TEST_0_SKIP_CLEAN_WORKING_DIRECTORY TRUE [)]"
      "DriveAdvancedTest"
      "drive_advanced_test"
    )

  message("***\n*** Add a single package executable with three arguments\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_basic_exec_1_args_3
    TEST_0 EXEC ${EXEN} ARGS arg1 arg2 arg3
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    "\"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\" \"arg1\" \"arg2\" \"arg3\""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    1
    )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_TAAT_basic_exec_1_args_3.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\" \"arg1\" \"arg2\" \"arg3\""
      "NUM_CMNDS 1"
    )

  message("***\n*** Add a single package executable with quoted arguments containing semi-colons\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_basic_exec_1_args_3_quotes_semicolons
    TEST_0 EXEC ${EXEN} ARGS "arg1=val1<semicolon>val2" arg2 arg3
    LIST_SEPARATOR "<semicolon>"
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    "\"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\" \"arg1=val1<semicolon>val2\" \"arg2\" \"arg3\""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    1
    )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_TAAT_basic_exec_1_args_3_quotes_semicolons.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\" \"arg1=val1<semicolon>val2\" \"arg2\" \"arg3\""
      "NUM_CMNDS 1"
    )

  message("***\n*** Add two basic commands with 1 and two arguments\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_basic_cmnd_2_args_1_2
    TEST_0 CMND echo ARGS "Cats and Dogs"
    TEST_1 CMND ls ARGS Cats
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    2
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    "\"echo\" \"Cats and Dogs\""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_1
    "\"ls\" \"Cats\""
    )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_TAAT_basic_cmnd_2_args_1_2.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"echo\" \"Cats and Dogs\""
      "TEST_1_CMND \"ls\" \"Cats\""
      "NUM_CMNDS 2"
    )

  message("***\n*** Add a single basic command matching HOST\n***")
  set(${PROJECT_NAME}_HOSTNAME MyHost)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_basic_host
    TEST_0 CMND ${CMNDN}
    HOST MyHost
    ADDED_TEST_NAME_OUT  TAAT_basic_host_TEST_NAME
    )
  unittest_compare_const(TAAT_basic_host_TEST_NAME
    "${PACKAGE_NAME}_TAAT_basic_host")
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    "\"${CMNDN}\""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    1
    )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_TAAT_basic_host.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"ls\""
      "NUM_CMNDS 1"
    )

  message("***\n*** Add a single basic command not matching HOST\n***")
  set(${PROJECT_NAME}_HOSTNAME MyHost)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_basic_host
    TEST_0 CMND ${CMNDN}
    HOST NotMyHost
    ADDED_TEST_NAME_OUT  TAAT_basic_host_TEST_NAME
    )
  unittest_compare_const(TAAT_basic_host_TEST_NAME "")
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    ""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )

  # ToDo: Add 6 more tests testing XHOST, HOSTTYPE, and XHOSTTYPE

  message("***\n*** Tests not enabled\n***")
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)
  set(${PROJECT_NAME}_TRACE_ADD_TEST ON)
  set(${PACKAGE_NAME}_ENABLE_TESTS OFF)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_tests_disabled
    TEST_0 CMND ${CMNDN}
    ADDED_TEST_NAME_OUT  TAAT_tests_disabled_TEST_NAME
    )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- PackageA_TAAT_tests_disabled: NOT added test because PackageA_ENABLE_TESTS='OFF'."
    )
  unittest_compare_const(TAAT_tests_disabled_TEST_NAME "")
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    ""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )

endfunction()


function(unittest_tribits_add_advanced_test_categories)

  message("\n***")
  message("*** Testing tribits_add_advanced_test( ... CATEGORIES ... )")
  message("***\n")

  # Needed by tribits_add_test(...)
  set(PACKAGE_NAME PackageA)
  set(${PACKAGE_NAME}_ENABLE_TESTS ON)

  set(${PROJECT_NAME}_TEST_CATEGORIES "")

  # Used locally
  set(EXEN SomeExec)
  set(PACKEXEN ${PACKAGE_NAME}_${EXEN}.exe)

  # Turn on tracing for the rest of the tests!
  set(${PROJECT_NAME}_TRACE_ADD_TEST ON)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)

  message("\n*** Test empty CATEGORIES matching the BASIC category\n")

  message("Test empty CATEGORIES matching CONTINUOUS category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_Empty_Nightly)
  set(${PROJECT_NAME}_TEST_CATEGORIES CONTINUOUS)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 1"
    )

  message("Test empty CATEGORIES matching NIGHTLY category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_Empty_Nightly)
  set(${PROJECT_NAME}_TEST_CATEGORIES NIGHTLY)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_${TEST_NAME}: Added test (BASIC, PROCESSORS=1)!"
    )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 1"
    )

  message("Test empty CATEGORIES matching HEAVY category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_Empty_Heavy)
  set(${PROJECT_NAME}_TEST_CATEGORIES HEAVY)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 1"
    )

  message("Test WEEKLY category becomes HEAVY")
  set(TEST_NAME PackageAddAdvancedTestCategory_Empty_Weekly)
  set(${PROJECT_NAME}_TEST_CATEGORIES HEAVY)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES WEEKLY )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 1"
      "set[(]CATEGORIES HEAVY."
    )

  message("Test empty CATEGORIES *not* matching PERFORMANCE category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_Empty_Performance)
  set(${PROJECT_NAME}_TEST_CATEGORIES PERFORMANCE)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_${TEST_NAME}: NOT added test because ${PROJECT_NAME}_TEST_CATEGORIES='${${PROJECT_NAME}_TEST_CATEGORIES}' does not match this test's CATEGORIES='BASIC'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    ""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )

  message("\n*** Test CATEGORIES BASIC\n")

  message("Test CATEGORIES BASIC matching BASIC category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_BASIC_BASIC)
  set(${PROJECT_NAME}_TEST_CATEGORIES BASIC)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES BASIC )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_${TEST_NAME}: Added test (BASIC, PROCESSORS=1)!"
    )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 1"
    )

  message("Test CATEGORIES BASIC matching CONTINUOUS category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_BASIC_CONTINUOUS)
  set(${PROJECT_NAME}_TEST_CATEGORIES CONTINUOUS)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES BASIC )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_${TEST_NAME}: Added test (BASIC, PROCESSORS=1)!"
    )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 1"
    )

  message("Test CATEGORIES BASIC matching NIGHTLY category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_BASIC_NIGHTLY)
  set(${PROJECT_NAME}_TEST_CATEGORIES NIGHTLY)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES BASIC )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_${TEST_NAME}: Added test (BASIC, PROCESSORS=1)!"
    )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 1"
    )

  message("Test CATEGORIES BASIC matching HEAVY category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_BASIC_HEAVY)
  set(${PROJECT_NAME}_TEST_CATEGORIES NIGHTLY)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES BASIC )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 1"
    )

  message("\n*** Test CATEGORIES CONTINUOUS\n")

  message("Test CATEGORIES CONTINUOUS *not* matching BASIC category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_CONTINUOUS_BASIC)
  set(${PROJECT_NAME}_TEST_CATEGORIES BASIC)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES CONTINUOUS )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_${TEST_NAME}: NOT added test because ${PROJECT_NAME}_TEST_CATEGORIES='BASIC' does not match this test's CATEGORIES='CONTINUOUS'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    ""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )

  message("Test CATEGORIES CONTINUOUS matching CONTINUOUS category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_CONTINUOUS_CONTINUOUS)
  set(${PROJECT_NAME}_TEST_CATEGORIES CONTINUOUS)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES CONTINUOUS )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 1"
    )

  message("Test CATEGORIES CONTINUOUS matching NIGHTLY category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_CONTINUOUS_NIGHTLY)
  set(${PROJECT_NAME}_TEST_CATEGORIES NIGHTLY)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES CONTINUOUS )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_${TEST_NAME}: Added test (CONTINUOUS, PROCESSORS=1)!"
    )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 1"
    )

  message("\n*** Test CATEGORIES NIGHTLY\n")

  message("Test CATEGORIES NIGHTLY *not* matching BASIC category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_NIGHTLY_BASIC)
  set(${PROJECT_NAME}_TEST_CATEGORIES BASIC)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES NIGHTLY )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_${TEST_NAME}: NOT added test because ${PROJECT_NAME}_TEST_CATEGORIES='BASIC' does not match this test's CATEGORIES='NIGHTLY'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    ""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )

  message("Test CATEGORIES NIGHTLY *not* matching CONTINUOUS category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_NIGHTLY_CONTINUOUS)
  set(${PROJECT_NAME}_TEST_CATEGORIES CONTINUOUS)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES NIGHTLY )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_${TEST_NAME}: NOT added test because ${PROJECT_NAME}_TEST_CATEGORIES='CONTINUOUS' does not match this test's CATEGORIES='NIGHTLY'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    ""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )

  message("Test CATEGORIES NIGHTLY matching NIGHTLY category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_NIGHTLY_NIGHTLY)
  set(${PROJECT_NAME}_TEST_CATEGORIES NIGHTLY)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES NIGHTLY )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 1"
    )

  message("Test CATEGORIES NIGHTLY matching HEAVY category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_NIGHTLY_HEAVY)
  set(${PROJECT_NAME}_TEST_CATEGORIES HEAVY)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES NIGHTLY )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 1"
    )

  message("\n*** Test CATEGORIES HEAVY\n")

  message("Test CATEGORIES HEAVY *not* matching BASIC category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_HEAVY_BASIC)
  set(${PROJECT_NAME}_TEST_CATEGORIES BASIC)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES HEAVY )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_${TEST_NAME}: NOT added test because ${PROJECT_NAME}_TEST_CATEGORIES='BASIC' does not match this test's CATEGORIES='HEAVY'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    ""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )

  message("Test CATEGORIES HEAVY *not* matching CONTINUOUS category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_HEAVY_CONTINUOUS)
  set(${PROJECT_NAME}_TEST_CATEGORIES CONTINUOUS)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES HEAVY )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_${TEST_NAME}: NOT added test because ${PROJECT_NAME}_TEST_CATEGORIES='CONTINUOUS' does not match this test's CATEGORIES='HEAVY'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    ""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )

  message("Test CATEGORIES HEAVY *not* matching NIGHTLY category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_HEAVY_NIGHTLY)
  set(${PROJECT_NAME}_TEST_CATEGORIES NIGHTLY)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES HEAVY )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_${TEST_NAME}: NOT added test because ${PROJECT_NAME}_TEST_CATEGORIES='NIGHTLY' does not match this test's CATEGORIES='HEAVY'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    ""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )

  message("Test CATEGORIES HEAVY matching HEAVY category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_HEAVY_HEAVY)
  set(${PROJECT_NAME}_TEST_CATEGORIES HEAVY)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES HEAVY )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 1"
    )

  message("\n*** Test CATEGORIES PERFORMANCE\n")

  message("Test CATEGORIES PERFORMANCE *not* matching BASIC category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_Empty_Performance)
  set(${PROJECT_NAME}_TEST_CATEGORIES BASIC)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES PERFORMANCE )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_${TEST_NAME}: NOT added test because ${PROJECT_NAME}_TEST_CATEGORIES='BASIC' does not match this test's CATEGORIES='PERFORMANCE'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    ""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )

  message("Test CATEGORIES PERFORMANCE *not* matching CONTINUOUS category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_Empty_Performance)
  set(${PROJECT_NAME}_TEST_CATEGORIES CONTINUOUS)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES PERFORMANCE )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_${TEST_NAME}: NOT added test because ${PROJECT_NAME}_TEST_CATEGORIES='CONTINUOUS' does not match this test's CATEGORIES='PERFORMANCE'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    ""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )

  message("Test CATEGORIES PERFORMANCE *not* matching NIGHTLY category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_Empty_Performance)
  set(${PROJECT_NAME}_TEST_CATEGORIES NIGHTLY)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES PERFORMANCE )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_${TEST_NAME}: NOT added test because ${PROJECT_NAME}_TEST_CATEGORIES='NIGHTLY' does not match this test's CATEGORIES='PERFORMANCE'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    ""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )

  message("Test PERFORMANCE category matching PERFORMANCE category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_Performance_Performance)
  set(${PROJECT_NAME}_TEST_CATEGORIES PERFORMANCE)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES PERFORMANCE )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_${TEST_NAME}: Added test (PERFORMANCE, PROCESSORS=1)!"
    )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 1"
    )

  message("Test PERFORMANCE category matching HEAVY, PERFORMANCE category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_Performance_Heavy_Performance)
  set(${PROJECT_NAME}_TEST_CATEGORIES PERFORMANCE)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES HEAVY PERFORMANCE )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_${TEST_NAME}: Added test (HEAVY, PERFORMANCE, PROCESSORS=1)!"
    )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 1"
    )

  message("Test HEAVY category matching CONTINUOUS, PERFORMANCE category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_Heavy_Continuous_Performance)
  set(${PROJECT_NAME}_TEST_CATEGORIES HEAVY)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES CONTINUOUS PERFORMANCE )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_${TEST_NAME}: Added test (CONTINUOUS, PERFORMANCE, PROCESSORS=1)!"
    )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 1"
    )

  message("Test CONTINUOUS category *not* matching HEAVY, PERFORMANCE category set by client")
  set(TEST_NAME PackageAddAdvancedTestCategory_Continuous_Heavy_Performance)
  set(${PROJECT_NAME}_TEST_CATEGORIES CONTINUOUS)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} CATEGORIES HEAVY PERFORMANCE )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_${TEST_NAME}: NOT added test because ${PROJECT_NAME}_TEST_CATEGORIES='CONTINUOUS' does not match this test's CATEGORIES='HEAVY;PERFORMANCE'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    ""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )

  # NOTE: The above tests ensure that the CATEGORIES argument is accepted and
  # processed correctly.  The unit tests in
  # unittest_tribits_add_test_categories() test the behavior of the logic for
  # selecting tests based on CATEGORIES.

endfunction()


function(unittest_tribits_add_advanced_test_comm)

  message("\n***")
  message("*** Testing tribits_add_advanced_test( ... COMM ... )")
  message("***\n")

  # Needed by tribits_add_advanced_test(...)
  set(PACKAGE_NAME PackageA)
  set(${PACKAGE_NAME}_ENABLE_TESTS ON)

  # Used locally
  set(EXEN SomeExec)
  set(PACKEXEN ${PACKAGE_NAME}_${EXEN})
  set(CMNDN ls)

  # Turn on tracing for the rest of the tests!
  set(${PROJECT_NAME}_TRACE_ADD_TEST ON)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)

  set(TRIBITS_ADD_ADVANCED_TEST_SKIP_SCRIPT TRUE)

  #
  # A) Default serial mode
  #

  set(TPL_ENABLE_MPI OFF)

  message("***\n*** Add a test with no COMM argument\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( DummyTest
    TEST_0 CMND ${CMNDN}
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    "\"${CMNDN}\""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    1
    )

  message("***\n*** Add a 'COMM serial' test with TPL_ENABLE_MPI=OFF\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( DummyTest
    TEST_0 CMND ${CMNDN}
    COMM serial
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    "\"${CMNDN}\""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    1
    )

  message("***\n*** Add a 'COMM serial' test with NUM_MPI_PROCS=2 (will not add test)\n***")
  tribits_add_advanced_test_unittest_reset()
  set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)
  tribits_add_advanced_test( DummyTest
    TEST_0 EXEC ${CMNDN} NUM_MPI_PROCS 2
    COMM serial
    )
  unset(${PROJECT_NAME}_VERBOSE_CONFIGURE)
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_DummyTest: NOT added test because NUM_MPI_PROCS='2' > MPI_EXEC_MAX_NUMPROCS='1'!" )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )

  #
  # B) Doing MPI mode
  #

  set(TPL_ENABLE_MPI ON)
  set(MPI_EXEC_MAX_NUMPROCS 5)
  set(MPI_EXEC_DEFAULT_NUMPROCS 3)
  set(MPI_EXEC mpiexec)
  set(MPI_EXEC_PRE_NUMPROCS_FLAGS "--pre-num-procs-flags1;--pre-num-procs-flags2")
  set(MPI_EXEC_NUMPROCS_FLAG --num-procs)
  set(MPI_EXEC_POST_NUMPROCS_FLAGS "--post-num-procs-flags1;--post-num-procs-flags1;")

  message("***\n*** Add serial-only test with TPL_ENABLE_MPI=ON (will not add the test)\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( DummyTest
    TEST_0 CMND ${CMNDN}
    COMM serial
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    ""
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )

  message("***\n*** Add an advanced test for MPI with no COMM input but with two args\n***")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_advanced_test( TAAT_mpi_exec_1_args_2
    TEST_0 EXEC ${EXEN} ARGS arg1 arg2 )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    1
    )
  tribits_join_exec_process_set_args(TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0_EXPECTED
    ${MPI_EXEC} ${MPI_EXEC_PRE_NUMPROCS_FLAGS} ${MPI_EXEC_NUMPROCS_FLAG}
    ${MPI_EXEC_DEFAULT_NUMPROCS} ${MPI_EXEC_POST_NUMPROCS_FLAGS}
    ${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe arg1 arg2 )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0
    ${TRIBITS_ADD_ADVANCED_TEST_CMND_ARRAY_0_EXPECTED} )

endfunction()


function(unittest_tribits_add_advanced_test_num_mpi_procs)

  message("\n***")
  message("*** Testing tribits_add_advanced_test( ... [OVERALL_]_NUM_MPI_PROCS ... )")
  message("***\n")

  # Needed by tribits_add_advanced_test(...)
  set(PACKAGE_NAME PackageA)
  set(${PACKAGE_NAME}_ENABLE_TESTS ON)

  # Used locally
  set(EXEN SomeExec)
  set(PACKEXEN ${PACKAGE_NAME}_${EXEN})
  set(MPI_EXEC_MAX_NUMPROCS 5)
  set(MPI_EXEC_DEFAULT_NUMPROCS 3)
  set(MPI_EXEC mpiexec)
  set(MPI_EXEC_PRE_NUMPROCS_FLAGS "--pre-num-procs-flags1;--pre-num-procs-flags2")
  set(MPI_EXEC_NUMPROCS_FLAG --num-procs)
  set(MPI_EXEC_POST_NUMPROCS_FLAGS "--post-num-procs-flags1;--post-num-procs-flags1;")
  set(CMNDN ls)

  set(TPL_ENABLE_MPI ON)

  # Turn on tracing for the rest of the tests!
  set(${PROJECT_NAME}_TRACE_ADD_TEST ON)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)
  set(TRIBITS_ADD_ADVANCED_TEST_SKIP_SCRIPT TRUE)
  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT ON)

  message("***\n*** CMND-only test and verify PROCESSORS=1\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_cmnd_1
    TEST_0 CMND someCmnd TEST_1 CMND someOtherCmnd )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_cmnd_0_cmnd_1: Added test (BASIC, PROCESSORS=1)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKAGE_NAME}_TAAT_mpi_cmnd_0_cmnd_1;PROPERTIES;PROCESSORS;1")

  message("***\n*** CMND-only test with OVERALL_NUM_MPI_PROCS=2 and verify PROCESSORS=2\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_cmnd_1
    OVERALL_NUM_MPI_PROCS 2 TEST_0 CMND someCmnd TEST_1 CMND someOtherCmnd )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_cmnd_0_cmnd_1: Added test (BASIC, PROCESSORS=2)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKAGE_NAME}_TAAT_mpi_cmnd_0_cmnd_1;PROPERTIES;PROCESSORS;2")

  message("***\n*** Mix of EXEC and CMND test cases and verify PROCESSORS\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_exec_1
    OVERALL_NUM_MPI_PROCS 2 # Set as the default PROCESSORS
    TEST_0 CMND someCmnd TEST_1 EXEC someExec NUM_MPI_PROCS 4 )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_cmnd_0_exec_1: Added test (BASIC, NUM_MPI_PROCS=4, PROCESSORS=4)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKAGE_NAME}_TAAT_mpi_cmnd_0_exec_1;PROPERTIES;PROCESSORS;4")

  message("***\n*** Two EXEC test cases (first num_procs larger) and verify PROCESSORS\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_exec_1
    OVERALL_NUM_MPI_PROCS 2 # Set as the default PROCESSORS
    TEST_0 EXEC someExec0 NUM_MPI_PROCS 4 TEST_1 EXEC someExec1 NUM_MPI_PROCS 3 )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_cmnd_0_exec_1: Added test (BASIC, NUM_MPI_PROCS=4, PROCESSORS=4)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKAGE_NAME}_TAAT_mpi_cmnd_0_exec_1;PROPERTIES;PROCESSORS;4")

  message("***\n*** Two EXEC test cases (second num_procs larger) and verify PROCESSORS\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_exec_1
    OVERALL_NUM_MPI_PROCS 2 # Set as the default PROCESSORS
    TEST_0 EXEC someExec0 NUM_MPI_PROCS 3 TEST_1 EXEC someExec1 NUM_MPI_PROCS 4 )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_cmnd_0_exec_1: Added test (BASIC, NUM_MPI_PROCS=4, PROCESSORS=4)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKAGE_NAME}_TAAT_mpi_cmnd_0_exec_1;PROPERTIES;PROCESSORS;4")

  message("***\n*** Two EXEC test cases (overall_num_procs larger) and verify PROCESSORS\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_exec_1
    OVERALL_NUM_MPI_PROCS 5 # Set as the default PROCESSORS
    TEST_0 EXEC someExec0 NUM_MPI_PROCS 3 TEST_1 EXEC someExec1 NUM_MPI_PROCS 4 )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_cmnd_0_exec_1: Added test (BASIC, NUM_MPI_PROCS=5, PROCESSORS=5)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKAGE_NAME}_TAAT_mpi_cmnd_0_exec_1;PROPERTIES;PROCESSORS;5")

  # ToDo: Add EXEC test where OVERALL_NUM_MPI_PROCS < MPI_EXEC_MAX_NUMPROCS

  # ToDo: Add EXEC test where NUM_MPI_PROCS < MPI_EXEC_MAX_NUMPROCS

  # ToDo: Add EXEC test where OVERALL_NUM_MPI_PROCS == MPI_EXEC_MAX_NUMPROCS

  # ToDo: Add EXEC test where NUM_MPI_PROCS == MPI_EXEC_MAX_NUMPROCS

  message("***\n*** Add EXEC test where OVERALL_NUM_MPI_PROCS > MPI_EXEC_MAX_NUMPROCS\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_exec_0_exec_1
    OVERALL_NUM_MPI_PROCS 6 TEST_0 EXEC someExec TEST_1 EXEC someOtherExec )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_exec_0_exec_1: NOT added test because OVERALL_NUM_MPI_PROCS='6' > MPI_EXEC_MAX_NUMPROCS='5'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )

  message("***\n*** Add EXEC test where NUM_MPI_PROCS > MPI_EXEC_MAX_NUMPROCS\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_exec_0_exec_1
    OVERALL_NUM_MPI_PROCS 2 TEST_0 EXEC someExec TEST_1 EXEC someOtherExec NUM_MPI_PROCS 7 )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_exec_0_exec_1: NOT added test because NUM_MPI_PROCS='7' > MPI_EXEC_MAX_NUMPROCS='5'!"
    )
  unittest_compare_const(
    TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS
    ""
    )

  message("***\n*** Add CMND test setting OVERALL_NUM_TOTAL_CORES_USED\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0
    OVERALL_NUM_TOTAL_CORES_USED 4 TEST_0 CMND someCmnd )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_cmnd_0: Added test (BASIC, PROCESSORS=4)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "1")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKAGE_NAME}_TAAT_mpi_cmnd_0;PROPERTIES;PROCESSORS;4")

  message("***\n*** Add CMND test setting OVERALL_NUM_TOTAL_CORES_USED and NUM_TOTAL_CORES_USED\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0
    OVERALL_NUM_TOTAL_CORES_USED 2 TEST_0 CMND someCmnd NUM_TOTAL_CORES_USED 3 )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_cmnd_0: Added test (BASIC, PROCESSORS=3)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "1")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKAGE_NAME}_TAAT_mpi_cmnd_0;PROPERTIES;PROCESSORS;3")

  message("***\n*** Add CMDN 2 test setting OVERALL_NUM_TOTAL_CORES_USED and NUM_TOTAL_CORES_USED \n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0
    OVERALL_NUM_TOTAL_CORES_USED 1
       TEST_0 CMND someCmnd1 NUM_TOTAL_CORES_USED 2
       TEST_1 CMND someCmnd2 NUM_TOTAL_CORES_USED 3 )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_cmnd_0: Added test (BASIC, PROCESSORS=3)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKAGE_NAME}_TAAT_mpi_cmnd_0;PROPERTIES;PROCESSORS;3")

  message("***\n*** Add CMDN 2 test setting OVERALL_NUM_TOTAL_CORES_USED and NUM_TOTAL_CORES_USED \n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0
    OVERALL_NUM_TOTAL_CORES_USED 1
       TEST_0 CMND someCmnd1 NUM_TOTAL_CORES_USED 3
       TEST_1 CMND someCmnd2 NUM_TOTAL_CORES_USED 2 )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_cmnd_0: Added test (BASIC, PROCESSORS=3)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKAGE_NAME}_TAAT_mpi_cmnd_0;PROPERTIES;PROCESSORS;3")

  message("***\n*** Add CMDN 2 test setting OVERALL_NUM_TOTAL_CORES_USED and NUM_TOTAL_CORES_USED \n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0
    OVERALL_NUM_TOTAL_CORES_USED 3
       TEST_0 CMND someCmnd1 NUM_TOTAL_CORES_USED 1
       TEST_1 CMND someCmnd2 NUM_TOTAL_CORES_USED 2 )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_cmnd_0: Added test (BASIC, PROCESSORS=2)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKAGE_NAME}_TAAT_mpi_cmnd_0;PROPERTIES;PROCESSORS;2")

  message("***\n*** Add EXEC test setting OVERALL_NUM_TOTAL_CORES_USED > NUM_MPI_PROCS\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_exec_0
    OVERALL_NUM_TOTAL_CORES_USED 4 TEST_0 EXEC someExec NUM_MPI_PROCS 2 )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_exec_0: Added test (BASIC, PROCESSORS=4)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "1")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKAGE_NAME}_TAAT_mpi_exec_0;PROPERTIES;PROCESSORS;4")

  message("***\n*** Add EXEC test setting OVERALL_NUM_TOTAL_CORES_USED < NUM_MPI_PROCS\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_exec_0
    OVERALL_NUM_TOTAL_CORES_USED 1 TEST_0 EXEC someExec NUM_MPI_PROCS 2 )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;ERROR: ${PACKAGE_NAME}_TAAT_mpi_exec_0: NUM_MPI_PROCS='2' > OVERALL_NUM_TOTAL_CORES_USED='1' not allowed!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "")

  message("***\n*** Add EXEC test setting OVERALL_NUM_TOTAL_CORES_USED and NUM_TOTAL_CORES_USED\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_exec_0
    OVERALL_NUM_TOTAL_CORES_USED 1
       TEST_0 EXEC someExec NUM_MPI_PROCS 2 NUM_TOTAL_CORES_USED 3 )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_exec_0: Added test (BASIC, PROCESSORS=3)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "1")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKAGE_NAME}_TAAT_mpi_exec_0;PROPERTIES;PROCESSORS;3")

  message("***\n*** Add EXEC test setting OVERALL_NUM_MPI_PROCS < NUM_TOTAL_CORES_USED\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_exec_0
    OVERALL_NUM_TOTAL_CORES_USED 4  OVERALL_NUM_MPI_PROCS 2
       TEST_0 EXEC someExec )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_exec_0: Added test (BASIC, NUM_MPI_PROCS=2, PROCESSORS=4)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "1")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKAGE_NAME}_TAAT_mpi_exec_0;PROPERTIES;PROCESSORS;4")

  message("***\n*** Add EXEC test setting OVERALL_NUM_MPI_PROCS > NUM_TOTAL_CORES_USED\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_exec_0
    OVERALL_NUM_TOTAL_CORES_USED 2  OVERALL_NUM_MPI_PROCS 3
       TEST_0 EXEC someExec )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;ERROR: ${PACKAGE_NAME}_TAAT_mpi_exec_0: OVERALL_NUM_MPI_PROCS='3' > OVERALL_NUM_TOTAL_CORES_USED='2' not allowed!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "")

  message("***\n*** Add CMND test setting OVERALL_NUM_MPI_PROCS < NUM_TOTAL_CORES_USED\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0
    OVERALL_NUM_TOTAL_CORES_USED 4  OVERALL_NUM_MPI_PROCS 2
       TEST_0 CMND someCmnd )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_cmnd_0: Added test (BASIC, PROCESSORS=4)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "1")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKAGE_NAME}_TAAT_mpi_cmnd_0;PROPERTIES;PROCESSORS;4")

  message("***\n*** Add CMDN test setting OVERALL_NUM_MPI_PROCS > OVERALL_NUM_TOTAL_CORES_USED\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0
    OVERALL_NUM_TOTAL_CORES_USED 2  OVERALL_NUM_MPI_PROCS 3
       TEST_0 CMND someCmnd )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;ERROR: ${PACKAGE_NAME}_TAAT_mpi_cmnd_0: OVERALL_NUM_MPI_PROCS='3' > OVERALL_NUM_TOTAL_CORES_USED='2' not allowed!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "")

  message("***\n*** Add EXEC test setting OVERALL_NUM_MPI_PROCS > NUM_TOTAL_CORES_USED\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_exec_0
    OVERALL_NUM_MPI_PROCS 3
       TEST_0 EXEC someExec NUM_TOTAL_CORES_USED 2 )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;ERROR: ${PACKAGE_NAME}_TAAT_mpi_exec_0: OVERALL_NUM_MPI_PROCS='3' > NUM_TOTAL_CORES_USED='2' not allowed!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "")

  message("***\n*** Add EXEC test setting NUM_MPI_PROCS > NUM_TOTAL_CORES_USED\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_exec_0
       TEST_0 EXEC someExec NUM_MPI_PROCS 3 NUM_TOTAL_CORES_USED 2 )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;ERROR: ${PACKAGE_NAME}_TAAT_mpi_exec_0: NUM_MPI_PROCS='3' > NUM_TOTAL_CORES_USED='2' not allowed!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "")

  message("***\n*** Add EXEC test setting NUM_MPI_PROCS > OVERALL_NUM_TOTAL_CORES_USED\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_exec_0
       OVERALL_NUM_TOTAL_CORES_USED 2
       TEST_0 EXEC someExec NUM_MPI_PROCS 3 )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;ERROR: ${PACKAGE_NAME}_TAAT_mpi_exec_0: NUM_MPI_PROCS='3' > OVERALL_NUM_TOTAL_CORES_USED='2' not allowed!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "")

endfunction()


function(unittest_tribits_add_advanced_test_directroy)

  message("\n***")
  message("*** Testing tribits_add_advanced_test( ... DIRECTORY ... )")
  message("***\n")


  # Needed by tribits_add_test(...)
  set(PACKAGE_NAME PackageA)
  set(${PACKAGE_NAME}_ENABLE_TESTS ON)

  set(TRIBITS_ADD_ADVANCED_TEST_SKIP_SCRIPT FALSE)

  # Used locally
  set(EXEN SomeExec)
  set(PACKEXEN ${PACKAGE_NAME}_${EXEN}.exe)

  message("\n*** Two tests with no DIRECTORY argument \n")
  set(TEST_NAME PackageAddAdvancedTestDirectory_None)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} TEST_1 EXEC ${EXEN} )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "TEST_1_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 2"
    )

  message("\n*** Two tests, first test with DIRECTORY argument \n")
  set(TEST_NAME PackageAddAdvancedTestDirectory_None)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} DIRECTORY ../dir1 TEST_1 EXEC ${EXEN} )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/../dir1/${PACKEXEN}\""
      "TEST_1_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "NUM_CMNDS 2"
    )

  message("\n*** Two tests, second test with relative DIRECTORY argument \n")
  set(TEST_NAME PackageAddAdvancedTestDirectory_None)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} TEST_1 EXEC ${EXEN} DIRECTORY ../dir2 )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "TEST_1_CMND \"${CMAKE_CURRENT_BINARY_DIR}/../dir2/${PACKEXEN}\""
      "NUM_CMNDS 2"
    )


  message("\n*** Two tests, second test with absolute DIRECTORY argument \n")
  set(TEST_NAME PackageAddAdvancedTestDirectory_None)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} TEST_1 EXEC ${EXEN} DIRECTORY /some/abs/path )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}\""
      "TEST_1_CMND \"/some/abs/path/${PACKEXEN}\""
      "NUM_CMNDS 2"
    )

  message("\n*** Two tests, both tests with DIRECTORY argument \n")
  set(TEST_NAME PackageAddAdvancedTestDirectory_None)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME} TEST_0 EXEC ${EXEN} DIRECTORY ../dir1 TEST_1 EXEC ${EXEN} DIRECTORY ../dir2 )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_CMND \"${CMAKE_CURRENT_BINARY_DIR}/../dir1/${PACKEXEN}\""
      "TEST_1_CMND \"${CMAKE_CURRENT_BINARY_DIR}/../dir2/${PACKEXEN}\""
      "NUM_CMNDS 2"
    )

endfunction()


function(unittest_tribits_add_advanced_test_properties)

  message("\n***")
  message("*** Testing the setting of test properties with tribits_add_advanced_test(...)")
  message("***\n")

  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT ON)
  set(TRIBITS_ADD_ADVANCED_TEST_SKIP_SCRIPT TRUE)

  # Needed by tribits_add_advanced_test(...)
  set(PACKAGE_NAME PackageA)
  set(${PACKAGE_NAME}_ENABLE_TESTS ON)

  # Used locally
  set(EXEN SomeExec)
  set(PACKEXEN ${PACKAGE_NAME}_${EXEN}.exe)
  set(CMNDN someCmnd)

  # Turn on tracing for the rest of the tests!
  set(${PROJECT_NAME}_TRACE_ADD_TEST ON)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)

  message("Test setting default properites")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_basic_cmnd_1_args_0
    TEST_0 CMND ${CMNDN}
    )
  unittest_compare_const(
    TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_TAAT_basic_cmnd_1_args_0;PROPERTY;REQUIRED_FILES;someCmnd;PackageA_TAAT_basic_cmnd_1_args_0;APPEND;PROPERTY;LABELS;PackageA_TAAT_basic_cmnd_1_args_0;PROPERTIES;PROCESSORS;1;PackageA_TAAT_basic_cmnd_1_args_0;PROPERTIES;PASS_REGULAR_EXPRESSION;OVERALL FINAL RESULT: TEST PASSED .PackageA_TAAT_basic_cmnd_1_args_0."
    )
  # NOTE: Above, in unit test mode, tribits_add_advanced_test() changes is
  # final pass expression so as to not match the outer run of
  # tribits_add_advanced_test() that looks for it.  Otherwise, the outer
  # tribits_add_advanced_test() always thinks these unit tests pass!

  message("Test setting FINAL_PASS_REGULAR_EXPRESSION")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_final_pass_regular_expression
    TEST_0 CMND ${CMNDN} FINAL_PASS_REGULAR_EXPRESSION
     "[^a-zA-Z0-9_]My first pass Regex" "*/second pass regex")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_TAAT_final_pass_regular_expression;PROPERTIES;PASS_REGULAR_EXPRESSION;[^a-zA-Z0-9_]My first pass Regex;*/second pass regex")

  message("Test setting FINAL_FAIL_REGULAR_EXPRESSION")
  tribits_add_advanced_test( TAAT_final_fail_regular_expression
    TEST_0 CMND ${CMNDN} FINAL_FAIL_REGULAR_EXPRESSION
     "[^a-zA-Z0-9_]My first fail Regex" "*/second fail regex")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_TAAT_final_fail_regular_expression;PROPERTIES;FAIL_REGULAR_EXPRESSION;[^a-zA-Z0-9_]My first fail Regex;*/second fail regex")

  message("Test setting non-integer TIMEOUT with no scaling (not even defined)")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_basic_cmnd_1_args_0
    TEST_0 CMND ${CMNDN} TIMEOUT 333.0 )
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- PackageA_TAAT_basic_cmnd_1_args_0: Added test (BASIC, PROCESSORS=1, TIMEOUT=333.0)!" )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_TAAT_basic_cmnd_1_args_0;PROPERTIES;TIMEOUT;333.0")

  message("Test non-integral scaling 1.5 of non-integer TIMEOUT")
  set(${PROJECT_NAME}_SCALE_TEST_TIMEOUT 1.5)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_basic_cmnd_1_args_0
    TEST_0 CMND ${CMNDN} TIMEOUT 200.0 )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_TAAT_basic_cmnd_1_args_0;PROPERTIES;TIMEOUT;300")
  set(${PROJECT_NAME}_SCALE_TEST_TIMEOUT 1.0)

  message("Test setting ENVIRONMENT")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_basic_cmnd_1_args_0
    TEST_0 CMND ${CMNDN} ENVIRONMENT var1=val1 var2=val2 )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageA_TAAT_basic_cmnd_1_args_0;PROPERTY;ENVIRONMENT;var1=val1;var2=val2")

  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT OFF)
  set(TRIBITS_ADD_ADVANCED_TEST_SKIP_SCRIPT OFF)

endfunction()


function(unittest_tribits_add_test_cuda_gpu_ctest_resources)

  message("\n***")
  message("*** Testing tribits_add_test() for CUDA GPU limiting")
  message("***\n")

  # Needed by tribits_add_test(...)
  set(PACKAGE_NAME PackageB)
  set(PARENT_PACKAGE_NAME PackageB)
  set(${PACKAGE_NAME}_ENABLE_TESTS ON)

  # Put into using testing mode
  set(${PROJECT_NAME}_TRACE_ADD_TEST ON)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)
  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT ON)

  # Setup for CUDA
  set(TPL_ENABLE_CUDA ON)
  tribits_add_test_helpers_init()

  # Used locally
  set(TPL_ENABLE_MPI ON)
  set(EXEN SomeExec)
  set(PACKEXEN ${PACKAGE_NAME}_${EXEN})
  set(MPI_EXEC_MAX_NUMPROCS 5)
  set(MPI_EXEC_DEFAULT_NUMPROCS 3)
  set(MPI_EXEC mpiexec)
  set(MPI_EXEC_PRE_NUMPROCS_FLAGS "--pre-num-procs-flags1;--pre-num-procs-flags2")
  set(MPI_EXEC_NUMPROCS_FLAG --num-procs)
  set(MPI_EXEC_POST_NUMPROCS_FLAGS "--post-num-procs-flags1;--post-num-procs-flags2")

  message("Call tribits_add_test( ... NUM_MPI_PROCS 4 ... ) with GPU limiting")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_test( ${EXEN} NUM_MPI_PROCS 4 )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- ${PACKEXEN}_MPI_4: Added test (BASIC, NUM_MPI_PROCS=4, PROCESSORS=4)!"
    )
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKEXEN}.exe;${PACKEXEN}_MPI_4;PROPERTIES;PROCESSORS;4")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKEXEN}_MPI_4;APPEND;PROPERTY;ENVIRONMENT;CTEST_KOKKOS_DEVICE_TYPE=gpus")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKEXEN}_MPI_4;PROPERTIES;RESOURCE_GROUPS;4,gpus:1")
  unittest_compare_const(
    TRIBITS_ADD_TEST_ADD_TEST_INPUT
    "NAME;${PACKEXEN}_MPI_4;COMMAND;${MPI_EXEC};${MPI_EXEC_PRE_NUMPROCS_FLAGS};${MPI_EXEC_NUMPROCS_FLAG};4;${MPI_EXEC_POST_NUMPROCS_FLAGS};${CMAKE_CURRENT_BINARY_DIR}/${PACKEXEN}.exe"
    )

  message("Call tribits_add_advanced_test( ... NUM_MPI_PROCS 3 ... ) with GPU limiting")
  global_set(TRIBITS_ADD_TEST_ADD_TEST_INPUT)
  tribits_add_advanced_test( TAAT_cmnd_1_args_0
     TEST_0 EXEC ${EXEN} NUM_MPI_PROCS 3 )
  unittest_compare_const(
    MESSAGE_WRAPPER_INPUT
    "-- PackageB_TAAT_cmnd_1_args_0: Added test (BASIC, PROCESSORS=3)!"
    )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "1")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageB_TAAT_cmnd_1_args_0;PROPERTIES;PROCESSORS;3")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "PackageB_TAAT_cmnd_1_args_0;APPEND;PROPERTY;ENVIRONMENT;CTEST_KOKKOS_DEVICE_TYPE=gpus")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "RESOURCE_GROUPS;3,gpus:1")

endfunction()


function(unittest_tribits_add_advanced_test_copy_files_to_test_dir)

  message("\n***")
  message("*** Testing COPY_FILES_TO_TEST_DIR with tribits_add_advanced_test(...)")
  message("***\n")

  set(TRIBITS_ADD_ADVANCED_TEST_SKIP_SCRIPT FALSE)

  # Needed by tribits_add_advanced_test(...)
  set(PACKAGE_NAME PackageA)
  set(${PACKAGE_NAME}_ENABLE_TESTS ON)

  # Used locally
  set(EXEN SomeExec)
  set(PACKEXEN ${PACKAGE_NAME}_${EXEN}.exe)
  set(CMNDN someCmnd)

  # Turn on tracing for the rest of the tests!
  set(${PROJECT_NAME}_TRACE_ADD_TEST ON)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)

  message("\n*** Test basic copy of a couple of files\n")
  set(TEST_NAME TAAT_COPY_FILES_TO_TEST_DIR_1_test_explicit_dirs)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME}
    TEST_0 COPY_FILES_TO_TEST_DIR file1 file2
      SOURCE_DIR /the/source/dir
      DEST_DIR /the/dest/dir
    )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_COPY_FILES_TO_TEST_DIR \"file1,file2\""
      "TEST_0_SOURCE_DIR \"/the/source/dir\""
      "TEST_0_DEST_DIR \"/the/dest/dir\""
      "NUM_CMNDS 1"
    )

  message("\n*** Test basic copy of files to dest dir under working dir with test name\n")
  set(TEST_NAME TAAT_COPY_FILES_TO_TEST_DIR_1_test_explicit_dirs)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME}
    OVERALL_WORKING_DIRECTORY TEST_NAME
    TEST_0 COPY_FILES_TO_TEST_DIR file1 file2
      SOURCE_DIR /the/source/dir
    )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_COPY_FILES_TO_TEST_DIR \"file1,file2\""
      "TEST_0_SOURCE_DIR \"/the/source/dir\""
      "TEST_0_DEST_DIR \"${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}\""
      "NUM_CMNDS 1"
    )

  message("\n*** Test basic copy of files to dest dir under special-named working dir\n")
  set(TEST_NAME TAAT_COPY_FILES_TO_TEST_DIR_1_test_explicit_dirs)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME}
    OVERALL_WORKING_DIRECTORY ${TEST_NAME}_other
    TEST_0 COPY_FILES_TO_TEST_DIR file1 file2
      SOURCE_DIR /the/source/dir
    )
  unittest_file_regex(
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${TEST_NAME}.cmake"
    REGEX_STRINGS
      "TEST_0_COPY_FILES_TO_TEST_DIR \"file1,file2\""
      "TEST_0_SOURCE_DIR \"/the/source/dir\""
      "TEST_0_DEST_DIR \"${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}_other\""
      "NUM_CMNDS 1"
    )

  message("\n*** Test using an empty COPY_FILES_TO_TEST_DIR values\n")
  set(TEST_NAME TAAT_COPY_FILES_TO_TEST_DIR_missing_files)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME}
    TEST_0 COPY_FILES_TO_TEST_DIR
      SOURCE_DIR /the/source/dir
      DEST_DIR /the/dest/dir
    )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;ERROR: COPY_FILES_TO_TEST_DIR must have at least one value!" )

  message("\n*** Test using two SOURCE_DIR values\n")
  set(TEST_NAME TAAT_COPY_FILES_TO_TEST_DIR_SORUCE_DIR_two_vals)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME}
    TEST_0 COPY_FILES_TO_TEST_DIR file1
      SOURCE_DIR /the/source/dir /another/source/dir
    )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;ERROR: SOURCE_DIR='/the/source/dir;/another/source/dir' can not have more than one value!" )

  message("\n*** Test using two DEST_DIR values\n")
  set(TEST_NAME TAAT_COPY_FILES_TO_TEST_DIR_SORUCE_DIR_two_vals)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( ${TEST_NAME}
    TEST_0 COPY_FILES_TO_TEST_DIR file1
      DEST_DIR /the/dest/dir /another/dest/dir
    )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;ERROR: DEST_DIR='/the/dest/dir;/another/dest/dir' can not have more than one value!" )

endfunction()


function(unittest_tribits_add_advanced_test_excludes)

  message("\n***")
  message("*** Testing excluding tribits_add_advanced_test(...) based on different criteria")
  message("***\n")

  # Needed by tribits_add_test(...)
  set(PACKAGE_NAME PackageA)
  set(${PACKAGE_NAME}_ENABLE_TESTS ON)

  # Turn on tracing for the rest of the tests!
  set(${PROJECT_NAME}_TRACE_ADD_TEST ON)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)
  set(TRIBITS_ADD_ADVANCED_TEST_SKIP_SCRIPT TRUE)
  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT ON)

  # Used locally
  set(EXEN SomeExec)
  set(PACKEXEN ${PACKAGE_NAME}_${EXEN}.exe)

  message("***\n*** EXCLUDE_IF_NOT_TRUE <true>\n***")
  set(VAR_THAT_IS_TRUE TRUE)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_cmnd_1
     EXCLUDE_IF_NOT_TRUE  VAR_THAT_IS_TRUE
    TEST_0 CMND someCmnd TEST_1 CMND someOtherCmnd )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_cmnd_0_cmnd_1: Added test (BASIC, PROCESSORS=1)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKAGE_NAME}_TAAT_mpi_cmnd_0_cmnd_1;PROPERTIES;PROCESSORS;1")

  #
  # EXCLUDE_IF_NOT_TRUE
  #

  message("***\n*** EXCLUDE_IF_NOT_TRUE <true> <true>\n***")
  set(VAR_THAT_IS_TRUE1 TRUE)
  set(VAR_THAT_IS_TRUE2 TRUE)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_cmnd_1
     EXCLUDE_IF_NOT_TRUE VAR_THAT_IS_TRUE1 VAR_THAT_IS_TRUE2
    TEST_0 CMND someCmnd TEST_1 CMND someOtherCmnd )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_cmnd_0_cmnd_1: Added test (BASIC, PROCESSORS=1)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "${PACKAGE_NAME}_TAAT_mpi_cmnd_0_cmnd_1;PROPERTIES;PROCESSORS;1")

  message("***\n*** EXCLUDE_IF_NOT_TRUE <false>\n***")
  set(VAR_THAT_IS_FALSE FALSE)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_cmnd_1
    EXCLUDE_IF_NOT_TRUE  VAR_THAT_IS_FALSE
    TEST_0 CMND someCmnd TEST_1 CMND someOtherCmnd )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_cmnd_0_cmnd_1: NOT added test because EXCLUDE_IF_NOT_TRUE VAR_THAT_IS_FALSE='FALSE'!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "")

  message("***\n*** EXCLUDE_IF_NOT_TRUE <true> <false>\n***")
  set(VAR_THAT_IS_TRUE TRUE)
  set(VAR_THAT_IS_FALSE FALSE)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_cmnd_1
    EXCLUDE_IF_NOT_TRUE  VAR_THAT_IS_TRUE  VAR_THAT_IS_FALSE
    TEST_0 CMND someCmnd TEST_1 CMND someOtherCmnd )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_cmnd_0_cmnd_1: NOT added test because EXCLUDE_IF_NOT_TRUE VAR_THAT_IS_FALSE='FALSE'!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "")

  message("***\n*** EXCLUDE_IF_NOT_TRUE <false> <true>\n***")
  set(VAR_THAT_IS_TRUE TRUE)
  set(VAR_THAT_IS_FALSE FALSE)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_cmnd_1
    EXCLUDE_IF_NOT_TRUE  VAR_THAT_IS_FALSE  VAR_THAT_IS_TRUE
    TEST_0 CMND someCmnd TEST_1 CMND someOtherCmnd )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- ${PACKAGE_NAME}_TAAT_mpi_cmnd_0_cmnd_1: NOT added test because EXCLUDE_IF_NOT_TRUE VAR_THAT_IS_FALSE='FALSE'!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "")

  #
  # DISABLED, DISABLED_AND_MSG
  #

  message("***\n*** DISABLED <msg> (trace add test on)\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_cmnd_1
    DISABLED "Disabled because of A and B"
    TEST_0 CMND someCmnd TEST_1 CMND someOtherCmnd )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- PackageA_TAAT_mpi_cmnd_0_cmnd_1: Added test (BASIC, PROCESSORS=1, DISABLED)!;--  => Reason DISABLED: Disabled because of A and B" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "DISABLED;ON")

  message("***\n*** DISABLED <msg> (trace add test off)\n***")
  set(${PROJECT_NAME}_TRACE_ADD_TEST OFF)
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_cmnd_1
    DISABLED "Disabled because of C and B"
    TEST_0 CMND someCmnd TEST_1 CMND someOtherCmnd )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT "")
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "DISABLED;ON")
  set(${PROJECT_NAME}_TRACE_ADD_TEST ON)

  message("***\n*** DISABLED FALSE\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_cmnd_1
    DISABLED FALSE
    TEST_0 CMND someCmnd TEST_1 CMND someOtherCmnd )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- PackageA_TAAT_mpi_cmnd_0_cmnd_1: Added test (BASIC, PROCESSORS=1)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_not_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "DISABLED;ON")

  message("***\n*** DISABLED no\n***")
  tribits_add_advanced_test_unittest_reset()
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_cmnd_1
    DISABLED no
    TEST_0 CMND someCmnd TEST_1 CMND someOtherCmnd )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- PackageA_TAAT_mpi_cmnd_0_cmnd_1: Added test (BASIC, PROCESSORS=1)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_not_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "DISABLED;ON")

  message("***\n*** <fullTestName>_SET_DISABLED_AND_MSG=<msg>\n***")
  tribits_add_advanced_test_unittest_reset()
  set(${PACKAGE_NAME}_TAAT_mpi_cmnd_0_cmnd_1_SET_DISABLED_AND_MSG
    "Disabled using cache var")
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_cmnd_1
    TEST_0 CMND someCmnd TEST_1 CMND someOtherCmnd )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- PackageA_TAAT_mpi_cmnd_0_cmnd_1: Added test (BASIC, PROCESSORS=1, DISABLED)!;--  => Reason DISABLED: Disabled using cache var" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "DISABLED;ON")

  message("***\n*** DISABLED <msg> then <fullTestName>_SET_DISABLED_AND_MSG=false\n***")
  tribits_add_advanced_test_unittest_reset()
  set(${PACKAGE_NAME}_TAAT_mpi_cmnd_0_cmnd_1_SET_DISABLED_AND_MSG false)
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_cmnd_1
    DISABLED "Disabled because of blah blash (but not really due to above var)"
    TEST_0 CMND someCmnd TEST_1 CMND someOtherCmnd )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- PackageA_TAAT_mpi_cmnd_0_cmnd_1: Added test (BASIC, PROCESSORS=1)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_not_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "DISABLED")

endfunction()


function(unittest_tribits_add_advanced_test_run_serial)

  message("\n***")
  message("*** Testing excluding tribits_add_advanced_test(...) with RUN_SERIAL")
  message("***\n")

  # Needed by tribits_add_test(...)
  set(PACKAGE_NAME PackageA)
  set(${PACKAGE_NAME}_ENABLE_TESTS ON)

  # Turn on tracing for the rest of the tests!
  set(${PROJECT_NAME}_TRACE_ADD_TEST ON)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)
  set(TRIBITS_ADD_ADVANCED_TEST_SKIP_SCRIPT TRUE)
  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT ON)

  # Used locally
  set(EXEN SomeExec)
  set(PACKEXEN ${PACKAGE_NAME}_${EXEN}.exe)

  message("***\n*** <fullTestName>_SET_RUN_SERIAL=ON\n***")
  tribits_add_advanced_test_unittest_reset()
  set(${PACKAGE_NAME}_TAAT_mpi_cmnd_0_cmnd_1_SET_RUN_SERIAL ON)
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_cmnd_1
    TEST_0 CMND someCmnd TEST_1 CMND someOtherCmnd )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- PackageA_TAAT_mpi_cmnd_0_cmnd_1: Added test (BASIC, PROCESSORS=1, RUN_SERIAL)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "RUN_SERIAL;ON")

  message("***\n*** Set RUN_SERIAL input option\n***")
  tribits_add_advanced_test_unittest_reset()
  set(${PACKAGE_NAME}_TAAT_mpi_cmnd_0_cmnd_1_SET_RUN_SERIAL "")
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_cmnd_1 RUN_SERIAL
    TEST_0 CMND someCmnd TEST_1 CMND someOtherCmnd )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- PackageA_TAAT_mpi_cmnd_0_cmnd_1: Added test (BASIC, PROCESSORS=1, RUN_SERIAL)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "RUN_SERIAL;ON")

  message("***\n*** Set RUN_SERIAL input option but set <fullTestName>_SET_RUN_SERIAL=OFF\n***")
  tribits_add_advanced_test_unittest_reset()
  set(${PACKAGE_NAME}_TAAT_mpi_cmnd_0_cmnd_1_SET_RUN_SERIAL OFF) # Overrides RUN_SERIAL!
  tribits_add_advanced_test( TAAT_mpi_cmnd_0_cmnd_1 RUN_SERIAL
    TEST_0 CMND someCmnd TEST_1 CMND someOtherCmnd )
  unittest_compare_const( MESSAGE_WRAPPER_INPUT
    "-- PackageA_TAAT_mpi_cmnd_0_cmnd_1: Added test (BASIC, PROCESSORS=1)!" )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")
  unittest_not_has_substr_const(TRIBITS_SET_TEST_PROPERTIES_INPUT
    "RUN_SERIAL")  # Make sure RUN_SERIAL prop not even set!

endfunction()


function(unittest_tribits_add_advanced_test_change_max_num_test_blocks)

  message("\n***")
  message("*** Testing tribits_add_advanced_test(...) changing TRIBITS_ADD_ADVANCED_TEST_MAX_NUM_TEST_BLOCKS")
  message("***\n")

  set(PACKAGE_NAME PackageA)
  set(${PACKAGE_NAME}_ENABLE_TESTS ON)

  # Turn on tracing for the rest of the tests!
  set(${PROJECT_NAME}_TRACE_ADD_TEST ON)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE TRUE)
  set(TRIBITS_ADD_ADVANCED_TEST_SKIP_SCRIPT TRUE)
  set(TRIBITS_SET_TEST_PROPERTIES_CAPTURE_INPUT ON)

  # Test changing TRIBITS_ADD_ADVANCED_TEST_MAX_NUM_TEST_BLOCKS
  set(TRIBITS_ADD_ADVANCED_TEST_MAX_NUM_TEST_BLOCKS 2)

  # Test with just two TEST_<idx> blocks that passes
  tribits_add_advanced_test( TAAT_test_blocks_2
    TEST_0 CMND someCmnd0
    TEST_1 CMND someCmnd1 )
  unittest_compare_const(TRIBITS_ADD_ADVANCED_TEST_NUM_CMNDS "2")

  # Test with three TEST_<idx> blocks that will fail
  tribits_add_advanced_test( TAAT_test_blocks_3
    TEST_0 CMND someCmnd0
    TEST_1 CMND someCmnd1
    TEST_2 CMND someCmnd2 )
  unittest_has_substr_const( MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;PackageA_TAAT_test_blocks_3: ERROR: Test block TEST_2 exceeds the max allowed test block TEST_1 as allowed by TRIBITS_ADD_ADVANCED_TEST_MAX_NUM_TEST_BLOCKS=2.  To fix this, call set(TRIBITS_ADD_ADVANCED_TEST_MAX_NUM_TEST_BLOCKS <larger-num>) before calling tribits_add_advanced_test()")

endfunction()


function(unittest_tribits_add_executable_and_test)

  set(TRIBITS_ADD_EXECUTABLE_AND_TEST_TEST_MODE ON)

  message("\n***")
  message("*** Test passing basic arguments to tribits_add_executable_and_test( ... )")
  message("***\n")

  set(PACKAGE_NAME PackageA)
  set(${PACKAGE_NAME}_ENABLE_TESTS ON)

  tribits_add_executable_and_test(
    execName
    ADDED_TESTS_NAMES_OUT execName_TEST_NAME
    ADDED_EXE_TARGET_NAME_OUT execName_TARGET_NAME
    LIST_SEPARATOR <semicolon>
    TIMEOUT 11.5
    WILL_FAIL
    ENVIRONMENT env1=envval1 env2=envval2
    FAIL_REGULAR_EXPRESSION "regex1;regex2"
    PASS_REGULAR_EXPRESSION "regex1;regex2"
    STANDARD_PASS_OUTPUT
    KEYWORDS keyword1 keyword2
    DEFINES -DSOMEDEFINE2
    TARGET_DEFINES -DSOMEDEFINE1
    ADD_DIR_TO_NAME
    LINKER_LANGUAGE C
    NUM_MPI_PROCS numProcs
    COMM serial mpi
    DIRECTORY dir
    TESTONLYLIBS tolib1 tolib2
    IMPORTEDLIBS ilib1 ilib2
    NOEXESUFFIX
    NOEXEPREFIX
    DISABLED "Disable this test because I said"
    EXCLUDE_IF_NOT_TRUE var1 var2
    XHOSTTYPE hosttype1 hosttype2
    HOSTTYPE hosttype1 hosttype2
    XHOST host1 host2
    DEPLIBS lib1 lib2  # Deprecated
    HOST host1 host2
    CATEGORIES category1 category2
    NAME_POSTFIX testNamePostfix
    NAME testName
    SOURCES src1 src2
    )
  unittest_compare_const(
    TRIBITS_ADD_EXECUTABLE_CAPTURE_ARGS
    "execName;COMM;serial;mpi;CATEGORIES;category1;category2;HOST;host1;host2;XHOST;host1;host2;HOSTTYPE;hosttype1;hosttype2;XHOSTTYPE;hosttype1;hosttype2;EXCLUDE_IF_NOT_TRUE;var1;var2;NOEXEPREFIX;NOEXESUFFIX;SOURCES;src1;src2;DEPLIBS;lib1;lib2;TESTONLYLIBS;tolib1;tolib2;IMPORTEDLIBS;ilib1;ilib2;DIRECTORY;dir;ADD_DIR_TO_NAME;LINKER_LANGUAGE;C;TARGET_DEFINES;-DSOMEDEFINE1;DEFINES;-DSOMEDEFINE2;ADDED_EXE_TARGET_NAME_OUT;ADDED_EXE_TARGET_NAME"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_CAPTURE_ARGS
    "execName;COMM;serial;mpi;CATEGORIES;category1;category2;HOST;host1;host2;XHOST;host1;host2;HOSTTYPE;hosttype1;hosttype2;XHOSTTYPE;hosttype1;hosttype2;EXCLUDE_IF_NOT_TRUE;var1;var2;NOEXEPREFIX;NOEXESUFFIX;NAME;testName;NAME_POSTFIX;testNamePostfix;DIRECTORY;dir;KEYWORDS;keyword1;keyword2;NUM_MPI_PROCS;numProcs;PASS_REGULAR_EXPRESSION;regex1;regex2;FAIL_REGULAR_EXPRESSION;regex1;regex2;ENVIRONMENT;env1=envval1;env2=envval2;DISABLED;Disable this test because I said;STANDARD_PASS_OUTPUT;WILL_FAIL;TIMEOUT;11.5;LIST_SEPARATOR;<semicolon>;ADD_DIR_TO_NAME;ADDED_TESTS_NAMES_OUT;ADDED_TESTS_NAMES_OUT;ADDED_TESTS_NAMES"
    )
  # NOTE: Above, we input the list in reverse order to prove that the
  # arguments are handled correctly internally.

  message("\n***")
  message("*** Test passing in XHOST_TEST and XHOSTTYPE_TEST into tribits_add_executable_and_test(...)")
  message("***\n")

  tribits_add_executable_and_test(
    execName
    SOURCES src1 src2
    XHOST_TEST host1 host2
    XHOSTTYPE_TEST hosttype1 hosttype2
    )
  unittest_compare_const(
    TRIBITS_ADD_EXECUTABLE_CAPTURE_ARGS
    "execName;SOURCES;src1;src2"
    )
  unittest_compare_const(
    TRIBITS_ADD_TEST_CAPTURE_ARGS
    "execName;XHOST;host1;host2;XHOSTTYPE;hosttype1;hosttype2"
    )

endfunction()


################################################################################
#
# Testing TriBITS ETI support code
#
################################################################################


function(unittest_tribits_eti_type_expansion)

  message("*** Test passing invalid arguments to tribits_eti_type_expansion( ... )\n")

  set(result "This is left over from other module!")  # See #199

  unset(expansion)
  tribits_eti_type_expansion(expansion "badformat")
  unittest_compare_const(
    expansion
    "TRIBITS_ETI_BAD_ARGUMENTS"
    )

  message("*** Test passing valid arguments to tribits_eti_type_expansion( ... )\n")

  # test rank-one
  unset(expansion)
  tribits_eti_type_expansion(expansion "f1=ta|tb")
  unittest_compare_const(
    expansion
    "f1={ta};f1={tb}"
    )

  # test accumulation into ${expansion}
  tribits_eti_type_expansion(expansion "f2=tc|td|te")
  unittest_compare_const(
    expansion
    "f1={ta};f1={tb};f2={tc};f2={td};f2={te}"
    )

  # test rank-two
  unset(expansion)
  tribits_eti_type_expansion(expansion "f1=ta|tb" "f2=tc")
  unittest_compare_const(
    expansion
    "f1={ta} f2={tc};f1={tb} f2={tc}"
    )

  # test rank-three
  unset(expansion)
  tribits_eti_type_expansion(expansion "f1=ta|tb" "f2=tc" "f3=td|te")
  unittest_compare_const(
    expansion
    "f1={ta} f2={tc} f3={td};f1={ta} f2={tc} f3={te};f1={tb} f2={tc} f3={td};f1={tb} f2={tc} f3={te}"
    )

endfunction()


function(unittest_tribits_eti_check_exclusion)

  message("*** Test passing valid arguments to tribits_eti_check_exclusion( ... )\n")

  message("empty exclusion list...")
  tribits_eti_check_exclusion("" "ta|tb|tc" result)
  unittest_compare_const(
    result
    OFF
  )

  message("inst not excluded (no match)...")
  tribits_eti_check_exclusion("td|te|tf" "ta|tb|tc" result)
  unittest_compare_const(
    result
    OFF
  )

  message("matches only on present types...")
  tribits_eti_check_exclusion("ta|ta|tb" "ta|TYPE-MISSING|tb" result)
  unittest_compare_const(
    result
    ON
  )

  message("no match: exclusion has the wrong rank (not an error)...")
  tribits_eti_check_exclusion("ta|ta" "ta|tb|tc" result)
  unittest_compare_const(
    result
    OFF
  )

  message("inst not excluded (partial match)...")
  tribits_eti_check_exclusion("ta|tb|ta;tb|tb|tc;ta|ta|tc" "ta|tb|tc" result)
  unittest_compare_const(
    result
    OFF
  )

  message("inst excluded (full explicit)...")
  tribits_eti_check_exclusion("abcdf;ta|tb|tc" "ta|tb|tc" result)
  unittest_compare_const(
    result
    ON
  )

  message("inst excluded (full regex)...")
  tribits_eti_check_exclusion("abcdf;.*|.*|.*" "ta|tb|tc" result)
  unittest_compare_const(
    result
    ON
  )

endfunction()


function(unittest_tribits_eti_index_macro_fields)

  message("*** Test passing valid arguments to tribits_eti_index_macro_fields( ... )\n")

  # check simple
  tribits_eti_index_macro_fields("F1;F2;F3" "F3" var)
  unittest_compare_const(
    var
    "2"
    )

  # check complex
  tribits_eti_index_macro_fields("F1;F2;F3" "F3;F2;F1" var)
  unittest_compare_const(
    var
    "2;1;0"
    )

  # check complex with spaces
  tribits_eti_index_macro_fields("F1;F2;F3" " F2 ;   F2 ; F2 " var)
  unittest_compare_const(
    var
    "1;1;1"
    )

endfunction()


function(unittest_tribits_add_eti_instantiations_initial)

  message("*** Testing TRIBITS_ADD_ETI_INSTANTIATIONS... )\n")

  set(package ${PROJECT_NAME}Framework)
  global_null_set(${package}_ETI_LIBRARYSET)
  tribits_add_eti_instantiations(${package} "someinst")
  unittest_compare_const(
    ${package}_ETI_LIBRARYSET
    "someinst"
    )

endfunction()


function(unittest_tribits_add_eti_instantiations_cumulative)

  set(package ${PROJECT_NAME}Framework)
  tribits_add_eti_instantiations(${package} "anotherinst")
  unittest_compare_const(
    ${package}_ETI_LIBRARYSET
    "someinst;anotherinst"
    )

endfunction()


function(unittest_tribits_eti_explode)

  message("*** Test passing valid arguments to tribits_eti_explode( ... )\n")

  # no fields -> no results
  set(FIELDS "")
  tribits_eti_explode("${FIELDS}" "F1=type1 F2=type2 F3=type3" parsed)
  unittest_compare_const(
    parsed
    ""
    )

  # order doesn't matter; also, results should be bracketed
  set(FIELDS F FF G)
  tribits_eti_explode("${FIELDS}" "F=type1 FF=type2 G={type3}" parsed)
  unittest_compare_const(
    parsed
    "type1|type2|type3"
    )
  tribits_eti_explode("${FIELDS}" "FF=type2 F={type1} G=type3" parsed)
  unittest_compare_const(
    parsed
    "type1|type2|type3"
    )
  tribits_eti_explode("${FIELDS}" "G=type3 FF=type2 F={type1}" parsed)
  unittest_compare_const(
    parsed
    "type1|type2|type3"
    )

  # empty for missing fields

  # missing field handled properly, extra fields ignored
  set(FIELDS F FF G)
  tribits_eti_explode("${FIELDS}" "F=type1 G=type3 H=type4" parsed)
  unittest_compare_const(
    parsed
    "type1|TYPE-MISSING|type3"
    )

  # bad bracketing doesn't work
  tribits_eti_explode("F" "F=typea}" parsed)
  unittest_compare_const(
    parsed
    "TRIBITS_ETI_BAD_PARSE"
    )
  tribits_eti_explode("F" "F={typea" parsed)
  unittest_compare_const(
    parsed
    "TRIBITS_ETI_BAD_PARSE"
    )
  tribits_eti_explode("F" "F={typea}}" parsed)
  unittest_compare_const(
    parsed
    "TRIBITS_ETI_BAD_PARSE"
    )
  tribits_eti_explode("F" "F={{typea}" parsed)
  unittest_compare_const(
    parsed
    "TRIBITS_ETI_BAD_PARSE"
    )
  tribits_eti_explode("F" "F=typeaG=typeb" parsed)
  unittest_compare_const(
    parsed
    "TRIBITS_ETI_BAD_PARSE"
    )

endfunction()


function(unittest_tribits_eti_mangle_symbol)

  message("*** Testing ETI Mangling ***")

  # this one is ugly...
  tribits_eti_mangle_symbol(mangled "std::pair< std::complex< double > , std::complex< float > >")
  unittest_compare_const(
    mangled
    "std_pair2std_complex1double1_std_complex0float02")

  # test that POD isn't mangled, and that the method accumulates into the typedef list
  set(defs_orig "do not delete")
  set(defs "${defs_orig}")
  set(symbol "double")
  set(mangling_list "")
  tribits_eti_mangle_symbol_augment_macro(defs symbol mangling_list)
  unittest_compare_const(
    symbol
    "double")
  unittest_compare_const(
    defs
    "${defs_orig}")
  unittest_compare_const(
    mangling_list
    "")

  # this is more like what we expect
  set(defs "")
  set(mangling_list "")
  #
  set(symbol "std::complex<float>")
  tribits_eti_mangle_symbol_augment_macro(defs symbol mangling_list)
  unittest_compare_const(
    symbol
    "std_complex0float0")
  #
  set(symbol "std::pair<float,float>")
  tribits_eti_mangle_symbol_augment_macro(defs symbol mangling_list)
  unittest_compare_const(
    symbol
    "std_pair0float_float0")
  #
  unittest_compare_const(
    mangling_list
    "std_complex0float0;std_pair0float_float0")
  unittest_compare_const(
    defs
    "typedef std::complex<float> std_complex0float0;typedef std::pair<float,float> std_pair0float_float0")

endfunction()


function(unittest_tribits_eti_generate_macros)

  message("*** Test tribits_eti_generate_macros( ... )\n")

  tribits_eti_type_expansion(
    etiset
    "F1=Teuchos::ArrayRCP<Teuchos::ArrayRCP<double> > | double"
    "F2=int | long"
    "F3=float"
  )
  tribits_eti_type_expansion(
    exclset
    "F1=.*"
    "F2=long"
    "F3=.*"
  )
  tribits_eti_generate_macros(
    "F1|F2|F3"
    "${etiset}"
    "${exclset}"
    mangling_list     typedef_list
    "f1(F1)"          macro_f1_var
    "f312(F3,F1,F2)"  macro_f312_var
  )
  unittest_compare_const(
    macro_f1_var
"#define f1(INSTMACRO)\\
\tINSTMACRO( Teuchos_ArrayRCP1Teuchos_ArrayRCP0double01 )\\
\tINSTMACRO( double )
"
    )
  unittest_compare_const(
    macro_f312_var
"#define f312(INSTMACRO)\\
\tINSTMACRO( float , Teuchos_ArrayRCP1Teuchos_ArrayRCP0double01 , int )\\
\tINSTMACRO( float , double , int )
"
    )
  unittest_compare_const(
    typedef_list
    "typedef Teuchos::ArrayRCP<Teuchos::ArrayRCP<double> > Teuchos_ArrayRCP1Teuchos_ArrayRCP0double01")
  unittest_compare_const(
    mangling_list
    "Teuchos_ArrayRCP1Teuchos_ArrayRCP0double01")

  set(mangling_list "")
  set(typedef_list  "")
  tribits_eti_generate_macros(
    "F1|F2"
    "F1=a F2=b;F2=c;G1=d G2=e;G3=f"
    ""
    mangling_list     typedef_list
    "f2(F2)"          macro_f2_var
    "f12(F1,F2)"      macro_f12_var
  )
  unittest_compare_const( typedef_list "")
  unittest_compare_const( mangling_list "")
  unittest_compare_const(
    macro_f2_var
"#define f2(INSTMACRO)\\
\tINSTMACRO( b )\\
\tINSTMACRO( c )
")
  unittest_compare_const(
    macro_f12_var
"#define f12(INSTMACRO)\\
\tINSTMACRO( a , b )
")

endfunction()


################################################################################
#
# Execute the unit tests
#
################################################################################

# Set up some global environment stuff
set(${PROJECT_NAME}_HOSTNAME testhost.nowhere.com)
set(CMAKE_HOST_SYSTEM_NAME UnspecifiedHostSystemName)

unittest_initialize_vars()

# Set up the tribits_add_test(...) and tribits_add_advanced_test() functions
# for unit test mode.
set( TRIBITS_ADD_TEST_ADD_TEST_UNITTEST TRUE )

# Capture the add_test() arguments for tribits_add_test().
set( TRIBITS_ADD_TEST_ADD_TEST_CAPTURE TRUE )

message("\n***")
message("*** Testing misc TriBITS functions and macros")
message("***\n")

unittest_append_string_var()
unittest_tribits_find_python_interp()
unittest_tribits_standardize_abs_paths()
unittest_tribits_dir_is_basedir()
unittest_tribits_get_dir_array_below_base_dir()
unittest_tribits_misc()
unittest_tribits_strip_quotes_from_str()
unittest_tribits_get_version_date_from_raw_git_commit_utc_time()
unittest_tribits_get_raw_git_commit_utc_time()
unittest_tribits_git_repo_sha1()
unittest_tribits_tpl_allow_pre_find_package()
unittest_tribits_report_invalid_tribits_usage()

# Set the default test categories
set(${PROJECT_NAME}_TEST_CATEGORIES NIGHTLY)

message("\n***")
message("*** Testing tribits_add_test(...)")
message("***\n")

unittest_tribits_add_test_basic()
unittest_tribits_add_test_disable()
unittest_tribits_add_test_categories()
unittest_tribits_add_test_comm()
unittest_tribits_add_test_properties()

message("\n***")
message("*** Testing tribits_add_advanced_test(...)")
message("***\n")

unittest_tribits_add_advanced_test_basic()
unittest_tribits_add_advanced_test_categories()
unittest_tribits_add_advanced_test_comm()
unittest_tribits_add_advanced_test_num_mpi_procs()
unittest_tribits_add_advanced_test_directroy()
unittest_tribits_add_advanced_test_properties()
unittest_tribits_add_test_cuda_gpu_ctest_resources()
unittest_tribits_add_advanced_test_copy_files_to_test_dir()
unittest_tribits_add_advanced_test_excludes()
unittest_tribits_add_advanced_test_run_serial()
unittest_tribits_add_advanced_test_change_max_num_test_blocks()

message("\n***")
message("*** Testing tribits_add_executable_and_test(...)")
message("***\n")

unittest_tribits_add_executable_and_test()

message("\n***")
message("*** Testing Explicit Template Instantiation functionality")
message("***\n")

unittest_tribits_eti_explode()
unittest_tribits_eti_type_expansion()
unittest_tribits_eti_check_exclusion()
unittest_tribits_eti_index_macro_fields()
unittest_tribits_add_eti_instantiations_initial()
unittest_tribits_add_eti_instantiations_cumulative()
unittest_tribits_eti_mangle_symbol()
unittest_tribits_eti_generate_macros()

message("\n***")
message("*** Determine final result of all unit tests")
message("***\n")

# Pass in the number of expected tests that must pass!
unittest_final_result(703)
