#!/bin/bash
 
# Used to test TriBITS on crf450 using different versions of CMake.
#
# USAGE:
#
#   ./checkin-test-crf450.sh [--push]
#
# Do not pass in any extra action items.
#
# NOTE: The repo must be in a state ready to be pushed and this will do an
# initial pull.  To do local testing, just run each script individually
# instead.
#

if [ "$TRIBITS_BASE_DIR" == "" ] ; then
  _ABS_FILE_PATH=`readlink -f $0`
  _SCRIPT_DIR=`dirname $_ABS_FILE_PATH`
  TRIBITS_BASE_DIR=$_SCRIPT_DIR/../..
fi

TRIBITS_BASE_DIR_ABS=$(readlink -f $TRIBITS_BASE_DIR)
#echo "TRIBITS_BASE_DIR_ABS = $TRIBITS_BASE_DIR_ABS"

${_SCRIPT_DIR}/checkin-test-crf450-cmake-3.17.0.sh \
--send-email-to= --do-all

${_SCRIPT_DIR}/checkin-test-crf450-cmake-3.17.0-python-3.5.2.sh \
--send-email-to= --configure --build --test

$TRIBITS_BASE_DIR_ABS/checkin-test.py \
--default-builds= \
--st-extra-builds=MPI_DEBUG_CMake-3.17.0,SERIAL_RELEASE_CMake-3.17.0,MPI_DEBUG_CMake-3.17.0_Python-3.5.2 \
"$@"
