###############################################################
# test a passing call order, will be run by default turn this 
# off when you turn on one of the other sections
###############################################################
if(NOT ${PACKAGE_NAME}_turn_off_passing_call_order)
  tribits_package(PkgWithUserErrors)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  tribits_add_library(
    pkgwithusererrors
    HEADERS PkgWithUserErrorsLib.hpp
    SOURCES PkgWithUserErrorsLib.cpp
    )
  tribits_add_test_directories(test)
  tribits_package_postprocess()
endif()


###############################################################
# User forgets to call TRIBITS_PACKAGE_POSTPROCESS 
###############################################################
if(${PACKAGE_NAME}_no_POSTPROCESS_call)
  message("User forgets to call TRIBITS_PACKAGE_POSTPROCESS")
  tribits_package(PkgWithUserErrors)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  tribits_add_library(
    pkgwithusererrors
    HEADERS PkgWithUserErrorsLib.hpp
    SOURCES PkgWithUserErrorsLib.cpp
    )
  tribits_add_test_directories(test)
endif()


###############################################################
# User forgets to call TRIBITS_PACKAGE before adding lib
###############################################################
if(${PACKAGE_NAME}_ADD_LIBRARY_with_no_package_init)
  message("")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  tribits_add_library(
    pkgwithusererrors
    HEADERS PkgWithUserErrorsLib.hpp
    SOURCES PkgWithUserErrorsLib.cpp
    )
  tribits_add_test_directories(test)
  tribits_package_postprocess()
endif()


###############################################################
# User forgets to call TRIBITS_PACKAGE before adding exe
###############################################################
if(${PACKAGE_NAME}_ADD_EXECUTABLE_with_no_package_init)
  message("User forgets to call TRIBITS_PACKAGE before adding exe")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  tribits_add_executable(
    usererrorexec
    SOURCES  PkgWithUserErrorsEX.cpp
  )
  tribits_add_test_directories(test)
  tribits_package_postprocess()
endif()


###############################################################
# no call to TRIBITS_PACKAGE before call to POSTPROCESS
###############################################################
if(${PACKAGE_NAME}_POSTPROCESS_with_no_package_init)
  message("no call to TRIBITS_PACKAGE before call to POSTPROCESS")
  tribits_package_postprocess()
endif()


###############################################################
# multiple calls to TRIBITS_PACKAGE
###############################################################
if(${PACKAGE_NAME}_multiple_calls_to_PACKAGE)
  message("multiple calls to TRIBITS_PACKAGE")
  tribits_package(PkgWithUserErrors)
  tribits_package(PkgWithUserErrors)
  tribits_package_postprocess()
endif()


###############################################################
# adding a library after POSTPROCESS
###############################################################
if(${PACKAGE_NAME}_ADD_LIBRARY_after_POSTPROCESS)
  message("adding a library after POSTPROCESS")
  tribits_package(PkgWithUserErrors)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  tribits_package_postprocess()
  tribits_add_library(
    pkgwithusererrors
    HEADERS PkgWithUserErrorsLib.hpp
    SOURCES PkgWithUserErrorsLib.cpp
    )
endif()


###############################################################
# adding an exe after POSTPROCESS
###############################################################
if(${PACKAGE_NAME}_ADD_EXECUTABLE_after_POSTPROCESS)
  message("adding an exe after POSTPROCESS")
  tribits_package(PkgWithUserErrors)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  tribits_package_postprocess()
  tribits_add_executable(
    usererrorexec
    SOURCES  PkgWithUserErrorsEX.cpp
  )
endif()


###############################################################
# multiple calls to POSTPROCESS
###############################################################
if(${PACKAGE_NAME}_multiple_calls_to_POSTPROCESS)
  message("multiple calls to POSTPROCESS")
  tribits_package(PkgWithUserErrors)
  tribits_package_postprocess()
  tribits_package_postprocess()
endif()


###############################################################
# unparsed arguments in ADD_LIBRARY
###############################################################
if(${PACKAGE_NAME}_UNPARSED_ARGUMENTS_ADD_LIBRARY)
  message("unparsed arguments in ADD_LIBRARY")
  tribits_package(PkgWithUserErrors)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  message("building library")
  tribits_add_library(
    pkgwithusererrors
    this_shouldnt_be_here
    HEADERS PkgWithUserErrorsLib.hpp
    SOURCES PkgWithUserErrorsLib.cpp
    )
  tribits_add_test_directories(test)
  tribits_package_postprocess()
endif()


###############################################################
# unparsed arguments in ADD_EXECUTABLE
###############################################################
if(${PACKAGE_NAME}_UNPARSED_ARGUMENTS_ADD_EXECUTABLE)
  message("unparsed arguments in ADD_EXECUTABLE")
  tribits_package(PkgWithUserErrors)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  message("building executable")
  tribits_add_executable(
    usererrorexec
    misspelled_argument
    SOURCES  PkgWithUserErrorsEX.cpp
    SPORCOOS some_file.cpp
  )
  tribits_add_test_directories(test)
  tribits_package_postprocess()
endif()


if (${PACKAGE_NAME}_using_package_with_subpackage_commands)
  message("Using package with subpackage commands in error")
  tribits_package_decl(PkgWithUserErrors)
  tribits_process_subpackages()
  tribits_package_def()
  tribits_package_postprocess()
endif()
