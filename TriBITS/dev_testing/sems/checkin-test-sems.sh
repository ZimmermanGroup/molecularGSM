#!/bin/bash -e

# Used to test TriBITS on any SNL machine with the SEMS Dev ENv
#
# Requires that the TriBITS repo be cloned under Trilinos as:
#
#   Trilinos/
#     TriBITS/
#
# You can link this script into any location and it will work out of the box.
#
# NOTE: You will also want to create a local-checkin-test-defaults.py file to
# set -j<N> for the particular machine (see --help).
#

if [ "$TRIBITS_BASE_DIR" == "" ] ; then
  _ABS_FILE_PATH=`readlink -f $0`
  _SCRIPT_DIR=`dirname $_ABS_FILE_PATH`
  TRIBITS_BASE_DIR=$_SCRIPT_DIR/../..
fi

TRIBITS_BASE_DIR_ABS=$(readlink -f $TRIBITS_BASE_DIR)
#echo "TRIBITS_BASE_DIR_ABS = $TRIBITS_BASE_DIR_ABS"

TRILINOS_BASE_DIR_ABS=$(readlink -f $TRIBITS_BASE_DIR_ABS/..)
#echo "TRIBITS_BASE_DIR_ABS = $TRIBITS_BASE_DIR_ABS"

# Make sure the right env is loaded!
export TRILINOS_SEMS_DEV_ENV_VERBOSE=1
source $TRILINOS_BASE_DIR_ABS/cmake/load_sems_dev_env.sh default default sems-cmake/3.17.1

echo "
" > MPI_DEBUG.config

echo "
-DCMAKE_C_COMPILER=gcc
-DCMAKE_CXX_COMPILER=g++
-DCMAKE_Fortran_COMPILER=gfortran
" > SERIAL_RELEASE.config

# Create local defaults file if one does not exist
_LOCAL_CHECKIN_TEST_DEFAULTS=local-checkin-test-defaults.py
if [ -f $_LOCAL_CHECKIN_TEST_DEFAULTS ] ; then
  echo "File $_LOCAL_CHECKIN_TEST_DEFAULTS already exists, leaving it!"
else
  echo "Creating default file $_LOCAL_CHECKIN_TEST_DEFAULTS!"
  echo "
defaults = [
  \"-j8\",
  ]
  " > $_LOCAL_CHECKIN_TEST_DEFAULTS
fi

$TRIBITS_BASE_DIR_ABS/checkin-test.py \
--ctest-timeout=180 \
--skip-case-no-email \
"$@"

