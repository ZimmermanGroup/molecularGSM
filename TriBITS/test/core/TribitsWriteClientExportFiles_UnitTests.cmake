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

message("CURRENT_TEST_DIRECTORY = ${CURRENT_TEST_DIRECTORY}")

include(${CMAKE_CURRENT_LIST_DIR}/TribitsAdjustPackageEnablesHelpers.cmake)
include(TribitsPackageMacros)
include(TribitsWriteClientExportFiles)


#####################################################################
#
# Unit tests for code in TribitsWriteClientExportFiles.cmake
#
#####################################################################


macro(setup_write_specialized_package_export_makefile_test_stuff)

  # These would be set the TriBITS env probing code or by CMake
  set(${PROJECT_NAME}_ENABLE_C ON)
  set(${PROJECT_NAME}_ENABLE_CXX ON)
  set(${PROJECT_NAME}_ENABLE_Fortran ON)

  # These would be set automatically by CMake if we were not in script mode!
  set(CMAKE_LINK_LIBRARY_FLAG -l)
  set(CMAKE_LIBRARY_PATH_FLAG -L)

  # Make sure this is defined!
  assert_defined(${PROJECT_NAME}_TRIBITS_DIR)

  # Need to define these:
  set(${PROJECT_NAME}_INSTALL_LIB_DIR "dummy_install_lib_dir")
  set(${PROJECT_NAME}_INSTALL_INCLUDE_DIR "dummy_install_include_dir")

endmacro()


#
# A) Test basic package processing and reading dependencies
#


function(unittest_write_specialized_package_export_makefile_rtop_before_libs)

  message("\n***")
  message("*** Testing the generation of RTOpConfig.cmake file *before* libs")
  message("***\n")

  setup_write_specialized_package_export_makefile_test_stuff()

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)
  #set(TRIBITS_WRITE_FLEXIBLE_PACKAGE_CLIENT_EXPORT_FILES_DEBUG_DUMP ON)
  set(${PROJECT_NAME}_DUMP_PACKAGE_DEPENDENCIES ON)

  set(${PROJECT_NAME}_ENABLE_RTOp ON)
  set(${PROJECT_NAME}_GENERATE_EXPORT_FILE_DEPENDENCIES ON)

  unittest_helper_read_and_process_packages()

  # These are basic global VARS we want to pass along
  set(CMAKE_BUILD_TYPE DEBUG)

  # These vars would be set up by the FindTPL<TPLNAME>.cmake modules if they
  # were called
  set(TPL_BLAS_LIBRARIES "blaspath/lib/libblas.a")
  set(TPL_BLAS_LIBRARY_DIRS "blashpath/lib")
  set(TPL_BLAS_INCLUDE_DIRS "blaspath/include")
  set(TPL_LAPACK_LIBRARIES "lapackpath/lib/liblapack.a")
  set(TPL_LAPACK_LIBRARY_DIRS "lapackhpath/lib")
  set(TPL_LAPACK_INCLUDE_DIRS "lapackhpath/include")

  # These vars should be generated automatically by tribits_package() that
  # begins with the upstreams packages.
  set(Teuchos_LIBRARY_DIRS "teuchos/core/src;teuchos/numeric/src")
  set(Teuchos_INCLUDE_DIRS "teuchos/core/include;teuchos/numeric/include")
  set(Teuchos_LIBRARIES "teuchoscore;teuchosnumeric")
  set(Teuchos_HAS_NATIVE_LIBRARIES_TO_INSTALL TRUE)

  set(GENERATED_EXPORT_CONFIG_FOR_BUILD_BASE_DIR
    "${CURRENT_TEST_DIRECTORY}/RTOpBeforeConfig_for_build" )
  set(GENERATED_EXPORT_CONFIG_FOR_BUILD
    "${GENERATED_EXPORT_CONFIG_FOR_BUILD_BASE_DIR}/RTOpConfig.cmake")
  set(GENERATED_EXPORT_CONFIG_FOR_INSTALL_BASE_DIR
    "${CURRENT_TEST_DIRECTORY}/RTOpBeforeConfig_for_install" )
  set(GENERATED_EXPORT_CONFIG_FOR_INSTALL
    "${GENERATED_EXPORT_CONFIG_FOR_INSTALL_BASE_DIR}/RTOpConfig_install.cmake")

  tribits_write_flexible_package_client_export_files(
    PACKAGE_NAME RTOp
    EXPORT_FILE_VAR_PREFIX RTOp1
    PACKAGE_CONFIG_FOR_BUILD_BASE_DIR
      "${GENERATED_EXPORT_CONFIG_FOR_BUILD_BASE_DIR}"
    PACKAGE_CONFIG_FOR_INSTALL_BASE_DIR
      "${GENERATED_EXPORT_CONFIG_FOR_INSTALL_BASE_DIR}"
    )

  unittest_file_regex("${GENERATED_EXPORT_CONFIG_FOR_BUILD}"
    REGEX_STRINGS
      "set[(]RTOp1_CMAKE_BUILD_TYPE .DEBUG."
      "set[(]RTOp1_INCLUDE_DIRS ..[)]"
      "set[(]RTOp1_LIBRARY_DIRS ..[)]"
      "set[(]RTOp1_LIBRARIES .teuchoscore.teuchosnumeric.[)]"
      "set[(]RTOp1_TPL_INCLUDE_DIRS ..[)]"
      "set[(]RTOp1_TPL_LIBRARY_DIRS ..[)]"
      "set[(]RTOp1_TPL_LIBRARIES .LAPACK::all_libs[;]BLAS::all_libs.[)]"
      "set[(]RTOp1_PACKAGE_LIST .Teuchos.[)]"
      "set[(]RTOp1_TPL_LIST .LAPACK.BLAS.[)]"
      "set[(]RTOp1_ENABLE_Teuchos ON[)]"

    )

  unittest_file_regex("${GENERATED_EXPORT_CONFIG_FOR_INSTALL}"
    REGEX_STRINGS
      "set[(]RTOp1_CMAKE_BUILD_TYPE .DEBUG."
      "set[(]RTOp1_INCLUDE_DIRS ..[)]"
      "set[(]RTOp1_LIBRARY_DIRS ..[)]"
      "set[(]RTOp1_LIBRARIES .teuchoscore.teuchosnumeric.[)]"
      "set[(]RTOp1_TPL_INCLUDE_DIRS ..[)]"
      "set[(]RTOp1_TPL_LIBRARY_DIRS ..[)]"
      "set[(]RTOp1_TPL_LIBRARIES .LAPACK::all_libs[;]BLAS::all_libs.[)]"
      "set[(]RTOp1_PACKAGE_LIST .Teuchos.[)]"
      "set[(]RTOp1_TPL_LIST .LAPACK.BLAS.[)]"
      "set[(]RTOp1_ENABLE_Teuchos ON[)]"
    )

endfunction()


function(unittest_write_specialized_package_export_makefile_rtop_after_libs)

  message("\n***")
  message("*** Testing the generation RTOpConfig.cmake *after* libs")
  message("***\n")

  setup_write_specialized_package_export_makefile_test_stuff()

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)
  #set(TRIBITS_WRITE_FLEXIBLE_PACKAGE_CLIENT_EXPORT_FILES_DEBUG_DUMP ON)

  set(${PROJECT_NAME}_ENABLE_RTOp ON)
  set(${PROJECT_NAME}_GENERATE_EXPORT_FILE_DEPENDENCIES ON)

  unittest_helper_read_and_process_packages()

  # These are basic global VARS we want to pass along
  set(CMAKE_BUILD_TYPE RELEASE)

  # These vars would be set up by the FindTPL<TPLNAME>.cmake modules if they
  # were called
  set(TPL_BLAS_LIBRARIES "blaspath/lib/libblas.a")
  set(TPL_BLAS_LIBRARY_DIRS "blashpath/lib")
  set(TPL_BLAS_INCLUDE_DIRS "blaspath/include")
  set(TPL_LAPACK_LIBRARIES "lapackpath/lib/liblapack.a")
  set(TPL_LAPACK_LIBRARY_DIRS "lapackhpath/lib")
  set(TPL_LAPACK_INCLUDE_DIRS "lapackhpath/include")

  # These vars should be generated automatically by tribits_package() that
  # begins with the upstreams packages.
  set(Teuchos_LIBRARY_DIRS "teuchos/core/src;teuchos/numeric/src")
  set(Teuchos_INCLUDE_DIRS "teuchos/core/include;teuchos/numeric/include")
  set(Teuchos_LIBRARIES "teuchoscore;teuchosnumeric")
  set(Teuchos_HAS_NATIVE_LIBRARIES_TO_INSTALL TRUE)
  set(RTOp_LIBRARY_DIRS "rtop/src;teuchos/core/src;teuchos/numeric/src")
  set(RTOp_INCLUDE_DIRS "rtop/include;teuchos/core/include;teuchos/numeric/include")
  set(RTOp_LIBRARIES "rtop")
  set(RTOp_HAS_NATIVE_LIBRARIES_TO_INSTALL TRUE)

  set(GENERATED_EXPORT_CONFIG_FOR_BUILD_BASE_DIR
    "${CURRENT_TEST_DIRECTORY}/RTOpAfterConfig_for_build" )
  set(GENERATED_EXPORT_CONFIG_FOR_BUILD
    "${GENERATED_EXPORT_CONFIG_FOR_BUILD_BASE_DIR}/RTOpConfig.cmake")
  set(GENERATED_EXPORT_CONFIG_FOR_INSTALL_BASE_DIR
    "${CURRENT_TEST_DIRECTORY}/RTOpAfterConfig_for_install" )
  set(GENERATED_EXPORT_CONFIG_FOR_INSTALL
    "${GENERATED_EXPORT_CONFIG_FOR_INSTALL_BASE_DIR}/RTOpConfig_install.cmake")

  tribits_write_flexible_package_client_export_files(
    PACKAGE_NAME RTOp
    EXPORT_FILE_VAR_PREFIX RTOp2
    PACKAGE_CONFIG_FOR_BUILD_BASE_DIR
      "${GENERATED_EXPORT_CONFIG_FOR_BUILD_BASE_DIR}"
    PACKAGE_CONFIG_FOR_INSTALL_BASE_DIR
      "${GENERATED_EXPORT_CONFIG_FOR_INSTALL_BASE_DIR}"
    )

  unittest_file_regex("${GENERATED_EXPORT_CONFIG_FOR_BUILD}"
    REGEX_STRINGS
      "set[(]RTOp2_CMAKE_BUILD_TYPE .RELEASE."
      "set[(]RTOp2_INCLUDE_DIRS ..[)]"
      "set[(]RTOp2_LIBRARY_DIRS ..[)]"
      "set[(]RTOp2_LIBRARIES .rtop.teuchoscore.teuchosnumeric.[)]"
      "set[(]RTOp2_TPL_INCLUDE_DIRS ..[)]"
      "set[(]RTOp2_TPL_LIBRARY_DIRS ..[)]"
      "set[(]RTOp2_TPL_LIBRARIES .LAPACK::all_libs[;]BLAS::all_libs.[)]"
      "set[(]RTOp2_PACKAGE_LIST .RTOp.Teuchos.[)]"
      "set[(]RTOp2_TPL_LIST .LAPACK.BLAS.[)]"
      "set[(]RTOp2_ENABLE_Teuchos ON[)]"
    )

  # ToDo: Check the generated RTOpConfig_install.cmake file!

endfunction()


#####################################################################
#
# Execute the unit tests
#
#####################################################################

unittest_initialize_vars()

#
# Run the unit tests
#

unittest_write_specialized_package_export_makefile_rtop_before_libs()
unittest_write_specialized_package_export_makefile_rtop_after_libs()

# Pass in the number of expected tests that must pass!
unittest_final_result(30)
