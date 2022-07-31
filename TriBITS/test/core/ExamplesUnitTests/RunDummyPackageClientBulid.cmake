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
# This file is run as a cmake -P script to create a new dummy project
# to test the generated <Package>Config.cmake export file
#

message("DUMMY_PROJECT_NAME = '${DUMMY_PROJECT_NAME}'")
message("DUMMY_PROJECT_DIR = '${DUMMY_PROJECT_DIR}'")
message("FIND_PACKAGE_NAME = '${FIND_PACKAGE_NAME}'")
message("FIND_PACKAGE_COMPONENTS = '${FIND_PACKAGE_COMPONENTS}'")
message("FIND_PACKAGE_OPTIONAL_COMPONENTS = '${FIND_PACKAGE_OPTIONAL_COMPONENTS}'")
message("CMAKE_PREFIX_PATH = '${CMAKE_PREFIX_PATH}'")
message("EXPORT_VAR_PREFIX = '${EXPORT_VAR_PREFIX}'")
message("CMAKE_COMMAND = '${CMAKE_COMMAND}'")

message("Create the dummy client directory ...")
file(MAKE_DIRECTORY "${DUMMY_PROJECT_DIR}")

set(DUMMY_PACKAGE_CLIENT_CMAKELIST_FILE_IN
  "${CMAKE_CURRENT_LIST_DIR}/DummyPackageClientCMakeLists.txt" )
set(DUMMY_PACKAGE_CLIENT_CMAKELIST_FILE_OUT
  "${DUMMY_PROJECT_DIR}/CMakeLists.txt" )

message("Create dummy ${DUMMY_PACKAGE_CLIENT_CMAKELIST_FILE_OUT} file ...")
configure_file(
  "${DUMMY_PACKAGE_CLIENT_CMAKELIST_FILE_IN}"
  "${DUMMY_PACKAGE_CLIENT_CMAKELIST_FILE_OUT}"
  COPYONLY
  )

message("Configure the dummy project to print the variables in ${CMAKE_CURRENT_BINARY_DIR}/${DUMMY_PROJECT_DIR} ...")
execute_process(
  COMMAND "${CMAKE_COMMAND}"
    -DDUMMY_PROJECT_NAME=${DUMMY_PROJECT_NAME}
    "-DFIND_PACKAGE_NAME=${FIND_PACKAGE_NAME}"
    "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
    "-DFIND_PACKAGE_COMPONENTS=${FIND_PACKAGE_COMPONENTS}"
    "-DFIND_PACKAGE_OPTIONAL_COMPONENTS=${FIND_PACKAGE_OPTIONAL_COMPONENTS}"
    -DEXPORT_VAR_PREFIX=${EXPORT_VAR_PREFIX}
    .
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${DUMMY_PROJECT_DIR}"
  )
