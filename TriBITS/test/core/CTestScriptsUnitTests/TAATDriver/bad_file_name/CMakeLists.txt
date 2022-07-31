cmake_minimum_required(VERSION 3.17.0)
set(PROJECT_NAME TAATDriver)
project(${PROJECT_NAME} NONE)

include(${CMAKE_CURRENT_LIST_DIR}/../CommonTAATDriverBoilerPlate.cmake)

tribits_add_advanced_test(
  TAAT_COPY_FILES_TO_TEST_DIR_bad_file_name
  OVERALL_WORKING_DIRECTORY TEST_NAME
  TEST_0 COPY_FILES_TO_TEST_DIR  this_file_does_not_exist.txt
  TEST_1 COPY_FILES_TO_TEST_DIR  test_file_a.txt
    SOURCE_DIR ${COPY_FILE_SOURCE_DIR}
  TEST_2 CMND cat ARGS test_file_a.txt
    PASS_REGULAR_EXPRESSION "This is Test File A"
  )
