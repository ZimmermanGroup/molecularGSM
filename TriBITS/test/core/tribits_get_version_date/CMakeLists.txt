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
# Function to make running tests easy
#

function(tribits_get_version_test_case  case_name_dir  expected_output
  git_version  expected_to_pass
  )

  if (expected_to_pass)
    set(fail_on_return_arg ALWAYS_FAIL_ON_NONZERO_RETURN)
  else()
    set(fail_on_return_arg ALWAYS_FAIL_ON_ZERO_RETURN)
  endif()

  set(test_name tribits_get_version_date_${case_name_dir})

  set(test_abs_dir "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_${test_name}")

  tribits_add_advanced_test( ${test_name}

    OVERALL_WORKING_DIRECTORY TEST_NAME

    TEST_0 CMND ${CMAKE_COMMAND} ARGS -E copy
      "${CMAKE_CURRENT_SOURCE_DIR}/${case_name_dir}/mock_git_inout.txt"
      .mockprogram_inout.txt

    TEST_1
      CMND ${CMAKE_COMMAND}
      ARGS
        -DPROJECT_NAME=DummyProj
        -DDummyProj_SOURCE_DIR="${test_abs_dir}"
        -DCOMMIT_REF=HEAD
        -DGIT_EXECUTABLE=${${PROJECT_NAME}_TRIBITS_DIR}/python_utils/mockprogram.py
        -DGIT_VERSION_STRING=${git_version}
        -P "${${PROJECT_NAME}_TRIBITS_DIR}/core/package_arch/tribits_get_version_date.cmake"
      ${fail_on_return_arg}
      PASS_REGULAR_EXPRESSION_ALL
        "${expected_output}"
    )

endfunction()


#
# Define the tests
#

tribits_get_version_test_case(good_input_1  "2024111512"  "2.10.1"  TRUE)
tribits_get_version_test_case(good_input_2  "2019032616"  "2.11.2"  TRUE)
tribits_get_version_test_case(wrong_git_version
  "Error, GIT_VERSION_STRING=2[.]6[.]1 < 2[.]10[.]0!"  "2.6.1" FALSE)
tribits_get_version_test_case(git_command_failed
  "ERROR: GIT_CMD_RTN=15 != 0!;Error Message: Command failed for some reason"
  "2.12.5"  FALSE)
