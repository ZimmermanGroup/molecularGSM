include("${CMAKE_CURRENT_LIST_DIR}/../TribitsProjCTestDriver.cmake")

#
# Set the options specific to this build case
#

set(COMM_TYPE SERIAL)
set(BUILD_TYPE DEBUG)
set(BUILD_DIR_NAME GCC_${COMM_TYPE}_${BUILD_TYPE})
set(CTEST_TEST_TIMEOUT 60)

set_default_and_from_env( CTEST_BUILD_FLAGS "-j8 -k" )

set_default_and_from_env( CTEST_PARALLEL_LEVEL "8" )

set( EXTRA_CONFIGURE_OPTIONS
  "-DBUILD_SHARED_LIBS:BOOL=ON"
  "-DCMAKE_BUILD_TYPE=DEBUG"
  "-DCMAKE_C_COMPILER=gcc"
  "-DCMAKE_CXX_COMPILER=g++"
  "-DCMAKE_Fortran_COMPILER=gfortran"
  "-DTriBITS_TRACE_ADD_TEST=ON"
  )

set(CTEST_TEST_TYPE Nightly)

#
# Run the CTest driver and submit to CDash
#

tribits_proj_ctest_driver()
