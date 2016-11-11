## Gives access to SET_DEFAULT_AND_FROM_ENV function
## This is file defined in TriBITS. Like a header file in C.
#INCLUDE(SetDefaultAndFromEnv)
#
## Settings for CTest/CDash
#IF(NOT DEFINED CTEST_DROP_METHOD)
#    SET_DEFAULT_AND_FROM_ENV(CTEST_DROP_METHOD "https")
#ENDIF()
#
#IF(CTEST_DROP_METHOD STREQUAL "https")
#    # The normal default is ${HOST_TYPE}-${COMPUTER_VERSION}-${BUILD_DIR}
#    # We are over-riding it by using set.
#    SET(CTEST_BUILD_NAME "Linux-HW2-mjafari")
#
#    # To learn more about this function look at the TriBits documentation
#    SET_DEFAULT_AND_FROM_ENV(CTEST_PROJECT_NAME "GSM")
#    SET_DEFAULT_AND_FROM_ENV(CTEST_TRIGGER_SITE "")
#
#    # CDash server hostname
#    SET_DEFAULT_AND_FROM_ENV(CTEST_DROP_SITE "cdash-ners590.aura.arc-ts.umich.edu")
#
#    # The rest of the web address
#    SET_DEFAULT_AND_FROM_ENV(CTEST_DROP_LOCATION "/submit.php?project=GSM")
#
#    # YES SUBMIT RESULTS! I WANT MY 10 POINTS!
#    SET_DEFAULT_AND_FROM_ENV(CTEST_DROP_SITE_CDASH TRUE)
#ENDIF()
