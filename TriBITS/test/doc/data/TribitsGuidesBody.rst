This is a stand-in for the TribitsGuidesBody.rst file with indented includes.

Something something:

.. include:: ../../examples/TribitsExampleProject/ProjectName.cmake
   :literal:

4) Add custom CTest -S driver scripts.

  For driving different builds and tests, one needs to set up one or more
  CTest -S driver scripts.  There are various ways to do this but a simple
  approach that avoids duplication is to first create a file like
  ``TribitsExampleProject/cmake/ctest/TribitsExProjCTestDriver.cmake``:

  .. include:: ../../examples/TribitsExampleProject/cmake/ctest/TribitsExProjCTestDriver.cmake
     :literal:

  and then create a set of CTest -S driver scripts that uses that file.  One
  example is the file
  ``TribitsExampleProject/cmake/ctest/general_gcc/ctest_serial_debug.cmake``:

  .. include:: ../../examples/TribitsExampleProject/cmake/ctest/general_gcc/ctest_serial_debug.cmake
     :literal:
