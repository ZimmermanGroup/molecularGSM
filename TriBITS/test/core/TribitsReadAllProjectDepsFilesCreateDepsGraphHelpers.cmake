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

message("The outer project: PROJECT_NAME = ${PROJECT_NAME}")
message("The outer project: ${PROJECT_NAME}_TRIBITS_DIR = ${${PROJECT_NAME}_TRIBITS_DIR}")

set( CMAKE_MODULE_PATH
  "${${PROJECT_NAME}_TRIBITS_DIR}/core/utils"
  "${${PROJECT_NAME}_TRIBITS_DIR}/core/package_arch"
  )


include(PrintVar)


#####################################################################
#
# Set common/base options
#
#####################################################################

set(PROJECT_SOURCE_DIR "${${PROJECT_NAME}_TRIBITS_DIR}/examples/MockTrilinos")
print_var(PROJECT_SOURCE_DIR)
set(REPOSITORY_DIR ".")
print_var(REPOSITORY_DIR)

# Before we change the project name, we have to set the TRIBITS_DIR so that it
# will point in the right place.  There is TriBITS code being called that must
# have this variable set!

set(Trilinos_TRIBITS_DIR ${${PROJECT_NAME}_TRIBITS_DIR})

# Set the mock project name last to override the outer project
set(PROJECT_NAME "Trilinos")
message("The inner test project: PROJECT_NAME = ${PROJECT_NAME}")
message("The inner tets project: ${PROJECT_NAME}_TRIBITS_DIR = ${${PROJECT_NAME}_TRIBITS_DIR}")

# Includes

include(TribitsReadAllProjectDepsFilesCreateDepsGraph)
include(TribitsProcessTplsLists)
include(UnitTestHelpers)
include(GlobalSet)

# For Running tribits_read_all_project_deps_files_create_deps_graph()

set(${PROJECT_NAME}_NATIVE_REPOSITORIES .)

set(${PROJECT_NAME}_PACKAGES_FILE_OVERRIDE
  ${CMAKE_CURRENT_LIST_DIR}/MiniMockTrilinosFiles/PackagesList.cmake)
set(${PROJECT_NAME}_TPLS_FILE_OVERRIDE
  ${CMAKE_CURRENT_LIST_DIR}/MiniMockTrilinosFiles/TPLsList.cmake)

set(${PROJECT_NAME}_EXTRA_REPOSITORIES extraRepoTwoPackages)

# For running other lower-level functions

set(REPOSITORY_NAME "Trilinos")

include(${CMAKE_CURRENT_LIST_DIR}/MiniMockTrilinosFiles/PackagesList.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/MiniMockTrilinosFiles/TPLsList.cmake)

set(EXTRA_REPO_NAME extraRepoTwoPackages)
set(EXTRA_REPO_DIR extraRepoTwoPackages)

set(REPOSITORY_NAME ${EXTRA_REPO_NAME})
include(${PROJECT_SOURCE_DIR}/${EXTRA_REPO_NAME}/PackagesList.cmake)

set(${EXTRA_REPO_NAME}_TPLS_FINDMODS_CLASSIFICATIONS)

set(${PROJECT_NAME}_ALL_REPOSITORIES "." "${EXTRA_REPO_NAME}")

set( ${PROJECT_NAME}_ASSERT_MISSING_PACKAGES ON )


#####################################################################
#
# Helper macros for unit tests
#
#####################################################################


macro(unittest_helper_read_packages_and_dependencies)

  set(${PROJECT_NAME}_ALL_REPOSITORIES)
  tribits_read_all_project_deps_files_create_deps_graph()

endmacro()
