########################################################################
# TribitsExampleProject + TribitsExampleProjectAddons
########################################################################


tribits_add_advanced_test( TribitsExampleProject_TribitsExampleProjectAddons
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Copy TribitsExampleProject so that we can copy in TribitsExampleProjectAddons."
    CMND cp
    ARGS -r ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject .

  TEST_1
    MESSAGE "Copy TribitsExampleProjectAddons to base dir."
    CMND cp
    ARGS -r ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProjectAddons
      TribitsExampleProject/.

  TEST_2
    MESSAGE "Configure with cmake/ExtraRepositoriesList.cmake file enabling all packages"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_Fortran=OFF
      -DTribitsExProj_ENABLE_DEBUG=OFF
      -DTribitsExProj_ENABLE_ALL_PACKAGES=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      -DTribitsExProj_EXTRAREPOS_FILE=cmake/ExtraRepositoriesList.cmake
      -DTribitsExProj_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE=Continuous
      TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "-- Adding POST extra Continuous repository TribitsExampleProjectAddons "
      "Reading list of native packages from .*/TribitsExampleProject/PackagesList.cmake"
      "Reading list of native TPLs from .*/TribitsExampleProject/TPLsList.cmake"
      "Reading list of POST extra packages from .*/TribitsExampleProject/TribitsExampleProjectAddons/PackagesList.cmake"
      "Reading list of POST extra TPLs from .*/TribitsExampleProject/TribitsExampleProjectAddons/TPLsList.cmake"
      "Final set of enabled SE packages:  SimpleCxx .* Addon1"
      "Processing enabled package: SimpleCxx [(]Libs, Tests, Examples[)]"
      "Processing enabled package: Addon1 [(]Libs, Tests, Examples[)]"
      "Configuring done"
      "Generating done"

  TEST_3 CMND make
    ARGS ${CTEST_BUILD_FLAGS}
    PASS_REGULAR_EXPRESSION_ALL
      "Linking CXX executable Addon1_test.exe"
      "Built target Addon1_test"

  TEST_4 CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
    PASS_REGULAR_EXPRESSION_ALL
      "Test .*: Addon1_test .* Passed"
      "100% tests passed, 0 tests failed out of"

  TEST_5 CMND make
    ARGS ${CTEST_BUILD_FLAGS} clean

  TEST_6
    MESSAGE "Configure again enabling all packages using TribitsExProj_EXTRA_REPOSITORIES only"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_EXTRAREPOS_FILE=
      -DTribitsExProj_EXTRA_REPOSITORIES=TribitsExampleProjectAddons
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Processing list of POST extra repos from TribitsExProj_EXTRA_REPOSITORIES='TribitsExampleProjectAddons'"
      "Reading list of native packages from .*/TribitsExampleProject/PackagesList.cmake"
      "Reading list of native TPLs from .*/TribitsExampleProject/TPLsList.cmake"
      "Reading list of POST extra packages from .*/TribitsExampleProject/TribitsExampleProjectAddons/PackagesList.cmake"
      "Reading list of POST extra TPLs from .*/TribitsExampleProject/TribitsExampleProjectAddons/TPLsList.cmake"
      "Final set of enabled SE packages:  SimpleCxx .* Addon1"
      "Processing enabled package: SimpleCxx [(]Libs, Tests, Examples[)]"
      "Processing enabled package: Addon1 [(]Libs, Tests, Examples[)]"
      "Configuring done"
      "Generating done"

  TEST_7 CMND make
    ARGS ${CTEST_BUILD_FLAGS}
    PASS_REGULAR_EXPRESSION_ALL
      "Linking CXX executable Addon1_test.exe"
      "Built target Addon1_test"

  TEST_8 CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
    PASS_REGULAR_EXPRESSION_ALL
      "Test .*: Addon1_test .* Passed"
      "100% tests passed, 0 tests failed out of"

  )
