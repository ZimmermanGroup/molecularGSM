include("${CMAKE_CURRENT_LIST_DIR}/../TribitsProjCTestDriver.cmake")

set(CTEST_BUILD_NAME $ENV{CTEST_BUILD_NAME})

set(CTEST_TEST_TIMEOUT 60)
set(CTEST_DO_UPDATES TRUE)
set(CTEST_UPDATE_VERSION_ONLY TRUE)
set_default_and_from_env(CTEST_BUILD_FLAGS "-j2")
set_default_and_from_env(CTEST_PARALLEL_LEVEL "2")

if (CTEST_BUILD_NAME MATCHES ".*_nofortran")
  set(fortranCompilerStr "")
  set(enableFortranStr "-DTriBITS_ENABLE_Fortran=OFF")
else()
  set(fortranCompilerStr "-DCMAKE_Fortran_COMPILER=gfortran")
  set(enableFortranStr "-DTriBITS_ENABLE_Fortran=ON")
endif()

set(buildTweaksFile "$ENV{TRIBITS_BUILD_TWEAKS_FILE}")
if (EXISTS "${buildTweaksFile}")
  set(configureOptionsFilesStr
    "-DTriBITS_CONFIGURE_OPTIONS_FILE=${buildTweaksFile}")
else()
  set(configureOptionsFilesStr "")
endif()

set( EXTRA_CONFIGURE_OPTIONS
  "${configureOptionsFilesStr}"
  "-DBUILD_SHARED_LIBS:BOOL=ON"
  "-DCMAKE_BUILD_TYPE=DEBUG"
  "-DCMAKE_C_COMPILER=gcc"
  "-DCMAKE_CXX_COMPILER=g++"
  "${fortranCompilerStr}"
  "${enableFortranStr}"
  "-DTriBITS_CTEST_DRIVER_COVERAGE_TESTS=TRUE"
  "-DTriBITS_CTEST_DRIVER_MEMORY_TESTS=TRUE"
  "-DTriBITS_ENABLE_DOC_GENERATION_TESTS=ON"
  "-DTriBITS_ENABLE_REAL_GIT_CLONE_TESTS=TRUE"
  "-DTriBITS_TRACE_ADD_TEST=ON"
  "-DTriBITS_SHOW_TEST_START_END_DATE_TIME=ON"
  )

if ("$ENV{CTEST_TEST_TYPE}" STREQUAL "")
  set(CTEST_TEST_TYPE Experimental)
endif()

#
# Run the CTest driver and submit to CDash
#

tribits_proj_ctest_driver()
