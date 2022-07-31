TriBITS/ Directory Contents
+++++++++++++++++++++++++++

This base directory for TriBITS acts as a TriBITS Project, a TriBITS
Repository, and a TriBITS Package.  As such, it contains the standard files
that are found in a TriBITS Project, Repository, and Package::

  ProjectName.cmake   # PROJECT_NAME=TriBITS
  CMakeLists.txt      # PROJECT_NAME = PACKAGE_NAME = TriBITS
  PackagesList.cmake  # Lists just "TriBITS . PT"
  TPLsList.cmake      # Lists only MPI
  cmake/              # Dependencies.cmake, etc.

The core functionality of TriBITS is provided in the following directory, 'tribits'/:

**tribits/**: The part of TriBITS that CMake projects use to access TriBITS
functionality and assimilate into the TriBITS framework.  It also contains
basic documentation and examples.  Files and directories from here are what
get installed on the system or are snapshotted into
``<projectDir>/cmake/tribits/``.  Each TriBITS Project decides what parts of
it wants to install or shapshot using the script
``tribits/snapshot_tribits.py`` (which takes arguments for what dirs to
snapshot, see below). This directory contains no tests at all. All of the
tests for TriBITS are in the ``test/`` directory (see below). The breakdown of
the contents of ``tribits/`` are described in the file
``tribits/README.DIRECTORY_CONTENTS.rst``.

The following directories are not snapshotted into
``<projectDir>/cmake/tribits/`` by the script ``tribits/snapshot_tribits.py``:

**test/**: Contains all of the automated tests for TriBITS as part of the
TriBITS "TriBITS" package. When doing development, these tests are critical.

**dev_testing/:** Contains scripts that support the development of the TriBITS
system itself in various contexts.

**common_tools/:** Contains misc utilities that are not central to the TriBITS
system but are very helpful to keep around and do not take up too much space.

**refactoring/:** Some scripts and other files that have aided in various
refactorings of TriBITS and are used to upgrade client TriBITS projects.
