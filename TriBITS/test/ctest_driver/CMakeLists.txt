tribits_add_advanced_test( CTestDriverUnitTests
  OVERALL_WORKING_DIRECTORY TEST_NAME
  TEST_0 CMND ${CMAKE_COMMAND}
    ARGS
      -DPROJECT_NAME=${PROJECT_NAME}
      -D${PROJECT_NAME}_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -P "${CMAKE_CURRENT_SOURCE_DIR}/CTestDriverUnitTests.cmake"
    PASS_REGULAR_EXPRESSION_ALL
      "Final UnitTests Result: num_run = 9"
      "Final UnitTests Result: PASSED"
  )


tribits_add_advanced_test( TribitsGetCTestTestXmlDir
  OVERALL_WORKING_DIRECTORY TEST_NAME

  TEST_0 CMND ${CMAKE_COMMAND}
    ARGS
      -DPROJECT_NAME=${PROJECT_NAME}
      -D${PROJECT_NAME}_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DCTEST_BUILD_DIR="${CMAKE_CURRENT_LIST_DIR}/data/dummy_build_dir"
      -P "${${PROJECT_NAME}_TRIBITS_DIR}/ctest_driver/TribitsGetCTestTestXmlDir.cmake"
    PASS_REGULAR_EXPRESSION_ALL
      "${CMAKE_CURRENT_LIST_DIR}/data/dummy_build_dir/Testing/20101015-1112"
  )


add_subdirectory(MockCTestDriver)


if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
  add_subdirectory(TribitsExampleProject)
endif()

set(${PROJECT_NAME}_ENABLE_REAL_GIT_CLONE_TESTS OFF CACHE BOOL
  "If TRUE, then perform tests that do real clones of repos from GitHub.")

if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux" AND
    ${PROJECT_NAME}_ENABLE_REAL_GIT_CLONE_TESTS
  )
  add_subdirectory(TribitsExampleMetaProject)
endif()
