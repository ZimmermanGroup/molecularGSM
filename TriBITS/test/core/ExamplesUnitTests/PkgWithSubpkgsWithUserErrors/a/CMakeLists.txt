###############################################################
# test a passing call order, will be run by default turn this 
# off when you turn on one of the other sections
###############################################################
if(NOT subpackageA_turn_off_passing_call_order)
  #
  # A) Define the subpackage
  #

  tribits_subpackage(A)

  #
  # B) Set up subpackage-specific options
  #
  # Typically there are none or are few as most options are picked up from the
  # parent package's CMakeLists.txt file!
  
  #
  # C) Add the libraries, tests, and examples
  #

  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  tribits_add_library(pwswue_a
    SOURCES A.cpp
    HEADERS A.hpp
    NOINSTALLHEADERS 
  )

  tribits_add_test_directories(tests)

  #
  # D) Do standard post processing
  #

  tribits_subpackage_postprocess()
endif()


###############################################################
# Add a library after the postprocess call
###############################################################
if(subPackageA_ADD_LIBRARY_AFTER_POST_PROCESS)
  message("Adding library in subpackage after post process")
  
  tribits_subpackage(A)
  tribits_subpackage_postprocess()
  tribits_add_library(lib_after_post_process
    SOURCES A.cpp
    HEADERS A.hpp
    NOINSTALLHEADERS 
  )
endif()


###############################################################
# Add an executable after the postprocess call
###############################################################
if(subPackageA_ADD_EXECUTABLE_AFTER_POST_PROCESS)
  message("Adding executable in subpackage after post process")

  tribits_subpackage(A)
  tribits_subpackage_postprocess()
  tribits_add_executable(exe_after_post_process
    SOURCES A.cpp
    HEADERS A.hpp
  )
endif()


###############################################################
# no subpackage call before adding librbary
###############################################################
if(subPackageA_add_lib_no_PREPROCESS)
  tribits_add_library(lib_after_post_process
    SOURCES A.cpp
    HEADERS A.hpp
    NOINSTALLHEADERS 
  )
  tribits_subpackage_postprocess()
endif()


###############################################################
# no subpackage call before adding executable
###############################################################
if(subPackageA_add_exe_no_PREPROCESS)
  tribits_add_executable(exe_after_post_process
    SOURCES A.cpp
    HEADERS A.hpp
  )
  tribits_subpackage_postprocess()
endif()


###############################################################
# Multiple calls to SUBPACKAGE
###############################################################
if(subPackageA_DOUBLE_SUBPACKAGE_INIT)
  tribits_subpackage(A)
  tribits_subpackage(A)
  tribits_subpackage_postprocess()
endif()


###############################################################
# Multiple calls to SUBPACKAGE_POSTPROCESS
###############################################################
if(subPAckageA_DOUBLE_SUBPACKAGE_POSTPROCESS)
  tribits_subpackage(A)
  tribits_subpackage_postprocess()
  tribits_subpackage_postprocess()
endif()


###############################################################
# no call to SUBPACKAGE before SUBPACKAGE_POSTPROCESS
###############################################################
if(subPackageA_no_SUBPACKAGE_before_POSTPROCESS)
  tribits_subpackage_postprocess()
endif()


###############################################################
# call PACKAGE from a subpackage file
###############################################################
if(subPackageA_call_PACKAGE)
  tribits_package(A)
endif()

###############################################################
# call PACKAGE_DECL from a subpackage file
###############################################################
if(subPackageA_call_PACKAGE_DECL)
  tribits_package_decl(A)
endif()

###############################################################
# call PACKAGE_DEF from a subpackage file
###############################################################
if(subPackageA_call_PACKAGE_DEF)
  tribits_package_def(A)
endif()


###############################################################
# call PACKAGE_POSTPROCESS from a subpackage file
###############################################################
if(subPackageA_call_PACKAGE_POSTPROCESS)
  tribits_subpackage(A)
  tribits_package_postprocess()
endif()


###############################################################
# call PROCESS_SUBPACKAGES from a subpackage file
###############################################################
if(subPackageA_call_PROCESS_SUBPACKAGES)
  tribits_subpackage(A)
  tribits_process_subpackages()
  tribits_subpackage_postprocess()
endif()

###############################################################
# ADD_TEST_DIRECTORIES without SUBPACKAGE call
###############################################################
if(subPackageA_call_ADD_TEST_DIRECTORY_without_SUBPACKAGE)
  tribits_add_test_directories(tests)
  tribits_subpackage_postprocess()
endif()

###############################################################
# ADD_TEST_DIRECTORIES after POSTPROCESS
###############################################################
if(subPackageA_call_ADD_TEST_DIRECTORY_after_POSTPROCESS)
  tribits_subpackage(A)
  tribits_subpackage_postprocess()
  tribits_add_test_directories(tests)
endif()

###############################################################
# ADD_EXAMPLE_DIRECTORIES without SUBPACKAGE call
###############################################################
if(subPackageA_call_ADD_EXAMPLE_DIRECTORY_without_SUBPACKAGE)
  tribits_add_example_directories(tests)
  tribits_subpackage_postprocess()
endif()

###############################################################
# ADD_EXAMPLE_DIRECTORIES after POSTPROCESS
###############################################################
if(subPackageA_call_ADD_EXAMPLE_DIRECTORY_after_POSTPROCESS)
  tribits_subpackage(A)
  tribits_subpackage_postprocess()
  tribits_add_example_directories(tests)
endif()
