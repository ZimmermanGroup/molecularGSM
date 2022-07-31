cmake_minimum_required(VERSION 3.17.0)
set(PROJECT_NAME TAATDriver)
project(${PROJECT_NAME} NONE)

include(${CMAKE_CURRENT_LIST_DIR}/../CommonTAATDriverBoilerPlate.cmake)

tribits_add_advanced_test(
  TAAT_COPY_FILES_TO_TEST_DIR_bad_dest_dir
  OVERALL_WORKING_DIRECTORY TEST_NAME
  TEST_0 CMND mkdir ARGS read_only_dir
  TEST_1 CMND chmod ARGS a-w read_only_dir
  TEST_2 COPY_FILES_TO_TEST_DIR  test_file_b.txt  # Good file
    SOURCE_DIR ${COPY_FILE_SOURCE_DIR}   # Good source dir
    DEST_DIR read_only_dir/subdir        # Can't create directory in read-only dir
  TEST_3 COPY_FILES_TO_TEST_DIR  test_file_a.txt
    SOURCE_DIR ${COPY_FILE_SOURCE_DIR}
    DEST_DIR good_dest_dir
  TEST_4 CMND cat ARGS good_dest_dir/test_file_a.txt
    PASS_REGULAR_EXPRESSION "This is Test File A"
  )
# NOTE: The above failing test is used to test that a bad dest dir is handled
# gracefully and allows later commands to run and pass (but the overall test
# will fail).
