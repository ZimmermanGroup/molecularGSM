include(SetDefaultAndFromEnv)

set(CTEST_NIGHTLY_START_TIME "04:00:00 UTC")  # Midnight EST
# NOTE: This does not need to be set for git checkouts, but it does need to be
# set for the build time stamp and there is a defect in ctest_start(
# ... APPEND ) that requires this be set.  See:
#
#    https://gitlab.kitware.com/cmake/cmake/issues/20471
#
# This currently matches the start time for the project on CDash which is
# 00:00:00 EST.

if (NOT DEFINED CTEST_DROP_METHOD)
  set_default_and_from_env(CTEST_DROP_METHOD "http")
endif()

if (CTEST_DROP_METHOD STREQUAL "http" OR CTEST_DROP_METHOD STREQUAL "https")
  set_default_and_from_env(CTEST_DROP_SITE "testing.sandia.gov")
  set_default_and_from_env(TRIBITS_2ND_CTEST_DROP_SITE "testing-dev.sandia.gov")
  set_default_and_from_env(CTEST_PROJECT_NAME "TriBITS")
  set_default_and_from_env(CTEST_DROP_LOCATION "/cdash/submit.php?project=TriBITS")
  set_default_and_from_env(CTEST_TRIGGER_SITE "")
  set_default_and_from_env(CTEST_DROP_SITE_CDASH TRUE)
endif()
