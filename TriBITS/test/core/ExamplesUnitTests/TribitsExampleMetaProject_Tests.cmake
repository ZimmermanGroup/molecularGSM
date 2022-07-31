########################################################################
# TribitsExampleMetaProject
########################################################################


tribits_add_advanced_test( TribitsExampleMetaProject_Empty
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Configure TribitsExampleMetaProject with nothing in it"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${COMMON_ENV_ARGS_PASSTHROUGH}
      -DTribitsExMetaProj_ENABLE_Fortran=OFF
      -DTribitsExMetaProj_IGNORE_MISSING_EXTRA_REPOSITORIES=TRUE
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleMetaProject
    PASS_REGULAR_EXPRESSION_ALL
      "NOTE: Ignoring missing extra repo 'TribitsExampleProject' as requested since .*/TribitsExampleMetaProject/TribitsExampleProject does not exist"
      "NOTE: Ignoring missing extra repo 'TribitsExampleProjectAddons' as requested since .*/TribitsExampleMetaProject/TribitsExampleProjectAddons does not exist"
      "Final set of enabled SE packages:  0"
      "Final set of non-enabled SE packages:  0"
      "WARNING:  There were no packages configured so no libraries or tests/examples will be built"
      "Configuring done"
      "Generating done"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  )
# NOTE: That above test is the only test that triggers an empty list that
# tries to get reversed.  This is a TriBITS project with no packages and no
# TPLs.  While not common, it is a starter sitiation that users will have so
# it should be handled smoothly.


tribits_add_advanced_test( TribitsExampleMetaProject
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Copy TribitsExampleMetaProject"
    CMND cp
    ARGS -r ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleMetaProject .

  TEST_1
    MESSAGE "Copy TribitsExampleProject to base dir"
    CMND cp
    ARGS -r ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject
      TribitsExampleMetaProject/.

  TEST_2
    MESSAGE "Copy TribitsExampleProjectAddons to base dir"
    CMND cp
    ARGS -r ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProjectAddons
      TribitsExampleMetaProject/.

  TEST_3
    MESSAGE "Configure enabling all packages using cmake/ExtraRepositoriesList.cmake"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${COMMON_ENV_ARGS_PASSTHROUGH}
      -DTribitsExMetaProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExMetaProj_ENABLE_Fortran=OFF
      -DTribitsExMetaProj_ENABLE_DEBUG=OFF
      -DTribitsExMetaProj_ENABLE_ALL_PACKAGES=ON
      -DTribitsExMetaProj_ENABLE_TESTS=ON
      TribitsExampleMetaProject
    PASS_REGULAR_EXPRESSION_ALL
      "Reading the list of extra repositories from .*cmake/ExtraRepositoriesList.cmake"
      "-- Adding POST extra Continuous repository TribitsExampleProject "
      "-- Adding POST extra Continuous repository TribitsExampleProject "
      "Reading list of native packages from .*/TribitsExampleMetaProject/PackagesList.cmake"
      "Reading list of native TPLs from .*/TribitsExampleMetaProject/TPLsList.cmake"
      "Reading list of POST extra packages from .*/TribitsExampleMetaProject/TribitsExampleProject/PackagesList.cmake"
      "Reading list of POST extra TPLs from .*/TribitsExampleMetaProject/TribitsExampleProject/TPLsList.cmake"
      "Reading list of POST extra packages from .*/TribitsExampleMetaProject/TribitsExampleProjectAddons/PackagesList.cmake"
      "Reading list of POST extra TPLs from .*/TribitsExampleMetaProject/TribitsExampleProjectAddons/TPLsList.cmake"
      "Final set of enabled SE packages:  SimpleCxx .* Addon1"
      "Processing enabled package: SimpleCxx [(]Libs, Tests, Examples[)]"
      "Processing enabled package: Addon1 [(]Libs, Tests, Examples[)]"
      "Configuring done"
      "Generating done"

  TEST_4 CMND make
    ARGS ${CTEST_BUILD_FLAGS}
    PASS_REGULAR_EXPRESSION_ALL
      "Linking CXX executable Addon1_test.exe"
      "Built target Addon1_test"

  TEST_5 CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
    PASS_REGULAR_EXPRESSION_ALL
      "Test .*: Addon1_test .* Passed"
      "100% tests passed, 0 tests failed out of"

  TEST_6 CMND make
    ARGS ${CTEST_BUILD_FLAGS} clean

  TEST_7
    MESSAGE "Configure again enabling all packages using TribitsExMetaProj_PRE_REPOSITORIES and TribitsExMetaProj_EXTRA_REPOSITORIES only"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${COMMON_ENV_ARGS_PASSTHROUGH}
      -DTribitsExMetaProj_EXTRAREPOS_FILE=
      -DTribitsExMetaProj_PRE_REPOSITORIES=TribitsExampleProject
      -DTribitsExMetaProj_EXTRA_REPOSITORIES=TribitsExampleProjectAddons
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Processing list of PRE extra repos from TribitsExMetaProj_PRE_REPOSITORIES='TribitsExampleProject'"
      "Reading list of PRE extra packages from .*/TribitsExampleMetaProject/TribitsExampleProject/PackagesList.cmake"
      "Reading list of PRE extra TPLs from .*/TribitsExampleMetaProject/TribitsExampleProject/TPLsList.cmake"
      "Reading list of native packages from .*/TribitsExampleMetaProject/PackagesList.cmake"
      "Reading list of native TPLs from .*/TribitsExampleMetaProject/TPLsList.cmake"
      "Reading list of POST extra packages from .*/TribitsExampleMetaProject/TribitsExampleProjectAddons/PackagesList.cmake"
      "Reading list of POST extra TPLs from .*/TribitsExampleMetaProject/TribitsExampleProjectAddons/TPLsList.cmake"
      "Final set of enabled SE packages:  SimpleCxx .* Addon1"
      "Processing enabled package: SimpleCxx [(]Libs, Tests, Examples[)]"
      "Processing enabled package: Addon1 [(]Libs, Tests, Examples[)]"
      "Configuring done"
      "Generating done"

  TEST_8 CMND make
    ARGS ${CTEST_BUILD_FLAGS}
    PASS_REGULAR_EXPRESSION_ALL
      "Linking CXX executable Addon1_test.exe"
      "Built target Addon1_test"

  TEST_9 CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
    PASS_REGULAR_EXPRESSION_ALL
      "Test .*: Addon1_test .* Passed"
      "100% tests passed, 0 tests failed out of"

  )


tribits_add_advanced_test( TribitsExampleMetaProject_version_date_undef
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Copy TribitsExampleMetaProject"
    CMND cp
    ARGS -r ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleMetaProject .

  TEST_1
    MESSAGE "Copy TribitsExampleProject to base dir"
    CMND cp
    ARGS -r ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject
      TribitsExampleMetaProject/.

  TEST_2
    MESSAGE "Copy TribitsExampleProjectAddons to base dir"
    CMND cp
    ARGS -r ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProjectAddons
      TribitsExampleMetaProject/.

  TEST_3
    MESSAGE "Configure enabling all packages and generate version files"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${COMMON_ENV_ARGS_PASSTHROUGH}
      -DTribitsExMetaProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExMetaProj_ENABLE_Fortran=OFF
      -DTribitsExMetaProj_ENABLE_DEBUG=OFF
      -DTribitsExMetaProj_ENABLE_ALL_PACKAGES=ON
      -DTribitsExMetaProj_ENABLE_TESTS=ON
      -DTribitsExMetaProj_GENERATE_VERSION_DATE_FILES=TRUE
      -DTribitsExMetaProj_TRACE_FILE_PROCESSING=ON
      TribitsExampleMetaProject
    PASS_REGULAR_EXPRESSION_ALL
      "-- NOTE: Can't fill in version date files for TribitsExMetaProj since .*/TribitsExampleMetaProject/.git/ does not exist!"
      "-- File Trace: REPOSITORY CONFIGURE  .*/TribitsExMetaProj_version_date.h"
      "-- NOTE: Can't fill in version date files for TribitsExampleProject since .*/TribitsExampleMetaProject/TribitsExampleProject/.git/ does not exist!"
      "-- File Trace: REPOSITORY CONFIGURE  .*/TribitsExampleProject/TribitsExampleProject_version_date.h"
      "-- NOTE: Can't fill in version date files for TribitsExampleProjectAddons since .*/TribitsExampleMetaProject/TribitsExampleProjectAddons/.git/ does not exist!"
      "-- File Trace: REPOSITORY CONFIGURE  .*/TribitsExampleProjectAddons/TribitsExampleProjectAddons_version_date.h"

  TEST_4
    MESSAGE "Check that the TribitsExMetaProjec_version_date.h for undef macro"
    CMND cat
    ARGS TribitsExMetaProj_version_date.h
    PASS_REGULAR_EXPRESSION
      "#undef TRIBITSEXMETAPROJ_VERSION_DATE"

  TEST_5
    MESSAGE "Check TribitsExampleProject_version_date.h for undef macro"
    CMND cat
    ARGS TribitsExampleProject/TribitsExampleProject_version_date.h
    PASS_REGULAR_EXPRESSION
      "#undef TRIBITSEXAMPLEPROJECT_VERSION_DATE"

  TEST_6
    MESSAGE "Check TribitsExampleProjectAddons_version_date.h for undef macro"
    CMND cat
    ARGS TribitsExampleProjectAddons/TribitsExampleProjectAddons_version_date.h
    PASS_REGULAR_EXPRESSION
      "#undef TRIBITSEXAMPLEPROJECTADDONS_VERSION_DATE"

  )
