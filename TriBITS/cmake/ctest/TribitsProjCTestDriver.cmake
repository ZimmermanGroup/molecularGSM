
# Set the locations of things for this project
#

set(TRIBITS_PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}/../..")
set(TriBITS_TRIBITS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../tribits")

#
# Include the TriBITS file to get other modules included
#

include("${CMAKE_CURRENT_LIST_DIR}/../../tribits/ctest_driver/TribitsCTestDriverCore.cmake")

#
# Define a caller for the TriBITS Project
#

macro(tribits_proj_ctest_driver)
  set_default(TriBITS_REPOSITORY_LOCATION_DEFAULT
    "https://github.com/TriBITSPub/TriBITS.git")
  set_default(TriBITS_REPOSITORY_LOCATION_NIGHTLY_DEFAULT 
    "${TriBITS_REPOSITORY_LOCATION_DEFAULT}")
  print_var(TriBITS_REPOSITORY_LOCATION_DEFAULT)
  tribits_ctest_driver()
endmacro()