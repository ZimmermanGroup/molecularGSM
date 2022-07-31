########################################################################
# TribitsHelloWorld
########################################################################


set(TribitsHelloWorld_COMMON_CONFIG_ARGS
  ${SERIAL_PASSTHROUGH_CONFIGURE_ARGS}
  )

tribits_add_advanced_test( TribitsHelloWorld
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DTribitsHelloWorld_ENABLE_TESTS=ON
      -DHelloWorld_ENABLE_CPACK_PACKAGING=ON
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL
      "Configuring done"
      "Generating done"
      "Build files have been written to: .*ExamplesUnitTests/TriBITS_TribitsHelloWorld"
    FAIL_REGULAR_EXPRESSION
      "Check for working Fortran compiler" # Should not be looking for Fortran!
  TEST_1 CMND make
    ARGS ${CTEST_BUILD_FLAGS}
    PASS_REGULAR_EXPRESSION_ALL
      "Built target hello_world_lib"
      "Built target hello_world"
      "Built target HelloWorld_unit_tests"
  TEST_2 CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
    PASS_REGULAR_EXPRESSION_ALL
      ": HelloWorld_hello_world .*  Passed"
      ": HelloWorld_unit_tests .*  Passed"
      "100% tests passed, 0 tests failed out of 2"
  )


tribits_add_advanced_test( TribitsHelloWorld_EXE_DISABLE
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DTribitsHelloWorld_ENABLE_TESTS=ON
      -DHelloWorld_unit_tests_EXE_DISABLE=ON
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL
      "-- HelloWorld_unit_tests EXE NOT being built due to HelloWorld_unit_tests_EXE_DISABLE=[']ON[']"
      "Configuring done"
      "Generating done"
      "Build files have been written to: .*ExamplesUnitTests/TriBITS_TribitsHelloWorld"
  TEST_1 CMND make
    ARGS ${CTEST_BUILD_FLAGS}
    PASS_REGULAR_EXPRESSION_ALL
      "Built target hello_world_lib"
      "Built target hello_world"
  TEST_2 CMND ${CMAKE_CTEST_COMMAND} ARGS -VV
    PASS_REGULAR_EXPRESSION_ALL
      "Test [#]2: HelloWorld_unit_tests [.]+[*][*][*]Not Run"
      ": HelloWorld_hello_world .*  Passed"
      "2 - HelloWorld_unit_tests [(]Not Run[)]"
      "50% tests passed, 1 tests failed out of 2"
  )
  # NOTE: Above we are testing the <exec_name>_EXE_DISABLE option in a full
  # configure and build case because the tribits_add_executable() command is
  # not set up for unit-testing mode.


tribits_add_advanced_test( TribitsHelloWorld_InSourceBuildErrors
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    CMND ${CMAKE_COMMAND}
    ARGS -E copy_directory
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
      TribitsHelloWorld

  TEST_1
    MESSAGE "Try an in-source configure and check error message"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DTribitsHelloWorld_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      .
    WORKING_DIRECTORY TribitsHelloWorld
    SKIP_CLEAN_WORKING_DIRECTORY
    PASS_REGULAR_EXPRESSION_ALL
      "CMAKE_CURRENT_SOURCE_DIR=.*/TriBITS_TribitsHelloWorld_InSourceBuildErrors/TribitsHelloWorld"
      "CMAKE_CURRENT_BINARY_DIR=.*/TriBITS_TribitsHelloWorld_InSourceBuildErrors/TribitsHelloWorld"
      "TribitsHelloWorld does not support in source builds"
      "NOTE: You must now delete the CMakeCache.txt file and the CMakeFiles/"
      "Please create a different directory and configure"
    ALWAYS_FAIL_ON_ZERO_RETURN

  TEST_2
    MESSAGE "Try configure in build subdir and check for bad CMakeCache.txt file"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DTribitsHelloWorld_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      ..
    WORKING_DIRECTORY TribitsHelloWorld/BUILD
    PASS_REGULAR_EXPRESSION_ALL
      "TriBITS_TribitsHelloWorld_InSourceBuildErrors/TribitsHelloWorld/CMakeCache.txt"
      "exists from a likely prior attempt to do an in-source build"
      ""
    ALWAYS_FAIL_ON_ZERO_RETURN

  )


tribits_add_advanced_test( TribitsHelloWorld_DefaultGlobalTimeout
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Configure first time not setting any default timeout."
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    ALWAYS_FAIL_ON_NONZERO_RETURN
  TEST_1
    MESSAGE "Make sure that 'TimeOut' is set to the CMake default of 1500"
    CMND grep ARGS "^TimeOut: " DartConfiguration.tcl
    PASS_REGULAR_EXPRESSION "TimeOut: 1500"
  TEST_2
    MESSAGE "Make sure DART_TESTING_TIMEOUT in cache is the CMake default 1500!"
    CMND grep ARGS "^DART_TESTING_TIMEOUT:" CMakeCache.txt
    PASS_REGULAR_EXPRESSION "DART_TESTING_TIMEOUT:STRING=1500"

  TEST_3
    MESSAGE "Reconfigure and make sure the timeout is still set correctly"
    CMND ${CMAKE_COMMAND} ARGS .
    PASS_REGULAR_EXPRESSION_ALL "Generating done"
  TEST_4
    MESSAGE "Make sure that 'TimeOut' is set correctly on reconfigure"
    CMND grep ARGS "^TimeOut: " DartConfiguration.tcl
    PASS_REGULAR_EXPRESSION "TimeOut: 1500"
  TEST_5
    MESSAGE "Make sure DART_TESTING_TIMEOUT in cache is still the default"
    CMND grep ARGS "^DART_TESTING_TIMEOUT:" CMakeCache.txt
    PASS_REGULAR_EXPRESSION "DART_TESTING_TIMEOUT:STRING=1500"

  )


tribits_add_advanced_test( TribitsHelloWorld_DefaultGlobalTimeout_ScaleTimeout
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Configure first time not setting any default timeout but scale it."
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DTribitsHelloWorld_SCALE_TEST_TIMEOUT=2.0
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    ALWAYS_FAIL_ON_NONZERO_RETURN
  TEST_1
    MESSAGE "Make sure that default 'TimeOut' is scaled correctly by 2.0"
    CMND grep ARGS "^TimeOut: " DartConfiguration.tcl
    PASS_REGULAR_EXPRESSION "TimeOut: 3000"
  TEST_2
    MESSAGE "Make sure DART_TESTING_TIMEOUT in cache is CMake default 1500"
    CMND grep ARGS "^DART_TESTING_TIMEOUT:" CMakeCache.txt
    PASS_REGULAR_EXPRESSION "DART_TESTING_TIMEOUT:STRING=1500"

  TEST_3
    MESSAGE "Reconfigure and make sure the timeout is still set correctly"
    CMND ${CMAKE_COMMAND} ARGS .
    PASS_REGULAR_EXPRESSION_ALL "Generating done"
  TEST_4
    MESSAGE "Make sure that 'TimeOut' is set correctly on reconfigure"
    CMND grep ARGS "^TimeOut: " DartConfiguration.tcl
    PASS_REGULAR_EXPRESSION "TimeOut: 3000"
  TEST_5
    MESSAGE "Make sure DART_TESTING_TIMEOUT in cache is still the default"
    CMND grep ARGS "^DART_TESTING_TIMEOUT:" CMakeCache.txt
    PASS_REGULAR_EXPRESSION "DART_TESTING_TIMEOUT:STRING=1500"

  )


tribits_add_advanced_test( TribitsHelloWorld_ScaleTimeout_FirstConfig
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Configure first time out scaling the timeout."
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DDART_TESTING_TIMEOUT=200.0
      -DTribitsHelloWorld_SCALE_TEST_TIMEOUT=1.5
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL "Generating done"
  TEST_1
    MESSAGE "Make sure that 'TimeOut' is set correctly on the first try"
    CMND grep ARGS "^TimeOut: " DartConfiguration.tcl
    PASS_REGULAR_EXPRESSION "TimeOut: 300"
  TEST_2
    MESSAGE "Make sure DART_TESTING_TIMEOUT in cache is what the user passed in!"
    CMND grep ARGS "^DART_TESTING_TIMEOUT:" CMakeCache.txt
    PASS_REGULAR_EXPRESSION "DART_TESTING_TIMEOUT:STRING=200.0"

  TEST_3
    MESSAGE "Reconfigure and make sure the timeout is still set correctly"
    CMND ${CMAKE_COMMAND} ARGS .
    PASS_REGULAR_EXPRESSION_ALL "Generating done"
  TEST_4
    MESSAGE "Make sure that 'TimeOut' is set correctly on reconfigure"
    CMND grep ARGS "^TimeOut: " DartConfiguration.tcl
    PASS_REGULAR_EXPRESSION "TimeOut: 300"
  TEST_5
    MESSAGE "Make sure DART_TESTING_TIMEOUT in cache is still what the user passed in!"
    CMND grep ARGS "^DART_TESTING_TIMEOUT:" CMakeCache.txt
    PASS_REGULAR_EXPRESSION "DART_TESTING_TIMEOUT:STRING=200.0"

  )


tribits_add_advanced_test( TribitsHelloWorld_ScaleTimeout_Reconfig
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0 CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DDART_TESTING_TIMEOUT=200.0
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL "Generating done"
    MESSAGE "Configure with default 1.0 scaling"
  TEST_1 CMND grep ARGS "^TimeOut: " DartConfiguration.tcl
    PASS_REGULAR_EXPRESSION "TimeOut: 200.0"
  TEST_2 CMND grep ARGS "^DART_TESTING_TIMEOUT:" CMakeCache.txt
    MESSAGE "DART_TESTING_TIMEOUT in cache does not change!"
    PASS_REGULAR_EXPRESSION "DART_TESTING_TIMEOUT:STRING=200.0"

  TEST_3 CMND ${CMAKE_COMMAND}
    ARGS
      -DTribitsHelloWorld_SCALE_TEST_TIMEOUT=1.5
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    MESSAGE "Configure with 1.5 scaling"
    PASS_REGULAR_EXPRESSION_ALL
      "DART_TESTING_TIMEOUT=200.0 being scaled by TribitsHelloWorld_SCALE_TEST_TIMEOUT=1.5 to 300"
      "Generating done"

  TEST_4 CMND grep ARGS "^TimeOut: " DartConfiguration.tcl
    PASS_REGULAR_EXPRESSION "TimeOut: 300"
  TEST_5 CMND grep ARGS "^DART_TESTING_TIMEOUT:" CMakeCache.txt
    MESSAGE "DART_TESTING_TIMEOUT in cache does not change!"
    PASS_REGULAR_EXPRESSION "DART_TESTING_TIMEOUT:STRING=200.0"

  TEST_6 CMND ${CMAKE_COMMAND}
    ARGS
      -DTribitsHelloWorld_SCALE_TEST_TIMEOUT=2.0
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    MESSAGE "Configure with 2.0 scaling"
    PASS_REGULAR_EXPRESSION_ALL
      "DART_TESTING_TIMEOUT=200.0 being scaled by TribitsHelloWorld_SCALE_TEST_TIMEOUT=2.0 to 400"
      "Generating done"
  TEST_7 CMND grep ARGS "^TimeOut: " DartConfiguration.tcl
    PASS_REGULAR_EXPRESSION "TimeOut: 400"
  TEST_8 CMND grep ARGS "^DART_TESTING_TIMEOUT:" CMakeCache.txt
    MESSAGE "DART_TESTING_TIMEOUT in cache does not change!"
    PASS_REGULAR_EXPRESSION "DART_TESTING_TIMEOUT:STRING=200.0"

  )


if (TriBITS_EANBLE_Fortran)
 set(TribitsHelloWorld_XSDK_DEFAULTS_Fortran_REGEX
    "-- XSDK: Setting CMAKE_Fortran_COMPILER from env var FC=")
else()
 set(TribitsHelloWorld_XSDK_DEFAULTS_Fortran_REGEX)
endif()


tribits_add_advanced_test( TribitsHelloWorld_XSDK_DEFAULTS
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0 CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DUSE_XSDK_DEFAULTS=TRUE
      -DCMAKE_C_COMPILER=
      -DCMAKE_CXX_COMPILER=
      -DCMAKE_Fortran_COMPILER=
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL
      "-- XSDK: Setting default BUILD_SHARED_LIBS=TRUE"
      "USE_XSDK_DEFAULTS=.TRUE."
      "-- XSDK: Setting CMAKE_C_COMPILER from env var CC="
      "-- XSDK: Setting CMAKE_CXX_COMPILER from env var CXX="
      "${TribitsHelloWorld_XSDK_DEFAULTS_Fortran_REGEX}"
      "-- XSDK: Setting default CMAKE_BUILD_TYPE=DEBUG"
      "-- CMAKE_BUILD_TYPE='DEBUG'"
      "-- BUILD_SHARED_LIBS='TRUE'"
      "Generating done"
 
  ENVIRONMENT
    CC=${CMAKE_C_COMPILER}
    CXX=${CMAKE_CXX_COMPILER}
    FC=${CMAKE_Fortran_COMPILER}
  )


tribits_add_advanced_test( TribitsHelloWorld_CONFIGURE_OPTIONS_FILE
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Copy TribitsHelloWorld so that we can copy things into it."
    CMND ${CMAKE_COMMAND}
    ARGS -E copy_directory ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld TribitsHelloWorld

  TEST_1
    MESSAGE "Get the initial configure out of the way"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DTribitsHelloWorld_TRIBITS_DIR=${${PROJECT_NAME}_TRIBITS_DIR}
      TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL
      "Generating done"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_2
    MESSAGE "Copy ConfigOptions1.cmake to source dir"
    CMND cp
    ARGS ${CMAKE_CURRENT_SOURCE_DIR}/ConfigOptions1.cmake TribitsHelloWorld/

  TEST_3
    MESSAGE "Configure using default FILEPATH pointing to ConfigOptions1.cmake (should fail)"
    CMND ${CMAKE_COMMAND}
    ARGS -DTribitsHelloWorld_CONFIGURE_OPTIONS_FILE=ConfigOptions1.cmake
      .
    PASS_REGULAR_EXPRESSION_ALL
      "(include|INCLUDE) could not find (load|requested) file"
      "TriBITS_TribitsHelloWorld_CONFIGURE_OPTIONS_FILE/ConfigOptions1.cmake"
    # NOTE: Above shows that FILEPATH type causes relative paths to be
    # evaluated w.r.t. the current working directory.

  TEST_4
    MESSAGE "Configure using STRING pointing to ConfigOptions1.cmake (should pass)"
    CMND ${CMAKE_COMMAND}
    ARGS -DTribitsHelloWorld_CONFIGURE_OPTIONS_FILE:STRING=ConfigOptions1.cmake
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Reading in configuration options from ConfigOptions1.cmake"
      "Included ConfigOptions1.cmake"
    ALWAYS_FAIL_ON_NONZERO_RETURN
    # NOTE: Above shows that STRING type causes relative paths to be evaluated
    # w.r.t. the current working directory.

  TEST_5
    MESSAGE "Configure using FILEPATH pointing to ConfigOptions2.cmake (should pass)"
    CMND ${CMAKE_COMMAND}
    ARGS -DTribitsHelloWorld_CONFIGURE_OPTIONS_FILE:FILEPATH=${CMAKE_CURRENT_SOURCE_DIR}/ConfigOptions2.cmake
      .
    PASS_REGULAR_EXPRESSION_ALL
      "Reading in configuration options from .*/test/core/ExamplesUnitTests/ConfigOptions2.cmake"
      "Included ConfigOptions2.cmake"
    ALWAYS_FAIL_ON_NONZERO_RETURN
    # NOTE: Above shows that STRING type causes relative paths to be evaluated
    # w.r.t. the current working directory.

  )


tribits_add_advanced_test( TribitsHelloWorld_Shared_NoVersion
  OVERALL_WORKING_DIRECTORY TEST_NAME
  EXCLUDE_IF_NOT_TRUE IS_REAL_LINUX_SYSTEM
  OVERALL_NUM_MPI_PROCS 1
  TEST_0 CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DBUILD_SHARED_LIBS=ON
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL
      "Configuring done"
      "Generating done"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  TEST_1 CMND make
    ARGS ${CTEST_BUILD_FLAGS}
    PASS_REGULAR_EXPRESSION_ALL
      "Built target hello_world_lib"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  TEST_2 CMND ls ARGS hello_world
    PASS_REGULAR_EXPRESSION_ALL
      "libhello_world_lib[.]so"
    FAIL_REGULAR_EXPRESSION
      "libhello_world_lib[.]so[.]SOVERSION"
    ALWAYS_FAIL_ON_NONZERO_RETURN
  )


tribits_add_advanced_test( TribitsHelloWorld_install_perms
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Do initial configure with just libs not tests with default install settings"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DCMAKE_INSTALL_PREFIX=install
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL
      "Configuring done"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_1
    MESSAGE "Verify that set_installed_group_and_permissions.cmake is *not* in build dir"
    CMND ls ARGS set_installed_group_and_permissions.cmake
    PASS_REGULAR_EXPRESSION
      "No such file or directory"
    ALWAYS_FAIL_ON_ZERO_RETURN
  # NOTE: We don't want running any extra code if we are just using the stock
  # built-in CMake permissions scheme.  (See trilinos/Trilinos#7881)

  TEST_2
    MESSAGE "Do make to build everything"
    CMND make ARGS ${CTEST_BUILD_FLAGS}
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_3
    MESSAGE "Make install with default directory settings"
    CMND make ARGS ${CTEST_BUILD_FLAGS} install
    PASS_REGULAR_EXPRESSION_ALL
      "Installing: .*/TriBITS_TribitsHelloWorld_install_perms/install/lib/libhello_world_lib.a"
      "Installing: .*/TriBITS_TribitsHelloWorld_install_perms/install/include/hello_world_lib.hpp"
      "Installing: .*/TriBITS_TribitsHelloWorld_install_perms/install/bin/hello_world.exe"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_4
    MESSAGE "Check some default install directory permissions"
    CMND ls ARGS -ld
      install install/include install/lib install/bin
    PASS_REGULAR_EXPRESSION_ALL
      "drwx.* .* install"
      "drwx.* .* install/bin"
      "drwx.* .* install/include"
      "drwx.* .* install/lib"
    ALWAYS_FAIL_ON_NONZERO_RETURN
    # NOTE: Above we can't change the default group or other permissions
    # because that depends on the user's umask when the run these tests.  But
    # the owner should have read/write/execute on the directories or it could
    # not possibly install anything.

  TEST_5
    MESSAGE "Check some default install file permissions"
    CMND ls ARGS -l
      install install/include install/lib install/bin
    PASS_REGULAR_EXPRESSION_ALL
      "[-]rwxr-xr-x.* .* hello_world.exe"
      "[-]rw-r--r--.* .* .* libhello_world_lib.a"
    ALWAYS_FAIL_ON_NONZERO_RETURN
    # NOTE: The above permissions are the default install permissions of CMake
    # for files that it installs.

  TEST_6
    MESSAGE "Reconfigure with <Project>_MAKE_INSTALL_WORLD_READABLE=ON"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DTribitsHelloWorld_MAKE_INSTALL_WORLD_READABLE=ON
      -DCMAKE_INSTALL_PREFIX=install
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL
      "-- TribitsHelloWorld_MAKE_INSTALL_WORLD_READABLE='ON'"
      "-- TribitsHelloWorld_MAKE_INSTALL_GROUP_READABLE=''"
      "-- CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS = [(]OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE[)]"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_7
    MESSAGE "Verify that set_installed_group_and_permissions.cmake *is* in build dir"
    CMND ls ARGS set_installed_group_and_permissions.cmake
    PASS_REGULAR_EXPRESSION
      "set_installed_group_and_permissions.cmake"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_8
    MESSAGE "Remove the install directory"
    CMND rm ARGS -r install
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_9
    MESSAGE "Re-install"
    CMND make ARGS ${CTEST_BUILD_FLAGS} install
    PASS_REGULAR_EXPRESSION_ALL
      "Installing: .*/TriBITS_TribitsHelloWorld_install_perms/install/bin/hello_world.exe"
      "0: Running: chmod -R g[+]rX,o[+]rX /.*/TriBITS_TribitsHelloWorld_install_perms/install"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_10
    MESSAGE "Check updated permissions for <Project>_MAKE_INSTALL_WORLD_READABLE=ON"
    CMND ls ARGS -ld
      install install/include install/lib install/bin
    PASS_REGULAR_EXPRESSION_ALL
      "drwxr-xr-x.* .* install"
      "drwxr-xr-x.* .* install/bin"
      "drwxr-xr-x.* .* install/include"
      "drwxr-xr-x.* .* install/lib"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_11
    MESSAGE "Reconfigure with <Project>_MAKE_INSTALL_GROUP_READABLE=ON and <Project>_MAKE_INSTALL_WORLD_READABLE=OFF"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DTribitsHelloWorld_MAKE_INSTALL_GROUP_READABLE=ON
      -DTribitsHelloWorld_MAKE_INSTALL_WORLD_READABLE=OFF
      -DCMAKE_INSTALL_PREFIX=install
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL
      "-- TribitsHelloWorld_MAKE_INSTALL_GROUP_READABLE='ON'"
      "-- TribitsHelloWorld_MAKE_INSTALL_WORLD_READABLE='OFF'"
      "-- CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS = [(]OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE[)]"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_12
    MESSAGE "Remove the install directory"
    CMND rm ARGS -r install
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_13
    MESSAGE "Re-install"
    CMND make ARGS ${CTEST_BUILD_FLAGS} install
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_14
    MESSAGE "Check updated permissions for <Project>_MAKE_INSTALL_GROUP_READABLE=ON and <Project>_MAKE_INSTALL_WORLD_READABLE=OFF"
    CMND ls ARGS -ld
      install install/include install/lib install/bin
    PASS_REGULAR_EXPRESSION_ALL
      "drwxr-x---.* .* install"
      "drwxr-x---.* .* install/bin"
      "drwxr-x---.* .* install/include"
      "drwxr-x---.* .* install/lib"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_15
    MESSAGE "Reconfigure with <Project>_MAKE_INSTALL_GROUP_READABLE=OFF and <Project>_MAKE_INSTALL_WORLD_READABLE=OFF"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DTribitsHelloWorld_MAKE_INSTALL_GROUP_READABLE=OFF
      -DTribitsHelloWorld_MAKE_INSTALL_WORLD_READABLE=OFF
      -DCMAKE_INSTALL_PREFIX=install
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL
      "-- TribitsHelloWorld_MAKE_INSTALL_GROUP_READABLE='OFF'"
      "-- TribitsHelloWorld_MAKE_INSTALL_WORLD_READABLE='OFF'"
      "-- CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS = [(]OWNER_READ OWNER_WRITE OWNER_EXECUTE[)]"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_16
    MESSAGE "Remove the install directory"
    CMND rm ARGS -r install
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_17
    MESSAGE "Re-install"
    CMND make ARGS ${CTEST_BUILD_FLAGS} install
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_18
    MESSAGE "Check updated permissions for <Project>_MAKE_INSTALL_GROUP_READABLE=OFF and <Project>_MAKE_INSTALL_WORLD_READABLE=OFF"
    CMND ls ARGS -ld
      install install/include install/lib install/bin
    PASS_REGULAR_EXPRESSION_ALL
      "drwx------.* .* install"
      "drwx------.* .* install/bin"
      "drwx------.* .* install/include"
      "drwx------.* .* install/lib"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  )
  # NOTE: Above we are testing different directory install options.


tribits_add_advanced_test( TribitsHelloWorld_install_package_by_package
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Do initial configure with just libs"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DCMAKE_INSTALL_PREFIX=install
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL
      "Configuring done"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_1
    MESSAGE "Do make to build everything"
    CMND make ARGS ${CTEST_BUILD_FLAGS}
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_2
    MESSAGE "Make install_package_by_package without a runner"
    CMND make ARGS ${CTEST_BUILD_FLAGS} install_package_by_package
    PASS_REGULAR_EXPRESSION_ALL
      "Installing: .*/TriBITS_TribitsHelloWorld_install_package_by_package/install/lib/libhello_world_lib[.]a"
      "Installing: .*/TriBITS_TribitsHelloWorld_install_package_by_package/install/include/hello_world_lib[.]hpp"
      "Installing: .*/TriBITS_TribitsHelloWorld_install_package_by_package/install/bin/hello_world[.]exe"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_3
    MESSAGE "Check for some installed files"
    CMND ls ARGS
      install/include/hello_world_lib.hpp
      install/lib/libhello_world_lib.a
      install/bin/hello_world.exe
    PASS_REGULAR_EXPRESSION_ALL
      "install/include/hello_world_lib[.]hpp"
      "install/lib/libhello_world_lib[.]a"
      "install/bin/hello_world[.]exe"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  )
  # NOTE: Above we are testing the 'install_package_by_package' target and
  # without using an install runner.


tribits_add_advanced_test( TribitsHelloWorld_install_package_by_package_with_runner
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Do initial configure with an install_package_by_package runner"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DTribitsHelloWorld_INSTALL_PBP_RUNNER=${CMAKE_CURRENT_SOURCE_DIR}/run-program.sh
      -DCMAKE_INSTALL_PREFIX=install
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL
      "Configuring done"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_1
    MESSAGE "Do make to build everything"
    CMND make ARGS ${CTEST_BUILD_FLAGS}
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_2
    MESSAGE "Make install_package_by_package with a runner"
    CMND make ARGS ${CTEST_BUILD_FLAGS} install_package_by_package
    PASS_REGULAR_EXPRESSION_ALL
      "Running: .*/cmake.* -P cmake_pbp_install[.]cmake"
      "Installing: .*/TriBITS_TribitsHelloWorld_install_package_by_package_with_runner/install/lib/libhello_world_lib[.]a"
      "Installing: .*/TriBITS_TribitsHelloWorld_install_package_by_package_with_runner/install/include/hello_world_lib[.]hpp"
      "Installing: .*/TriBITS_TribitsHelloWorld_install_package_by_package_with_runner/install/bin/hello_world[.]exe"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_3
    MESSAGE "Check for some installed files"
    CMND ls ARGS
      install/include/hello_world_lib.hpp
      install/lib/libhello_world_lib.a
      install/bin/hello_world.exe
    PASS_REGULAR_EXPRESSION_ALL
      "install/include/hello_world_lib[.]hpp"
      "install/lib/libhello_world_lib[.]a"
      "install/bin/hello_world[.]exe"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  )
  # NOTE: Above we are testing the 'install_package_by_package' with an
  # install runner for that target set with <Project>_INSTALL_PBP_RUNNER.


tribits_add_advanced_test( TribitsHelloWorld_install_perms_windows_error
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Cconfigure with <Project>_MAKE_INSTALL_WORLD_READABLE=ON and Windows"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DTribitsHelloWorld_MAKE_INSTALL_GROUP_WRITABLE=on
      -DTribitsHelloWorld_MAKE_INSTALL_GROUP_READABLE=true
      -DTribitsHelloWorld_MAKE_INSTALL_WORLD_READABLE=TRUE
      -DTribitsHelloWorld_MAKE_INSTALL_GROUP=dummy-group
      -DTribitsHelloWorld_HOSTTYPE=Windows
      -DCMAKE_INSTALL_PREFIX=install
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL
      "ERROR: The options"
      "    TribitsHelloWorld_MAKE_INSTALL_GROUP_WRITABLE='on'"
      "    TribitsHelloWorld_MAKE_INSTALL_GROUP_READABLE='true'"
      "    TribitsHelloWorld_MAKE_INSTALL_WORLD_READABLE='TRUE'"
      "    TribitsHelloWorld_MAKE_INSTALL_GROUP='dummy-group'"
      "are not supported on Windows!"
      "Please remove these options and configure from scratch!"
    ALWAYS_FAIL_ON_ZERO_RETURN

  )


tribits_add_advanced_test( TribitsHelloWorld_install_config_dummy_proj
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Do initial configure with just libs"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DCMAKE_INSTALL_PREFIX=install
      -DMPI_EXEC=mympiexec
      -DMPI_EXEC_PRE_NUMPROCS_FLAGS="--pre-flags"    # Can't use ';'
      -DMPI_EXEC_NUMPROCS_FLAG="-mynp"
      -DMPI_EXEC_POST_NUMPROCS_FLAGS="--post-flags"  # Can't use ';'
      -DTribitsHelloWorld_ENABLE_INSTALL_CMAKE_CONFIG_FILES=ON
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL
      "Configuring done"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_1
    MESSAGE "Do make install to build and everything"
    CMND make ARGS ${CTEST_BUILD_FLAGS} install
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_2
    MESSAGE "Create and configure a dummy project that calls"
      " find_package(TribitsHelloWorld) and finds it in the the install tree"
    CMND ${CMAKE_COMMAND}
    ARGS
      -DDUMMY_PROJECT_NAME=DummyProject
      -DDUMMY_PROJECT_DIR=dummy_client_of_TribitsHelloWorld
      -DEXPORT_VAR_PREFIX=TribitsHelloWorld
      -DFIND_PACKAGE_NAME=TribitsHelloWorld
      -DCMAKE_PREFIX_PATH=../install
      -DCMAKE_COMMAND=${CMAKE_COMMAND}
      -P ${CMAKE_CURRENT_SOURCE_DIR}/RunDummyPackageClientBulid.cmake
    PASS_REGULAR_EXPRESSION_ALL
      "DUMMY_PROJECT_NAME = 'DummyProject'"
      "DUMMY_PROJECT_DIR = 'dummy_client_of_TribitsHelloWorld'"
      "EXPORT_VAR_PREFIX = 'TribitsHelloWorld'"
      "Configure the dummy project to print the variables in .*/TriBITS_TribitsHelloWorld_install_config_dummy_proj/dummy_client_of_TribitsHelloWorld ..."
      "DUMMY_PROJECT_NAME = 'DummyProject'"
      "EXPORT_VAR_PREFIX = 'TribitsHelloWorld'"
      "Calling: find_package[(]TribitsHelloWorld REQUIRED COMPONENTS  OPTIONAL_COMPONENTS  [)]"
      "TribitsHelloWorld_CMAKE_BUILD_TYPE = 'RELEASE'"
      "TribitsHelloWorld_CXX_COMPILER = '${CMAKE_CXX_COMPILER_FOR_REGEX}'"
      "TribitsHelloWorld_C_COMPILER = '${CMAKE_C_COMPILER_FOR_REGEX}'"
      "TribitsHelloWorld_Fortran_COMPILER = ''"
      "TribitsHelloWorld_FORTRAN_COMPILER = ''"
      "TribitsHelloWorld_CXX_FLAGS = ''"
      "TribitsHelloWorld_C_FLAGS = ''"
      "TribitsHelloWorld_Fortran_FLAGS = ''"
      "TribitsHelloWorld_FORTRAN_FLAGS = ''"
      "TribitsHelloWorld_EXTRA_LD_FLAGS = ''"
      "TribitsHelloWorld_SHARED_LIB_RPATH_COMMAND = ''"
      "TribitsHelloWorld_BUILD_SHARED_LIBS = 'FALSE'"
      "TribitsHelloWorld_LINKER = '.*'"
      "TribitsHelloWorld_AR = '.*'"
      "TribitsHelloWorld_INSTALL_DIR = '.*/TriBITS_TribitsHelloWorld_install_config_dummy_proj/install'"
      "TribitsHelloWorld_INCLUDE_DIRS = '.*/TriBITS_TribitsHelloWorld_install_config_dummy_proj/install/include'"
      "TribitsHelloWorld_LIBRARY_DIRS = ''"
      "TribitsHelloWorld_LIBRARIES = 'HelloWorld::hello_world_lib'"
      "TribitsHelloWorld_TPL_INCLUDE_DIRS = '"
      "TribitsHelloWorld_TPL_LIBRARY_DIRS = ''"
      "TribitsHelloWorld_TPL_LIBRARIES = ''"
      "TribitsHelloWorld_MPI_LIBRARIES = ''"
      "TribitsHelloWorld_MPI_LIBRARY_DIRS = ''"
      "TribitsHelloWorld_MPI_INCLUDE_DIRS = ''"
      "TribitsHelloWorld_MPI_EXEC = 'mympiexec"
      "TribitsHelloWorld_MPI_EXEC_MAX_NUMPROCS = '[1-9]*'"  # Is null for an MPI build
      "TribitsHelloWorld_MPI_EXEC_PRE_NUMPROCS_FLAGS = '--pre-flags'"
      "TribitsHelloWorld_MPI_EXEC_NUMPROCS_FLAG = '-mynp"
      "TribitsHelloWorld_MPI_EXEC_POST_NUMPROCS_FLAGS = '--post-flags'"
      "TribitsHelloWorld_PACKAGE_LIST = 'HelloWorld'"
      "TribitsHelloWorld_SELECTED_PACKAGE_LIST = 'HelloWorld'"
      "TribitsHelloWorld_TPL_LIST = ''"  # Must work for no MPI too
      "TribitsHelloWorld_TPL_LIST = ''"
      "TribitsHelloWorld::all_libs  INTERFACE_LINK_LIBRARIES: 'HelloWorld::all_libs'"
      "TribitsHelloWorld::all_selected_libs  INTERFACE_LINK_LIBRARIES: 'HelloWorld::all_libs'"
      "-- Configuring done"
      "-- Generating done"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  )
  # NOTE: Above we are testing:
  #
  # * 'install' target builds and installs by default
  #
  # * The <Project>Config.cmake file works if there are no TPLs enabled
  #   (defect trilinos/trilinos#5213)
  #


# NOTE: In the below tests, we are only testing TriBITS support for generating
# the ctest_resources.json file, setting the RESOURCES_GROUP and ENVIRONMENT
# test properties, and testing CMake support for recognizing the
# RESOURCES_GROUP test property and putting it in the CTestTestfile.cmake
# file.  We are not actually running ctest or looking for the var
# CTEST_RESOURCE_SPEC_FILE set in the CTestTestfile.cmake file.  (Support for
# CTEST_RESOURCE_SPEC_FILE was not added proper until 3.18 so we can't rely on
# it in these tests that work with CMake 3.17.)


tribits_add_advanced_test( TribitsHelloWorld_ctest_resources_file_autogen_2_3
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Configure and generate the ctest_resources.json file"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DTribitsHelloWorld_ENABLE_TESTS=ON
      -DTribitsHelloWorld_AUTOGENERATE_TEST_RESOURCE_FILE=ON
      -DTribitsHelloWorld_CUDA_NUM_GPUS=2
      -DTribitsHelloWorld_CUDA_SLOTS_PER_GPU=3
      -DTPL_ENABLE_CUDA=ON
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL
      "Configuring done"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_1
    MESSAGE "Determine that ctest_resources.json was generated correctly"
    CMND diff
    ARGS ctest_resources.json
      "${CMAKE_CURRENT_SOURCE_DIR}/ctest_resources/ctest_resources.autogen_2_3_.json"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_2
    MESSAGE "Check that the CTestTestfile.cmake file has the right entries"
    CMND cat
    ARGS hello_world/CTestTestfile.cmake
    PASS_REGULAR_EXPRESSION_ALL
      "ENVIRONMENT .CTEST_KOKKOS_DEVICE_TYPE=gpus."
      "PROCESSORS .1."
      "RESOURCE_GROUPS .1,gpus:1."
    ALWAYS_FAIL_ON_NONZERO_RETURN

  )
  # NOTE: The above test ensures that the ctest_resources.json file gets
  # auto-generated correctly and ensures that the RESOURCE_GROUPS test
  # property gets set correctly in a CUDA build of a project.  (What is
  # interesting about the above case is that bare bones TriBITS does not
  # really do anything special when setting TPL_ENABLE_CUDA=ON.  All of the
  # special logic for CUDA is handled in Kokkos and Trilinos with the usage of
  # nvcc_wrapper.)


tribits_add_advanced_test( TribitsHelloWorld_no_ctest_resources_file
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Configure without any ctest resources file, just enable CUDA"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DTribitsHelloWorld_ENABLE_TESTS=ON
      -DTPL_ENABLE_CUDA=ON
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL
      "Configuring done"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_1
    MESSAGE "Check that the CTestTestfile.cmake file has the right entries"
    CMND cat
    ARGS hello_world/CTestTestfile.cmake
    PASS_REGULAR_EXPRESSION_ALL
      "ENVIRONMENT .CTEST_KOKKOS_DEVICE_TYPE=gpus."
      "PROCESSORS .1."
      "RESOURCE_GROUPS .1,gpus:1."
    ALWAYS_FAIL_ON_NONZERO_RETURN

  )
  # NOTE: The above test ensures that the RESOURCE_GROUPS test property gets
  # set correctly in a CUDA build of a project, regardless if a ctest
  # resources file gets set or not.  (The user can always explicitly pass in a
  # ctest resources file later directly to ctest.)


tribits_add_advanced_test( TribitsHelloWorld_set_other_ctest_resources_file_autogen_on
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Copy in pre-created ctest resource file to make sure it is not overwritten"
    CMND cp
    ARGS "${CMAKE_CURRENT_SOURCE_DIR}/ctest_resources/other_ctest_resources.json" .

  TEST_1
    MESSAGE "Configure pointing to non-default ctest resource file with autogenerate turned on "
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DTribitsHelloWorld_ENABLE_TESTS=ON
      -DTribitsHelloWorld_AUTOGENERATE_TEST_RESOURCE_FILE=ON
      -DTribitsHelloWorld_CUDA_NUM_GPUS=2        # Will not get used!
      -DTribitsHelloWorld_CUDA_SLOTS_PER_GPU=3   # Will not get used!
      -DTPL_ENABLE_CUDA=ON
      -DCTEST_RESOURCE_SPEC_FILE=other_ctest_resources.json
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL
      "NOTE: The test resource file CTEST_RESOURCE_SPEC_FILE='.*/TriBITS_TribitsHelloWorld_set_other_ctest_resources_file_autogen_on/other_ctest_resources[.]json' will not be auto-generated even through TribitsHelloWorld_AUTOGENERATE_TEST_RESOURCE_FILE=ON because its location does not match the default location '.*/TriBITS_TribitsHelloWorld_set_other_ctest_resources_file_autogen_on/ctest_resources[.]json'[.]  If you want to auto-generate this file, please clear CTEST_RESOURCE_SPEC_FILE and reconfigure or create that file on your own and clear TribitsHelloWorld_AUTOGENERATE_TEST_RESOURCE_FILE[.]"
      "Configuring done"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_2
    MESSAGE "Check that the pre-existing other_ctest_resources.json file was not overwritten"
    CMND diff
    ARGS "${CMAKE_CURRENT_SOURCE_DIR}/ctest_resources/other_ctest_resources.json"
      other_ctest_resources.json

  TEST_3
    MESSAGE "Check that default ctest_resources.json file is not generated"
    CMND ls
    ARGS ctest_resources.json
    WILL_FAIL

  )
  # NOTE: The above test ensures that when CTEST_RESOURCE_SPEC_FILE is set to
  # the non-default location then the autogenerate feature will not overwrite
  # the file and will not generate the default ctest_resources.json file.


tribits_add_advanced_test( TribitsHelloWorld_set_default_ctest_resources_file_autogen_on
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Copy in pre-created ctest resource file into the default autogen name and location"
    CMND cp
    ARGS "${CMAKE_CURRENT_SOURCE_DIR}/ctest_resources/other_ctest_resources.json"
      ctest_resources.json

  TEST_1
    MESSAGE "Configure pointing to default ctest resource file with autogenerate turned on"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DTribitsHelloWorld_ENABLE_TESTS=ON
      -DTribitsHelloWorld_AUTOGENERATE_TEST_RESOURCE_FILE=ON
      -DTribitsHelloWorld_CUDA_NUM_GPUS=2
      -DTribitsHelloWorld_CUDA_SLOTS_PER_GPU=3
      -DTPL_ENABLE_CUDA=ON
      -DCTEST_RESOURCE_SPEC_FILE=ctest_resources.json
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL
      "Configuring done"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_2
    MESSAGE "Check that the pre-existing ctest_resources.json file **was** overwritten"
    CMND diff
    ARGS "${CMAKE_CURRENT_SOURCE_DIR}/ctest_resources/ctest_resources.autogen_2_3_.json"
      ctest_resources.json

  )
  # NOTE: The above test shows that if the user happens to provide a ctest
  # resources file of the same name and the same location as the default
  # generated file, then TriBITS will actually overwrite it if the
  # autogenerate option is enabled!


tribits_add_advanced_test( TribitsHelloWorld_set_default_ctest_resources_file_autogen_off
  OVERALL_WORKING_DIRECTORY TEST_NAME
  OVERALL_NUM_MPI_PROCS 1

  TEST_0
    MESSAGE "Copy in pre-created ctest resource file into the default autogen name and location"
    CMND cp
    ARGS "${CMAKE_CURRENT_SOURCE_DIR}/ctest_resources/other_ctest_resources.json"
      ctest_resources.json

  TEST_1
    MESSAGE "Configure pointing to default ctest resource file with autogenerate turned off"
    CMND ${CMAKE_COMMAND}
    ARGS
      ${TribitsHelloWorld_COMMON_CONFIG_ARGS}
      -DTribitsHelloWorld_ENABLE_TESTS=ON
      -DTribitsHelloWorld_CUDA_NUM_GPUS=2
      -DTribitsHelloWorld_CUDA_SLOTS_PER_GPU=3
      -DTPL_ENABLE_CUDA=ON
      -DCTEST_RESOURCE_SPEC_FILE=ctest_resources.json
      ${${PROJECT_NAME}_TRIBITS_DIR}/examples/TribitsHelloWorld
    PASS_REGULAR_EXPRESSION_ALL
      "Configuring done"
    ALWAYS_FAIL_ON_NONZERO_RETURN

  TEST_2
    MESSAGE "Check that the pre-existing ctest_resources.json file was **not** overwritten"
    CMND diff
    ARGS "${CMAKE_CURRENT_SOURCE_DIR}/ctest_resources/other_ctest_resources.json"
      ctest_resources.json

  )
  # NOTE: The above test shows that if the user happens to provide a ctest
  # resources file of the same name and the same location as the default
  # generated file, then TriBITS will not overwrite that file if the
  # autgenerate option is disabled.
