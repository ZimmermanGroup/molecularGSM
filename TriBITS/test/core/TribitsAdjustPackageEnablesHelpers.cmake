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

include("${CMAKE_CURRENT_LIST_DIR}/TribitsReadAllProjectDepsFilesCreateDepsGraphHelpers.cmake")
include(TribitsAdjustPackageEnables)


#####################################################################
#
# Helper macros for unit tests
#
#####################################################################


macro(unittest_helper_read_and_process_packages)
  tribits_process_packages_and_dirs_lists(${PROJECT_NAME} ".")
  set(${PROJECT_NAME}_TPLS_FILE "dummy")
  tribits_process_tpls_lists(${PROJECT_NAME} ".")
  tribits_process_packages_and_dirs_lists(${EXTRA_REPO_NAME} ${EXTRA_REPO_DIR})
  set(${EXTRA_REPO_NAME}_TPLS_FILE "dummy")
  tribits_process_tpls_lists(${EXTRA_REPO_NAME} ${EXTRA_REPO_DIR})
  tribits_read_deps_files_create_deps_graph()
  set_default(${PROJECT_NAME}_ENABLE_ALL_PACKAGES OFF)
  set_default(${PROJECT_NAME}_ENABLE_SECONDARY_TESTED_CODE OFF)
  set(DO_PROCESS_MPI_ENABLES ON) # Should not be needed but CMake is not working!
  foreach(SE_PKG ${${PROJECT_NAME}_SE_PACKAGES})
    global_set(${SE_PKG}_FULL_ENABLED_DEP_PACKAGES)
  endforeach()
  tribits_print_enables_before_adjust_package_enables()
  tribits_adjust_package_enables(TRUE)
  tribits_print_enables_after_adjust_package_enables()
  tribits_set_up_enabled_only_dependencies()
endmacro()
