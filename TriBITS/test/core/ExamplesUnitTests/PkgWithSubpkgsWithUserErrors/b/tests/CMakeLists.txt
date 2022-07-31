tribits_add_executable( b_test SOURCES b_test.cpp )

tribits_add_advanced_test( test_of_b
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 EXEC b_test
    PASS_REGULAR_EXPRESSION_ALL 
      "B label is: B"
      "B deps are: A no_deps_for_A"
  )
