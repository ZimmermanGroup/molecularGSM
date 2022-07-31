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

#
# The tests defined here are used to test the dynamic behavior of the
# CMake/CTest functions tribits_add_test(...) and
# tribits_add_advanced_test(...).
#

assert_defined(${PACKAGE_NAME}_ENABLE_MPI)
if (${PACKAGE_NAME}_ENABLE_MPI)
  add_definitions(-DHAVE_MPI)
endif()

tribits_add_executable(
  GenericDriver
  SOURCES GenericDriver.cpp
  )

tribits_add_advanced_test(
  CTestScripts_cmnd_1_args_0
  TEST_0 EXEC GenericDriver
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_test( GenericDriver
  NAME CTestScripts_simple_pass_0
  ARGS "0"
  PASS_REGULAR_EXPRESSION "0"
  NUM_MPI_PROCS 1
  )

tribits_add_test( GenericDriver
  NAME CTestScripts_simple_pass_N
  ARGS "N"
  PASS_REGULAR_EXPRESSION "N"
  NUM_MPI_PROCS 1
  )

tribits_add_test( GenericDriver
  NAME CTestScripts_simple_pass_regex_1
  ARGS "\"This is a crazy test!\" 1"
  PASS_REGULAR_EXPRESSION "[^=]This is a crazy test"
  NUM_MPI_PROCS 1
  )

tribits_add_test( GenericDriver
  NAME CTestScripts_simple_pass_regex_2
  POSTFIX_AND_ARGS_0 test1 "This is a crazy test!" 1
  PASS_REGULAR_EXPRESSION "does not match" "This is a crazy test"
  NUM_MPI_PROCS 1
  )

tribits_add_test( GenericDriver
  NAME CTestScripts_simple_posfix_and_args_1
  POSTFIX_AND_ARGS_0 test1 "This is a crazy test!" 1
  PASS_REGULAR_EXPRESSION "[^=]This is a crazy test"
  NUM_MPI_PROCS 1
  )

tribits_add_test( GenericDriver
  NAME CTestScripts_simple_posfix_and_args_2
  POSTFIX_AND_ARGS_0 test1 "This is a crazy test!" 0
  POSTFIX_AND_ARGS_1 test2 "This is a silly test!" 0
  NUM_MPI_PROCS 1
  )

tribits_add_test( GenericDriver
  NAME CTestScripts_will_fail
  POSTFIX_AND_ARGS_0 test1 "This test is supposed to fail!" 1
  WILL_FAIL
  NUM_MPI_PROCS 1
  )
  # If you comment out WILL_FAIL above, then the final test result is "failed"
  # due to the non-zero returncode being.

tribits_add_test( GenericDriver
  NAME CTestScripts_will_fail_pass_regular_expression
  POSTFIX_AND_ARGS_0 test1 "This test is expected the regex!" 0
  PASS_REGULAR_EXPRESSION "Does not match string"
  WILL_FAIL
  NUM_MPI_PROCS 1
  )
  # The above test shows that the built-in WILL_FAIL just inverts the
  # pass/fail result of the other criteria, PASS_REGULAR_EXPRESSION in this
  # case.  If you comment out the WILL_FAIL, then this test final result is
  # "failed"!

tribits_add_test( GenericDriver
  NAME CTestScripts_will_fail_fail_regular_exprssion
  POSTFIX_AND_ARGS_0 test1 "Printing a string that is matched with FAIL_REGULAR_EXPRESSION!" 0
  FAIL_REGULAR_EXPRESSION "matched with FAIL_REGULAR_EXPRESSION"
  WILL_FAIL
  NUM_MPI_PROCS 1
  )
  # The above test shows that the built-in WILL_FAIL just inverts the
  # pass/fail result of the other criteria, FAIL_REGULAR_EXPRESSION in this
  # case.  If you comment out WILL_FAIL, then this test final result is failed
  # (because the FAIL_REGULAR_EXPRESSION matched the STDOUT).

tribits_add_test( GenericDriver
  NAME CTestScripts_pass_regular_expression_fail_regular_exprssion_will_fail
  POSTFIX_AND_ARGS_0 test1 "This says PASS!  This says FAIL!" 1
  PASS_REGULAR_EXPRESSION "This says PASS"
  FAIL_REGULAR_EXPRESSION "This says FAIL"
  WILL_FAIL
  NUM_MPI_PROCS 1
  )
  # The above test shows that FAIL_REGULAR_EXPRESSION takes precedence over
  # PASS_REGULAR_EXPRESSION.  The final WILL_FAIL inverts that and allows the
  # test to pass.  If you comment out WILL_FAIL, the final result will be
  # "failed".  If you comment out FAIL_REGULAR_EXPRESSION, the final result
  # will be "failed".

tribits_add_test( GenericDriver
  NAME CTestScripts_pass_regular_expression_fail_regular_exprssion_2_will_fail
  POSTFIX_AND_ARGS_0 test1 "This says PASS!  This says FAIL!" 1
  PASS_REGULAR_EXPRESSION "This says PASS"
  FAIL_REGULAR_EXPRESSION "Does not match" "This says FAIL"
  WILL_FAIL
  NUM_MPI_PROCS 1
  )

function(ctestscripts_fail_regular_exprssion_w_circular_ref_detection_tests)

  # Create private namespace for setting var
  set(${PACKAGE_NAME}_ENABLE_CIRCULAR_REF_DETECTION_FAILURE TRUE)

  tribits_add_test( GenericDriver
    NAME CTestScripts_fail_regular_exprssion_w_circular_ref_detection_1
    POSTFIX_AND_ARGS_0 test1 "Printing a string that is matched with FAIL_REGULAR_EXPRESSION!" 0
    FAIL_REGULAR_EXPRESSION "matched with FAIL_REGULAR_EXPRESSION"
    WILL_FAIL
    NUM_MPI_PROCS 1
    )
    # The above tests makes sure that the passed-in regex is used to match and
    # fail the test.  If you comment out WILL_FAIL, you will see the test
    # fail.

  tribits_add_test( GenericDriver
    NAME CTestScripts_fail_regular_exprssion_w_circular_ref_detection_2
    POSTFIX_AND_ARGS_0 test1 "Some stuff\n\nThe following Teuchos::RCPNode objects were created\n\nsome other stuff" 0
    FAIL_REGULAR_EXPRESSION "Some other bad outptut that does not match"
    WILL_FAIL
    NUM_MPI_PROCS 1
    )
    # The above tests makes sure that the passed-in regex is used to match and
    # fail the test does not overwrite the circular detection regex.  If you
    # comment out WILL_FAIL, you will see the test fail.

  tribits_add_test( GenericDriver
    NAME CTestScripts_fail_regular_exprssion_w_circular_ref_detection_3
    POSTFIX_AND_ARGS_0 test1 "Some stuff\n\nThe following Teuchos::RCPNode objects were created\n\nsome other stuff" 0
    WILL_FAIL
    NUM_MPI_PROCS 1
    )
    # The above tests makes sure that the default circular detection regex is
    # present.  If you comment out WILL_FAIL, you will see the test fail.

endfunction()

ctestscripts_fail_regular_exprssion_w_circular_ref_detection_tests()

tribits_add_advanced_test(
  CTestScripts_cmnd_1_args_1_print_msg
  TEST_0 EXEC GenericDriver ARGS "This is a crazy test!" 1
  FINAL_PASS_REGULAR_EXPRESSION "\"This is a crazy test"
  )

tribits_add_advanced_test(
  CTestScripts_cmnd_1_args_1_print_msg_no_echo_output
  TEST_0 EXEC GenericDriver ARGS "This is a crazy test!" 0
    NO_ECHO_OUTPUT
  FINAL_FAIL_REGULAR_EXPRESSION "\"This is a crazy test\""
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_cmnd_1_args_1_return_code
  TEST_0 EXEC GenericDriver ARGS "Dummy msg" 1
  FINAL_PASS_REGULAR_EXPRESSION "TEST_0: Return code = 1"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_cmnd_1_args_1_print_msg_out_file_pass_regex_1
  TEST_0 EXEC GenericDriver ARGS "This is a crazy test!" 5
    OUTPUT_FILE cmnd_1_args_1_print_msg_out_file_outputFile.out
    PASS_REGULAR_EXPRESSION "This is a crazy test"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_cmnd_1_args_1_print_msg_out_file_pass_regex_2
  TEST_0 EXEC GenericDriver ARGS "This is a crazy test!" 5
    OUTPUT_FILE cmnd_1_args_1_print_msg_out_file_outputFile.out
    PASS_REGULAR_EXPRESSION "does not match" "This is a crazy test"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_cmnd_1_args_1_print_msg_out_file_fail_regex_2_will_fail
  TEST_0 EXEC GenericDriver ARGS "This is a crazy test!" 0
    OUTPUT_FILE cmnd_1_args_1_print_msg_out_file_outputFile.out
    PASS_REGULAR_EXPRESSION "This is a crazy test"
    FAIL_REGULAR_EXPRESSION "Does not match" "This is a crazy test"
    WILL_FAIL
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_test(GenericDriver
  NAME CTestScripts_tat_args_with_semicolon
  ARGS "arg<sep>with<sep>a<sep>few<sep>semicolons 0"
  PASS_REGULAR_EXPRESSION "arg[;]with[;]a;few[;]semicolons"
  LIST_SEPARATOR "<sep>"
  NUM_MPI_PROCS 1
  )

tribits_add_test(GenericDriver
  NAME CTestScripts_tat_args_with_semicolon_and_spaces
  POSTFIX_AND_ARGS_0 ""
    "arg<sep>with<sep>semicolons and<sep>several spaces"
    0
  PASS_REGULAR_EXPRESSION "arg[;]with[;]semicolons and[;]several spaces"
  LIST_SEPARATOR "<sep>"
  NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_taat_cmnd_1_args_with_semicolon
  TEST_0 EXEC GenericDriver ARGS "arg<sep>with<sep>a<sep>few<sep>semicolons" 0
    PASS_REGULAR_EXPRESSION "arg[;]with[;]a;few[;]semicolons"
  LIST_SEPARATOR "<sep>"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_taat_cmnd_1_args_with_semicolon_and_spaces
  TEST_0 EXEC GenericDriver ARGS "arg<sep>with<sep>semicolons and<sep>several spaces" 0
    PASS_REGULAR_EXPRESSION "arg[;]with[;]semicolons and[;]several spaces"
  LIST_SEPARATOR "<sep>"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_test("${CMAKE_COMMAND}" NOEXEPREFIX NOEXESUFFIX
  NAME CTestScripts_tat_env_vars_with_semicolon
  POSTFIX_AND_ARGS_0 ""
    -DENV_VAR_0=MY_ENV_VAR_0
    -DENV_VAR_1=MY_ENV_VAR_1
    -P "${CMAKE_CURRENT_LIST_DIR}/echoEnvVars.cmake"
  PASS_REGULAR_EXPRESSION
      "MY_ENV_VAR_0 = 'val0a[;]val0b[;]val0c'\nMY_ENV_VAR_1 = 'val1a[;]val1b[;]val1c'"
  ENVIRONMENT
    MY_ENV_VAR_0=val0a<semicol>val0b<semicol>val0c
    MY_ENV_VAR_1=val1a<semicol>val1b<semicol>val1c
  LIST_SEPARATOR "<semicol>"
  NUM_MPI_PROCS 1
  )

tribits_add_test("${CMAKE_COMMAND}" NOEXEPREFIX NOEXESUFFIX
  NAME CTestScripts_tat_env_vars_with_semicolon_and_spaces
  POSTFIX_AND_ARGS_0 ""
    -DENV_VAR_0=MY_ENV_VAR_0
    -DENV_VAR_1=MY_ENV_VAR_1
    -P "${CMAKE_CURRENT_LIST_DIR}/echoEnvVars.cmake"
  PASS_REGULAR_EXPRESSION
      "MY_ENV_VAR_0 = 'val 0a[;]val 0b[;]val 0c'\nMY_ENV_VAR_1 = 'val 1a[;]val 1b[;]val 1c'"
  ENVIRONMENT
    "MY_ENV_VAR_0=val 0a<semicol>val 0b<semicol>val 0c"
    "MY_ENV_VAR_1=val 1a<semicol>val 1b<semicol>val 1c"
  LIST_SEPARATOR "<semicol>"
  NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_taat_cmnd_1_env_vars_with_semicolon
  TEST_0 CMND "${CMAKE_COMMAND}" ARGS
    -DENV_VAR_0=MY_ENV_VAR_0
    -DENV_VAR_1=MY_ENV_VAR_1
    -P "${CMAKE_CURRENT_LIST_DIR}/echoEnvVars.cmake"
    PASS_REGULAR_EXPRESSION_ALL
      "MY_ENV_VAR_0 = 'val0a[;]val0b[;]val0c'"
      "MY_ENV_VAR_1 = 'val1a[;]val1b[;]val1c'"
  ENVIRONMENT
    MY_ENV_VAR_0=val0a<semicol>val0b<semicol>val0c
    MY_ENV_VAR_1=val1a<semicol>val1b<semicol>val1c
  LIST_SEPARATOR "<semicol>"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_taat_cmnd_1_env_vars_with_semicolon_and_spaces
  TEST_0 CMND "${CMAKE_COMMAND}" ARGS
    -DENV_VAR_0=MY_ENV_VAR_0
    -DENV_VAR_1=MY_ENV_VAR_1
    -P "${CMAKE_CURRENT_LIST_DIR}/echoEnvVars.cmake"
    PASS_REGULAR_EXPRESSION_ALL
      "MY_ENV_VAR_0 = 'val 0a[;]val 0b[;]val 0c'"
      "MY_ENV_VAR_1 = 'val 1a[;]val 1b[;]val 1c'"
  ENVIRONMENT
    "MY_ENV_VAR_0=val 0a<semicol>val 0b<semicol>val 0c"
    "MY_ENV_VAR_1=val 1a<semicol>val 1b<semicol>val 1c"
  LIST_SEPARATOR "<semicol>"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_cmnd_1_args_1_pass_any_test_0
  TEST_0 EXEC GenericDriver ARGS "Dummy msg" 7
    PASS_ANY
  FINAL_PASS_REGULAR_EXPRESSION "TEST_0: Pass criteria = Pass Any"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_cmnd_1_args_1_pass_any_overall
  TEST_0 EXEC GenericDriver ARGS "Dummy msg" 7
    PASS_ANY
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_cmnd_1_args_1_standard_pass_output
  TEST_0 EXEC GenericDriver ARGS "Dummy msg" 7
    STANDARD_PASS_OUTPUT
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_cmnd_1_args_2_stderr_pass_regular_expression_all
  TEST_0 EXEC GenericDriver
    ARGS "This is a crazy test\n\nThis is not the best test" 5 "This goes to stderr"
    OUTPUT_FILE cmnd_1_args_1_pass_regular_expression_all_outputFile.out
    PASS_REGULAR_EXPRESSION_ALL "This is a crazy test" "This is not the best test" "This goes to stderr"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_cmnd_1_args_1_pass_regular_expression_all_compare_1
  TEST_0 EXEC GenericDriver
    ARGS "This is a crazy test\n\nThis is not the best test" 5
    OUTPUT_FILE cmnd_1_args_1_pass_regular_expression_all_compare_1_outputFile.out
    PASS_REGULAR_EXPRESSION_ALL "This is a crazy test" "This is not the best test"
  FINAL_PASS_REGULAR_EXPRESSION "TEST_0: Pass criteria = Match all REGEX {This is a crazy test} [^a]PASSED[^a]"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_cmnd_1_args_1_pass_regular_expression_all_compare_2
  TEST_0 EXEC GenericDriver
    ARGS "This is a crazy test\n\nThis is not the best test" 5
    OUTPUT_FILE cmnd_1_args_1_pass_regular_expression_all_compare_2_outputFile.out
    PASS_REGULAR_EXPRESSION_ALL "This is a crazy test" "This is not the best test"
  FINAL_PASS_REGULAR_EXPRESSION "TEST_0: Pass criteria = Match all REGEX {This is not the best test} [^a]PASSED[^a]"
  OVERALL_NUM_MPI_PROCS 1
  )

# 2009/04/06: rabartl: Above, I can't use '[' or ']' in the regular expression
# because it is interpreted as a char list.  Therefore, I leave it as not 'a'
# and that is good enough for me.

tribits_add_advanced_test(
  CTestScripts_cmnd_1_args_1_print_msg_out_file_no_echo_output
  TEST_0 EXEC GenericDriver
    ARGS "This is a crazy test!" 5
    OUTPUT_FILE cmnd_1_args_1_print_msg_out_file_no_echo_output_outputFile.out
    NO_ECHO_OUTPUT
  FINAL_FAIL_REGULAR_EXPRESSION "^This is a crazy test"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_cmnd_1_args_1_message
  TEST_0 EXEC GenericDriver
    ARGS "Dummy msg" 1
    MESSAGE "This is a crazy test!"
  FINAL_PASS_REGULAR_EXPRESSION "This is a crazy test"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_cmnd_1_args_1_working_directory
  TEST_0 EXEC GenericDriver
    ARGS "This is a crazy test!" 0
    WORKING_DIRECTORY TEST_NAME
    OUTPUT_FILE cmnd_1_args_1_working_directory_console.out
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_cmnd_2_args_1_basic
  TEST_0 EXEC GenericDriver
    ARGS "This is the first test" 3
    PASS_REGULAR_EXPRESSION "first test"
  TEST_1 EXEC GenericDriver
    ARGS "This is the second test" 4
    PASS_REGULAR_EXPRESSION "second test"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_cmnd_2_args_1_check_second_print
  TEST_0 EXEC GenericDriver
    ARGS "This is the first test" 3
    PASS_REGULAR_EXPRESSION "first test"
  TEST_1 EXEC GenericDriver
    ARGS "This is the second test" 4
    PASS_REGULAR_EXPRESSION "second test"
  FINAL_PASS_REGULAR_EXPRESSION "second test"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_cmnd_2_fail_fast
  TEST_0 EXEC GenericDriver
    ARGS "This first test will fail" 3
  TEST_1 EXEC GenericDriver
    ARGS "This is the second test will never be run" 0
  FAIL_FAST
  FINAL_PASS_REGULAR_EXPRESSION "TEST_0: FAIL FAST, SKIPPING REST OF TEST CASES!"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_cmnd_2_no_fail_fast_print_msg
  TEST_0 EXEC GenericDriver
    ARGS "This first test will fail" 3
  TEST_1 EXEC GenericDriver
    ARGS "This is the second test should be run" 0
  FINAL_PASS_REGULAR_EXPRESSION "This is the second test should be run"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_cmnd_2_no_fail_fast_final_fail
  TEST_0 EXEC GenericDriver
    ARGS "This first test will fail" 3
  TEST_1 EXEC GenericDriver
    ARGS "This is the second test should be run" 0
  FINAL_PASS_REGULAR_EXPRESSION "OVERALL FINAL RESULT: TEST FAILED"
  OVERALL_NUM_MPI_PROCS 1
  )

if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
  # cmake -E compare_files only gives return != 0 if the files don't compare.
  # It gives no diff output.
  set(DIFF_COMMAND_STUFF "${CMAKE_COMMAND}" ARGS -E compare_files)
else()
  # Diff gives output of the diff so it is better to use if we have it.
  # Otherwise, I need to write a portable diff (using a CMake -P script).
  set(DIFF_COMMAND_STUFF diff ARGS)
endif()

tribits_add_advanced_test(
  CTestScripts_cmnd_3_args_1_overall_working_directory
  OVERALL_WORKING_DIRECTORY TEST_NAME
  TEST_0 EXEC GenericDriver
    ARGS "This is a crazy test!" 0 "This is a bad crazy test!"
    OUTPUT_FILE cmnd_3_args_1_overall_working_directory_console.out
  TEST_1 CMND cat
    ARGS cmnd_3_args_1_overall_working_directory_console.out
    PASS_REGULAR_EXPRESSION_ALL
       "This is a crazy test"
       "This is a bad crazy test"
  OVERALL_NUM_MPI_PROCS 1
  XHOSTTYPE Windows
  )
# NOTE: The above test does not check the order of STDOUT and STDERR, only
# that both exist in the output file.  It seems that execute_process() and MPI
# do not behave well together w.r.t. to STDOUT and STDERR outputting.  The
# fault may be with the MPI implementation.  Also, even in serial it would
# seem that the order of STDOUT and STDERR gets messed up when the machine is
# loaded.
#
# NOTE: The above test does not run on Windows because it relies on the 'cat'
# command.  The form of this test is needed in order to check that both STDOUT
# and STDERR output are both in the output file but we can't rely on the
# order.  It seems that execute_process() and mpiexec don't work together to
# produce ordered STDOUT and STDERR very well.  Also, even in serial, it seems
# that when the machine is loaded that the order of STDOUT and STDERR can get
# messed up.  It is not clear if this is a problem with execute_process() or
# not.


#
# Test creation and optional preservation of working directories
#

tribits_add_advanced_test(
  CTestScripts_cmnd_3_args_1_overall_working_directory_working_directory
  OVERALL_WORKING_DIRECTORY
    CTestScripts_cmnd_3_args_1_overall_working_directory_working_directory
  TEST_0 EXEC GenericDriver
    ARGS "This is another crazy test!" 0
    OUTPUT_FILE cmnd_3_args_1_overall_working_directory_working_directory_console.out
    WORKING_DIRECTORY GenericDriver
    SKIP_CLEAN_WORKING_DIRECTORY
  TEST_1 CMND "${CMAKE_COMMAND}"
    ARGS -E copy
      ../GenericDriver/cmnd_3_args_1_overall_working_directory_working_directory_console.out
      cmnd_3_args_1_overall_working_directory_working_directory_console.gold.out
    WORKING_DIRECTORY Gold
  TEST_2 CMND ${DIFF_COMMAND_STUFF}  # Contains the ARGS part
    GenericDriver/cmnd_3_args_1_overall_working_directory_working_directory_console.out
    Gold/cmnd_3_args_1_overall_working_directory_working_directory_console.gold.out
  OVERALL_NUM_MPI_PROCS 1
  ADDED_TEST_NAME_OUT
    CTestScripts_cmnd_3_args_1_overall_working_directory_working_directory_TEST_NAME
  )

# This test uses the same working directory and relies on it not being deleted
# so it still contains the output file.
tribits_add_advanced_test(
  CTestScripts_cmnd_3_args_1_overall_working_directory_working_directory_skip_clean
  OVERALL_WORKING_DIRECTORY
    "${CMAKE_CURRENT_BINARY_DIR}/CTestScripts_cmnd_3_args_1_overall_working_directory_working_directory"
  SKIP_CLEAN_OVERALL_WORKING_DIRECTORY
  SKIP_CLEAN_OVERALL_WORKING_DIRECTORY
  TEST_0 CMND "${CMAKE_COMMAND}"
    ARGS -E copy
      cmnd_3_args_1_overall_working_directory_working_directory_console.out
      cmnd_3_args_1_overall_working_directory_working_directory_console.gold.out
    WORKING_DIRECTORY
      "${CMAKE_CURRENT_BINARY_DIR}/CTestScripts_cmnd_3_args_1_overall_working_directory_working_directory/GenericDriver"
    SKIP_CLEAN_WORKING_DIRECTORY
  TEST_1 CMND ${DIFF_COMMAND_STUFF}  # Contains the ARGS part
    GenericDriver/cmnd_3_args_1_overall_working_directory_working_directory_console.out
    GenericDriver/cmnd_3_args_1_overall_working_directory_working_directory_console.gold.out
  OVERALL_NUM_MPI_PROCS 1
  ADDED_TEST_NAME_OUT
    CTestScripts_cmnd_3_args_1_overall_working_directory_working_directory_skip_clean_TEST_NAME
  )

# You have to run the above two tests in sequence it the second test can't
# pass.
set_property(  TEST
  ${CTestScripts_cmnd_3_args_1_overall_working_directory_working_directory_skip_clean_TEST_NAME}
  PROPERTY DEPENDS
    ${CTestScripts_cmnd_3_args_1_overall_working_directory_working_directory_TEST_NAME}
  )

# Demonstrate processes never dying

if (PERL_EXECUTABLE AND TriBITS_ENABLE_RUNS_FOREVER_TEST)
  
  tribits_add_executable(
    RunsForever
    SOURCES RunsForever.cpp
    )
  
  tribits_add_advanced_test(
    CTestScripts_RunsForever
    TEST_0 CMND ${PERL_EXECUTABLE} ARGS ${CMAKE_CURRENT_SOURCE_DIR}/runs_forever.pl
    OVERALL_NUM_MPI_PROCS 1
    )

endif()

# ENVIRONMENT

tribits_add_test( "${CMAKE_COMMAND}" NOEXEPREFIX NOEXESUFFIX
  NAME CTestScripts_TAT_pass_env
  ARGS "-P ${CMAKE_CURRENT_SOURCE_DIR}/EchoEnvVarForTest.cmake"
  NUM_MPI_PROCS 1
  ENVIRONMENT PASS_IN_FROM_ENV_VAR=ThisWasPassedCorrectly
  PASS_REGULAR_EXPRESSION "PASS_IN_FROM_ENV_VAR: ThisWasPassedCorrectly"
  )

tribits_add_advanced_test( CTestScripts_TAAT_pass_env
  TEST_0 CMND ${CMAKE_COMMAND}
    ARGS -P ${CMAKE_CURRENT_SOURCE_DIR}/EchoEnvVarForTest.cmake
    NUM_MPI_PROCS 1
    PASS_REGULAR_EXPRESSION "PASS_IN_FROM_ENV_VAR: ThisWasPassedCorrectly"
  ENVIRONMENT PASS_IN_FROM_ENV_VAR=ThisWasPassedCorrectly
  )

# taat() WILL_FAIL and FAIL_REGULAR_EXPRESSION

tribits_add_advanced_test(
  CTestScripts_TAAT_will_fail_return_code
  TEST_0 EXEC GenericDriver
    ARGS "We expect this test to fail" 1
    WILL_FAIL
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_TAAT_will_fail_pass_regular_expression
  TEST_0 EXEC GenericDriver
    ARGS "This test passes?" 1
    PASS_REGULAR_EXPRESSION "This test PASSES" # will not match
    WILL_FAIL
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_TAAT_will_fail_fail_regular_expression
  TEST_0 EXEC GenericDriver
    ARGS "This test failed!" 0
    FAIL_REGULAR_EXPRESSION "This test failed"
    WILL_FAIL
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_TAAT_will_fail_pass_regular_expression_fail_regular_expression
  TEST_0 EXEC GenericDriver
    ARGS "This test passed!  This test failed" 1
    PASS_REGULAR_EXPRESSION "This test passed"
    FAIL_REGULAR_EXPRESSION "This test failed"
    WILL_FAIL
  OVERALL_NUM_MPI_PROCS 1
  )

# taat() ALWAYS_FAIL_ON_NONZERO_RETURN

tribits_add_advanced_test(
  CTestScripts_TAAT_always_fail_on_nonzero_return
  TEST_0 EXEC GenericDriver
    ALWAYS_FAIL_ON_NONZERO_RETURN
    ARGS "This test passed!" 1
    WILL_FAIL
  OVERALL_NUM_MPI_PROCS 1
  )
  # Above, ALWAYS_FAIL_ON_NONZERO_RETURN is listed belore ARGS so that the
  # parsing will error out if it is not defined.

tribits_add_advanced_test(
  CTestScripts_TAAT_pass_regular_expression_always_fail_on_nonzero_return
  TEST_0 EXEC GenericDriver
    ALWAYS_FAIL_ON_NONZERO_RETURN
    ARGS "This test passed!" 1
    PASS_REGULAR_EXPRESSION "This test passed"
    WILL_FAIL
  OVERALL_NUM_MPI_PROCS 1
  )
  # Without the behavior of ALWAYS_FAIL_ON_NONZERO_RETURN, this test will fail
  # because the PASS_REGULAR_EXPRESSION match will pass.

tribits_add_advanced_test(
  CTestScripts_TAAT_pass_regular_expression_all_always_fail_on_nonzero_return
  TEST_0 EXEC GenericDriver
    ALWAYS_FAIL_ON_NONZERO_RETURN
    ARGS "This test passed!" 1
    PASS_REGULAR_EXPRESSION_ALL  "This test"  "This test passed"
    WILL_FAIL
  OVERALL_NUM_MPI_PROCS 1
  )
  # Without the behavior of ALWAYS_FAIL_ON_NONZERO_RETURN, this test will fail
  # because the PASS_REGULAR_EXPRESSION match will pass.

# taat() ALWAYS_FAIL_ON_ZERO_RETURN

tribits_add_advanced_test(
  CTestScripts_TAAT_always_fail_on_zero_return
  TEST_0 EXEC GenericDriver
    ALWAYS_FAIL_ON_ZERO_RETURN
    ARGS "This test failed but returned 0 when it should have returned nonzero!" 0
    WILL_FAIL
  OVERALL_NUM_MPI_PROCS 1
  )
  # Above, ALWAYS_FAIL_ON_ZERO_RETURN is listed belore ARGS so that the
  # parsing will error out if it is not defined.  Also, without the behavior
  # of ALWAYS_FAIL_ON_ZERO_RETURN, this test will fail since the default
  # pass/fail is return value of 0 which is negated with WILL_FAIL.

tribits_add_advanced_test(
  CTestScripts_TAAT_pass_regular_expression_always_fail_on_zero_return
  TEST_0 EXEC GenericDriver
    ALWAYS_FAIL_ON_ZERO_RETURN
    ARGS "This test failed but should have returned a nonzero return code!" 0
    PASS_REGULAR_EXPRESSION "This test failed"
    WILL_FAIL
  OVERALL_NUM_MPI_PROCS 1
  )
  # Above: Without the behavior of ALWAYS_FAIL_ON_NONZERO_RETURN, this test
  # will fail because the PASS_REGULAR_EXPRESSION match will pass and
  # WILL_FAIL will negate that.

tribits_add_advanced_test(
  CTestScripts_TAAT_pass_regular_expression_all_always_fail_on_zero_return
  TEST_0 EXEC GenericDriver
    ALWAYS_FAIL_ON_ZERO_RETURN
    ARGS "This test failed but should have returned a nonzero return code!" 0
    PASS_REGULAR_EXPRESSION_ALL  "This test"  "This test failed"
    WILL_FAIL
  OVERALL_NUM_MPI_PROCS 1
  )
  # Above: Without the behavior of ALWAYS_FAIL_ON_NONZERO_RETURN, this test
  # will fail because the PASS_REGULAR_EXPRESSION_ALL match will pass and
  # WILL_FAIL will negate that.

# taat() COPY_TO_TEST_DIR

tribits_add_advanced_test(
  CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_IMPLICIT_SOURCE_DIR_DEST_DIR_NO_OVERALL_WORKING_DIR
  TEST_0 COPY_FILES_TO_TEST_DIR
    test_file_a.CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_IMPLICIT_SOURCE_DIR_DEST_DIR_NO_OVERALL_WORKING_DIR.txt
    test_file_b.CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_IMPLICIT_SOURCE_DIR_DEST_DIR_NO_OVERALL_WORKING_DIR.txt
  TEST_1 CMND cat
    ARGS test_file_a.CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_IMPLICIT_SOURCE_DIR_DEST_DIR_NO_OVERALL_WORKING_DIR.txt
    PASS_REGULAR_EXPRESSION "This is Test File A for CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_IMPLICIT_SOURCE_DIR_DEST_DIR_NO_OVERALL_WORKING_DIR"
  TEST_2 CMND cat
    ARGS test_file_b.CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_IMPLICIT_SOURCE_DIR_DEST_DIR_NO_OVERALL_WORKING_DIR.txt
    PASS_REGULAR_EXPRESSION "This is Test File B for CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_IMPLICIT_SOURCE_DIR_DEST_DIR_NO_OVERALL_WORKING_DIR"
  OVERALL_NUM_MPI_PROCS 1
  )
  # Above: The files being copied must have a unique name so that they don't
  # clash with test files with another name.

tribits_add_advanced_test(
  CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_REL_SOURCE_DIR_IMPLICIT_DEST_DIR
  OVERALL_WORKING_DIRECTORY TEST_NAME
  TEST_0 COPY_FILES_TO_TEST_DIR  test_file_a.txt  test_file_b.txt
    SOURCE_DIR test_data
  TEST_1 CMND cat ARGS test_file_a.txt
    PASS_REGULAR_EXPRESSION "This is Test File A"
  TEST_2 CMND cat ARGS test_file_b.txt
    PASS_REGULAR_EXPRESSION "This is Test File B"
  OVERALL_NUM_MPI_PROCS 1
  )
  # ToDo: Guard these for Linux systems or replace 'cat' with the right
  # command on Windows systems.

tribits_add_advanced_test(
  CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_ABS_SOURCE_DIR
  OVERALL_WORKING_DIRECTORY TEST_NAME
  TEST_0 COPY_FILES_TO_TEST_DIR  test_file_a.txt  test_file_b.txt
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test_data"
  TEST_1 CMND cat ARGS test_file_a.txt
    PASS_REGULAR_EXPRESSION "This is Test File A"
  TEST_2 CMND cat ARGS test_file_b.txt
    PASS_REGULAR_EXPRESSION "This is Test File B"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_IMPLICIT_SOURCE_DIR
  OVERALL_WORKING_DIRECTORY TEST_NAME
  TEST_0 COPY_FILES_TO_TEST_DIR  test_file_c.txt
  TEST_1 CMND cat ARGS test_file_c.txt
    PASS_REGULAR_EXPRESSION "This is Test File C"
  )

tribits_add_advanced_test(
  CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_REL_DEST_DIR
  OVERALL_WORKING_DIRECTORY TEST_NAME
  TEST_0 COPY_FILES_TO_TEST_DIR  test_file_a.txt  test_file_b.txt
    SOURCE_DIR test_data
    DEST_DIR .
  TEST_1 CMND cat ARGS test_file_a.txt
    PASS_REGULAR_EXPRESSION "This is Test File A"
  TEST_2 CMND cat ARGS test_file_b.txt
    PASS_REGULAR_EXPRESSION "This is Test File B"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_REL_DEST_DIR_SUBDIR
  OVERALL_WORKING_DIRECTORY TEST_NAME
  TEST_0 COPY_FILES_TO_TEST_DIR  test_file_a.txt  test_file_b.txt
    SOURCE_DIR test_data
    DEST_DIR subtestdir
  TEST_1 CMND cat ARGS subtestdir/test_file_a.txt
    PASS_REGULAR_EXPRESSION "This is Test File A"
  TEST_2 CMND cat ARGS subtestdir/test_file_b.txt
    PASS_REGULAR_EXPRESSION "This is Test File B"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_ABS_DEST_DIR
  OVERALL_WORKING_DIRECTORY TEST_NAME
  TEST_0 COPY_FILES_TO_TEST_DIR  test_file_a.txt  test_file_b.txt
    SOURCE_DIR test_data
    DEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_ABS_DEST_DIR"
  TEST_1 CMND cat ARGS test_file_a.txt
    PASS_REGULAR_EXPRESSION "This is Test File A"
  TEST_2 CMND cat ARGS test_file_b.txt
    PASS_REGULAR_EXPRESSION "This is Test File B"
  OVERALL_NUM_MPI_PROCS 1
  )

tribits_add_advanced_test(
  CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_bad_file_name_DRIVER
  OVERALL_WORKING_DIRECTORY TEST_NAME
  TEST_0 CMND ${CMAKE_COMMAND}
    ARGS -DTRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DCOPY_FILE_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/test_data
      ${CMAKE_CURRENT_SOURCE_DIR}/TAATDriver/bad_file_name
    ALWAYS_FAIL_ON_NONZERO_RETURN
    PASS_REGULAR_EXPRESSION_ALL
      "TAATDriver_TAAT_COPY_FILES_TO_TEST_DIR_bad_file_name: Added test [(]BASIC, PROCESSORS=1[)]"
      "Generating done"
  TEST_1 CMND make
  TEST_2 CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
    PASS_REGULAR_EXPRESSION_ALL
      "Error copying file .+/TAATDriver/bad_file_name/this_file_does_not_exist.txt. to .+/TriBITS_CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_bad_file_name_DRIVER/TAATDriver_TAAT_COPY_FILES_TO_TEST_DIR_bad_file_name/"
      "TEST_0: Result = FAILED"
      "TEST_1: Result = PASSED"
      "This is Test File A"
      "TEST_2: Return code = 0"
      "TEST_2: Pass criteria = Match any REGEX .This is Test File A. .PASSED."
      "TEST_2: Result = PASSED"                                
      "OVERALL FINAL RESULT: TEST FAILED [(]TAATDriver_TAAT_COPY_FILES_TO_TEST_DIR_bad_file_name[)]"
  )
  # NOTE: The above test runs a ctest added with taat() and it ensures that
  # bad file names are handled gracefully.  In this case, it ensures that the
  # later TEST_<IDX> blocks are still run and can even pass (but the overall
  # TAAT test fails).  This is a very strong test.  It actually asserts what
  # is printed by taat() tests.  A more targeted test should really be added
  # to do this.

tribits_add_advanced_test(
  CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_bad_dest_dir_DRIVER
  OVERALL_WORKING_DIRECTORY TEST_NAME
  TEST_0 CMND ${CMAKE_COMMAND}
    ARGS -DTRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DCOPY_FILE_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/test_data
      ${CMAKE_CURRENT_SOURCE_DIR}/TAATDriver/bad_dest_dir
    ALWAYS_FAIL_ON_NONZERO_RETURN
    PASS_REGULAR_EXPRESSION_ALL
      "TAATDriver_TAAT_COPY_FILES_TO_TEST_DIR_bad_dest_dir: Added test [(]BASIC, PROCESSORS=1[)]"
      "Generating done"
  TEST_1 CMND make
  TEST_2 CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
    PASS_REGULAR_EXPRESSION_ALL
      "Running: .mkdir. .read_only_dir."
      "TEST_0: Result = PASSED"
      "Running: .chmod. .a-w. .read_only_dir."
      "TEST_1: Result = PASSED"
      "Creating dest directory .*/TriBITS_CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_bad_dest_dir_DRIVER/TAATDriver_TAAT_COPY_FILES_TO_TEST_DIR_bad_dest_dir/read_only_dir/subdir/"
      "Error .*ing directory .*/TriBITS_CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_bad_dest_dir_DRIVER/TAATDriver_TAAT_COPY_FILES_TO_TEST_DIR_bad_dest_dir/read_only_dir/subdir.[.]"
      "Error copying file .*/CTestScriptsUnitTests/test_data/test_file_b.txt. to .*/TriBITS_CTestScripts_TAAT_COPY_FILES_TO_TEST_DIR_bad_dest_dir_DRIVER/TAATDriver_TAAT_COPY_FILES_TO_TEST_DIR_bad_dest_dir/read_only_dir/subdir/."
      "TEST_2: Result = FAILED"
      "TEST_3: Result = PASSED"
      "This is Test File A"
      "TEST_4: Return code = 0"
      "TEST_4: Pass criteria = Match any REGEX .This is Test File A. .PASSED."
      "TEST_4: Result = PASSED"                                
      "OVERALL FINAL RESULT: TEST FAILED [(]TAATDriver_TAAT_COPY_FILES_TO_TEST_DIR_bad_dest_dir[)]"
  )
  # NOTE: The above test ensures that a bad dest dir creation attempt is
  # handled gracefully.  Also note `cmake -E make_directory ...` with older
  # CMake versions print "Error making directory ..." while newer versions
  # print "Error creating director ...".  Hence, we need the silly "Error
  # .*ing directory" regex above :-(
