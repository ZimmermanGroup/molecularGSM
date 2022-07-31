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

include(TribitsCMakePolicies  NO_POLICY_SCOPE)
include(TribitsProcessExtraRepositoriesList)
include(UnitTestHelpers)
include(GlobalSet)


#####################################################################
#
# Unit tests for code in TribitsProcessExtraRepositoriesList.cmake
#
#####################################################################


function(unittest_tribits_parse_extrarepo_packstat)

  message("\n*** Test tribits_parse_extrarepo_packstat(), PACKSTAT=''\n")
  tribits_parse_extrarepo_packstat(""  HASPKGS  PREPOST)
  unittest_compare_const( HASPKGS "HASPACKAGES")
  unittest_compare_const( PREPOST "POST")

  message("\n*** Test tribits_parse_extrarepo_packstat(), PACKSTAT='NOPACKAGES'\n")
  tribits_parse_extrarepo_packstat("NOPACKAGES"  HASPKGS  PREPOST)
  unittest_compare_const( HASPKGS "NOPACKAGES")
  unittest_compare_const( PREPOST "POST")

  message("\n*** Test tribits_parse_extrarepo_packstat(), PACKSTAT='POST'\n")
  tribits_parse_extrarepo_packstat("POST"  HASPKGS  PREPOST)
  unittest_compare_const( HASPKGS "HASPACKAGES")
  unittest_compare_const( PREPOST "POST")

  message("\n*** Test tribits_parse_extrarepo_packstat(), PACKSTAT='HASPACKAGES'\n")
  tribits_parse_extrarepo_packstat("HASPACKAGES"  HASPKGS  PREPOST)
  unittest_compare_const( HASPKGS "HASPACKAGES")
  unittest_compare_const( PREPOST "POST")

  message("\n*** Test tribits_parse_extrarepo_packstat(), PACKSTAT='HASPACKAGES,POST'\n")
  tribits_parse_extrarepo_packstat("HASPACKAGES,POST"  HASPKGS  PREPOST)
  unittest_compare_const( HASPKGS "HASPACKAGES")
  unittest_compare_const( PREPOST "POST")

  message("\n*** Test tribits_parse_extrarepo_packstat(), PACKSTAT=',POST'\n")
  tribits_parse_extrarepo_packstat(",POST"  HASPKGS  PREPOST)
  unittest_compare_const( HASPKGS "HASPACKAGES")
  unittest_compare_const( PREPOST "POST")

  message("\n*** Test tribits_parse_extrarepo_packstat(), PACKSTAT='POST,HASPACKAGES'\n")
  tribits_parse_extrarepo_packstat("POST,HASPACKAGES"  HASPKGS  PREPOST)
  unittest_compare_const( HASPKGS "HASPACKAGES")
  unittest_compare_const( PREPOST "POST")

  message("\n*** Test tribits_parse_extrarepo_packstat(), PACKSTAT='HASPACKAGES, POST'\n")
  tribits_parse_extrarepo_packstat("HASPACKAGES, POST"  HASPKGS  PREPOST)
  unittest_compare_const( HASPKGS "HASPACKAGES")
  unittest_compare_const( PREPOST "POST")

  message("\n*** Test tribits_parse_extrarepo_packstat(), PACKSTAT='PRE'\n")
  tribits_parse_extrarepo_packstat("PRE"  HASPKGS  PREPOST)
  unittest_compare_const( HASPKGS "HASPACKAGES")
  unittest_compare_const( PREPOST "PRE")

  message("\n*** Test tribits_parse_extrarepo_packstat(), PACKSTAT='PRE,NOPACKAGES'\n")
  tribits_parse_extrarepo_packstat("PRE,NOPACKAGES"  HASPKGS  PREPOST)
  unittest_compare_const( HASPKGS "NOPACKAGES")
  unittest_compare_const( PREPOST "PRE")

  message("\n*** Test tribits_parse_extrarepo_packstat(), PACKSTAT='NOPACKAGES,PRE'\n")
  tribits_parse_extrarepo_packstat("NOPACKAGES,PRE"  HASPKGS  PREPOST)
  unittest_compare_const( HASPKGS "NOPACKAGES")
  unittest_compare_const( PREPOST "PRE")

  message("\n*** Test tribits_parse_extrarepo_packstat(), PACKSTAT='BAD'\n")
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE  TRUE)
  global_set(MESSAGE_WRAPPER_INPUT)
  tribits_parse_extrarepo_packstat("BAD"  HASPKGS  PREPOST)
  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;Error, the value of 'PACKSTAT' element; 'BAD' is not valid!  Valid choices are '' (empty),; 'HASPACKAGES', 'NOPACKAGES', 'PRE', and 'POST'.  The defaults if all; fields are empty are 'HASPACKAGES' and 'POST'")
  unittest_compare_const( HASPKGS "HASPACKAGES")
  unittest_compare_const( PREPOST "POST")

endfunction()


function(unittest_tribits_process_extrarepos_lists)

  message("\n***")
  message("*** Test tribits_process_extrarepos_lists() getting Nightly")
  message("***\n")

  #set(TRIBITS_PROCESS_EXTRAREPOS_LISTS_DEBUG  TRUE)

  set(${PROJECT_NAME}_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE  Nightly)

  tribits_project_define_extra_repositories(
     repo0_name  ""              GIT   "git@url0.com:repo0"  ""           Continuous
     repo1_name  "some/sub/dir"  SVN   "git@url1.com:repo1"  NOPACKAGES   Nightly
     )
  
  tribits_process_extrarepos_lists()

  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DEFAULT
    "repo0_name;repo1_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DIRS
    "repo0_name;some/sub/dir")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_VCTYPES
    "GIT;SVN")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_REPOURLS
    "git@url0.com:repo0;git@url1.com:repo1")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_HASPKGS
    "HASPACKAGES;NOPACKAGES")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_PREPOSTS
    "POST;POST")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_CATEGORIES
    "Continuous;Nightly")
  unittest_compare_const( ${PROJECT_NAME}_PRE_REPOSITORIES_DEFAULT
    "")
  unittest_compare_const( ${PROJECT_NAME}_EXTRA_REPOSITORIES_DEFAULT
    "repo0_name;repo1_name")

  message("\n***")
  message("*** Test tribits_process_extrarepos_lists() getting Continuous")
  message("***\n")

  #set(TRIBITS_PROCESS_EXTRAREPOS_LISTS_DEBUG  TRUE)

  set(${PROJECT_NAME}_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE  Continuous)

  tribits_project_define_extra_repositories(
     repo0_name  ""              GIT   "git@url0.com:repo0"  ""           Continuous
     repo1_name  "some/sub/dir"  SVN   "git@url1.com:repo1"  NOPACKAGES   Nightly
     )
  
  tribits_process_extrarepos_lists()

  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DEFAULT
    "repo0_name;repo1_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DIRS
    "repo0_name;some/sub/dir")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_VCTYPES
    "GIT;SVN")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_REPOURLS
    "git@url0.com:repo0;git@url1.com:repo1")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_HASPKGS
    "HASPACKAGES;NOPACKAGES")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_PREPOSTS
    "POST;POST")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_CATEGORIES
    "Continuous;Nightly")
  unittest_compare_const( ${PROJECT_NAME}_PRE_REPOSITORIES_DEFAULT
    "")
  unittest_compare_const( ${PROJECT_NAME}_EXTRA_REPOSITORIES_DEFAULT
    "repo0_name;repo1_name")

  message("\n***")
  message("*** Test tribits_process_extrarepos_lists() PACKSTAT='PRE,HASPACKAGES'")
  message("***\n")

  #set(TRIBITS_PROCESS_EXTRAREPOS_LISTS_DEBUG  TRUE)

  set(${PROJECT_NAME}_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE  Continuous)

  tribits_project_define_extra_repositories(
     repo0_name  ""              GIT   "git@url0.com:repo0"  "PRE,HASPACKAGES"           Continuous
     )
  
  tribits_process_extrarepos_lists()

  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DEFAULT
    "repo0_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DIRS
    "repo0_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_VCTYPES
    "GIT")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_REPOURLS
    "git@url0.com:repo0")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_HASPKGS
    "HASPACKAGES")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_PREPOSTS
    "PRE")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_CATEGORIES
    "Continuous")
  unittest_compare_const( ${PROJECT_NAME}_PRE_REPOSITORIES_DEFAULT
    "repo0_name")
  unittest_compare_const( ${PROJECT_NAME}_EXTRA_REPOSITORIES_DEFAULT
    "")

  message("\n***")
  message("*** Test tribits_process_extrarepos_lists() empty VC type and URL")
  message("***\n")

  #set(TRIBITS_PROCESS_EXTRAREPOS_LISTS_DEBUG  TRUE)

  set(${PROJECT_NAME}_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE  Continuous)

  tribits_project_define_extra_repositories(
     repo0_name  ""              ""   ""  ""           Continuous
     )
  
  tribits_process_extrarepos_lists()

  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DEFAULT
    "repo0_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DIRS
    "repo0_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_VCTYPES
    "")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_REPOURLS
    "")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_HASPKGS
    "HASPACKAGES")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_PREPOSTS
    "POST")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_CATEGORIES
    "Continuous")
  unittest_compare_const( ${PROJECT_NAME}_PRE_REPOSITORIES_DEFAULT
    "")
  unittest_compare_const( ${PROJECT_NAME}_EXTRA_REPOSITORIES_DEFAULT
    "repo0_name")

  # ToDo: Test error when setting non-empty REPOURL when REPOTYPE is empty

  # ToDo: Test error when invalid VC type is given

  message("\n***")
  message("*** Test tribits_process_extrarepos_lists() POST before PRE")
  message("***\n")

  set(${PROJECT_NAME}_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE  Nightly)
  set(MESSAGE_WRAPPER_UNIT_TEST_MODE  TRUE)
  global_set(MESSAGE_WRAPPER_INPUT)

  tribits_project_define_extra_repositories(
     repo0_name  ""              GIT   "git@url0.com:repo0"  POST  Continuous
     repo1_name  "some/sub/dir"  SVN   "git@url1.com:repo1"  PRE   Nightly
     )
  
  tribits_process_extrarepos_lists()

  unittest_compare_const(MESSAGE_WRAPPER_INPUT
    "FATAL_ERROR;Error, the 'PRE' extra repo 'repo1_name'; specified in the PACKSTAT field 'PRE' came directly after; a 'POST' extra repo!  All 'PRE' extra repos must be listed before all; 'POST' extra repos!")

  # ToDo: Test that all PRE repos come before all POST repos

endfunction()


function(unittest_tribits_process_extrarepos_lists_old_repotype)

  message("\n***")
  message("*** Test tribits_process_extrarepos_lists() with old REPOTYPE var name")
  message("***\n")

  #set(TRIBITS_PROCESS_EXTRAREPOS_LISTS_DEBUG  TRUE)

  set(${PROJECT_NAME}_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE  Continuous)

  set( ${PROJECT_NAME}_EXTRAREPOS_DIR_REPOTYPE_REPOURL_PACKSTAT_CATEGORY
     repo0_name  ""              GIT   "git@url0.com:repo0"  ""           Continuous
     )
  
  tribits_process_extrarepos_lists()

  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DEFAULT
    "repo0_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DIRS
    "repo0_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_VCTYPES
    "GIT")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_REPOURLS
    "git@url0.com:repo0")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_HASPKGS
    "HASPACKAGES")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_PREPOSTS
    "POST")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_CATEGORIES
    "Continuous")
  unittest_compare_const( ${PROJECT_NAME}_PRE_REPOSITORIES_DEFAULT
    "")
  unittest_compare_const( ${PROJECT_NAME}_EXTRA_REPOSITORIES_DEFAULT
    "repo0_name")

endfunction()


function(unittest_tribits_get_and_process_extra_repositories_lists)

  set(${PROJECT_NAME}_EXTRAREPOS_FILE  "${CMAKE_CURRENT_LIST_DIR}/emptyFile.cmake")

  tribits_project_define_extra_repositories(
     repo0_name  ""              GIT   "git@url0.com:repo0"  PRE               Continuous
     repo1_name  ""              SVN   "git@url1.com:repo1"  PRE,HASPACKAGES   Continuous
     repo2_name  "some/sub/dir"  SVN   "git@url2.com:repo2"  NOPACKAGES        Nightly
     repo3_name  ""              GIT   "git@url3.com:repo3"  POST              Nightly
     )

  message("\n***")
  message("*** Test tribits_get_and_process_extra_repositories_lists() empty PRE_REPOSITORIES and EXTRA_REPOSITORIES ")
  message("***\n")

  #set(TRIBITS_PROCESS_EXTRAREPOS_LISTS_DEBUG  TRUE)

  set(${PROJECT_NAME}_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE  Nightly)
  set(${PROJECT_NAME}_PRE_REPOSITORIES "")
  set(${PROJECT_NAME}_EXTRA_REPOSITORIES  "")
  
  tribits_get_and_process_extra_repositories_lists()

  unittest_compare_const( ${PROJECT_NAME}_PRE_REPOSITORIES_DEFAULT
    "repo0_name;repo1_name")
  unittest_compare_const( ${PROJECT_NAME}_EXTRA_REPOSITORIES_DEFAULT
    "repo2_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DEFAULT
    "repo0_name;repo1_name;repo2_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_PRE_REPOSITORIES
    "repo0_name;repo1_name")
  unittest_compare_const( ${PROJECT_NAME}_EXTRA_REPOSITORIES
    "repo2_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES
    "repo0_name;repo1_name;repo2_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DIRS
    "repo0_name;repo1_name;some/sub/dir;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_VCTYPES
    "GIT;SVN;SVN;GIT")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_REPOURLS
    "git@url0.com:repo0;git@url1.com:repo1;git@url2.com:repo2;git@url3.com:repo3")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_HASPKGS
    "HASPACKAGES;HASPACKAGES;NOPACKAGES;HASPACKAGES")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_PREPOSTS
    "PRE;PRE;POST;POST")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_CATEGORIES
    "Continuous;Continuous;Nightly;Nightly")

  message("\n***")
  message("*** Test tribits_get_and_process_extra_repositories_lists(), EXTRA_REPOSITORIES=repo2_name")
  message("***\n")

  #set(TRIBITS_PROCESS_EXTRAREPOS_LISTS_DEBUG  TRUE)

  set(${PROJECT_NAME}_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE  Nightly)
  set(${PROJECT_NAME}_PRE_REPOSITORIES "")
  set(${PROJECT_NAME}_EXTRA_REPOSITORIES  repo2_name)
  
  tribits_get_and_process_extra_repositories_lists()

  unittest_compare_const( ${PROJECT_NAME}_PRE_REPOSITORIES_DEFAULT
    "repo0_name;repo1_name")
  unittest_compare_const( ${PROJECT_NAME}_EXTRA_REPOSITORIES_DEFAULT
    "repo2_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DEFAULT
    "repo0_name;repo1_name;repo2_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_PRE_REPOSITORIES
    "repo0_name;repo1_name")
  unittest_compare_const( ${PROJECT_NAME}_EXTRA_REPOSITORIES
    "repo2_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES
    "repo0_name;repo1_name;repo2_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DIRS
    "repo0_name;repo1_name;some/sub/dir")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_VCTYPES
    "GIT;SVN;SVN")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_REPOURLS
    "git@url0.com:repo0;git@url1.com:repo1;git@url2.com:repo2")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_HASPKGS
    "HASPACKAGES;HASPACKAGES;NOPACKAGES")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_PREPOSTS
    "PRE;PRE;POST")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_CATEGORIES
    "Continuous;Continuous;Nightly")

  message("\n***")
  message("*** Test tribits_get_and_process_extra_repositories_lists(), EXTRA_REPOSITORIES=repo3_name")
  message("***\n")

  #set(TRIBITS_PROCESS_EXTRAREPOS_LISTS_DEBUG  TRUE)

  set(${PROJECT_NAME}_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE  Nightly)
  set(${PROJECT_NAME}_PRE_REPOSITORIES "")
  set(${PROJECT_NAME}_EXTRA_REPOSITORIES  repo3_name)
  
  tribits_get_and_process_extra_repositories_lists()

  unittest_compare_const( ${PROJECT_NAME}_PRE_REPOSITORIES_DEFAULT
    "repo0_name;repo1_name")
  unittest_compare_const( ${PROJECT_NAME}_EXTRA_REPOSITORIES_DEFAULT
    "repo2_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DEFAULT
    "repo0_name;repo1_name;repo2_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_PRE_REPOSITORIES
    "repo0_name;repo1_name")
  unittest_compare_const( ${PROJECT_NAME}_EXTRA_REPOSITORIES
    "repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES
    "repo0_name;repo1_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DIRS
    "repo0_name;repo1_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_VCTYPES
    "GIT;SVN;GIT")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_REPOURLS
    "git@url0.com:repo0;git@url1.com:repo1;git@url3.com:repo3")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_HASPKGS
    "HASPACKAGES;HASPACKAGES;HASPACKAGES")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_PREPOSTS
    "PRE;PRE;POST")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_CATEGORIES
    "Continuous;Continuous;Nightly")

  message("\n***")
  message("*** Test tribits_get_and_process_extra_repositories_lists(), PRE_REPOSITORIES=repo0_name")
  message("***\n")

  #set(TRIBITS_PROCESS_EXTRAREPOS_LISTS_DEBUG  TRUE)

  set(${PROJECT_NAME}_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE  Nightly)
  set(${PROJECT_NAME}_PRE_REPOSITORIES repo0_name)
  set(${PROJECT_NAME}_EXTRA_REPOSITORIES  "")
  
  tribits_get_and_process_extra_repositories_lists()

  unittest_compare_const( ${PROJECT_NAME}_PRE_REPOSITORIES_DEFAULT
    "repo0_name;repo1_name")
  unittest_compare_const( ${PROJECT_NAME}_EXTRA_REPOSITORIES_DEFAULT
    "repo2_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DEFAULT
    "repo0_name;repo1_name;repo2_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_PRE_REPOSITORIES
    "repo0_name")
  unittest_compare_const( ${PROJECT_NAME}_EXTRA_REPOSITORIES
    "repo2_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES
    "repo0_name;repo2_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DIRS
    "repo0_name;some/sub/dir;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_VCTYPES
    "GIT;SVN;GIT")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_REPOURLS
    "git@url0.com:repo0;git@url2.com:repo2;git@url3.com:repo3")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_HASPKGS
    "HASPACKAGES;NOPACKAGES;HASPACKAGES")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_PREPOSTS
    "PRE;POST;POST")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_CATEGORIES
    "Continuous;Nightly;Nightly")

  message("\n***")
  message("*** Test tribits_get_and_process_extra_repositories_lists(), PRE_REPOSITORIES=repo1_name")
  message("***\n")

  #set(TRIBITS_PROCESS_EXTRAREPOS_LISTS_DEBUG  TRUE)

  set(${PROJECT_NAME}_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE  Nightly)
  set(${PROJECT_NAME}_PRE_REPOSITORIES repo1_name)
  set(${PROJECT_NAME}_EXTRA_REPOSITORIES  "")
  
  tribits_get_and_process_extra_repositories_lists()

  unittest_compare_const( ${PROJECT_NAME}_PRE_REPOSITORIES_DEFAULT
    "repo0_name;repo1_name")
  unittest_compare_const( ${PROJECT_NAME}_EXTRA_REPOSITORIES_DEFAULT
    "repo2_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DEFAULT
    "repo0_name;repo1_name;repo2_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_PRE_REPOSITORIES
    "repo1_name")
  unittest_compare_const( ${PROJECT_NAME}_EXTRA_REPOSITORIES
    "repo2_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES
    "repo1_name;repo2_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DIRS
    "repo1_name;some/sub/dir;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_VCTYPES
    "SVN;SVN;GIT")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_REPOURLS
    "git@url1.com:repo1;git@url2.com:repo2;git@url3.com:repo3")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_HASPKGS
    "HASPACKAGES;NOPACKAGES;HASPACKAGES")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_PREPOSTS
    "PRE;POST;POST")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_CATEGORIES
    "Continuous;Nightly;Nightly")

  message("\n***")
  message("*** Test tribits_get_and_process_extra_repositories_lists(), PRE_REPOSITORIES=repo0_name, EXTRA_REPOSITORIES=repo3_name")
  message("***\n")

  set(${PROJECT_NAME}_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE  Nightly)
  set(${PROJECT_NAME}_PRE_REPOSITORIES repo0_name)
  set(${PROJECT_NAME}_EXTRA_REPOSITORIES  repo3_name)
  
  tribits_get_and_process_extra_repositories_lists()

  unittest_compare_const( ${PROJECT_NAME}_PRE_REPOSITORIES_DEFAULT
    "repo0_name;repo1_name")
  unittest_compare_const( ${PROJECT_NAME}_EXTRA_REPOSITORIES_DEFAULT
    "repo2_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DEFAULT
    "repo0_name;repo1_name;repo2_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_PRE_REPOSITORIES
    "repo0_name")
  unittest_compare_const( ${PROJECT_NAME}_EXTRA_REPOSITORIES
    "repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES
    "repo0_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_DIRS
    "repo0_name;repo3_name")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_VCTYPES
    "GIT;GIT")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_REPOURLS
    "git@url0.com:repo0;git@url3.com:repo3")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_HASPKGS
    "HASPACKAGES;HASPACKAGES")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_PREPOSTS
    "PRE;POST")
  unittest_compare_const( ${PROJECT_NAME}_ALL_EXTRA_REPOSITORIES_CATEGORIES
    "Continuous;Nightly")

endfunction()


#####################################################################
#
# Execute the unit tests
#
#####################################################################

unittest_initialize_vars()

# Set common/base options
set(PROJECT_NAME "DummyProject")
set(PROJECT_SOURCE_DIR "/home/me/DummyProject")

unittest_tribits_parse_extrarepo_packstat()

unittest_tribits_process_extrarepos_lists()
unittest_tribits_process_extrarepos_lists_old_repotype()

unittest_tribits_get_and_process_extra_repositories_lists()

# Pass in the number of expected tests that must pass!
unittest_final_result(143)
