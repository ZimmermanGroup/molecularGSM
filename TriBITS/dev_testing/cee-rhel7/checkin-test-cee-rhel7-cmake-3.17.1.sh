#!/bin/bash
 
# Used to test TriBITS on any CEE RHEL7 machine which uses CMake 3.17.1
#
# You can link this script into any location and it will work out of the box.

if [ "$TRIBITS_BASE_DIR" == "" ] ; then
  _ABS_FILE_PATH=`readlink -f $0`
  _SCRIPT_DIR=`dirname $_ABS_FILE_PATH`
  TRIBITS_BASE_DIR=$_SCRIPT_DIR/../..
fi

TRIBITS_BASE_DIR_ABS=$(readlink -f $TRIBITS_BASE_DIR)
#echo "TRIBITS_BASE_DIR_ABS = $TRIBITS_BASE_DIR_ABS"

# Load the env
source ${_SCRIPT_DIR}/load-env.sh

# Create extra builds run by this script

echo "
-DTPL_ENABLE_MPI:BOOL=ON
-DCMAKE_BUILD_TYPE:STRING=DEBUG
-DTriBITS_ENABLE_DEBUG:BOOL=ON
-DTriBITS_ENABLE_Fortran:BOOL=ON
-DTriBITS_CTEST_DRIVER_COVERAGE_TESTS=TRUE
-DTriBITS_CTEST_DRIVER_MEMORY_TESTS=TRUE
-DTribitsExProj_INSTALL_BASE_DIR=/tmp/tribits_install_tests
-DTribitsExProj_INSTALL_OWNING_GROUP=wg-run-as-atdm-devops
-DTriBITS_ENABLE_REAL_GIT_CLONE_TESTS=ON
" > MPI_DEBUG_CMake-3.17.1.config

echo "
-DTPL_ENABLE_MPI:BOOL=OFF
-DCMAKE_BUILD_TYPE:STRING=RELEASE
-DTriBITS_ENABLE_DEBUG:BOOL=OFF
-DCMAKE_C_COMPILER=gcc
-DCMAKE_CXX_COMPILER=g++
-DCMAKE_Fortran_COMPILER=gfortran
-DTriBITS_CTEST_DRIVER_COVERAGE_TESTS=TRUE
-DTriBITS_CTEST_DRIVER_MEMORY_TESTS=TRUE
-DTribitsExProj_INSTALL_BASE_DIR=/tmp/tribits_install_tests
-DTribitsExProj_INSTALL_OWNING_GROUP=wg-run-as-atdm-devops
-DTriBITS_ENABLE_REAL_GIT_CLONE_TESTS=ON \
" > SERIAL_RELEASE_CMake-3.17.1.config

# Run checkin-test.py

$TRIBITS_BASE_DIR_ABS/checkin-test.py \
--default-builds= \
--st-extra-builds=MPI_DEBUG_CMake-3.17.1,SERIAL_RELEASE_CMake-3.17.1 \
--extra-cmake-options="-DPYTHON_EXECUTABLE=/usr/bin/python2.7" \
--ctest-timeout=180 \
--skip-case-no-email \
-j 8 \
"$@"
