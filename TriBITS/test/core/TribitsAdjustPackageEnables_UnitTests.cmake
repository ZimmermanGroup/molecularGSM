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

include(${CMAKE_CURRENT_LIST_DIR}/TribitsAdjustPackageEnablesHelpers.cmake)


#####################################################################
#
# Unit tests for code in TribitsAdjustPackageEnables.cmake
#
#####################################################################


#
# A) Test enabled/disable logic
#


function(unittest_enable_no_packages)

  message("\n***")
  message("*** Test enabling no packages (the default)")
  message("***\n")

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)

  unittest_helper_read_and_process_packages()

  unittest_compare_const(${PROJECT_NAME}_TPLS "MPI;BLAS;LAPACK;Boost")
  unittest_compare_const(${PROJECT_NAME}_NUM_TPLS 4)
  unittest_compare_const(${PROJECT_NAME}_PACKAGES "Teuchos;RTOp;Ex2Package1;Ex2Package2")
  unittest_compare_const(${PROJECT_NAME}_NUM_PACKAGES 4)

#  unittest_compare_const(${PROJECT_NAME}_DEFINED_TPLS "MPI;BLAS;LAPACK;Boost")
#  unittest_compare_const(${PROJECT_NAME}_NUM_DEFINED_TPLS 4)
#  unittest_compare_const(${PROJECT_NAME}_DEFINED_INTERNAL_PACKAGES "Teuchos;RTOp;Ex2Package1;Ex2Package2")
#  unittest_compare_const(${PROJECT_NAME}_NUM_DEFINED_INTERNAL_PACKAGES 4)
#  unittest_compare_const(${PROJECT_NAME}_ALL_DEFINED_PACKAGES "MPI;BLAS;LAPACK;Boost;Teuchos;RTOp;Ex2Package1;Ex2Package2")
#  unittest_compare_const(${PROJECT_NAME}_NUM_ALL_DEFINED_PACKAGES 8)

  unittest_compare_const(TPL_ENABLE_MPI "")
  unittest_compare_const(TPL_ENABLE_BLAS "")
  unittest_compare_const(TPL_ENABLE_LAPACK "")
  unittest_compare_const(TPL_ENABLE_Boost "")
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Teuchos "")
  unittest_compare_const(${PROJECT_NAME}_ENABLE_RTOp "")
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package1 "")
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package2 "")

  unittest_compare_const(MPI_PACKAGE_BUILD_STATUS "EXTERNAL")
  unittest_compare_const(BLAS_PACKAGE_BUILD_STATUS "EXTERNAL")
  unittest_compare_const(LAPACK_PACKAGE_BUILD_STATUS "EXTERNAL")
  unittest_compare_const(Boost_PACKAGE_BUILD_STATUS "EXTERNAL")

  unittest_compare_const(Teuchos_PACKAGE_BUILD_STATUS "INTERNAL")
  unittest_compare_const(Teuchos_LIB_ENABLED_DEPENDENCIES "")
  unittest_compare_const(Teuchos_LIB_ALL_DEPENDENCIES "BLAS;LAPACK;Boost;MPI")

  unittest_compare_const(RTOp_PACKAGE_BUILD_STATUS "INTERNAL")
  unittest_compare_const(RTOp_LIB_ENABLED_DEPENDENCIES "")
  unittest_compare_const(RTOp_LIB_ALL_DEPENDENCIES "Teuchos")

  unittest_compare_const(Ex2Package1_PACKAGE_BUILD_STATUS "INTERNAL")
  unittest_compare_const(Ex2Package1_LIB_ENABLED_DEPENDENCIES "")
  unittest_compare_const(Ex2Package1_LIB_ALL_DEPENDENCIES "Teuchos;Boost")

  unittest_compare_const(Ex2Package2_PACKAGE_BUILD_STATUS "INTERNAL")
  unittest_compare_const(Ex2Package2_LIB_ENABLED_DEPENDENCIES "")
  unittest_compare_const(Ex2Package2_LIB_ALL_DEPENDENCIES "Teuchos;Ex2Package1")

endfunction()


function(unittest_enable_all_packages)

  message("\n***")
  message("*** Test enabling all packages")
  message("***\n")

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)
  #set(TRIBITS_PROCESS_PACKAGES_AND_DIRS_LISTS_VERBOSE ON)

  set(${PROJECT_NAME}_ENABLE_ALL_PACKAGES ON)

  unittest_helper_read_and_process_packages()

  unittest_compare_const(${PROJECT_NAME}_NUM_PACKAGES 4)
  unittest_compare_const(${PROJECT_NAME}_NUM_TPLS 4)
  unittest_compare_const(TPL_ENABLE_MPI "")
  unittest_compare_const(TPL_ENABLE_BLAS ON)
  unittest_compare_const(TPL_ENABLE_LAPACK ON)
  unittest_compare_const(TPL_ENABLE_Boost ON)
  unittest_compare_const(TPL_ENABLE_MPI "")
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Teuchos ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_RTOp ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package1 ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package2 "")

  unittest_compare_const(Teuchos_LIB_ENABLED_DEPENDENCIES "BLAS;LAPACK;Boost")
  unittest_compare_const(Teuchos_LIB_ALL_DEPENDENCIES "BLAS;LAPACK;Boost;MPI")

  unittest_compare_const(RTOp_LIB_ENABLED_DEPENDENCIES "Teuchos")
  unittest_compare_const(RTOp_LIB_ALL_DEPENDENCIES "Teuchos")

  unittest_compare_const(Ex2Package1_LIB_ENABLED_DEPENDENCIES "Teuchos;Boost")
  unittest_compare_const(Ex2Package1_LIB_ALL_DEPENDENCIES "Teuchos;Boost")

  unittest_compare_const(Ex2Package2_LIB_ENABLED_DEPENDENCIES "")
  unittest_compare_const(Ex2Package2_LIB_ALL_DEPENDENCIES "Teuchos;Ex2Package1")

endfunction()


function(unittest_enable_all_packages_st)

  message("\n***")
  message("*** Test enabling all secondary tested packages")
  message("***\n")

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)

  set(${PROJECT_NAME}_ENABLE_ALL_PACKAGES ON)
  set(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE ON)

  unittest_helper_read_and_process_packages()

  unittest_compare_const(${PROJECT_NAME}_NUM_PACKAGES 4)
  unittest_compare_const(${PROJECT_NAME}_NUM_TPLS 4)
  unittest_compare_const(TPL_ENABLE_MPI "")
  unittest_compare_const(TPL_ENABLE_BLAS ON)
  unittest_compare_const(TPL_ENABLE_LAPACK ON)
  unittest_compare_const(TPL_ENABLE_Boost ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Teuchos ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_RTOp ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package1 ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package2 ON)

  unittest_compare_const(Teuchos_ENABLE_BLAS ON)
  unittest_compare_const(Teuchos_ENABLE_LAPACK ON)
  unittest_compare_const(Teuchos_ENABLE_Boost ON)
  unittest_compare_const(Teuchos_ENABLE_MPI "")
  unittest_compare_const(Teuchos_LIB_ENABLED_DEPENDENCIES "BLAS;LAPACK;Boost")
  unittest_compare_const(Teuchos_LIB_ALL_DEPENDENCIES "BLAS;LAPACK;Boost;MPI")
  unittest_compare_const(Teuchos_TEST_ENABLED_DEPENDENCIES "")
  unittest_compare_const(Teuchos_TEST_ALL_DEPENDENCIES "")

  unittest_compare_const(RTOp_ENABLE_Teuchos ON)
  unittest_compare_const(RTOp_LIB_ENABLED_DEPENDENCIES "Teuchos")
  unittest_compare_const(RTOp_LIB_ALL_DEPENDENCIES "Teuchos")
  unittest_compare_const(RTOp_TEST_ENABLED_DEPENDENCIES "")
  unittest_compare_const(RTOp_TEST_ALL_DEPENDENCIES "")

  unittest_compare_const(Ex2Package1_ENABLE_Teuchos ON)
  unittest_compare_const(Ex2Package1_ENABLE_Boost ON)
  unittest_compare_const(Ex2Package1_LIB_ENABLED_DEPENDENCIES "Teuchos;Boost")
  unittest_compare_const(Ex2Package1_LIB_ALL_DEPENDENCIES "Teuchos;Boost")
  unittest_compare_const(Ex2Package1_TEST_ENABLED_DEPENDENCIES "")
  unittest_compare_const(Ex2Package1_TEST_ALL_DEPENDENCIES "")

  unittest_compare_const(Ex2Package2_ENABLE_Teuchos ON)
  unittest_compare_const(Ex2Package2_ENABLE_Ex2Package1 ON)
  unittest_compare_const(Ex2Package2_LIB_ENABLED_DEPENDENCIES "Teuchos;Ex2Package1")
  unittest_compare_const(Ex2Package2_LIB_ALL_DEPENDENCIES "Teuchos;Ex2Package1")
  unittest_compare_const(Ex2Package2_TEST_ENABLED_DEPENDENCIES "")
  unittest_compare_const(Ex2Package2_TEST_ALL_DEPENDENCIES "")

endfunction()


function(unittest_enable_all_packages_st_extra_test_deps)

  message("\n***")
  message("*** Test enabling all secondary tested packages and extra Packag2 test deps")
  message("***\n")

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)

  set(${PROJECT_NAME}_ENABLE_ALL_PACKAGES ON)
  set(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE ON)
  set(EXTRA_REPO_PACKAGE2_ADD_TEST_DEPS ON)
  set(TPL_ENABLE_MPI TRUE)  # Allow testing of those if-statments

  unittest_helper_read_and_process_packages()

  unittest_compare_const(${PROJECT_NAME}_NUM_PACKAGES 4)
  unittest_compare_const(${PROJECT_NAME}_NUM_TPLS 4)
  unittest_compare_const(TPL_ENABLE_MPI TRUE)
  unittest_compare_const(TPL_ENABLE_BLAS ON)
  unittest_compare_const(TPL_ENABLE_LAPACK ON)
  unittest_compare_const(TPL_ENABLE_Boost ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Teuchos ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_RTOp ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package1 ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package2 ON)

  unittest_compare_const(Teuchos_LIB_ENABLED_DEPENDENCIES "BLAS;LAPACK;Boost;MPI")
  unittest_compare_const(Teuchos_LIB_ALL_DEPENDENCIES "BLAS;LAPACK;Boost;MPI")
  unittest_compare_const(Teuchos_TEST_ENABLED_DEPENDENCIES "")
  unittest_compare_const(Teuchos_TEST_ALL_DEPENDENCIES "")

  unittest_compare_const(RTOp_LIB_ENABLED_DEPENDENCIES "Teuchos")
  unittest_compare_const(RTOp_LIB_ALL_DEPENDENCIES "Teuchos")
  unittest_compare_const(RTOp_TEST_ENABLED_DEPENDENCIES "")
  unittest_compare_const(RTOp_TEST_ALL_DEPENDENCIES "")

  unittest_compare_const(Ex2Package1_LIB_ENABLED_DEPENDENCIES "Teuchos;Boost")
  unittest_compare_const(Ex2Package1_LIB_ALL_DEPENDENCIES "Teuchos;Boost")
  unittest_compare_const(Ex2Package1_TEST_ENABLED_DEPENDENCIES "")
  unittest_compare_const(Ex2Package1_TEST_ALL_DEPENDENCIES "")

  unittest_compare_const(Ex2Package2_LIB_ENABLED_DEPENDENCIES "Teuchos;Ex2Package1")
  unittest_compare_const(Ex2Package2_LIB_ALL_DEPENDENCIES "Teuchos;Ex2Package1")
  unittest_compare_const(Ex2Package2_TEST_ENABLED_DEPENDENCIES "")
  unittest_compare_const(Ex2Package2_TEST_ALL_DEPENDENCIES
    "Teuchos;RTOp;Ex2Package1;Boost;MPI;Boost")

endfunction()


function(unittest_enable_all_packages_st_enable_tests_extra_test_deps)

  message("\n***")
  message("*** Test enabling all secondary tested packages and extra Packag2 test deps")
  message("***\n")

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)

  set(${PROJECT_NAME}_ENABLE_ALL_PACKAGES ON)
  set(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE ON)
  set(${PROJECT_NAME}_ENABLE_TESTS ON)
  set(EXTRA_REPO_PACKAGE2_ADD_TEST_DEPS ON)
  set(TPL_ENABLE_MPI TRUE)  # Allow testing of those if-statments

  unittest_helper_read_and_process_packages()

  unittest_compare_const(${PROJECT_NAME}_NUM_PACKAGES 4)
  unittest_compare_const(${PROJECT_NAME}_NUM_TPLS 4)
  unittest_compare_const(TPL_ENABLE_MPI TRUE)
  unittest_compare_const(TPL_ENABLE_BLAS ON)
  unittest_compare_const(TPL_ENABLE_LAPACK ON)
  unittest_compare_const(TPL_ENABLE_Boost ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Teuchos ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_RTOp ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package1 ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package2 ON)

  unittest_compare_const(Teuchos_LIB_ENABLED_DEPENDENCIES "BLAS;LAPACK;Boost;MPI")
  unittest_compare_const(Teuchos_LIB_ALL_DEPENDENCIES "BLAS;LAPACK;Boost;MPI")
  unittest_compare_const(Teuchos_TEST_ENABLED_DEPENDENCIES "")
  unittest_compare_const(Teuchos_TEST_ALL_DEPENDENCIES "")

  unittest_compare_const(RTOp_LIB_ENABLED_DEPENDENCIES "Teuchos")
  unittest_compare_const(RTOp_LIB_ALL_DEPENDENCIES "Teuchos")
  unittest_compare_const(RTOp_TEST_ENABLED_DEPENDENCIES "")
  unittest_compare_const(RTOp_TEST_ALL_DEPENDENCIES "")

  unittest_compare_const(Ex2Package1_LIB_ENABLED_DEPENDENCIES "Teuchos;Boost")
  unittest_compare_const(Ex2Package1_LIB_ALL_DEPENDENCIES "Teuchos;Boost")
  unittest_compare_const(Ex2Package1_TEST_ENABLED_DEPENDENCIES "")
  unittest_compare_const(Ex2Package1_TEST_ALL_DEPENDENCIES "")

  unittest_compare_const(Ex2Package2_ENABLE_Teuchos ON)
  unittest_compare_const(Ex2Package2_ENABLE_Ex2Package1 ON)
  unittest_compare_const(Ex2Package2_ENABLE_RTOp "") # We don't set for TEST deps
  unittest_compare_const(Ex2Package2_ENABLE_Boost "") # ""
  unittest_compare_const(Ex2Package2_ENABLE_MPI "") # ""
  unittest_compare_const(Ex2Package2_LIB_ENABLED_DEPENDENCIES "Teuchos;Ex2Package1")
  unittest_compare_const(Ex2Package2_LIB_ALL_DEPENDENCIES "Teuchos;Ex2Package1")
  unittest_compare_const(Ex2Package2_TEST_ENABLED_DEPENDENCIES
    "Teuchos;RTOp;Ex2Package1;Boost;MPI;Boost")
  unittest_compare_const(Ex2Package2_TEST_ALL_DEPENDENCIES
    "Teuchos;RTOp;Ex2Package1;Boost;MPI;Boost")

endfunction()


#
# B) Test generation of export file information
#


function(unittest_enable_all_generate_export_deps)

  message("\n***")
  message("*** Test generation of export dependencies enabling all PT")
  message("***\n")

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)
  #set(${PROJECT_NAME}_DUMP_PACKAGE_DEPENDENCIES ON)

  set(${PROJECT_NAME}_ENABLE_ALL_PACKAGES ON)
  set(${PROJECT_NAME}_GENERATE_EXPORT_FILE_DEPENDENCIES ON)

  unittest_helper_read_and_process_packages()

  unittest_compare_const(${PROJECT_NAME}_NUM_PACKAGES 4)
  unittest_compare_const(${PROJECT_NAME}_NUM_TPLS 4)
  unittest_compare_const(Teuchos_FULL_ENABLED_DEP_PACKAGES "")
  unittest_compare_const(RTOp_FULL_ENABLED_DEP_PACKAGES "Teuchos")
  unittest_compare_const(Ex2Package1_FULL_ENABLED_DEP_PACKAGES "Teuchos")
  unittest_compare_const(Ex2Package2_FULL_ENABLED_DEP_PACKAGES "")

endfunction()


function(unittest_enable_all_st_generate_export_deps)

  message("\n***")
  message("*** Test generation of export dependencies enabling all ST")
  message("***\n")

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)
  #set(${PROJECT_NAME}_DUMP_PACKAGE_DEPENDENCIES ON)

  set(${PROJECT_NAME}_ENABLE_ALL_PACKAGES ON)
  set(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE ON)
  set(${PROJECT_NAME}_GENERATE_EXPORT_FILE_DEPENDENCIES ON)

  unittest_helper_read_and_process_packages()

  unittest_compare_const(${PROJECT_NAME}_NUM_PACKAGES 4)
  unittest_compare_const(${PROJECT_NAME}_NUM_TPLS 4)
  unittest_compare_const(Teuchos_FULL_ENABLED_DEP_PACKAGES "")
  unittest_compare_const(RTOp_FULL_ENABLED_DEP_PACKAGES "Teuchos")
  unittest_compare_const(Ex2Package1_FULL_ENABLED_DEP_PACKAGES "Teuchos")
  unittest_compare_const(Ex2Package2_FULL_ENABLED_DEP_PACKAGES "Ex2Package1;Teuchos")

endfunction()


function(unittest_enable_all_st_generate_export_deps_only_ex2package1)

  message("\n***")
  message("*** Test generation of export dependencies only up to Ex2Package1")
  message("***\n")

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)
  #set(${PROJECT_NAME}_DUMP_PACKAGE_DEPENDENCIES ON)

  set(${PROJECT_NAME}_ENABLE_ALL_PACKAGES ON)
  set(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE ON)
  set(${PROJECT_NAME}_GENERATE_EXPORT_FILE_DEPENDENCIES ON)
  set(${PROJECT_NAME}_GENERATE_EXPORT_FILES_FOR_ONLY_LISTED_SE_PACKAGES
    Ex2Package1 RTOp)

  unittest_helper_read_and_process_packages()

  unittest_compare_const(${PROJECT_NAME}_NUM_PACKAGES 4)
  unittest_compare_const(${PROJECT_NAME}_NUM_TPLS 4)
  unittest_compare_const(Teuchos_FULL_ENABLED_DEP_PACKAGES "")
  unittest_compare_const(RTOp_FULL_ENABLED_DEP_PACKAGES "Teuchos")
  unittest_compare_const(Ex2Package1_FULL_ENABLED_DEP_PACKAGES "Teuchos")
  unittest_compare_const(Ex2Package2_FULL_ENABLED_DEP_PACKAGES "")

endfunction()


function(unittest_enable_rtop_generate_export_deps_only_ex2package1)

  message("\n***")
  message("*** Test generation of export dependencies only up to Ex2Package1, only enable RTOp")
  message("***\n")

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)
  #set(${PROJECT_NAME}_DUMP_PACKAGE_DEPENDENCIES ON)

  set(${PROJECT_NAME}_ENABLE_RTOp ON)
  set(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE ON)
  set(${PROJECT_NAME}_GENERATE_EXPORT_FILE_DEPENDENCIES ON)
  set(${PROJECT_NAME}_GENERATE_EXPORT_FILES_FOR_ONLY_LISTED_SE_PACKAGES
    Ex2Package1 RTOp)

  unittest_helper_read_and_process_packages()

  unittest_compare_const(${PROJECT_NAME}_NUM_PACKAGES 4)
  unittest_compare_const(${PROJECT_NAME}_NUM_TPLS 4)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_RTOp "ON")
  unittest_compare_const(Teuchos_FULL_ENABLED_DEP_PACKAGES "")
  unittest_compare_const(RTOp_FULL_ENABLED_DEP_PACKAGES "Teuchos")
  unittest_compare_const(Ex2Package1_FULL_ENABLED_DEP_PACKAGES "")
  unittest_compare_const(Ex2Package2_FULL_ENABLED_DEP_PACKAGES "")

endfunction()


function(unittest_enable_teuchos_generate_export_deps_only_ex2package1)

  message("\n***")
  message("*** Test generation of export dependencies only up to Ex2Package1, only enable Teuchos")
  message("***\n")

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)
  #set(${PROJECT_NAME}_DUMP_PACKAGE_DEPENDENCIES ON)

  set(${PROJECT_NAME}_ENABLE_Teuchos ON)
  set(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE ON)
  set(${PROJECT_NAME}_GENERATE_EXPORT_FILE_DEPENDENCIES ON)
  set(${PROJECT_NAME}_GENERATE_EXPORT_FILES_FOR_ONLY_LISTED_SE_PACKAGES
    Ex2Package1 RTOp)

  unittest_helper_read_and_process_packages()

  unittest_compare_const(${PROJECT_NAME}_NUM_PACKAGES 4)
  unittest_compare_const(${PROJECT_NAME}_NUM_TPLS 4)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Teuchos "ON")
  unittest_compare_const(Teuchos_FULL_ENABLED_DEP_PACKAGES "")
  unittest_compare_const(RTOp_FULL_ENABLED_DEP_PACKAGES "")
  unittest_compare_const(Ex2Package1_FULL_ENABLED_DEP_PACKAGES "")
  unittest_compare_const(Ex2Package2_FULL_ENABLED_DEP_PACKAGES "")

endfunction()


#
# C) Test primary meta-project package enable/disable logic
#


function(unittest_enable_tribits_is_primary_meta_project_package)

  message("\n***")
  message("*** Unit testing primary meta-project packages, no Trilinos, enable all")
  message("***\n")

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)
  #set(TRIBITS_PROCESS_PACKAGES_AND_DIRS_LISTS_VERBOSE ON)

  set(${PROJECT_NAME}_ENABLE_ALL_PACKAGES ON)
  #set(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE ON)
  set(Trilinos_NO_PRIMARY_META_PROJECT_PACKAGES ON)

  unittest_helper_read_and_process_packages()

  tribits_is_primary_meta_project_package(Teuchos  Teuchos_is_PMP)
  unittest_compare_const(Teuchos_is_PMP  FALSE)

  tribits_is_primary_meta_project_package(RTOp  RTOp_is_PMP)
  unittest_compare_const(RTOp_is_PMP  FALSE)

  tribits_is_primary_meta_project_package(Ex2Package1  Ex2Package1_is_PMP)
  unittest_compare_const(Ex2Package1_is_PMP  TRUE)

  tribits_is_primary_meta_project_package(Ex2Package2  Ex2Package2_is_PMP)
  unittest_compare_const(Ex2Package2_is_PMP  TRUE)

  unittest_compare_const(${PROJECT_NAME}_NUM_PACKAGES 4)
  unittest_compare_const(${PROJECT_NAME}_NUM_TPLS 4)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Teuchos ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_RTOp "")
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package1 ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package2 "")

endfunction()


function(unittest_enable_tribits_is_primary_meta_project_package_exclude_rtop_st)

  message("\n***")
  message("*** Unit testing primary meta-project packages, no Trilinos, exclude RTOp, enable all ST")
  message("***\n")

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)
  #set(TRIBITS_PROCESS_PACKAGES_AND_DIRS_LISTS_VERBOSE ON)

  set(${PROJECT_NAME}_ENABLE_ALL_PACKAGES ON)
  set(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE ON)
  set(Trilinos_NO_PRIMARY_META_PROJECT_PACKAGES ON)
  set(Trilinos_NO_PRIMARY_META_PROJECT_PACKAGES_EXCEPT  RTOp)

  unittest_helper_read_and_process_packages()

  tribits_is_primary_meta_project_package(Teuchos  Teuchos_is_PMP)
  unittest_compare_const(Teuchos_is_PMP  FALSE)

  tribits_is_primary_meta_project_package(RTOp  RTOp_is_PMP)
  unittest_compare_const(RTOp_is_PMP  TRUE)

  tribits_is_primary_meta_project_package(Ex2Package1  Ex2Package1_is_PMP)
  unittest_compare_const(Ex2Package1_is_PMP  TRUE)

  tribits_is_primary_meta_project_package(Ex2Package2  Ex2Package2_is_PMP)
  unittest_compare_const(Ex2Package2_is_PMP  TRUE)

  unittest_compare_const(${PROJECT_NAME}_NUM_PACKAGES 4)
  unittest_compare_const(${PROJECT_NAME}_NUM_TPLS 4)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Teuchos ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_RTOp ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package1 ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package2 ON)

endfunction()


function(unittest_enable_tribits_is_primary_meta_project_package_enable_teuchos_forward)

  message("\n***")
  message("*** Unit testing primary meta-project packages, enable Teuchos, enable forward")
  message("***\n")

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)
  #set(TRIBITS_PROCESS_PACKAGES_AND_DIRS_LISTS_VERBOSE ON)

  #set(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE ON)
  set(Trilinos_NO_PRIMARY_META_PROJECT_PACKAGES ON)
  set(Trilinos_NO_PRIMARY_META_PROJECT_PACKAGES_EXCEPT  RTOp)

  set(${PROJECT_NAME}_ENABLE_TESTS ON)
  set(${PROJECT_NAME}_ENABLE_ALL_FORWARD_DEP_PACKAGES ON)
  set(${PROJECT_NAME}_ENABLE_Teuchos ON)

  unittest_helper_read_and_process_packages()

  unittest_compare_const(${PROJECT_NAME}_ENABLE_Teuchos ON)
  unittest_compare_const(Teuchos_ENABLE_TESTS "")
  unittest_compare_const(${PROJECT_NAME}_ENABLE_RTOp ON)
  unittest_compare_const(RTOp_ENABLE_TESTS ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package1 ON)
  unittest_compare_const(Ex2Package1_ENABLE_TESTS ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package2 "")
  unittest_compare_const(Ex2Package2_ENABLE_TESTS "")

endfunction()


function(unittest_enable_tribits_is_primary_meta_project_package_enable_teuchos_tests_rtop_forward)

  message("\n***")
  message("*** Unit testing primary meta-project packages, enable Teuchos tests, enable RTOp, enable forward")
  message("***\n")

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)
  #set(TRIBITS_PROCESS_PACKAGES_AND_DIRS_LISTS_VERBOSE ON)

  #set(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE ON)
  set(Trilinos_NO_PRIMARY_META_PROJECT_PACKAGES ON)
  set(Trilinos_NO_PRIMARY_META_PROJECT_PACKAGES_EXCEPT  RTOp)

  set(${PROJECT_NAME}_ENABLE_TESTS ON)
  set(${PROJECT_NAME}_ENABLE_ALL_FORWARD_DEP_PACKAGES ON)

  set(${PROJECT_NAME}_ENABLE_Teuchos ON)
  set(Teuchos_ENABLE_TESTS ON)  # Must be explicitly turned on!

  set(${PROJECT_NAME}_ENABLE_RTOp ON)
  # Tests will get turned on automatically because RTOp is a it is a PMPP

  unittest_helper_read_and_process_packages()

  unittest_compare_const(${PROJECT_NAME}_ENABLE_Teuchos ON)
  unittest_compare_const(Teuchos_ENABLE_TESTS ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_RTOp ON)
  unittest_compare_const(RTOp_ENABLE_TESTS ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package1 ON)
  unittest_compare_const(Ex2Package1_ENABLE_TESTS ON)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package2 "")
  unittest_compare_const(Ex2Package2_ENABLE_TESTS "")

endfunction()


#####################################################################
#
# Execute the unit tests
#
#####################################################################

unittest_initialize_vars()

# A) Test enabled/disable logic
unittest_enable_no_packages()
unittest_enable_all_packages()
unittest_enable_all_packages_st()
unittest_enable_all_packages_st_extra_test_deps()

# B) Test generation of export file information
unittest_enable_all_generate_export_deps()
unittest_enable_all_st_generate_export_deps()
unittest_enable_all_st_generate_export_deps_only_ex2package1()
unittest_enable_rtop_generate_export_deps_only_ex2package1()
unittest_enable_teuchos_generate_export_deps_only_ex2package1()

# C) Test primary meta-project package enable/disable logic
unittest_enable_tribits_is_primary_meta_project_package()
unittest_enable_tribits_is_primary_meta_project_package_exclude_rtop_st()
unittest_enable_tribits_is_primary_meta_project_package_enable_teuchos_forward()
unittest_enable_tribits_is_primary_meta_project_package_enable_teuchos_tests_rtop_forward()

# Pass in the number of expected tests that must pass!
unittest_final_result(176)
