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

################################################################################
#
# This file contains unit tests for macros and functions in the file
# TribitsReadDepsFilesCreateDepsGraph.cmake and related files.
#
################################################################################

cmake_minimum_required(VERSION 3.17.0 FATAL_ERROR)

include("${CMAKE_CURRENT_LIST_DIR}/TribitsReadAllProjectDepsFilesCreateDepsGraphHelpers.cmake")

include(TribitsPackageDefineDependencies)


################################################################################
#
# Unit tests for code macros that are used to declare/define dependencies
#
################################################################################


function(unitest_tribits_define_repository_packages_dirs_classifications_empty)

  message("\n***")
  message("*** Testing tribits_repository_define_packages() empty")
  message("***\n")

  set(REPOSITORY_NAME RepoName)
  tribits_repository_define_packages()

  unittest_compare_const(${REPOSITORY_NAME}_PACKAGES_AND_DIRS_AND_CLASSIFICATIONS "")

endfunction()


function(unitest_tribits_define_repository_packages_dirs_classifications_1_package)

  message("\n***")
  message("*** Testing tribits_repository_define_packages() 1 package")
  message("***\n")

  set(REPOSITORY_NAME RepoName)
  tribits_repository_define_packages(
    Package1  .  PT
    )

  unittest_compare_const(${REPOSITORY_NAME}_PACKAGES_AND_DIRS_AND_CLASSIFICATIONS
    "Package1;.;PT")

endfunction()


function(unitest_tribits_define_repository_packages_dirs_classifications_2_packages)

  message("\n***")
  message("*** Testing tribits_repository_define_packages() 1 package")
  message("***\n")

  set(REPOSITORY_NAME RepoName)
  tribits_repository_define_packages(
    Package_a  packages/a  ST
    Package_b  packages/b  EX,UM
    )

  unittest_compare_const(${REPOSITORY_NAME}_PACKAGES_AND_DIRS_AND_CLASSIFICATIONS
    "Package_a;packages/a;ST;Package_b;packages/b;EX,UM")

endfunction()


function(unitest_tribits_define_repository_tpls_findmods_classifications_empty)

  message("\n***")
  message("*** Testing tribits_repository_define_tpls() empty")
  message("***\n")

  set(REPOSITORY_NAME RepoName)
  tribits_repository_define_tpls()

  unittest_compare_const(${REPOSITORY_NAME}_TPLS_FINDMODS_CLASSIFICATIONS "")

endfunction()


function(unitest_tribits_define_repository_tpls_findmods_classifications_1_tpl)

  message("\n***")
  message("*** Testing tribits_repository_define_tpls() 1 TPL")
  message("***\n")

  set(REPOSITORY_NAME RepoName)
  tribits_repository_define_tpls(
    TplName   cmake/TPLs/   PT
    )

  unittest_compare_const(${REPOSITORY_NAME}_TPLS_FINDMODS_CLASSIFICATIONS
    "TplName;cmake/TPLs/;PT")

endfunction()


function(unitest_tribits_define_repository_tpls_findmods_classifications_2_tpls)

  message("\n***")
  message("*** Testing tribits_repository_define_tpls() 2 TPLS")
  message("***\n")

  set(REPOSITORY_NAME RepoName)
  tribits_repository_define_tpls(
    TPL1   tpls/FindTPLTPL1.cmake   ST
    TPL2   tpls/   EX
    )

  unittest_compare_const(${REPOSITORY_NAME}_TPLS_FINDMODS_CLASSIFICATIONS
    "TPL1;tpls/FindTPLTPL1.cmake;ST;TPL2;tpls/;EX")

endfunction()


function(unitest_tribits_set_st_for_dev_mode)

  message("\n***")
  message("*** Testing tribits_set_st_for_dev_mode()")
  message("***\n")

  message("\nTest in dev mode, ST off ...")
  set(${PROJECT_NAME}_ENABLE_DEVELOPMENT_MODE ON)
  set(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE OFF)
  tribits_set_st_for_dev_mode(ENABLE_ST_FOR_DEV_PS_FOR_RELEASE)
  unittest_compare_const(ENABLE_ST_FOR_DEV_PS_FOR_RELEASE OFF)

  message("\nTest in dev mode, ST on ...")
  set(${PROJECT_NAME}_ENABLE_DEVELOPMENT_MODE ON)
  set(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE ON)
  tribits_set_st_for_dev_mode(ENABLE_ST_FOR_DEV_PS_FOR_RELEASE)
  unittest_compare_const(ENABLE_ST_FOR_DEV_PS_FOR_RELEASE ON)

  message("\nTest in rel mode, ST off ...")
  set(${PROJECT_NAME}_ENABLE_DEVELOPMENT_MODE OFF)
  set(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE OFF)
  tribits_set_st_for_dev_mode(ENABLE_ST_FOR_DEV_PS_FOR_RELEASE)
  unittest_compare_const(ENABLE_ST_FOR_DEV_PS_FOR_RELEASE ON)

  message("\nTest in rel mode, ST on ...")
  set(${PROJECT_NAME}_ENABLE_DEVELOPMENT_MODE OFF)
  set(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE ON)
  tribits_set_st_for_dev_mode(ENABLE_ST_FOR_DEV_PS_FOR_RELEASE)
  unittest_compare_const(ENABLE_ST_FOR_DEV_PS_FOR_RELEASE ON)

endfunction()


function(unitest_tribits_set_ss_for_dev_mode_backward_compatible)

  message("\n***")
  message("*** Testing tribits_set_ss_for_dev_mode() backward compatibility")
  message("***\n")

  message("\nTest in dev mode, ST off ...")
  set(${PROJECT_NAME}_ENABLE_DEVELOPMENT_MODE ON)
  set(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE OFF)
  tribits_set_ss_for_dev_mode(ENABLE_SS_FOR_DEV_PS_FOR_RELEASE)
  unittest_compare_const(ENABLE_SS_FOR_DEV_PS_FOR_RELEASE OFF)

  message("\nTest in dev mode, ST on ...")
  set(${PROJECT_NAME}_ENABLE_DEVELOPMENT_MODE ON)
  set(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE ON)
  tribits_set_ss_for_dev_mode(ENABLE_SS_FOR_DEV_PS_FOR_RELEASE)
  unittest_compare_const(ENABLE_SS_FOR_DEV_PS_FOR_RELEASE ON)

  message("\nTest in rel mode, ST off ...")
  set(${PROJECT_NAME}_ENABLE_DEVELOPMENT_MODE OFF)
  set(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE OFF)
  tribits_set_ss_for_dev_mode(ENABLE_SS_FOR_DEV_PS_FOR_RELEASE)
  unittest_compare_const(ENABLE_SS_FOR_DEV_PS_FOR_RELEASE ON)

  message("\nTest in rel mode, ST on ...")
  set(${PROJECT_NAME}_ENABLE_DEVELOPMENT_MODE OFF)
  set(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE ON)
  tribits_set_ss_for_dev_mode(ENABLE_SS_FOR_DEV_PS_FOR_RELEASE)
  unittest_compare_const(ENABLE_SS_FOR_DEV_PS_FOR_RELEASE ON)

endfunction()


function(unitest_tribits_define_package_dependencies_none)

  message("\n***")
  message("*** Testing tribits_package_define_dependencies()")
  message("***\n")

  tribits_prep_to_read_dependencies(DUMMY_PKG)
  set(REGRESSION_EMAIL_LIST NOT_SET)
  set(SUBPACKAGES_DIRS_CLASSIFICATIONS_OPTREQS NOT_SET)

  tribits_package_define_dependencies()

  unittest_compare_const(LIB_REQUIRED_DEP_PACKAGES "")
  unittest_compare_const(LIB_OPTIONAL_DEP_PACKAGES "")
  unittest_compare_const(TEST_REQUIRED_DEP_PACKAGES "")
  unittest_compare_const(TEST_OPTIONAL_DEP_PACKAGES "")
  unittest_compare_const(LIB_REQUIRED_DEP_TPLS "")
  unittest_compare_const(LIB_OPTIONAL_DEP_TPLS "")
  unittest_compare_const(TEST_REQUIRED_DEP_TPLS "")
  unittest_compare_const(TEST_OPTIONAL_DEP_TPLS "")
  unittest_compare_const(DUMMY_PKG_FORWARD_LIB_REQUIRED_DEP_PACKAGES "")
  unittest_compare_const(DUMMY_PKG_FORWARD_LIB_OPTIONAL_DEP_PACKAGES "")
  unittest_compare_const(DUMMY_PKG_FORWARD_TEST_REQUIRED_DEP_PACKAGES "")
  unittest_compare_const(DUMMY_PKG_FORWARD_TEST_OPTIONAL_DEP_PACKAGES "")
  unittest_compare_const(REGRESSION_EMAIL_LIST "")
  unittest_compare_const(SUBPACKAGES_DIRS_CLASSIFICATIONS_OPTREQS "")

endfunction()


function(unitest_tribits_define_package_dependencies_libs_required_packages_1)

  message("\n***")
  message("*** Testing tribits_package_define_dependencies(LIB_REQUIRED_PACKAGES PKG1)")
  message("***\n")

  tribits_prep_to_read_dependencies(DUMMY_PKG)

  tribits_package_define_dependencies(LIB_REQUIRED_PACKAGES PKG1)

  unittest_compare_const(LIB_REQUIRED_DEP_PACKAGES "PKG1")
  unittest_compare_const(LIB_OPTIONAL_DEP_PACKAGES "")
  unittest_compare_const(TEST_REQUIRED_DEP_PACKAGES "")
  unittest_compare_const(TEST_OPTIONAL_DEP_PACKAGES "")
  unittest_compare_const(LIB_REQUIRED_DEP_TPLS "")
  unittest_compare_const(LIB_OPTIONAL_DEP_TPLS "")
  unittest_compare_const(TEST_REQUIRED_DEP_TPLS "")
  unittest_compare_const(TEST_OPTIONAL_DEP_TPLS "")

endfunction()


function(unitest_tribits_define_package_dependencies_libs_required_packages_2)

  message("\n***")
  message("*** Testing tribits_package_define_dependencies(LIB_REQUIRED_PACKAGES PKG2 PKG1)")
  message("***\n")

  tribits_prep_to_read_dependencies(DUMMY_PKG)

  tribits_package_define_dependencies(LIB_REQUIRED_PACKAGES PKG2 PKG1)

  unittest_compare_const(LIB_REQUIRED_DEP_PACKAGES "PKG2;PKG1")
  unittest_compare_const(LIB_OPTIONAL_DEP_PACKAGES "")
  unittest_compare_const(TEST_REQUIRED_DEP_PACKAGES "")
  unittest_compare_const(TEST_OPTIONAL_DEP_PACKAGES "")
  unittest_compare_const(LIB_REQUIRED_DEP_TPLS "")
  unittest_compare_const(LIB_OPTIONAL_DEP_TPLS "")
  unittest_compare_const(TEST_REQUIRED_DEP_TPLS "")
  unittest_compare_const(TEST_OPTIONAL_DEP_TPLS "")

endfunction()


function(unitest_tribits_define_package_dependencies_all)

  message("\n***")
  message("*** Testing tribits_package_define_dependencies( ... all ...)")
  message("***\n")

  tribits_prep_to_read_dependencies(DUMMY_PKG)

  tribits_package_define_dependencies(
    LIB_REQUIRED_PACKAGES PKG1 PKG2
    LIB_OPTIONAL_PACKAGES PKG3 PKG4
    TEST_REQUIRED_PACKAGES PKG5 PKG6
    TEST_OPTIONAL_PACKAGES PKG7 PKG8
    LIB_REQUIRED_TPLS TPL1 TPL2
    LIB_OPTIONAL_TPLS TPL3 TPL4
    TEST_REQUIRED_TPLS TPL5 TPL6
    TEST_OPTIONAL_TPLS TPL7 TPL8
    REGRESSION_EMAIL_LIST email-address5
    SUBPACKAGES_DIRS_CLASSIFICATIONS_OPTREQS
       SPKG1  spkg1  EX  REQUIRED
       SPKG2  utils/spkg2  PT,PM  OPTIONAL
    )

  unittest_compare_const(LIB_REQUIRED_DEP_PACKAGES "PKG1;PKG2")
  unittest_compare_const(LIB_OPTIONAL_DEP_PACKAGES "PKG3;PKG4")
  unittest_compare_const(TEST_REQUIRED_DEP_PACKAGES "PKG5;PKG6")
  unittest_compare_const(TEST_OPTIONAL_DEP_PACKAGES "PKG7;PKG8")
  unittest_compare_const(LIB_REQUIRED_DEP_TPLS "TPL1;TPL2")
  unittest_compare_const(LIB_OPTIONAL_DEP_TPLS "TPL3;TPL4")
  unittest_compare_const(TEST_REQUIRED_DEP_TPLS "TPL5;TPL6")
  unittest_compare_const(TEST_OPTIONAL_DEP_TPLS "TPL7;TPL8")
  unittest_compare_const(REGRESSION_EMAIL_LIST "email-address5")
  unittest_compare_const(SUBPACKAGES_DIRS_CLASSIFICATIONS_OPTREQS
    "SPKG1;spkg1;EX;REQUIRED;SPKG2;utils/spkg2;PT,PM;OPTIONAL")

endfunction()


################################################################################
#
# Unit tests for tribits_read_all_project_deps_files_create_deps_graph()
#
################################################################################

function(unittest_read_packages_list_with_extra_repo)

  message("\n***")
  message("*** Testing the reading of packages list with extra repo")
  message("***\n")

  # Debugging
  #set(TRIBITS_PROCESS_PACKAGES_AND_DIRS_LISTS ON)
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)

  tribits_process_packages_and_dirs_lists(${PROJECT_NAME} ".")
  tribits_process_packages_and_dirs_lists(${EXTRA_REPO_NAME} ${EXTRA_REPO_DIR})

  unittest_compare_const( ${PROJECT_NAME}_PACKAGES
    "Teuchos;RTOp;Ex2Package1;Ex2Package2")
  unittest_compare_const( ${PROJECT_NAME}_NUM_PACKAGES 4 )

endfunction()


function(unittest_read_tpls_lists_wtih_duplicate_tpls)

  message("\n***")
  message("*** Testing the reading of TPL lists with duplicate TPLs ")
  message("***\n")

  # Debugging
  #set(TRIBITS_PROCESS_PACKAGES_AND_DIRS_LISTS ON)
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)

  tribits_repository_define_tpls(
    EXTPL2       tpls/    EX
    Boost        tpls/    PT
  )

  set(${PROJECT_NAME}_TPLS_FILE "dummy")
  set(${EXTRA_REPO_NAME}_TPLS_FILE "dummy")
  tribits_process_tpls_lists(${PROJECT_NAME} ".")
  tribits_process_tpls_lists(${EXTRA_REPO_NAME} ${EXTRA_REPO_DIR})

  # The TPL is not added again
  unittest_compare_const( ${PROJECT_NAME}_TPLS "MPI;BLAS;LAPACK;Boost;EXTPL2")
  unittest_compare_const( ${PROJECT_NAME}_NUM_TPLS "5" )
  unittest_compare_const( ${PROJECT_NAME}_REVERSE_TPLS "EXTPL2;Boost;LAPACK;BLAS;MPI" )
  unittest_compare_const( MPI_FINDMOD "cmake/TPLs/FindTPLMPI.cmake" )
  unittest_compare_const( MPI_TESTGROUP "PT" )
  unittest_compare_const( BLAS_FINDMOD "cmake/TPLs/FindTPLBLAS.cmake" )
  unittest_compare_const( BLAS_TESTGROUP "PT" )

  # The find module is overridden in extra repo
  unittest_compare_const( Boost_FINDMOD "${EXTRA_REPO_NAME}/tpls/FindTPLBoost.cmake" )

  # The classification is not overridden in extra repo
  unittest_compare_const( Boost_TESTGROUP "ST" )

endfunction()


function(unittest_read_packages_and_dependencies)

  message("\n***")
  message("*** Testing basic tribits_read_all_project_deps_files_create_deps_graph() functioning")
  message("***\n")

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)

  unittest_helper_read_packages_and_dependencies()

  unittest_compare_const(${PROJECT_NAME}_TPLS "MPI;BLAS;LAPACK;Boost")
  unittest_compare_const(${PROJECT_NAME}_NUM_TPLS 4)
  unittest_compare_const(${PROJECT_NAME}_PACKAGES "Teuchos;RTOp;Ex2Package1;Ex2Package2")
  unittest_compare_const(${PROJECT_NAME}_NUM_PACKAGES 4)

  unittest_compare_const(${PROJECT_NAME}_DEFINED_TPLS "MPI;BLAS;LAPACK;Boost")
  unittest_compare_const(${PROJECT_NAME}_NUM_DEFINED_TPLS 4)
  unittest_compare_const(${PROJECT_NAME}_DEFINED_INTERNAL_PACKAGES "Teuchos;RTOp;Ex2Package1;Ex2Package2")
  unittest_compare_const(${PROJECT_NAME}_NUM_DEFINED_INTERNAL_PACKAGES 4)
  unittest_compare_const(${PROJECT_NAME}_ALL_DEFINED_TOPLEVEL_PACKAGES "MPI;BLAS;LAPACK;Boost;Teuchos;RTOp;Ex2Package1;Ex2Package2")
  unittest_compare_const(${PROJECT_NAME}_NUM_ALL_DEFINED_TOPLEVEL_PACKAGES 8)

  # ToDo: Add checks for ${PACKAGE_NAME}_REL_SOURCE_DIR,,
  # ${PACKAGE_NAME}_SOURCE_DIR, ${PACKAGE_NAME}_TESTGROUP and other vars set
  # by tribits_read_all_project_deps_files_create_deps_graph() (#63)

endfunction()


function(unittest_standard_project_default_email_address_base)

  message("\n***")
  message("*** Testing the case where the TriBITS project has a default email address base and uses standard package regression email list names")
  message("***\n")

  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)

  tribits_process_packages_and_dirs_lists(${PROJECT_NAME} ".")
  tribits_process_packages_and_dirs_lists(${EXTRA_REPO_NAME} ${EXTRA_REPO_DIR})
  tribits_read_deps_files_create_deps_graph()

  unittest_compare_const(Teuchos_REGRESSION_EMAIL_LIST teuchos-regression@repo.site.gov)
  unittest_compare_const(RTOp_REGRESSION_EMAIL_LIST thyra-regression@software.sandia.gov)
  unittest_compare_const(Ex2Package1_REGRESSION_EMAIL_LIST ex2-package1-override@some.ornl.gov)
  unittest_compare_const(Ex2Package2_REGRESSION_EMAIL_LIST ex2package2-regression@project.site.gov)

endfunction()


function(unittest_single_repository_email_list)

  message("\n***")
  message("*** Test setting a single regression email address for all the packages in the first repo but defer to hard-coded package email addresses")
  message("***\n")

  # Debugging
  #set(TRIBITS_PROCESS_PACKAGES_AND_DIRS_LISTS ON)
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)

  set(${PROJECT_NAME}_REPOSITORY_MASTER_EMAIL_ADDRESS "my-repo@some.url.com")
  set(${PROJECT_NAME}_REPOSITORY_EMAIL_URL_ADDRESS_BASE OFF) # Will cause to be ignored!

  tribits_process_packages_and_dirs_lists(${PROJECT_NAME} ".")
  tribits_process_packages_and_dirs_lists(${EXTRA_REPO_NAME} ${EXTRA_REPO_DIR})
  tribits_read_deps_files_create_deps_graph()

  unittest_compare_const(Teuchos_REGRESSION_EMAIL_LIST "my-repo@some.url.com")
  unittest_compare_const(RTOp_REGRESSION_EMAIL_LIST thyra-regression@software.sandia.gov)
  unittest_compare_const(Ex2Package1_REGRESSION_EMAIL_LIST ex2-package1-override@some.ornl.gov)
  unittest_compare_const(Ex2Package2_REGRESSION_EMAIL_LIST ex2package2-regression@project.site.gov)

endfunction()


function(unittest_single_repository_email_list_override_0)

  message("\n***")
  message("*** Test setting a single regression email address for all the packages in the first repo with override")
  message("***\n")

  # Debugging
  #set(TRIBITS_PROCESS_PACKAGES_AND_DIRS_LISTS ON)
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)

  set(${PROJECT_NAME}_REPOSITORY_OVERRIDE_PACKAGE_EMAIL_LIST "my-repo@some.url.com")

  tribits_process_packages_and_dirs_lists(${PROJECT_NAME} ".")
  tribits_process_packages_and_dirs_lists(${EXTRA_REPO_NAME} ${EXTRA_REPO_DIR})
  tribits_read_deps_files_create_deps_graph()

  unittest_compare_const(Teuchos_REGRESSION_EMAIL_LIST "my-repo@some.url.com")
  unittest_compare_const(RTOp_REGRESSION_EMAIL_LIST "my-repo@some.url.com")
  unittest_compare_const(Ex2Package1_REGRESSION_EMAIL_LIST ex2-package1-override@some.ornl.gov)
  unittest_compare_const(Ex2Package2_REGRESSION_EMAIL_LIST ex2package2-regression@project.site.gov)

endfunction()


function(unittest_single_repository_email_list_override_1)

  message("\n***")
  message("*** Test setting a single regression email address for all the packages in the second repo with override")
  message("***\n")

  # Debugging
  #set(TRIBITS_PROCESS_PACKAGES_AND_DIRS_LISTS ON)
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)

  set(${EXTRA_REPO_NAME}_REPOSITORY_OVERRIDE_PACKAGE_EMAIL_LIST "extra-repo@some.url.com")

  tribits_process_packages_and_dirs_lists(${PROJECT_NAME} ".")
  tribits_process_packages_and_dirs_lists(${EXTRA_REPO_NAME} ${EXTRA_REPO_DIR})
  tribits_read_deps_files_create_deps_graph()

  unittest_compare_const(Teuchos_REGRESSION_EMAIL_LIST teuchos-regression@repo.site.gov)
  unittest_compare_const(RTOp_REGRESSION_EMAIL_LIST thyra-regression@software.sandia.gov)
  unittest_compare_const(Ex2Package1_REGRESSION_EMAIL_LIST extra-repo@some.url.com)
  unittest_compare_const(Ex2Package2_REGRESSION_EMAIL_LIST extra-repo@some.url.com)

endfunction()


function(unittest_single_project_email_list)

  message("\n***")
  message("*** Test setting a single regression email address for all the packages in a TriBITS Project but defer to hard-coded package email addresses")
  message("***\n")

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)

  set(${PROJECT_NAME}_PROJECT_MASTER_EMAIL_ADDRESS "my-project@some.url.com")
  set(${PROJECT_NAME}_PROJECT_EMAIL_URL_ADDRESS_BASE OFF)
  set(${PROJECT_NAME}_REPOSITORY_EMAIL_URL_ADDRESS_BASE OFF)

  tribits_process_packages_and_dirs_lists(${PROJECT_NAME} ".")
  tribits_process_packages_and_dirs_lists(${EXTRA_REPO_NAME} ${EXTRA_REPO_DIR})
  tribits_read_deps_files_create_deps_graph()

  unittest_compare_const(Teuchos_REGRESSION_EMAIL_LIST "my-project@some.url.com")
  unittest_compare_const(RTOp_REGRESSION_EMAIL_LIST thyra-regression@software.sandia.gov)
  unittest_compare_const(Ex2Package1_REGRESSION_EMAIL_LIST ex2-package1-override@some.ornl.gov)
  unittest_compare_const(Ex2Package2_REGRESSION_EMAIL_LIST my-project@some.url.com)

endfunction()


function(unittest_single_project_email_list_override)

  message("\n***")
  message("*** Test setting a single regression email address for all the packages in a TriBITS Project and overriding hard-coded package email addresses")
  message("***\n")

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)

  set(${PROJECT_NAME}_PROJECT_MASTER_EMAIL_ADDRESS "my-project@some.url.com")
  set(${PROJECT_NAME}_REPOSITORY_OVERRIDE_PACKAGE_EMAIL_LIST
    "${${PROJECT_NAME}_PROJECT_MASTER_EMAIL_ADDRESS}")
  set(${EXTRA_REPO_NAME}_REPOSITORY_OVERRIDE_PACKAGE_EMAIL_LIST
    "${${PROJECT_NAME}_PROJECT_MASTER_EMAIL_ADDRESS}")

  tribits_process_packages_and_dirs_lists(${PROJECT_NAME} ".")
  tribits_process_packages_and_dirs_lists(${EXTRA_REPO_NAME} ${EXTRA_REPO_DIR})
  tribits_read_deps_files_create_deps_graph()

  unittest_compare_const(Teuchos_REGRESSION_EMAIL_LIST "my-project@some.url.com")
  unittest_compare_const(RTOp_REGRESSION_EMAIL_LIST my-project@some.url.com)
  unittest_compare_const(Ex2Package1_REGRESSION_EMAIL_LIST my-project@some.url.com)
  unittest_compare_const(Ex2Package2_REGRESSION_EMAIL_LIST my-project@some.url.com)

endfunction()


function(unittest_extra_repo_missing_optional_package)

  message("\n***")
  message("*** Testing the reading of packages list with extra repo with missing optional upstream package")
  message("***\n")

  set(EXTRA_REPO_INCLUDE_MISSING_OPTIONAL_DEP_PACKAGE ON)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE ON)

  # Debugging
  #set(TRIBITS_PROCESS_PACKAGES_AND_DIRS_LISTS ON)
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)

  tribits_process_packages_and_dirs_lists(${PROJECT_NAME} ".")
  tribits_process_packages_and_dirs_lists(${EXTRA_REPO_NAME} ${EXTRA_REPO_DIR})

  global_set(MESSAGE_WRAPPER_INPUT)  							
  tribits_read_deps_files_create_deps_graph()

  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "-- ;Trilinos_NUM_SE_PACKAGES='4'")
  unittest_compare_const( ${PROJECT_NAME}_PACKAGES
    "Teuchos;RTOp;Ex2Package1;Ex2Package2")
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package1 "")

endfunction()


function(unittest_extra_repo_missing_optional_package_verbose)

  message("\n***")
  message("*** Testing the reading of packages list with extra repo with missing optional upstream package")
  message("***\n")

  set(${PROJECT_NAME}_WARN_ABOUT_MISSING_EXTERNAL_PACKAGES  ON)

  set(EXTRA_REPO_INCLUDE_MISSING_OPTIONAL_DEP_PACKAGE ON)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE ON)

  # Debugging
  #set(TRIBITS_PROCESS_PACKAGES_AND_DIRS_LISTS ON)
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)

  tribits_process_packages_and_dirs_lists(${PROJECT_NAME} ".")
  tribits_process_packages_and_dirs_lists(${EXTRA_REPO_NAME} ${EXTRA_REPO_DIR})

  global_set(MESSAGE_WRAPPER_INPUT)  							
  tribits_read_deps_files_create_deps_graph()

  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "NOTE: MissingUpstreamPackage is being ignored since its directory; is missing and MissingUpstreamPackage_ALLOW_MISSING_EXTERNAL_PACKAGE =; TRUE!;-- ;Trilinos_NUM_SE_PACKAGES='4'")
  unittest_compare_const( ${PROJECT_NAME}_PACKAGES
    "Teuchos;RTOp;Ex2Package1;Ex2Package2")
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package1 "")
  unittest_compare_const(${PROJECT_NAME}_ENABLE_MissingUpstreamPackage "OFF")
  unittest_compare_const(Ex2Package1_ENABLE_MissingUpstreamPackage "OFF")
endfunction()


function(unittest_extra_repo_missing_required_package)

  message("\n***")
  message("*** Testing the reading of packages list with extra repo with missing required upstream package")
  message("***\n")

  set(EXTRA_REPO_INCLUDE_MISSING_REQUIRED_DEP_PACKAGE ON)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE ON)

  # Debugging
  #set(TRIBITS_PROCESS_PACKAGES_AND_DIRS_LISTS ON)
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)

  tribits_process_packages_and_dirs_lists(${PROJECT_NAME} ".")
  tribits_process_packages_and_dirs_lists(${EXTRA_REPO_NAME} ${EXTRA_REPO_DIR})

  global_set(MESSAGE_WRAPPER_INPUT)  							
  tribits_read_deps_files_create_deps_graph()

  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "NOTE: Setting Trilinos_ENABLE_Ex2Package1=OFF because; package Ex2Package1 has a required dependency on missing; package MissingUpstreamPackage!;-- ;Trilinos_NUM_SE_PACKAGES='4'")
  unittest_compare_const( ${PROJECT_NAME}_PACKAGES
    "Teuchos;RTOp;Ex2Package1;Ex2Package2")
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package1 OFF)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_MissingUpstreamPackage "OFF")
  unittest_compare_const(Ex2Package1_ENABLE_MissingUpstreamPackage "OFF")

endfunction()


function(unittest_extra_repo_missing_required_package_verbose)

  message("\n***")
  message("*** Testing the reading of packages list with extra repo with missing required upstream package (verbose mode)")
  message("***\n")

  set(${PROJECT_NAME}_WARN_ABOUT_MISSING_EXTERNAL_PACKAGES  ON)

  set(EXTRA_REPO_INCLUDE_MISSING_REQUIRED_DEP_PACKAGE ON)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE ON)

  # Debugging
  #set(TRIBITS_PROCESS_PACKAGES_AND_DIRS_LISTS ON)
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)

  tribits_process_packages_and_dirs_lists(${PROJECT_NAME} ".")
  tribits_process_packages_and_dirs_lists(${EXTRA_REPO_NAME} ${EXTRA_REPO_DIR})

  global_set(MESSAGE_WRAPPER_INPUT)  							
  tribits_read_deps_files_create_deps_graph()

  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "NOTE: MissingUpstreamPackage is being ignored since its directory; is missing and MissingUpstreamPackage_ALLOW_MISSING_EXTERNAL_PACKAGE =; TRUE!;NOTE: Setting Trilinos_ENABLE_Ex2Package1=OFF because; package Ex2Package1 has a required dependency on missing; package MissingUpstreamPackage!;-- ;Trilinos_NUM_SE_PACKAGES='4'")
  unittest_compare_const( ${PROJECT_NAME}_PACKAGES
    "Teuchos;RTOp;Ex2Package1;Ex2Package2")
  unittest_compare_const(${PROJECT_NAME}_ENABLE_Ex2Package1 OFF)
  unittest_compare_const(${PROJECT_NAME}_ENABLE_MissingUpstreamPackage "OFF")
  unittest_compare_const(Ex2Package1_ENABLE_MissingUpstreamPackage "OFF")

endfunction()


function(unittest_elevate_subpackages_st_to_pt)

  message("\n***")
  message("*** Testing elevating packages and subpackages from ST to PT")
  message("***\n")

  # Debugging
  #set(${PROJECT_NAME}_VERBOSE_CONFIGURE ON)
  #set(TRIBITS_SET_DEP_PACKAGES_DEBUG_DUMP ON)
  #set(TRIBITS_INSERT_STANDARD_PACKAGE_OPTIONS_DEBUG ON)
  #set(TRIBITS_PROCESS_PACKAGES_AND_DIRS_LISTS_VERBOSE ON)

  #set(TRIBITS_PROCESS_PACKAGES_AND_DIRS_LISTS ON)

  set(${PROJECT_NAME}_ELEVATE_ST_TO_PT TRUE)

  tribits_process_packages_and_dirs_lists(${PROJECT_NAME} ".")

  set(REPOSITORY_NAME extraRepoOnePackageThreeSubpackages)
  include(${PROJECT_SOURCE_DIR}/extraRepoOnePackageThreeSubpackages/PackagesList.cmake)
  tribits_process_packages_and_dirs_lists(extraRepoOnePackageThreeSubpackages
    extraRepoOnePackageThreeSubpackages)

  tribits_read_deps_files_create_deps_graph()

  unittest_compare_const( ${PROJECT_NAME}_SE_PACKAGES
    "Teuchos;RTOp;extraRepoOnePackageThreeSubpackagesSP1;extraRepoOnePackageThreeSubpackagesSP2;extraRepoOnePackageThreeSubpackagesSP3;extraRepoOnePackageThreeSubpackages")
  unittest_compare_const(extraRepoOnePackageThreeSubpackagesSP1_SOURCE_DIR ${PROJECT_SOURCE_DIR}/extraRepoOnePackageThreeSubpackages/sp1)
  unittest_compare_const(extraRepoOnePackageThreeSubpackagesSP2_SOURCE_DIR ${PROJECT_SOURCE_DIR}/extraRepoOnePackageThreeSubpackages/sp2)
  unittest_compare_const(extraRepoOnePackageThreeSubpackagesSP3_SOURCE_DIR ${PROJECT_SOURCE_DIR}/extraRepoOnePackageThreeSubpackages/sp3)
  unittest_compare_const(extraRepoOnePackageThreeSubpackagesSP1_REL_SOURCE_DIR extraRepoOnePackageThreeSubpackages/sp1)
  unittest_compare_const(extraRepoOnePackageThreeSubpackagesSP2_REL_SOURCE_DIR extraRepoOnePackageThreeSubpackages/sp2)
  unittest_compare_const(extraRepoOnePackageThreeSubpackagesSP3_REL_SOURCE_DIR extraRepoOnePackageThreeSubpackages/sp3)
  unittest_compare_const(extraRepoOnePackageThreeSubpackagesSP1_TESTGROUP PT)
  unittest_compare_const(extraRepoOnePackageThreeSubpackagesSP2_TESTGROUP PT)
  unittest_compare_const(extraRepoOnePackageThreeSubpackagesSP3_TESTGROUP EX)

endfunction()


################################################################################
#
# Execute the unit tests
#
################################################################################

unittest_initialize_vars()

# Unit tests for code macros that are used to declare/define dependencies
unitest_tribits_define_repository_packages_dirs_classifications_empty()
unitest_tribits_define_repository_packages_dirs_classifications_1_package()
unitest_tribits_define_repository_packages_dirs_classifications_2_packages()
unitest_tribits_define_repository_tpls_findmods_classifications_empty()
unitest_tribits_define_repository_tpls_findmods_classifications_1_tpl()
unitest_tribits_define_repository_tpls_findmods_classifications_2_tpls()
unitest_tribits_set_st_for_dev_mode()
unitest_tribits_set_ss_for_dev_mode_backward_compatible()
unitest_tribits_define_package_dependencies_none()
unitest_tribits_define_package_dependencies_libs_required_packages_1()
unitest_tribits_define_package_dependencies_libs_required_packages_2()
unitest_tribits_define_package_dependencies_all()

# Unit tests for tribits_read_all_project_deps_files_create_deps_graph()
unittest_read_packages_list_with_extra_repo()
unittest_read_tpls_lists_wtih_duplicate_tpls()
unittest_read_packages_and_dependencies()
unittest_standard_project_default_email_address_base()
unittest_single_repository_email_list()
unittest_single_repository_email_list_override_0()
unittest_single_repository_email_list_override_1()
unittest_single_project_email_list()
unittest_single_project_email_list_override()
unittest_extra_repo_missing_optional_package()
unittest_extra_repo_missing_optional_package_verbose()
unittest_extra_repo_missing_required_package()
unittest_extra_repo_missing_required_package_verbose()
unittest_elevate_subpackages_st_to_pt()

# Pass in the number of expected tests that must pass!
unittest_final_result(127)
