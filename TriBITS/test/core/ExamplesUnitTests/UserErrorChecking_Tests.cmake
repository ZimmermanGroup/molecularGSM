########################################################################
# User error checking tests
########################################################################


tribits_add_advanced_test( TribitsExampleProject_PkgWithUserErrors_PASS
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Copy TribitsExampleProject so that we can copy in PkgWithUserErrors."
    CMND cp
    ARGS -r ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject .

  TEST_1
    MESSAGE "Copy PkgWithUserErrors to base dir."
    CMND cp
    ARGS -r ${CMAKE_CURRENT_SOURCE_DIR}/PkgWithUserErrors
      TribitsExampleProject/.

  TEST_2
    MESSAGE "Configure PkgWithUserErrors"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_Fortran=OFF
      -DTribitsExProj_EXTRA_REPOSITORIES=PkgWithUserErrors
      -DTribitsExProj_ENABLE_PkgWithUserErrors=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "Explicitly enabled packages on input [(]by user[)]:  PkgWithUserErrors 1"
      "Final set of enabled packages:  PkgWithUserErrors 1"
      "Processing enabled package: PkgWithUserErrors [(]Libs, Tests, Examples[)]"
      "Configuring done"
      "Generating done"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_3 CMND make
    ARGS ${CTEST_BUILD_FLAGS}
    PASS_REGULAR_EXPRESSION_ALL
      "Built target PkgWithUserErrors_PkgWithUserErrorsTest"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_4 CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
    PASS_REGULAR_EXPRESSION_ALL
      "Test [#]1: PkgWithUserErrors_PkgWithUserErrorsTest[_MPI1]* [.]+ +Passed"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  )


tribits_add_advanced_test( TribitsExampleProject_PkgWithUserErrors_PACKAGE_POST_PROCESS
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Copy TribitsExampleProject so that we can copy in PkgWithUserErrors."
    CMND cp
    ARGS -r ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject .

  TEST_1
    MESSAGE "Copy PkgWithUserErrors to base dir."
    CMND cp
    ARGS -r ${CMAKE_CURRENT_SOURCE_DIR}/PkgWithUserErrors
      TribitsExampleProject/.

  TEST_2
    MESSAGE "Configure PkgWithUserErrors"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_Fortran=OFF
      -DTribitsExProj_EXTRA_REPOSITORIES=PkgWithUserErrors
      -DTribitsExProj_ENABLE_PkgWithUserErrors=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      -DPkgWithUserErrors_turn_off_passing_call_order=TRUE
      -DPkgWithUserErrors_no_POSTPROCESS_call=TRUE
      TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "Processing enabled package: PkgWithUserErrors [(]Libs, Tests, Examples[)]"
      "ERROR: Forgot to call tribits_package_postprocess[(][)] .*/TribitsExampleProject/PkgWithUserErrors/CMakeLists.txt"
    ALWAYS_FAIL_ON_ZERO_RETURN
  )

tribits_add_advanced_test( TribitsExampleProject_PkgWithUserErrors_PACKAGE_INIT
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Copy TribitsExampleProject so that we can copy in PkgWithUserErrors."
    CMND cp
    ARGS -r ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject .

  TEST_1
    MESSAGE "Copy PkgWithUserErrors to base dir."
    CMND cp
    ARGS -r ${CMAKE_CURRENT_SOURCE_DIR}/PkgWithUserErrors
      TribitsExampleProject/.

  TEST_2
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_Fortran=OFF
      -DTribitsExProj_EXTRA_REPOSITORIES=PkgWithUserErrors
      -DTribitsExProj_ENABLE_PkgWithUserErrors=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      -DPkgWithUserErrors_turn_off_passing_call_order=TRUE
      -DPkgWithUserErrors_ADD_LIBRARY_with_no_package_init=TRUE
      TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "Must call tribits_package[(][)] or tribits_package_decl[(][)] before\n  tribits_add_library[(][)] in"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_3
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_Fortran=OFF
      -DTribitsExProj_EXTRA_REPOSITORIES=PkgWithUserErrors
      -DTribitsExProj_ENABLE_PkgWithUserErrors=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      -DPkgWithUserErrors_turn_off_passing_call_order=TRUE
      -DPkgWithUserErrors_ADD_LIBRARY_with_no_package_init=FALSE
      -DPkgWithUserErrors_ADD_EXECUTABLE_with_no_package_init=TRUE
      TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "Must call tribits_package[(][)] or tribits_package_decl[(][)] before\n  tribits_add_executable[(][)] in"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_4
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_Fortran=OFF
      -DTribitsExProj_EXTRA_REPOSITORIES=PkgWithUserErrors
      -DTribitsExProj_ENABLE_PkgWithUserErrors=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      -DPkgWithUserErrors_turn_off_passing_call_order=TRUE
      -DPkgWithUserErrors_ADD_LIBRARY_with_no_package_init=FALSE
      -DPkgWithUserErrors_ADD_EXECUTABLE_with_no_package_init=FALSE
      -DPkgWithUserErrors_POSTPROCESS_with_no_package_init=TRUE
      TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "Must call tribits_package[(][)] or tribits_package_def[(][)]"
      "tribits_package_postprocess[(][)]"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_5
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_Fortran=OFF
      -DTribitsExProj_EXTRA_REPOSITORIES=PkgWithUserErrors
      -DTribitsExProj_ENABLE_PkgWithUserErrors=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      -DPkgWithUserErrors_turn_off_passing_call_order=TRUE
      -DPkgWithUserErrors_ADD_LIBRARY_with_no_package_init=FALSE
      -DPkgWithUserErrors_ADD_EXECUTABLE_with_no_package_init=FALSE
      -DPkgWithUserErrors_POSTPROCESS_with_no_package_init=FALSE
      -DPkgWithUserErrors_multiple_calls_to_PACKAGE=TRUE
      TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "Package .* declared more than once!"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_6
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_Fortran=OFF
      -DTribitsExProj_EXTRA_REPOSITORIES=PkgWithUserErrors
      -DTribitsExProj_ENABLE_PkgWithUserErrors=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      -DPkgWithUserErrors_multiple_calls_to_PACKAGE=FALSE
      -DPkgWithUserErrors_using_package_with_subpackage_commands=TRUE
      TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "The TriBITS Package 'PkgWithUserErrors' does not have any subpackages[.]"
      "Therefore, you are not allowed to call tribits_process_subpackages[(][)]!"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  )

tribits_add_advanced_test( TribitsExampleProject_PkgWithUserErrors_AFTER_POSTPROCESS
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Copy TribitsExampleProject so that we can copy in PkgWithUserErrors."
    CMND cp
    ARGS -r ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject .

  TEST_1
    MESSAGE "Copy PkgWithUserErrors to base dir."
    CMND cp
    ARGS -r ${CMAKE_CURRENT_SOURCE_DIR}/PkgWithUserErrors
      TribitsExampleProject/.

  TEST_2
    MESSAGE "Configure PkgWithUserErrors"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_Fortran=OFF
      -DTribitsExProj_EXTRA_REPOSITORIES=PkgWithUserErrors
      -DTribitsExProj_ENABLE_PkgWithUserErrors=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      -DPkgWithUserErrors_turn_off_passing_call_order=TRUE
      -DPkgWithUserErrors_ADD_LIBRARY_after_POSTPROCESS=TRUE
      TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "Must call tribits_add_library[(][)] before tribits_package_postprocess[(][)] in"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_3
    MESSAGE "Configure PkgWithUserErrors"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_Fortran=OFF
      -DTribitsExProj_EXTRA_REPOSITORIES=PkgWithUserErrors
      -DTribitsExProj_ENABLE_PkgWithUserErrors=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      -DPkgWithUserErrors_turn_off_passing_call_order=TRUE
      -DPkgWithUserErrors_ADD_LIBRARY_after_POSTPROCESS=FALSE
      -DPkgWithUserErrors_ADD_EXECUTABLE_after_POSTPROCESS=TRUE
      TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "Must call tribits_add_executable[(][)] before tribits_package_postprocess[(][)] in"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_4
    MESSAGE "Configure PkgWithUserErrors"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_Fortran=OFF
      -DTribitsExProj_EXTRA_REPOSITORIES=PkgWithUserErrors
      -DTribitsExProj_ENABLE_PkgWithUserErrors=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      -DPkgWithUserErrors_turn_off_passing_call_order=TRUE
      -DPkgWithUserErrors_ADD_LIBRARY_after_POSTPROCESS=FALSE
      -DPkgWithUserErrors_ADD_EXECUTABLE_after_POSTPROCESS=FALSE
      -DPkgWithUserErrors_multiple_calls_to_POSTPROCESS=TRUE
      TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "tribits_package_postprocess[(][)] has been called more than once in"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  )

tribits_add_advanced_test( TribitsExampleProject_PkgWithUserErrors_UNPARSED_ARGUMENTS
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Copy TribitsExampleProject so that we can copy in PkgWithUserErrors."
    CMND cp
    ARGS -r ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject .

  TEST_1
    MESSAGE "Copy PkgWithUserErrors to base dir."
    CMND cp
    ARGS -r ${CMAKE_CURRENT_SOURCE_DIR}/PkgWithUserErrors
      TribitsExampleProject/.

  TEST_2
    MESSAGE "Configure PkgWithUserErrors"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_CHECK_FOR_UNPARSED_ARGUMENTS=FATAL_ERROR
      -DTribitsExProj_ENABLE_Fortran=OFF
      -DTribitsExProj_EXTRA_REPOSITORIES=PkgWithUserErrors
      -DTribitsExProj_ENABLE_PkgWithUserErrors=ON
      -DPkgWithUserErrors_UNPARSED_ARGUMENTS_DEFINE_DEPENDENCIES=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "CMake Error at.*TribitsParseArgumentsHelpers.cmake:"
      "Arguments passed in unrecognized.  PARSE_UNPARSED_ARGUMENTS ="
      "nonsense_jdslkfhlskd"
      "tribits_read_toplevel_package_deps_files_add_to_graph"
      "-- Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_3
    MESSAGE "Configure PkgWithUserErrors"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_Fortran=OFF
      -DTribitsExProj_EXTRA_REPOSITORIES=PkgWithUserErrors
      -DTribitsExProj_ENABLE_PkgWithUserErrors=ON
      -DPkgWithUserErrors_UNPARSED_ARGUMENTS_DEFINE_DEPENDENCIES=OFF
      -DPkgWithUserErrors_turn_off_passing_call_order=TRUE
      -DPkgWithUserErrors_UNPARSED_ARGUMENTS_ADD_LIBRARY=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "CMake Error at.*TribitsParseArgumentsHelpers.cmake:"
      "Arguments passed in unrecognized.  PARSE_UNPARSED_ARGUMENTS ="
      "this_shouldnt_be_here"
      "PkgWithUserErrors/CMakeLists.txt.*tribits_add_library"
      "-- Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_4
    MESSAGE "Configure PkgWithUserErrors"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_Fortran=OFF
      -DTribitsExProj_EXTRA_REPOSITORIES=PkgWithUserErrors
      -DTribitsExProj_ENABLE_PkgWithUserErrors=ON
      -DPkgWithUserErrors_UNPARSED_ARGUMENTS_DEFINE_DEPENDENCIES=OFF
      -DPkgWithUserErrors_turn_off_passing_call_order=TRUE
      -DPkgWithUserErrors_UNPARSED_ARGUMENTS_ADD_LIBRARY=OFF
      -DPkgWithUserErrors_UNPARSED_ARGUMENTS_ADD_EXECUTABLE=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "CMake Error at.*TribitsParseArgumentsHelpers.cmake:"
      "Arguments passed in unrecognized.  PARSE_UNPARSED_ARGUMENTS ="
      "misspelled_argument"
      "PkgWithUserErrors/CMakeLists.txt.*tribits_add_executable"
      "-- Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN
  )


tribits_add_advanced_test( TribitsExampleProject_PkgWithSubpkgsWithUserErrors_PASS
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Copy TribitsExampleProject so that we can copy in PkgWithSubpkgsWithUserErrors."
    CMND cp
    ARGS -r ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject .

  TEST_1
    MESSAGE "Copy PkgWithSubpkgsWithUserErrors to base dir."
    CMND cp
    ARGS -r ${CMAKE_CURRENT_SOURCE_DIR}/PkgWithSubpkgsWithUserErrors
      TribitsExampleProject/.

  TEST_2
    MESSAGE "Configure PkgWithSubpkgsWithUserErrors"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_Fortran=OFF
      -DTribitsExProj_EXTRA_REPOSITORIES=PkgWithSubpkgsWithUserErrors
      -DTribitsExProj_ENABLE_PkgWithSubpkgsWithUserErrors=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "Explicitly enabled packages on input [(]by user[)]:  PkgWithSubpkgsWithUserErrors 1"
      "Final set of enabled packages:  PkgWithSubpkgsWithUserErrors 1"
      "Processing enabled package: PkgWithSubpkgsWithUserErrors [(]A, B, Tests, Examples[)]"
      "Configuring done"
      "Generating done"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_3 CMND make
    ARGS ${CTEST_BUILD_FLAGS}
    PASS_REGULAR_EXPRESSION_ALL
      "libpwswue_a"
      "libpwswue_b"
      "PkgWithSubpkgsWithUserErrorsA_a_test"
      "PkgWithSubpkgsWithUserErrorsB_b_test"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_4 CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
    PASS_REGULAR_EXPRESSION_ALL
      "Test [#]1: PkgWithSubpkgsWithUserErrorsA_test_of_a [.]+ +Passed"
      "Test [#]2: PkgWithSubpkgsWithUserErrorsB_test_of_b [.]+ +Passed"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  )

tribits_add_advanced_test( TribitsExampleProject_PkgWithSubpkgsWithUserErrors_PACKAGE_USER_ERRORS
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Copy TribitsExampleProject so that we can copy in PkgWithSubpkgsWithUserErrors."
    CMND cp
    ARGS -r ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject .

  TEST_1
    MESSAGE "Copy PkgWithSubpkgsWithUserErrors to base dir."
    CMND cp
    ARGS -r ${CMAKE_CURRENT_SOURCE_DIR}/PkgWithSubpkgsWithUserErrors
      TribitsExampleProject/.

  TEST_2
    MESSAGE "Configure PkgWithSubpkgsWithUserErrors"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_Fortran=OFF
      -DTribitsExProj_EXTRA_REPOSITORIES=PkgWithSubpkgsWithUserErrors
      -DTribitsExProj_ENABLE_PkgWithSubpkgsWithUserErrors=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      -DPkgWithSubpkgsWithUserErrors_TURN_OFF_PASSING_CALL_ORDER=TRUE
      -DPkgWithSubpkgsWithUserErrors_no_PACKAGE_DECL_before_PROCESS_SUBPACKAGES=TRUE
      TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "Must call tribits_package_decl[(][)] before tribits_process_subpackages[(][)]"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_3
    CMND ${CMAKE_COMMAND}
    ARGS
      -DPkgWithSubpkgsWithUserErrors_no_PACKAGE_DECL_before_PROCESS_SUBPACKAGES=FALSE
      -DPkgWithSubpkgsWithUserErrors_POSTPROCESS_before_SUBPACKAGES=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      " Must call tribits_package_decl[(][)], tribits_process_subpackages[(][)] and"
      " tribits_package_def[(][)] before tribits_package_postprocess[(][)].  Because this"
      " package has subpackages you cannot use tribits_package[(][)] you must call"
      " these in the following order: tribits_package_decl[(][)]"
      " tribits_process_subpackages[(][)] tribits_package_def[(][)]"
      " tribits_package_postprocess[(][)] in:"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_4
    CMND ${CMAKE_COMMAND}
    ARGS
      -DPkgWithSubpkgsWithUserErrors_POSTPROCESS_before_SUBPACKAGES=FALSE
      -DPkgWithSubpkgsWithUserErrors_PACKAGE_DEF_before_SUBPACKAGES=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Must call tribits_package_def[(][)] after tribits_process_subpackages[(][)]"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_5
    CMND ${CMAKE_COMMAND}
    ARGS
      -DPkgWithSubpkgsWithUserErrors_PACKAGE_DEF_before_SUBPACKAGES=FALSE
      -DPkgWithSubpkgsWithUserErrors_multiple_calls_to_PACKAGE_DECL=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "tribits_package_decl[(][)] called more than once"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_6
    CMND ${CMAKE_COMMAND}
    ARGS
      -DPkgWithSubpkgsWithUserErrors_multiple_calls_to_PACKAGE_DECL=FALSE
      -DPkgWithSubpkgsWithUserErrors_multiple_calls_to_PACKAGE_DEF=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "tribits_package_def[(][)] was called more than once"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_7
    CMND ${CMAKE_COMMAND}
    ARGS
      -DPkgWithSubpkgsWithUserErrors_multiple_calls_to_PACKAGE_DEF=FALSE
      -DPkgWithSubpkgsWithUserErrors_call_PACKAGE_from_package_with_subpackages=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "This package has subpackages so you cannot use tribits_package[(][)]"
      "Instead use the following call order:"
      "tribits_project_decl[(]PkgWithSubpkgsWithUserErrors[)]"
      "tribits_process_subpackages[(][)]"
      "tribits_package_def[(][)]"
      "tribits_package_postprocess[(][)]"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_8
    CMND ${CMAKE_COMMAND}
    ARGS
      -DPkgWithSubpkgsWithUserErrors_call_PACKAGE_from_package_with_subpackages=FALSE
      -DPkgWithSubpkgsWithUserErrors_call_PACKAGE_after_PROCESS_SUBPACKAGES=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "This package has subpackages so you cannot use tribits_package[(][)]"
      "Instead use the following call order"
      "tribits_project_decl[(]PkgWithSubpkgsWithUserErrors[)]"
      "tribits_process_subpackages[(][)]"
      "tribits_package_def[(][)]"
      "tribits_package_postprocess[(][)]"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_9
    CMND ${CMAKE_COMMAND}
    ARGS
      -DPkgWithSubpkgsWithUserErrors_call_PACKAGE_after_PROCESS_SUBPACKAGES=FALSE
      -DPkgWithSubpkgsWithUserErrors_call_everthing_except_PROCESS_SUBPACKAGES=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      " Must call tribits_package_decl[(][)], tribits_process_subpackages[(][)] and"
      " tribits_package_def[(][)] before tribits_package_postprocess[(][)].  Because this"
      " package has subpackages you cannot use tribits_package[(][)] you must call"
      " these in the following order: tribits_package_decl[(][)]"
      " tribits_process_subpackages[(][)] tribits_package_def[(][)]"
      " tribits_package_postprocess[(][)] in:"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_10
    CMND ${CMAKE_COMMAND}
    ARGS
      -DPkgWithSubpkgsWithUserErrors_call_everthing_except_PROCESS_SUBPACKAGES=FALSE
      -DPkgWithSubpkgsWithUserErrors_call_PACKAGE_DECL_only=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      " Must call tribits_package_decl[(][)], tribits_process_subpackages[(][)] and"
      " tribits_package_def[(][)] before tribits_package_postprocess[(][)].  Because this"
      " package has subpackages you cannot use tribits_package[(][)] you must call"
      " these in the following order: tribits_package_decl[(][)]"
      " tribits_process_subpackages[(][)] tribits_package_def[(][)]"
      " tribits_package_postprocess[(][)] in:"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_11
    CMND ${CMAKE_COMMAND}
    ARGS
      -DPkgWithSubpkgsWithUserErrors_call_PACKAGE_DECL_only=FALSE
      -DPkgWithSubpkgsWithUserErrors_call_SUBPACKAGE_from_package_with_subpackages=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Cannot call tribits_subpackage[(][)] from a package.  Use tribits_package[(][)]"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_12
    CMND ${CMAKE_COMMAND}
    ARGS
      -DPkgWithSubpkgsWithUserErrors_call_SUBPACKAGE_from_package_with_subpackages=FALSE
      -DPkgWithSubpkgsWithUserErrors_call_SUBPACKAGE_POSTPROCESS=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Cannot call tribits_subpackage_postprocess[(][)] from a package.  Use"
      " tribits_package_postprocess[(][)] instead"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  )


tribits_add_advanced_test( TribitsExampleProject_PkgWithSubpkgsWithUserErrors_SUBPACKAGE_USER_ERRORS
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Copy TribitsExampleProject so that we can copy in PkgWithSubpkgsWithUserErrors."
    CMND cp
    ARGS -r ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject .

  TEST_1
    MESSAGE "Copy PkgWithSubpkgsWithUserErrors to base dir."
    CMND cp
    ARGS -r ${CMAKE_CURRENT_SOURCE_DIR}/PkgWithSubpkgsWithUserErrors
      TribitsExampleProject/.

  TEST_2
    MESSAGE "Configure PkgWithSubpkgsWithUserErrors"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DTribitsExProj_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      -DTribitsExProj_ENABLE_Fortran=OFF
      -DTribitsExProj_EXTRA_REPOSITORIES=PkgWithSubpkgsWithUserErrors
      -DTribitsExProj_ENABLE_PkgWithSubpkgsWithUserErrors=ON
      -DTribitsExProj_ENABLE_TESTS=ON
      -DsubpackageA_turn_off_passing_call_order=TRUE
      -DsubPackageA_call_PACKAGE=TRUE
      TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "Cannot call tribits_package[(][)] in a subpackage"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_3
    CMND ${CMAKE_COMMAND}
    ARGS
      -DsubPackageA_call_PACKAGE=FALSE
      -DsubPackageA_call_PACKAGE_DECL=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Cannot call tribits_package_decl[(][)] in a subpackage"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_4
    CMND ${CMAKE_COMMAND}
    ARGS
      -DsubPackageA_call_PACKAGE_DECL=FALSE
      -DsubPackageA_call_PACKAGE_DEF=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Cannot call tribits_package_def[(][)] in a subpackage"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_5
    CMND ${CMAKE_COMMAND}
    ARGS
      -DsubPackageA_call_PACKAGE_DEF=FALSE
      -DsubPackageA_call_PROCESS_SUBPACKAGES=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Cannot call tribits_process_subpackages[(][)] in a subpackage"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_6
    CMND ${CMAKE_COMMAND}
    ARGS
      -DsubPackageA_call_PACKAGE_POSTPROCESS=TRUE
      -DsubPackageA_call_PROCESS_SUBPACKAGES=FALSE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Cannot call tribits_package_postprocess[(][)] in a subpackage"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_7
    CMND ${CMAKE_COMMAND}
    ARGS
      -DsubPackageA_call_PACKAGE_POSTPROCESS=FALSE
      -DsubPackageA_DOUBLE_SUBPACKAGE_INIT=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Already called tribits_subpackge[(][)] for the PkgWithSubpkgsWithUserErrors"
      "subpackage A"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_8
    CMND ${CMAKE_COMMAND}
    ARGS
      -DsubPackageA_DOUBLE_SUBPACKAGE_INIT=FALSE
      -DsubPAckageA_DOUBLE_SUBPACKAGE_POSTPROCESS=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Already called tribits_subpackge_postprocess[(][)] for the"
      "PkgWithSubpkgsWithUserErrors subpackage A"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_9
    CMND ${CMAKE_COMMAND}
    ARGS
      -DsubPackageA_no_SUBPACKAGE_before_POSTPROCESS=TRUE
      -DsubPAckageA_DOUBLE_SUBPACKAGE_POSTPROCESS=FALSE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "tribits_subpackage[(][)] must be called before tribits_subpackage_postprocess[(][)]"
      " for the PkgWithSubpkgsWithUserErrors"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_10
    MESSAGE "Configure PkgWithSubpkgsWithUserErrors library after postprocess"
    CMND ${CMAKE_COMMAND}
    ARGS
      -DsubPackageA_no_SUBPACKAGE_before_POSTPROCESS=FALSE
      -DsubPackageA_ADD_LIBRARY_AFTER_POST_PROCESS=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Adding library in subpackage after post process"
      "Must call tribits_add_library[(][)] before tribits_subpackage_postprocess[(][)]"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_11
    MESSAGE "Configure PkgWithSubpkgsWithUserErrors exec after postprocess"
    CMND ${CMAKE_COMMAND}
    ARGS
      -DsubPackageA_ADD_LIBRARY_AFTER_POST_PROCESS=FALSE
      -DsubPackageA_ADD_EXECUTABLE_AFTER_POST_PROCESS=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Adding executable in subpackage after post process"
      "Must call tribits_add_executable[(][)] before tribits_subpackage_postprocess[(][)]"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_12
    MESSAGE "Configure PkgWithSubpkgsWithUserErrors lib with no preprocess"
    CMND ${CMAKE_COMMAND}
    ARGS
      -DsubPackageA_ADD_EXECUTABLE_AFTER_POST_PROCESS=FALSE
      -DsubPackageA_add_lib_no_PREPROCESS=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Must call tribits_subpackage[(][)] before tribits_add_library[(][)]"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_13
    MESSAGE "Configure PkgWithSubpkgsWithUserErrors exec with no preprocess"
    CMND ${CMAKE_COMMAND}
    ARGS
      -DsubPackageA_add_lib_no_PREPROCESS=FALSE
      -DsubPackageA_add_exe_no_PREPROCESS=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Must call tribits_subpackage[(][)] before tribits_add_executable[(][)]"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_14
    MESSAGE "Configure PkgWithSubpkgsWithUserErrors exec with no preprocess"
    CMND ${CMAKE_COMMAND}
    ARGS
      -DsubPackageA_add_exe_no_PREPROCESS=FALSE
      -DsubPackageA_call_ADD_TEST_DIRECTORY_without_SUBPACKAGE=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Must call tribits_subpackage[(][)] before tribits_add_test_directories[(][)]"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_15
    MESSAGE "Configure PkgWithSubpkgsWithUserErrors exec with no preprocess"
    CMND ${CMAKE_COMMAND}
    ARGS
      -DsubPackageA_call_ADD_TEST_DIRECTORY_without_SUBPACKAGE=FALSE
      -DsubPackageA_call_ADD_TEST_DIRECTORY_after_POSTPROCESS=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Must call tribits_add_test_directories[(][)] before"
      "tribits_subpackage_postprocess[(][)]"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_16
    MESSAGE "Configure PkgWithSubpkgsWithUserErrors exec with no preprocess"
    CMND ${CMAKE_COMMAND}
    ARGS
      -DsubPackageA_call_ADD_TEST_DIRECTORY_after_POSTPROCESS=FALSE
      -DsubPackageA_call_ADD_EXAMPLE_DIRECTORY_without_SUBPACKAGE=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Must call tribits_subpackage[(][)] before tribits_add_example_directories[(][)]"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_17
    MESSAGE "Configure PkgWithSubpkgsWithUserErrors exec with no preprocess"
    CMND ${CMAKE_COMMAND}
    ARGS
      -DsubPackageA_call_ADD_EXAMPLE_DIRECTORY_without_SUBPACKAGE=FALSE
      -DsubPackageA_call_ADD_EXAMPLE_DIRECTORY_after_POSTPROCESS=TRUE
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Must call tribits_add_example_directories[(][)] before"
      "tribits_subpackage_postprocess[(][)]"
      "Configuring incomplete, errors occurred!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  )


tribits_add_advanced_test( TribitsExampleProject_config_file_compiler_overrides
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0 CMND ${CMAKE_COMMAND}
    MESSAGE "Configure replacing the compilers listed in the generated Config.cmake files"
    ARGS
      ${TribitsExampleProject_COMMON_CONFIG_ARGS}
      -DCMAKE_CXX_COMPILER_FOR_CONFIG_FILE_BUILD_DIR=cxx_wrapper_build_dir
      -DCMAKE_C_COMPILER_FOR_CONFIG_FILE_BUILD_DIR=c_wrapper_build_dir
      -DCMAKE_Fortran_COMPILER_FOR_CONFIG_FILE_BUILD_DIR=fortran_wrapper_build_dir
      -DCMAKE_CXX_COMPILER_FOR_CONFIG_FILE_INSTALL_DIR=cxx_wrapper_install_dir
      -DCMAKE_C_COMPILER_FOR_CONFIG_FILE_INSTALL_DIR=c_wrapper_install_dir
      -DCMAKE_Fortran_COMPILER_FOR_CONFIG_FILE_INSTALL_DIR=fortran_wrapper_install_dir
      -DTribitsExProj_ENABLE_WithSubpackages=ON
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsExampleProject
    PASS_REGULAR_EXPRESSION_ALL
      "Configuring done"
      "Generating done"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_1
    MESSAGE "Check complers set in TribitsExProjConfig_install.cmake"
    CMND grep
    ARGS _COMPILER TribitsExProjConfig_install.cmake
    PASS_REGULAR_EXPRESSION_ALL
      "set[(]TribitsExProj_CXX_COMPILER .cxx_wrapper_install_dir.[)]"
      "set[(]TribitsExProj_C_COMPILER .c_wrapper_install_dir.[)]"
      "set[(]TribitsExProj_Fortran_COMPILER .fortran_wrapper_install_dir.[)]"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_2
    MESSAGE "Check complers set in WithSubpackagesConfig_install.cmake"
    CMND grep
    ARGS _COMPILER packages/with_subpackages/CMakeFiles/WithSubpackagesConfig_install.cmake
    PASS_REGULAR_EXPRESSION_ALL
      "set[(]WithSubpackages_CXX_COMPILER .cxx_wrapper_install_dir.[)]"
      "set[(]WithSubpackages_C_COMPILER .c_wrapper_install_dir.[)]"
      "set[(]WithSubpackages_Fortran_COMPILER .fortran_wrapper_install_dir.[)]"
      "set[(]WithSubpackages_FORTRAN_COMPILER .fortran_wrapper_install_dir.[)]"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_3
    MESSAGE "Check complers set in WithSubpackagesConfig.cmake for build dir"
    CMND grep
    ARGS _COMPILER cmake_packages/WithSubpackages/WithSubpackagesConfig.cmake
    PASS_REGULAR_EXPRESSION_ALL
      "set[(]WithSubpackages_CXX_COMPILER .cxx_wrapper_build_dir.[)]"
      "set[(]WithSubpackages_C_COMPILER .c_wrapper_build_dir.[)]"
      "set[(]WithSubpackages_Fortran_COMPILER .fortran_wrapper_build_dir.[)]"
      "set[(]WithSubpackages_FORTRAN_COMPILER .fortran_wrapper_build_dir.[)]"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  )
