include("${CMAKE_CURRENT_LIST_DIR}/../TribitsProjCTestDriver.cmake")

#
# Set the options specific to this build case
#

set(COMM_TYPE SERIAL)
set(BUILD_TYPE DEBUG)
set(BUILD_DIR_NAME ${COMM_TYPE}_${BUILD_TYPE}_TravisCI)
set(CTEST_SITE TravisCI)
set(CTEST_TEST_TIMEOUT 60)
set(CTEST_DO_UPDATES OFF)

set_default_and_from_env( CTEST_BUILD_FLAGS "-j1 -i" )

set_default_and_from_env( CTEST_PARALLEL_LEVEL "1" )

set( EXTRA_CONFIGURE_OPTIONS
  "-DBUILD_SHARED_LIBS:BOOL=ON"
  "-DCMAKE_BUILD_TYPE=DEBUG"
  "-DCMAKE_C_COMPILER=gcc"
  "-DCMAKE_CXX_COMPILER=g++"
  "-DCMAKE_Fortran_COMPILER=gfortran"
  "-DTriBITS_ENABLE_Fortran=ON"
  "-DTriBITS_CTEST_DRIVER_COVERAGE_TESTS=TRUE"
  "-DTriBITS_CTEST_DRIVER_MEMORY_TESTS=TRUE"
  "-DTriBITS_ENABLE_REAL_GIT_CLONE_TESTS=TRUE"
  "-DTriBITS_TRACE_ADD_TEST=ON"
  "-DTriBITS_SHOW_TEST_START_END_DATE_TIME=ON"
  "-DTriBITS_HOSTNAME=${CTEST_SITE}"
  )

set(CTEST_TEST_TYPE Continuous)

#
# Run the CTest driver and submit to CDash
#

tribits_proj_ctest_driver()
