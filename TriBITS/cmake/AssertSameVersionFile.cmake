# Make sure the base Version.cmake file is the same as the copy in
# tribits/Version.cmake We can't use a symlink because this breaks TriBITS on
# Windows (See TriBITSPub/TriBITS#129).  Later, this will be replaced with a
# configured Version.cmake file so this will not be an issue.
set(BASE_VERSION_CMAKE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Version.cmake")
set(TRIBITS_VERSION_CMAKE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/tribits/Version.cmake")
file(READ "${BASE_VERSION_CMAKE_FILE}" BASE_VERSION_CMAKE_STR)
file(READ "${TRIBITS_VERSION_CMAKE_FILE}" TRIBITS_VERSION_CMAKE_STR)
if (NOT BASE_VERSION_CMAKE_STR STREQUAL TRIBITS_VERSION_CMAKE_STR)
  message(FATAL_ERROR
    "ERROR: '${BASE_VERSION_CMAKE_FILE}' and '${TRIBITS_VERSION_CMAKE_FILE}' are"
    " different (see TriBITSPub/TriBITS#129)!")
endif()
