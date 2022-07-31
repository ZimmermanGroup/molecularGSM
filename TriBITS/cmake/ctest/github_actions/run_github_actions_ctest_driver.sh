#!/bin/bash -e

#
# Run local configure/build/test and submit to CDash with
# tribits_ctest_driver().
#
# Usage:
#
#   $ cd <some-base-dir>/
#
#   <this-dir>/run_github_actions_ctest_driver.sh \
#     --os <os> --cmake-ver <cmake-ver> \
#     --generator ninja|makefiles \
#     --python-ver <python-ver> \
#     --cxx-compiler-and-ver <cxx-compiler-and-ver> \
#     [ --fortran-compiler-and-ver <fortran-comiler-and-ver> ]
#     [ --no-have-ninja <no-have-ninja> ]
#     [ --github-repo-match-to-submit <github-repo-match-to-submit> ]
#
# This is called by the GitHub Actions script:
#
#   TriBITS/.github/workflows/tribits_testing.yml
#
# but can also be run locally to develop on and debug.
#
# A suffix can be added the each build name by setting the env var:
#
#   export CTEST_BUILD_NAME_SUFFIX=<suffix>
#
# This will add the suffix as `<build-name><suffix>` so if you want a `_` or
# `-` then you must add it manually as part of `<suffix>`.
#
# NOTE: This will create the subdir 'tribits-build' under $PWD if that
# directory does not already exist.  But it will not delete an existing
# directly 'tribits-build' if it already exists (but that is usually fine to
# do rebuilds).
#
# NOTE: One can control various behaviors and other options as documented with
# tribits_ctest_driver() including not submitting to CDash to do testing
# output spamming CDash by running with:
#
#   env CTEST_DO_SUBMIT=OFF <this-dir>/run_github_actions_ctest_driver.sh [options]
#
# NOTE: One can turn off submits by default by setting:
#
#   --github-repo-match-to-submit <github-repo-match-to-submit>
#
# and submits to CDash will be disabled setting CTEST_DO_SUBMIT=OFF if
# ${GITHUB_REPOSITORY} does not match <github-repo-match-to-submit>.  This is
# set the the GitHub Actions job by default to avoid submitting from froks of
# the repo (see TriBITSPub/TriBITS#415).
#

# Get location of TriBITS and this dir under TriBITS
if [[ "$TRIBITS_BASE_DIR" == "" ]] ; then
  _ABS_FILE_PATH=`readlink -f $0`
  THIS_BASE_DIR=`dirname $_ABS_FILE_PATH`
  TRIBITS_BASE_DIR=`realpath $THIS_BASE_DIR/../../..`
else
  THIS_BASE_DIR="${TRIBITS_BASE_DIR}/cmake/ctest/github_actions"
fi


#
# Functions
#


function assert_option_has_value {
  value=$1
  if [[ ${value:0:1} == "-" ]] ; then
    echo "Error, expecting a value, not next option '${value}'" >&2
    exit 1
  fi
}


function assert_required_option_set {
  arg_name=$1
  arg_value=$2
  #echo "${arg_name} ${arg_value}"
  if [[ "${arg_value}" == "" ]] ; then
    echo "Error, argument ${arg_name} can not be empty!"
    exit 1
  fi
}


#
# A) Parse and assert command-line arguments
#

os=
cmake_ver=
cmake_generator=
python_ver=
cxx_compiler_and_ver=
fortran_compiler_and_ver=
no_have_ninja=
github_repo_match_to_submit=

while (( "$#" )); do
  case "$1" in
    --os)
      assert_option_has_value $2
      os=$2
      shift 2
      ;;
    --cmake-ver)
      assert_option_has_value $2
      cmake_ver=$2
      shift 2
      ;;
    --generator)
      assert_option_has_value $2
      cmake_generator=$2
      shift 2
      ;;
    --python-ver)
      assert_option_has_value $2
      python_ver=$2
      shift 2
      ;;
    --cxx-compiler-and-ver)
      assert_option_has_value $2
      cxx_compiler_and_ver=$2
      shift 2
      ;;
    --fortran-compiler-and-ver)
      if [[ -n "$2" ]] && [[ ${2:0:1} != "-" ]]; then
        fortran_compiler_and_ver=$2
        shift 2
      else
        shift 1
      fi
      ;;
    --no-have-ninja)
      if [[ -n "$2" ]] && [[ ${2:0:1} != "-" ]]; then
        no_have_ninja=$2
        shift 2
      else
        shift 1
      fi
      ;;
    --github-repo-match-to-submit)
      if [[ -n "$2" ]] && [[ ${2:0:1} != "-" ]]; then
        github_repo_match_to_submit=$2
        shift 2
      else
        shift 1
      fi
      ;;
    *)
      echo "Error: The argument '$1' is not supported!" >&2
      exit 1
      ;;
  esac
done

assert_required_option_set --os ${os}
assert_required_option_set --cmake-ver "${cmake_ver}"
assert_required_option_set --generator "${cmake_generator}"
assert_required_option_set --python-ver "${python_ver}"
assert_required_option_set --cxx-compiler-and-ver "${cxx_compiler_and_ver}"
# NOTE: Fortran is not required!
# NOTE: Ninja is not required

#
# B) Set up options for running build
#

echo
echo "Options to run set in github actions driver:"
echo

# CTEST_SITE
export CTEST_SITE=${os}
echo "CTEST_SITE = '${CTEST_SITE}'"

# CTEST_BUILD_NAME
CTEST_BUILD_NAME=tribits_cmake-${cmake_ver}_${cmake_generator}_python-${python_ver}_${cxx_compiler_and_ver}
if [[ "${fortran_compiler_and_ver}" == "" ]] ; then
  CTEST_BUILD_NAME=${CTEST_BUILD_NAME}_nofortran
fi
if [[ "${no_have_ninja}" == "no-ninja" ]] ; then
  CTEST_BUILD_NAME=${CTEST_BUILD_NAME}_noninja
fi
export CTEST_BUILD_NAME
echo "CTEST_BUILD_NAME = '${CTEST_BUILD_NAME}'"

# TRIBITS_BUILD_TWEAKS_FILE
export TRIBITS_BUILD_TWEAKS_FILE="${THIS_BASE_DIR}/${CTEST_BUILD_NAME}_tweaks.cmake"
echo "TRIBITS_BUILD_TWEAKS_FILE = '${TRIBITS_BUILD_TWEAKS_FILE}'"

# Update CTEST_BUILD_NAME with suffix (after getting the tweaks file name)
echo "CTEST_BUILD_NAME_SUFFIX = '${CTEST_BUILD_NAME_SUFFIX}'"
if [[ "${CTEST_BUILD_NAME_SUFFIX}" != "" ]] ; then
  export CTEST_BUILD_NAME="${CTEST_BUILD_NAME}${CTEST_BUILD_NAME_SUFFIX}"
fi
echo "CTEST_BUILD_NAME = '${CTEST_BUILD_NAME}'"

# Update CTEST_BUILD_NAME with PR ID (after getting the tweaks file name)
if [[ "${GITHUB_EVENT_PATH}" != "" ]] ; then
  pull_number=$(jq --raw-output .pull_request.number "$GITHUB_EVENT_PATH")
  echo "pull_number = '${pull_number}'"
  if [[ "${pull_number}" != "null" ]] ; then
    export CTEST_BUILD_NAME="pr-${pull_number}_${CTEST_BUILD_NAME}${CTEST_BUILD_NAME_SUFFIX}"
    export CTEST_CHANGE_ID=${pull_number}
  fi
fi
echo "CTEST_BUILD_NAME = '${CTEST_BUILD_NAME}'"

# CTEST_CMAKE_GENERATOR
if [[ "${cmake_generator}" == "ninja" ]] ; then
  CTEST_CMAKE_GENERATOR=Ninja
elif [[ "${cmake_generator}" == "makefiles" ]] ; then
  CTEST_CMAKE_GENERATOR="Unix Makefiles"
else
  echo "Error, --generator ${cmake_generator} is invalid!  Only 'makefiles', 'ninja'!" >&2
  exit 1
fi
export CTEST_CMAKE_GENERATOR
echo "CTEST_CMAKE_GENERATOR = '${CTEST_CMAKE_GENERATOR}'"

# CTEST_TEST_TYPE, TriBITS_TRACK
TriBITS_TRACK=
if [[ "${GITHUB_EVENT_NAME}" == "schedule" ]]; then
  CTEST_TEST_TYPE=Nightly  # Only allows one build with a given site name and build name
elif [[ "${GITHUB_EVENT_NAME}" == "push" ]]; then
  CTEST_TEST_TYPE=Continuous
elif [[ "${GITHUB_EVENT_NAME}" == "pull_request" ]]; then
  CTEST_TEST_TYPE=Continuous  # Allows multiple builds with same site name and build name
  TriBITS_TRACK="Pull Request"
else
  CTEST_TEST_TYPE=Experimental
fi
export CTEST_TEST_TYPE
export TriBITS_TRACK
echo "CTEST_TEST_TYPE = '${CTEST_TEST_TYPE}'"
echo "TriBITS_TRACK = '${TriBITS_TRACK}'"

# CTETS_DO_SUBMIT
if [[ "${github_repo_match_to_submit}" != "" ]] \
  && [[  "${GITHUB_REPOSITORY}" != "${github_repo_match_to_submit}" ]] ; then
  echo "NOTE: Setting CTEST_DO_SUBMIT=OFF since GITHUB_REPOSITORY='${GITHUB_REPOSITORY}' != '${github_repo_match_to_submit}'!"
  export CTEST_DO_SUBMIT=OFF
fi
echo "CTEST_DO_SUBMIT = '${CTEST_DO_SUBMIT}'"


#
# C) Run the local configure, build, test and submit using exported vars above
#

echo

if [[ -d tribits-build ]] ; then
  echo "tribits-build/ already exists so leaving it."
else
  echo "tribits-build/ does not exist yet so creating it."
  mkdir tribits-build
fi

cd tribits-build/

echo

ctest -V -S \
  ${TRIBITS_BASE_DIR}/cmake/ctest/github_actions/ctest_github_actions_serial_debug.cmake
