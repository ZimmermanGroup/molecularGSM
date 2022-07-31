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
# Gather up arguments to pass through to inner configures and builds of
# example/test CMake and TriBITS projects.
#

set(SERIAL_PASSTHROUGH_CONFIGURE_ARGS
  -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  )

if ({${PROJECT_NAME}_ENABLE_Fortran)
  append_set(SERIAL_PASSTHROUGH_CONFIGURE_ARGS
    -DCMAKE_Fortran_COMPILER=${CMAKE_Fortran_COMPILER} )
endif()

set(COMMON_ENV_ARGS_PASSTHROUGH
  #-C ${${PROJECT_NAME}_TRIBITS_DIR}/core/utils/UseCcacheIfExists.cmake
  -DTPL_ENABLE_MPI=${TPL_ENABLE_MPI}
  -DHeaderOnlyTpl_INCLUDE_DIRS=${${PROJECT_NAME}_TRIBITS_DIR}/examples/tpls/HeaderOnlyTpl
  )

if (TPL_ENABLE_MPI)
  append_set(COMMON_ENV_ARGS_PASSTHROUGH
    -DMPI_C_COMPILER=${MPI_C_COMPILER}
    -DMPI_CXX_COMPILER=${MPI_CXX_COMPILER}
    -DMPI_Fortran_COMPILER=${MPI_Fortran_COMPILER}
    -DMPI_EXEC=${MPI_EXEC}
    -DMPI_EXEC_DEFAULT_NUMPROCS=${MPI_EXEC_DEFAULT_NUMPROCS}
    -DMPI_EXEC_MAX_NUMPROCS=${MPI_EXEC_MAX_NUMPROCS}
    -DMPI_EXEC_NUMPROCS_FLAG=${MPI_EXEC_NUMPROCS_FLAG}
    -DMPI_EXEC_PRE_NUMPROCS_FLAGS=${MPI_EXEC_PRE_NUMPROCS_FLAGS}
    -DMPI_EXEC_POST_NUMPROCS_FLAGS=${MPI_EXEC_POST_NUMPROCS_FLAGS}
    )
  set(TEST_MPI_1_SUFFIX "_MPI_1")
else()
  append_set(COMMON_ENV_ARGS_PASSTHROUGH
    ${SERIAL_PASSTHROUGH_CONFIGURE_ARGS}
    )
  set(TEST_MPI_1_SUFFIX "")
endif()

if (CMAKE_GENERATOR STREQUAL "Ninja")
  set(USING_GENERATOR_NINJA TRUE)
  set(GENERATOR_CONFIG_PASSTHORUGH_ARGS -GNinja)
  set(MAKE_PARALLEL_ARG NP=1)
else()
  set(USING_GENERATOR_NINJA FALSE)
  set(GENERATOR_CONFIG_PASSTHORUGH_ARGS)
  set(MAKE_PARALLEL_ARG -j1)
endif()
