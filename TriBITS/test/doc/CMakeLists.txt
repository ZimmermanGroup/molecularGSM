advanced_set( ${PROJECT_NAME}_ENABLE_DOC_GENERATION_TESTS OFF
  CACHE BOOL
  "Enable testing for the build of the documentation.  Requires docutils and will blow away and rebuild the existing documentation in the source tree since the documentation builds in place." )


tribits_add_advanced_test( sphinx_rst_generator_UnitTests
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  EXCLUDE_IF_NOT_TRUE HAS_PYTHON3
  TEST_0 CMND ${PYTHON_EXECUTABLE}
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/sphinx_rst_generator_UnitTests.py -v
    PASS_REGULAR_EXPRESSION "OK"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  )


tribits_add_advanced_test( build_docs
  EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_DOC_GENERATION_TESTS
    GIT_EXECUTABLE
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 MESSAGE "Clean out any prior build of the docs"
    CMND ${GIT_EXECUTABLE} ARGS clean -xdf
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tribits/doc
    SKIP_CLEAN_WORKING_DIRECTORY  # Don't delete our source tree!
  TEST_1 MESSAGE "Build the documentation"
    CMND bash ARGS ${PROJECT_SOURCE_DIR}/tribits/doc/build_docs.sh
  TEST_2 CMND ls
    ARGS ${PROJECT_SOURCE_DIR}/tribits/doc/build_ref/TribitsBuildReference.html
    PASS_REGULAR_EXPRESSION build_ref/TribitsBuildReference.html
    ALWAYS_FAIL_ON_NONZERO_RETURN
  TEST_3 CMND ls
    ARGS ${PROJECT_SOURCE_DIR}/tribits/doc/guides/users_guide/TribitsUsersGuide.html
    PASS_REGULAR_EXPRESSION guides/users_guide/TribitsUsersGuide.html
    ALWAYS_FAIL_ON_NONZERO_RETURN
  TEST_4 CMND ls
    ARGS ${PROJECT_SOURCE_DIR}/tribits/doc/guides/maintainers_guide/TribitsMaintainersGuide.html
    PASS_REGULAR_EXPRESSION guides/maintainers_guide/TribitsMaintainersGuide.html
    ALWAYS_FAIL_ON_NONZERO_RETURN
  ADDED_TEST_NAME_OUT build_docs_test_name
  )


tribits_add_advanced_test( build_sphinx_docs
  EXCLUDE_IF_NOT_TRUE ${PROJECT_NAME}_ENABLE_DOC_GENERATION_TESTS
    GIT_EXECUTABLE
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 MESSAGE "Clean out any prior build of the docs"
    CMND ${GIT_EXECUTABLE} ARGS clean -xdf
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tribits/doc
    SKIP_CLEAN_WORKING_DIRECTORY  # Don't delete our source tree!
  TEST_1 MESSAGE "Build the sphinx documentation"
    CMND bash ARGS ${PROJECT_SOURCE_DIR}/tribits/doc/sphinx/build_sphinx_docs.sh
  TEST_2 CMND ls
    ARGS ${PROJECT_SOURCE_DIR}/tribits/doc/sphinx/combined_docs/index.html
    PASS_REGULAR_EXPRESSION sphinx/combined_docs/index.html
    ALWAYS_FAIL_ON_NONZERO_RETURN
  TEST_3 CMND ls
    ARGS ${PROJECT_SOURCE_DIR}/tribits/doc/sphinx/combined_docs/build_ref/index.html
    PASS_REGULAR_EXPRESSION sphinx/combined_docs/build_ref/index.html
    ALWAYS_FAIL_ON_NONZERO_RETURN
  TEST_4 CMND ls
    ARGS ${PROJECT_SOURCE_DIR}/tribits/doc/sphinx/combined_docs/users_guide/index.html
    PASS_REGULAR_EXPRESSION sphinx/combined_docs/users_guide/index.html
    ALWAYS_FAIL_ON_NONZERO_RETURN
  TEST_5 CMND ls
    ARGS ${PROJECT_SOURCE_DIR}/tribits/doc/sphinx/combined_docs/maintainers_guide/index.html
    PASS_REGULAR_EXPRESSION sphinx/combined_docs/maintainers_guide/index.html
    ALWAYS_FAIL_ON_NONZERO_RETURN
  TEST_6 MESSAGE "Rebuild the sphinx documentation to show you can without error"
    CMND bash ARGS ${PROJECT_SOURCE_DIR}/tribits/doc/sphinx/build_sphinx_docs.sh
  ADDED_TEST_NAME_OUT build_sphinx_docs_test_name
  )

if (build_docs_test_name AND build_sphinx_docs_test_name)
  set_tests_properties( ${build_sphinx_docs_test_name}
    PROPERTIES DEPENDS ${build_docs_test_name})
  # Above sets a dependency of testing the build the sphinx documentation to
  # come after testing the build of the regular docutils documentation.  The
  # latter does not really depend on the fommer but both of these clean out
  # the uncommitted files under tribits/doc/ and we don't want these two tests
  # stomping on each other.  We also don't want to label these two tests with
  # RUN_SERIAL because they take a long time to run and we don't want to stop
  # other TriBITS tests from running at the same time as these two.
endif()


if (PYTHON_VERSION_MAJOR GREATER_EQUAL 3)
  set(HAS_PYTHON3 TRUE)
else()
  set(HAS_PYTHON3 FALSE)
endif()
