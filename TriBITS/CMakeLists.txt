################################################################################
#                                                                              #
#              TriBITS Project and Package CMakeLists.txt File                 #
#                                                                              #
################################################################################

if (NOT TRIBITS_PROCESSING_PACKAGE)
  # This CMakeLists.txt file is being processed as the TriBITS projects's base
  # CMakeLists.txt file!  (See comments at bottom of this file.)
  cmake_minimum_required(VERSION 3.17.0 FATAL_ERROR)
  include("${CMAKE_CURRENT_SOURCE_DIR}/ProjectName.cmake")
  project(${PROJECT_NAME} NONE)
  set(${PROJECT_NAME}_TRIBITS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tribits" CACHE PATH "")
  include("${${PROJECT_NAME}_TRIBITS_DIR}/TriBITS.cmake")
  set(${PROJECT_NAME}_ENABLE_TESTS ON CACHE BOOL "Enable tests by default.")
  include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/AssertSameVersionFile.cmake")
  tribits_project_enable_all()
else()
  # This CMakeLists.txt file is being processed as the TriBITS package file.
  tribits_package(TriBITS  DISABLE_CIRCULAR_REF_DETECTION_FAILURE)
  tribits_add_test_directories(test)
  tribits_package_postprocess()
endif()

# NOTE: In order to allow the `TriBITS/` directory to be both a TriBITS
# package (for inclusion in other TriBITS projects) and to be a TriBITS
# project itself, you only have to put in a simple if statement in this
# top-level CMakeLists.txt file.  That is all!  In fact, this `TriBiTS/`
# directory shows how the same directory can be used for a TriBITS package, a
# TriBITS repository, and a TriBITS project!
