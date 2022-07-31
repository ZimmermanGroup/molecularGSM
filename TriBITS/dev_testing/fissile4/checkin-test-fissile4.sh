#!/bin/bash

# Used to test TriBITS on any of the ORNL CASL Fissile/Spy machines
#
# This script requires that the VERA dev env be loaded by sourcing the script:
#
#  . /projects/vera/gcc-4.8.3/load_dev_env.[sh,csh]
#
# You can source this script either in your shell startup script
# (e.g. .bash_profile) or you can source it manually whenever you need to set
# up to build VERA software.
#
# You can link this script into any location and it will work out of the box.
#
# NOTE: You will also want to create a local-checkin-test-defaults.py file to
# set -j<N> for the particular machine (see --help).  If that file does not
# exist, one is created with -j16 (for the Fissile4 that have 32 total cores).
#

if [ "$TRIBITS_BASE_DIR" == "" ] ; then
  _ABS_FILE_PATH=`readlink -f $0`
  _SCRIPT_DIR=`dirname $_ABS_FILE_PATH`
  TRIBITS_BASE_DIR=$_SCRIPT_DIR/../..
fi

TRIBITS_BASE_DIR_ABS=$(readlink -f $TRIBITS_BASE_DIR)
#echo "TRIBITS_BASE_DIR_ABS = $TRIBITS_BASE_DIR_ABS"

# Check to make sure that the env has been loaded correctly
if [ "$LOADED_TRIBITS_DEV_ENV" != "gcc-4.8.3" ] ; then
  echo "Error, must source /projects/vera/gcc-4.8.3/load_dev_env.[sh,csh] before running checkin-test-vera.sh!"
  exit 1
fi

# Create local defaults file if one does not exist
_LOCAL_CHECKIN_TEST_DEFAULTS=local-checkin-test-defaults.py
if [ -f $_LOCAL_CHECKIN_TEST_DEFAULTS ] ; then
  echo "File $_LOCAL_CHECKIN_TEST_DEFAULTS already exists, leaving it!"
else
  echo "Creating default file $_LOCAL_CHECKIN_TEST_DEFAULTS!"
  echo "
defaults = [
  \"-j16\",
  ]
  " > $_LOCAL_CHECKIN_TEST_DEFAULTS
fi

# Use CMake 2.8.11 to test since that is the min version we are enforcing!
export PATH=/projects/vera/common_tools/cmake-2.8.11/bin:$PATH

$TRIBITS_BASE_DIR_ABS/checkin-test.py \
--extra-cmake-options="-DPYTHON_EXECUTABLE=/usr/bin/python2.6" \
--ctest-timeout=180 \
--skip-case-no-email \
"$@"
