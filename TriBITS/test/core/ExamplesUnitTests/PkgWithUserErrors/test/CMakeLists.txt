tribits_add_executable_and_test( PkgWithUserErrorsTest
  SOURCES  PkgWithUserErrorsTest.cpp
  NUM_MPI_PROCS 1
  PASS_REGULAR_EXPRESSION
    "PkgWithUserErrorsLib returns PkgWithUserErrorsThing"
  )
