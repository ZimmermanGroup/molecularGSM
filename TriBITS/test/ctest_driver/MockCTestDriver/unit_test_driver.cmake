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
# This is a helper CTest script that is used to drive unit testing of the
# TribitsCTestDriverCore.cmake script.
#
# NOTE: Some variables need to be set in the calling script in order to
# override options set in the environment from the parent TriBITS project run
# of TribitsCTestDriverCore.cmake
#

#
# A) General setup code:
#
# Do not modify any of this directly, use use environment variables instead!
#

message("CTEST_SCRIPT_DIRECTORY = '${CTEST_SCRIPT_DIRECTORY}'")

# The mock test project
set(MOCK_PROJECT_NAME Trilinos)

get_filename_component(${MOCK_PROJECT_NAME}_TRIBITS_DIR
  "${CTEST_SCRIPT_DIRECTORY}/../../../tribits" ABSOLUTE)
message("${MOCK_PROJECT_NAME}_TRIBITS_DIR = '${${MOCK_PROJECT_NAME}_TRIBITS_DIR}'")

set(TRIBITS_PROJECT_ROOT "${${MOCK_PROJECT_NAME}_TRIBITS_DIR}/examples/MockTrilinos")

set( CMAKE_MODULE_PATH
  "${${MOCK_PROJECT_NAME}_TRIBITS_DIR}/core/utils"  # To find general support macros
  "${${MOCK_PROJECT_NAME}_TRIBITS_DIR}/ctest_driver"  # To find TrilinosCMakeCoreDriver.cmake
  )

include(TribitsCTestDriverCore)


#
# B) Override some configuration variables
#

# All these can be changed by env vars
set(CTEST_TEST_TYPE Experimental)
#set(CTEST_DO_UPDATES FALSE)
set(${MOCK_PROJECT_NAME}_WARNINGS_AS_ERRORS_FLAGS "-DummyErrFlags")

# Don't change these in the env!
set(CTEST_START_WITH_EMPTY_BINARY_DIRECTORY FALSE)
set(CTEST_GENERATE_DEPS_XML_OUTPUT_FILE TRUE)
set(CTEST_WIPE_CACHE FALSE)

set(CTEST_SOURCE_DIRECTORY "${TRIBITS_PROJECT_ROOT}")
get_filename_component(PWD . REALPATH)
set(CTEST_BINARY_DIRECTORY "${PWD}")
set(BUILD_DIR_NAME UnitTests)
set(${MOCK_PROJECT_NAME}_ENABLE_DEVELOPMENT_MODE ON)

set_default_and_from_env(${MOCK_PROJECT_NAME}_CTEST_COMMAND ctest)
set(CTEST_COMMAND ${${MOCK_PROJECT_NAME}_CTEST_COMMAND})

set_default_and_from_env(${MOCK_PROJECT_NAME}_EXCLUDE_TPLS "")
foreach(EXCLUDE_TPL ${${MOCK_PROJECT_NAME}_EXCLUDE_TPLS})
  set(TPL_ENABLE_${EXCLUDE_TPL} OFF)
endforeach()


#
# C) Run the build/test/submit driver
#

tribits_ctest_driver()
