function(set_test_disables)
  foreach (testName ${ARGN})
    set(testNameDisableVar ${testName}_DISABLE)
    message("-- Setting ${testNameDisableVar}=ON")
    set(${testNameDisableVar} ON CACHE BOOL "Set in ${CMAKE_CURRENT_LIST_FILE}")
  endforeach()
endfunction()

set_test_disables(
  )
