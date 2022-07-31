add_subdirectory(core)

if (PYTHON_EXECUTABLE)
  add_subdirectories(
    python_utils
    ci_support
    ctest_driver
    devtools_install
    doc
    )
endif()
