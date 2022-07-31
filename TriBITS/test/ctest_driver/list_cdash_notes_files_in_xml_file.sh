#!/bin/bash -e

#
# Prints out a list of the notes files in
# <build-dir>/Testing/<buildstarttime>/Notes.xm
#
# Usage:
#
#   env \
#     TRIBITS_PROJECT_NAME=<project-name> \
#     ${TRIBITS_PROJECT_NAME}_TRIBITS_DIR=<tribits-dir> \
#     CTEST_BUILD_DIR=<build-dir> \
#   <this-dir>/list_cdash_notes_files_in_xml_file.sh \
#

#echo "TRIBITS_PROJECT_NAME=${TRIBITS_PROJECT_NAME}"
TRIBITS_TRIBITS_DIR_NAME=${TRIBITS_PROJECT_NAME}_TRIBITS_DIR
#echo "${TRIBITS_PROJECT_NAME}_TRIBITS_DIR=${!TRIBITS_TRIBITS_DIR_NAME}"
#echo "CTEST_BUILD_DIR=${CTEST_BUILD_DIR}"

CTEST_TESTING_XML_DIR=`cmake -DPROJECT_NAME=${TRIBITS_PROJECT_NAME} -D${TRIBITS_PROJECT_NAME}_TRIBITS_DIR=${!TRIBITS_TRIBITS_DIR_NAME} -DCTEST_BUILD_DIR=${CTEST_BUILD_DIR} -P ${!TRIBITS_TRIBITS_DIR_NAME}/ctest_driver/TribitsGetCTestTestXmlDir.cmake 2>&1`

echo "CTEST_TESTING_XML_DIR=$CTEST_TESTING_XML_DIR"

CTEST_NOTES_FILE="${CTEST_TESTING_XML_DIR}/Notes.xml"

grep "<Note " "${CTEST_NOTES_FILE}"
