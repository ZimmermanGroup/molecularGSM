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

include(MessageWrapper)
include(TribitsSetupBasicCompileLinkFlags)
include(TribitsPackageSetupCompilerFlags)
include(UnitTestHelpers)
include(GlobalSet)


#####################################################################
#
# Unit tests for setting up compiler options
#
#####################################################################


#
# Set up unit test functions that will be called below to actually run the
# unit tests.
#
# The reason that we use functions is so that we can change variables just
# inside of the functions that have their own variable scoping.  In that way,
# we can keep variables that are set in one unit test from affecting the
# others.
#


macro(tribits_set_all_compiler_id ID)
  set(CMAKE_C_COMPILER_ID ${ID})
  set(CMAKE_CXX_COMPILER_ID ${ID})
  set(CMAKE_Fortran_COMPILER_ID ${ID})
endmacro()


macro(tribits_compile_options_common_actions)
  if (NOT PACKAGE_NAME)
    set(PACKAGE_NAME "DummyPackage")
  endif()
  tribits_setup_basic_compile_link_flags()
  tribits_setup_compiler_flags(${PACKAGE_NAME})
endmacro()


function(unitest_gcc_base_options)

  message("\n***")
  message("*** Testing GCC base compiler options")
  message("***\n")

  body_unitest_gcc_base_options()

endfunction()

macro(body_unitest_gcc_base_options)

  tribits_set_all_compiler_id(GNU)

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS
    " -pedantic -Wall -Wno-long-long -std=c99" )
  unittest_compare_const( CMAKE_CXX_FLAGS
    " -pedantic -Wall -Wno-long-long -Wwrite-strings" )
  unittest_compare_const( CMAKE_Fortran_FLAGS "" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "-O3" )

endmacro()


function(unitest_gcc_std_override_options)

  message("\n***")
  message("*** Testing GCC compiler options with override of c standard to c34")
  message("***\n")

  tribits_set_all_compiler_id(GNU)
  set(${PROJECT_NAME}_C_Standard "c34" )

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS
    " -pedantic -Wall -Wno-long-long -std=c34" )
  unittest_compare_const( CMAKE_CXX_FLAGS
    " -pedantic -Wall -Wno-long-long -Wwrite-strings" )
  unittest_compare_const( CMAKE_Fortran_FLAGS "" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "-O3" )

endfunction()


function(unitest_gcc_enable_cxx11_options)

  message("\n***")
  message("*** Testing GCC with C++11 enabled")
  message("***\n")

  # This option has been removed.  Test that it has no effect.
  set(${PROJECT_NAME}_ENABLE_CXX11 ON)

  body_unitest_gcc_base_options()

endfunction()


function(unitest_gcc_project_defined_strong_c_options)

  message("\n***")
  message("*** Testing package defined strong C compiler options")
  message("***\n")

  multiline_set(DummyProject_COMMON_STRONG_COMPILE_WARNING_FLAGS
    " -common-opt1"
    " -common-opt2"
    )

  multiline_set(DummyProject_C_STRONG_COMPILE_WARNING_FLAGS
    ${DummyProject_COMMON_STRONG_COMPILE_WARNING_FLAGS}
    " -std=cverygood"
    )

  tribits_set_all_compiler_id(GNU)

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS
    " -common-opt1 -common-opt2 -std=cverygood" )
  unittest_compare_const( CMAKE_CXX_FLAGS
    " -common-opt1 -common-opt2 -Wwrite-strings" )
  unittest_compare_const( CMAKE_Fortran_FLAGS "" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "-O3" )

endfunction()


function(unitest_gcc_project_defined_strong_cxx_options)

  message("\n***")
  message("*** Testing package defined strong CXX compiler options")
  message("***\n")

  multiline_set(DummyProject_CXX_STRONG_COMPILE_WARNING_FLAGS
    " -pedantic" # Adds more static checking to remove non-ANSI GNU extensions
    " -Wall" # Enable a bunch of default warnings
    " -Wno-long-long" # Allow long long int since it is used by MPI, SWIG, etc.
  )

  tribits_set_all_compiler_id(GNU)

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS
    " -pedantic -Wall -Wno-long-long -std=c99" )
  unittest_compare_const( CMAKE_CXX_FLAGS
    " -pedantic -Wall -Wno-long-long" )
  unittest_compare_const( CMAKE_Fortran_FLAGS "" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "-O3" )

endfunction()


function(unitest_gcc_project_defined_strong_c_cxx_options)

  message("\n***")
  message("*** Testing package defined strong C and CXX compiler options")
  message("***\n")

  multiline_set(DummyProject_C_STRONG_COMPILE_WARNING_FLAGS
    "-std=c99" # Check for C99
    " -pedantic" # Adds more static checking to remove non-ANSI GNU extensions
    " -Wall" # Enable a bunch of default warnings
    " -Wno-long-long" # Allow long long int since it is used by MPI, SWIG, etc.
  )

  multiline_set(DummyProject_CXX_STRONG_COMPILE_WARNING_FLAGS
    " -pedantic" # Adds more static checking to remove non-ANSI GNU extensions
    " -Wall" # Enable a bunch of default warnings
    " -Wno-long-long" # Allow long long int since it is used by MPI, SWIG, etc.
    " -Wwrite-strings"
  )


  tribits_set_all_compiler_id(GNU)

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS
    "-std=c99 -pedantic -Wall -Wno-long-long" )
  unittest_compare_const( CMAKE_CXX_FLAGS
    " -pedantic -Wall -Wno-long-long -Wwrite-strings" )
  unittest_compare_const( CMAKE_Fortran_FLAGS "" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "-O3" )

endfunction()


function(unitest_gcc_with_shadow_options)

  message("\n***")
  message("*** Testing GCC base compiler options with shadow warnings")
  message("***\n")

  set(CMAKE_BUILD_TYPE DEBUG)
  tribits_set_all_compiler_id(GNU)
  set(PARSE_ENABLE_SHADOWING_WARNINGS TRUE)

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS
    " -pedantic -Wall -Wno-long-long -std=c99" )
  unittest_compare_const( CMAKE_CXX_FLAGS
    " -pedantic -Wall -Wno-long-long -Wwrite-strings -Wshadow -Woverloaded-virtual" )
  unittest_compare_const( CMAKE_Fortran_FLAGS "" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "-O3" )

endfunction()


function(unitest_gcc_global_enable_shadow_options)

  message("\n***")
  message("*** Testing GCC base compiler with global enable of shadow warnings")
  message("***\n")

  set(CMAKE_BUILD_TYPE DEBUG)
  tribits_set_all_compiler_id(GNU)
  set(${PROJECT_NAME}_ENABLE_SHADOW_WARNINGS ON)
  set(PARSE_ENABLE_SHADOWING_WARNINGS OFF)

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS
    " -pedantic -Wall -Wno-long-long -std=c99" )
  unittest_compare_const( CMAKE_CXX_FLAGS
    " -pedantic -Wall -Wno-long-long -Wwrite-strings -Wshadow -Woverloaded-virtual" )
  unittest_compare_const( CMAKE_Fortran_FLAGS "" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "-O3" )

endfunction()


function(unitest_gcc_global_disable_shadow_options)

  message("\n***")
  message("*** Testing GCC base compiler with global disable of shadow warnings")
  message("***\n")

  tribits_set_all_compiler_id(GNU)
  set(${PROJECT_NAME}_ENABLE_SHADOW_WARNINGS OFF)
  set(PARSE_ENABLE_SHADOWING_WARNINGS ON)

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS
    " -pedantic -Wall -Wno-long-long -std=c99" )
  unittest_compare_const( CMAKE_CXX_FLAGS
    " -pedantic -Wall -Wno-long-long -Wwrite-strings" )
  unittest_compare_const( CMAKE_Fortran_FLAGS "" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "-O3" )

endfunction()


function(unitest_gcc_with_coverage_options)

  message("\n***")
  message("*** Testing GCC base compiler options with coverage flags")
  message("***\n")

  tribits_set_all_compiler_id(GNU)
  set(${PROJECT_NAME}_ENABLE_COVERAGE_TESTING ON)

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS
    " -pedantic -Wall -Wno-long-long -std=c99 -fprofile-arcs -ftest-coverage" )
  unittest_compare_const( CMAKE_CXX_FLAGS
    " -pedantic -Wall -Wno-long-long -Wwrite-strings -fprofile-arcs -ftest-coverage" )
  unittest_compare_const( CMAKE_Fortran_FLAGS
    "-fprofile-arcs -ftest-coverage" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "-O3" )

endfunction()


function(unitest_gcc_with_checked_stl_options)

  message("\n***")
  message("*** Testing GCC base compiler options with checked STL enables")
  message("***\n")

  tribits_set_all_compiler_id(GNU)
  set(${PROJECT_NAME}_ENABLE_CHECKED_STL ON)

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS
    " -pedantic -Wall -Wno-long-long -std=c99  -D_GLIBCXX_DEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS
    " -pedantic -Wall -Wno-long-long -Wwrite-strings  -D_GLIBCXX_DEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS " -D_GLIBCXX_DEBUG" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "-O3" )

endfunction()


function(unitest_gcc_with_cleaned_options)

  message("\n***")
  message("*** Testing GCC base compiler options warnings as errors")
  message("***\n")

  tribits_set_all_compiler_id(GNU)
  set(PARSE_CLEANED TRUE)

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS
    "--warnings_as_errors_placeholder  -pedantic -Wall -Wno-long-long -std=c99" )
  unittest_compare_const( CMAKE_CXX_FLAGS
    "--warnings_as_errors_placeholder  -pedantic -Wall -Wno-long-long -Wwrite-strings" )
  unittest_compare_const( CMAKE_Fortran_FLAGS "" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "-O3" )

endfunction()


function(unitest_gcc_no_strong_warnings_options)

  message("\n***")
  message("*** Testing GCC without strong warnings")
  message("***\n")

  tribits_set_all_compiler_id(GNU)
  set(${PROJECT_NAME}_ENABLE_STRONG_C_COMPILE_WARNINGS FALSE)
  set(${PROJECT_NAME}_ENABLE_STRONG_CXX_COMPILE_WARNINGS FALSE)
  set(${PROJECT_NAME}_ENABLE_STRONG_Fortran_COMPILE_WARNINGS FALSE)

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS "" )
  unittest_compare_const( CMAKE_CXX_FLAGS "" )
  unittest_compare_const( CMAKE_Fortran_FLAGS "" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "-O3" )

endfunction()


function(unitest_gcc_no_package_strong_warnings_options)

  message("\n***")
  message("*** Testing GCC without strong warnings for a package")
  message("***\n")

  set(PACKAGE_NAME "MyPackage")

  tribits_set_all_compiler_id(GNU)
  set(${PACKAGE_NAME}_DISABLE_STRONG_WARNINGS TRUE)

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS "" )
  unittest_compare_const( CMAKE_CXX_FLAGS "" )
  unittest_compare_const( CMAKE_Fortran_FLAGS "" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "-O3" )

endfunction()


function(unitest_gcc_package_compiler_flags)

  message("\n***")
  message("*** Testing setting package-specific compiler options")
  message("***\n")

  set(PACKAGE_NAME "MyPackage")

  tribits_set_all_compiler_id(GNU)
  set(${PACKAGE_NAME}_DISABLE_STRONG_WARNINGS TRUE)

  set(${PACKAGE_NAME}_C_FLAGS "--pkg-c-flags1 --pkg-c-flags2")
  set(${PACKAGE_NAME}_CXX_FLAGS "--pkg-cxx-flags1 --pkg-cxx-flags2")
  set(${PACKAGE_NAME}_Fortran_FLAGS "--pkg-f-flags1 --pkg-f-flags2")

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS "--pkg-c-flags1 --pkg-c-flags2" )
  unittest_compare_const( CMAKE_CXX_FLAGS "--pkg-cxx-flags1 --pkg-cxx-flags2" )
  unittest_compare_const( CMAKE_Fortran_FLAGS "--pkg-f-flags1 --pkg-f-flags2" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "-O3" )

endfunction()


function(unitest_gcc_global_and_package_compiler_flags)

  message("\n***")
  message("*** Testing setting global and package-specific compiler options")
  message("***\n")

  set(PACKAGE_NAME "MyPackage")

  tribits_set_all_compiler_id(GNU)
  set(${PACKAGE_NAME}_DISABLE_STRONG_WARNINGS TRUE)

  set(CMAKE_C_FLAGS "--global-c-flags1 --global-c-flags2")
  set(CMAKE_CXX_FLAGS "--global-cxx-flags1 --global-cxx-flags2")
  set(CMAKE_Fortran_FLAGS "--global-f-flags1 --global-f-flags2")
  set(${PACKAGE_NAME}_C_FLAGS "--pkg-c-flags1 --pkg-c-flags2")
  set(${PACKAGE_NAME}_CXX_FLAGS "--pkg-cxx-flags1 --pkg-cxx-flags2")
  set(${PACKAGE_NAME}_Fortran_FLAGS "--pkg-f-flags1 --pkg-f-flags2")

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS
    "  --global-c-flags1 --global-c-flags2 --pkg-c-flags1 --pkg-c-flags2" )
  unittest_compare_const( CMAKE_CXX_FLAGS
    "  --global-cxx-flags1 --global-cxx-flags2 --pkg-cxx-flags1 --pkg-cxx-flags2" )
  unittest_compare_const( CMAKE_Fortran_FLAGS
    "  --global-f-flags1 --global-f-flags2 --pkg-f-flags1 --pkg-f-flags2" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "-O3" )

endfunction()


function(unitest_gcc_with_shadow_cleaned_checked_stl_coverage_options)

  message("\n***")
  message("*** Testing GCC base compiler options with shadow warnings,"
    " warnings as errors, checked stl, and coverage tests")
  message("***\n")

  set(CMAKE_BUILD_TYPE DEBUG)
  tribits_set_all_compiler_id(GNU)
  set(PARSE_ENABLE_SHADOWING_WARNINGS TRUE)
  set(${PROJECT_NAME}_ENABLE_COVERAGE_TESTING ON)
  set(${PROJECT_NAME}_ENABLE_CHECKED_STL ON)
  set(PARSE_CLEANED TRUE)

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS
    "--warnings_as_errors_placeholder  -pedantic -Wall -Wno-long-long -std=c99 -fprofile-arcs -ftest-coverage -D_GLIBCXX_DEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS
    "--warnings_as_errors_placeholder  -pedantic -Wall -Wno-long-long -Wwrite-strings -Wshadow -Woverloaded-virtual -fprofile-arcs -ftest-coverage -D_GLIBCXX_DEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS "-fprofile-arcs -ftest-coverage -D_GLIBCXX_DEBUG" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "-O3" )

endfunction()


function(unitest_gcc_additional_user_options)

  message("\n***")
  message("*** Testing GCC base compiler options with user amended options")
  message("***\n")

  tribits_set_all_compiler_id(GNU)
  # These flags stay at the end of CMAKE_<LANG>_FLAGS
  set(CMAKE_C_FLAGS "--additional-user-c-flags")
  set(CMAKE_CXX_FLAGS "--additional-user-cxx-flags")
  set(CMAKE_Fortran_FLAGS "--additional-user-fortran-flags")
  # These flags don't stay in CMAKE_<LANG>_FLAGS_<BUILDTYPE>
  set(CMAKE_C_FLAGS_DEBUG "--additional-user-c-flags-dbg")
  set(CMAKE_CXX_FLAGS_DEBUG "--additional-user-cxx-flags-dbg")
  set(CMAKE_Fortran_FLAGS_DEBUG "--additional-user-fortran-flags-dbg")
  set(CMAKE_C_FLAGS_RELEASE "--additional-user-c-flags-rel")
  set(CMAKE_CXX_FLAGS_RELEASE "--additional-user-cxx-flags-rel")
  set(CMAKE_Fortran_FLAGS_RELEASE "--additional-user-fortran-flags-rel")

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS
    " -pedantic -Wall -Wno-long-long -std=c99   --additional-user-c-flags" )
  unittest_compare_const( CMAKE_CXX_FLAGS
    " -pedantic -Wall -Wno-long-long -Wwrite-strings   --additional-user-cxx-flags" )
  unittest_compare_const( CMAKE_Fortran_FLAGS "  --additional-user-fortran-flags" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "-O3" )

endfunction()


function(unitest_gcc_user_debug_override_options)

  message("\n***")
  message("*** Testing GCC with user override of debug options")
  message("***\n")

  tribits_set_all_compiler_id(GNU)

  set(CMAKE_C_FLAGS_DEBUG_OVERRIDE "--additional-user-c-flags-dbg")
  set(CMAKE_CXX_FLAGS_DEBUG_OVERRIDE "--additional-user-cxx-flags-dbg")
  set(CMAKE_Fortran_FLAGS_DEBUG_OVERRIDE "--additional-user-fortran-flags-dbg")

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "--additional-user-c-flags-dbg" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "--additional-user-cxx-flags-dbg" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "--additional-user-fortran-flags-dbg" )

endfunction()


function(unitest_gcc_user_release_override_options)

  message("\n***")
  message("*** Testing GCC with user override of release options")
  message("***\n")

  tribits_set_all_compiler_id(GNU)

  set(CMAKE_C_FLAGS_RELEASE_OVERRIDE "--additional-user-c-flags-rel")
  set(CMAKE_CXX_FLAGS_RELEASE_OVERRIDE "--additional-user-cxx-flags-rel")
  set(CMAKE_Fortran_FLAGS_RELEASE_OVERRIDE "--additional-user-fortran-flags-rel")

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "--additional-user-c-flags-rel" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "--additional-user-cxx-flags-rel" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "--additional-user-fortran-flags-rel" )

endfunction()


function(unitest_gcc_user_override_options)

  message("\n***")
  message("*** Testing GCC with fully user overridden options")
  message("***\n")

  tribits_set_all_compiler_id(GNU)
  # These flags stay at the end of CMAKE_<LANG>_FLAGS
  set(CMAKE_C_FLAGS "--additional-user-c-flags")
  set(CMAKE_CXX_FLAGS "--additional-user-cxx-flags")
  set(CMAKE_Fortran_FLAGS "--additional-user-fortran-flags")
  # These flags don't stay in CMAKE_<LANG>_FLAGS_<BUILDTYPE>
  set(CMAKE_C_FLAGS_DEBUG "--additional-user-c-flags-dbg")
  set(CMAKE_CXX_FLAGS_DEBUG "--additional-user-cxx-flags-dbg")
  set(CMAKE_Fortran_FLAGS_DEBUG "--additional-user-fortran-flags-dbg")
  set(CMAKE_C_FLAGS_RELEASE "--additional-user-c-flags-rel")
  set(CMAKE_CXX_FLAGS_RELEASE "--additional-user-cxx-flags-rel")
  set(CMAKE_Fortran_FLAGS_RELEASE "--additional-user-fortran-flags-rel")
  # Turn off internal options
  #set(CMAKE_BUILD_TYPE NONE) This just affects what CMake uses
  set(${PROJECT_NAME}_ENABLE_STRONG_C_COMPILE_WARNINGS OFF)
  set(${PROJECT_NAME}_ENABLE_STRONG_CXX_COMPILE_WARNINGS OFF)

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS
    "  --additional-user-c-flags" )
  unittest_compare_const( CMAKE_CXX_FLAGS
    "  --additional-user-cxx-flags" )
  unittest_compare_const( CMAKE_Fortran_FLAGS "  --additional-user-fortran-flags" )
  unittest_compare_const( CMAKE_C_FLAGS_NONE "" )
  unittest_compare_const( CMAKE_CXX_FLAGS_NONE "" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_NONE "" )
  # Since CMAKE_BUILD_TYPE=NONE, these are not actually used
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "-O3" )

endfunction()


function(unitest_gcc_with_debug_symboles_options)

  message("\n***")
  message("*** Testing GCC base compiler options with debut symboles")
  message("***\n")

  tribits_set_all_compiler_id(GNU)
  set(${PROJECT_NAME}_ENABLE_DEBUG_SYMBOLS ON)

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS
    " -pedantic -Wall -Wno-long-long -std=c99  -g" )
  unittest_compare_const( CMAKE_CXX_FLAGS
    " -pedantic -Wall -Wno-long-long -Wwrite-strings  -g" )
  unittest_compare_const( CMAKE_Fortran_FLAGS " -g" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "-g -O0" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "-O3" )

endfunction()


function(unitest_other_with_shadow_cleaned_checked_stl_coverage_options)

  message("\n***")
  message("*** Testing OTHER base compiler options with shadow warnings,"
    " warnings as errors, checked stl, and coverage tests")
  message("***\n")

  tribits_set_all_compiler_id(OTHER)
  set(PARSE_ENABLE_SHADOWING_WARNINGS TRUE)
  set(${PROJECT_NAME}_ENABLE_COVERAGE_TESTING ON)
  set(${PROJECT_NAME}_ENABLE_CHECKED_STL ON)
  set(PARSE_CLEANED TRUE)
  set(CMAKE_C_FLAGS "--default-c-flags")
  set(CMAKE_CXX_FLAGS "--default-cxx-flags")
  set(CMAKE_Fortran_FLAGS "--default-fortran-flags")
  set(CMAKE_C_FLAGS_DEBUG "--default-c-flags-dbg")
  set(CMAKE_CXX_FLAGS_DEBUG "--default-cxx-flags-dbg")
  set(CMAKE_Fortran_FLAGS_DEBUG "--default-fortran-flags-dbg")
  set(CMAKE_C_FLAGS_RELEASE "--default-c-flags-rel")
  set(CMAKE_CXX_FLAGS_RELEASE "--default-cxx-flags-rel")
  set(CMAKE_Fortran_FLAGS_RELEASE "--default-fortran-flags-rel")

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS "--warnings_as_errors_placeholder --default-c-flags" )
  unittest_compare_const( CMAKE_CXX_FLAGS "--warnings_as_errors_placeholder --default-cxx-flags" )
  unittest_compare_const( CMAKE_Fortran_FLAGS "--default-fortran-flags" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "--default-c-flags-dbg" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "--default-cxx-flags-dbg" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "--default-fortran-flags-dbg" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "--default-c-flags-rel" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "--default-cxx-flags-rel" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "--default-fortran-flags-rel" )

endfunction()


function(unitest_other_base_options)

  message("\n***")
  message("*** Testing OTHER base compiler options")
  message("***\n")

  tribits_set_all_compiler_id(OTHER)
  set(CMAKE_C_FLAGS "--default-c-flags")
  set(CMAKE_CXX_FLAGS "--default-cxx-flags")
  set(CMAKE_Fortran_FLAGS "--default-fortran-flags")
  set(CMAKE_C_FLAGS_DEBUG "--default-c-flags-dbg")
  set(CMAKE_CXX_FLAGS_DEBUG "--default-cxx-flags-dbg")
  set(CMAKE_Fortran_FLAGS_DEBUG "--default-fortran-flags-dbg")
  set(CMAKE_C_FLAGS_RELEASE "--default-c-flags-rel")
  set(CMAKE_CXX_FLAGS_RELEASE "--default-cxx-flags-rel")
  set(CMAKE_Fortran_FLAGS_RELEASE "--default-fortran-flags-rel")

  tribits_compile_options_common_actions()

  unittest_compare_const( CMAKE_C_FLAGS "--default-c-flags" )
  unittest_compare_const( CMAKE_CXX_FLAGS "--default-cxx-flags" )
  unittest_compare_const( CMAKE_Fortran_FLAGS "--default-fortran-flags" )
  unittest_compare_const( CMAKE_C_FLAGS_DEBUG "--default-c-flags-dbg" )
  unittest_compare_const( CMAKE_CXX_FLAGS_DEBUG "--default-cxx-flags-dbg" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_DEBUG "--default-fortran-flags-dbg" )
  unittest_compare_const( CMAKE_C_FLAGS_RELEASE "--default-c-flags-rel" )
  unittest_compare_const( CMAKE_CXX_FLAGS_RELEASE "--default-cxx-flags-rel" )
  unittest_compare_const( CMAKE_Fortran_FLAGS_RELEASE "--default-fortran-flags-rel" )

endfunction()


# OTHER with shadow, warnings as errors, checked STL, and coverage

#

# ???


#####################################################################
#
# Execute the unit tests
#
#####################################################################

unittest_initialize_vars()

# Set common/base options
set(PROJECT_NAME "DummyProject")
set(${PROJECT_NAME}_ENABLE_C TRUE)
set(${PROJECT_NAME}_ENABLE_CXX TRUE)
set(${PROJECT_NAME}_ENABLE_Fortran TRUE)
set(${PROJECT_NAME}_ENABLE_C_DEBUG_COMPILE_FLAGS TRUE)
set(${PROJECT_NAME}_ENABLE_CXX_DEBUG_COMPILE_FLAGS TRUE)
set(${PROJECT_NAME}_ENABLE_Fortran_DEBUG_COMPILE_FLAGS TRUE)
set(${PROJECT_NAME}_ENABLE_STRONG_C_COMPILE_WARNINGS TRUE)
set(${PROJECT_NAME}_ENABLE_STRONG_CXX_COMPILE_WARNINGS TRUE)
set(${PROJECT_NAME}_ENABLE_STRONG_Fortran_COMPILE_WARNINGS TRUE)
set(${PROJECT_NAME}_WARNINGS_AS_ERRORS_FLAGS "--warnings_as_errors_placeholder")
set(${PROJECT_NAME}_ENABLE_SHADOW_WARNINGS "")
set(${PROJECT_NAME}_ENABLE_COVERAGE_TESTING OFF)
set(${PROJECT_NAME}_ENABLE_CHECKED_STL OFF)
set(${PROJECT_NAME}_ENABLE_DEBUG_SYMBOLS OFF)
set(${PROJECT_NAME}_VERBOSE_CONFIGURE TRUE)
set(PARSE_DISABLE_STRONG_WARNINGS FALSE)
set(PARSE_ENABLE_SHADOWING_WARNINGS FALSE)
set(PARSE_CLEANED FALSE)
set(CMAKE_BUILD_TYPE DEBUG)

unitest_gcc_enable_cxx11_options()
unitest_gcc_base_options()
unitest_gcc_std_override_options()
unitest_gcc_project_defined_strong_c_options()
unitest_gcc_project_defined_strong_cxx_options()
unitest_gcc_project_defined_strong_c_cxx_options()
unitest_gcc_with_shadow_options()
unitest_gcc_global_enable_shadow_options()
unitest_gcc_global_disable_shadow_options()
unitest_gcc_with_coverage_options()
unitest_gcc_with_checked_stl_options()
unitest_gcc_with_cleaned_options()
unitest_gcc_no_strong_warnings_options()
unitest_gcc_no_package_strong_warnings_options()
unitest_gcc_package_compiler_flags()
unitest_gcc_global_and_package_compiler_flags()
unitest_gcc_with_shadow_cleaned_checked_stl_coverage_options()
unitest_gcc_additional_user_options()
unitest_gcc_user_debug_override_options()
unitest_gcc_user_release_override_options()
unitest_gcc_user_override_options()
unitest_gcc_with_debug_symboles_options()
unitest_other_base_options()
unitest_other_with_shadow_cleaned_checked_stl_coverage_options()

# Pass in the number of expected tests that must pass!
unittest_final_result(207)
