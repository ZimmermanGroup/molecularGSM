.. image:: https://bestpractices.coreinfrastructure.org/projects/4839/badge
  :target: https://bestpractices.coreinfrastructure.org/projects/4839
  :alt: CII Best Practices

=================================================
TriBITS: Tribal Build, Integrate, and Test System
=================================================

The Tribal Build, Integrate, and Test System (TriBITS) is a framework designed
to handle large software development projects involving multiple independent
development teams and multiple source repositories which is built on top of
the open-source CMake set of tools.  TriBITS also defines a complete software
development, testing, and deployment system supporting processes consistent
with modern agile software development best practices.

Documentation
=============

See `TriBITS Documentation on tribits.org <http://tribits.org>`_

Developing on TriBITS
=====================

In order to make changes and enhancements to TriBITS (see `Contributing to
TriBITS`_ and the role `TriBITS System Developer`_), one must be able to
build, run, and extend the automated TriBITS test suite.  To develop on
TriBITS, one must minimally have CMake 3.17.0 (or newer) and a working C and
C++ compiler.  (A Fortran compiler is also desired to test Fortran-specific
features of TriBITS but it can be disabled, see below).

To set up to develop on TriBITS:

1) Clone the TriBITS repository

  ::

    $ cd <some-base-dir>/
    $ git clone git@github.com:TriBITSPub/TriBITS.git
  
2) Create and set up a build/test directory

  ::

    $ cd <some-base-dir>/
    $ mkdir BUILD
    $ cd BUILD/
    $ ln -s ../TriBITS/dev_testing/generic/do-configure-serial-debug-gcc \
      do-configure

  NOTE: Other do-configure scripts are also in that directory (e.g. for MPI).

3) Configure, build and run the TriBITS test suite

  ::

    $ ./do-configure
    $ make
    $ ctest -j12

NOTES:

* If you don't have a working and compatible Fortran compiler, then pass
  ``-DTriBITS_ENABLE_Fortran=OFF`` into the ``do-configure`` script as::

    $ ./do-configure -DTriBITS_ENABLE_Fortran=OFF

* On Mac OSX systems, one has to manually set the path the the TriBITS
  project base dir TRIBITS_BASE_DIR such as with::

    $ env TRIBITS_BASE_DIR=.. ./do-configure [other options]

* Use as many processes as you have with ``ctest`` (``-j12`` is just used as
  an example).

* All of the tests should pass on your machine before beginning any
  development work.  If there are any failures, then please `report them`_.
  To help show the failures you are seeing, do::

  $ ./do-configure -DCTEST_PARALLEL_LEVEL=12
  $ make dashboard

  and then provide the link to the CDash results in the TriBITS Issue when
  you report them.

Any change (refactoring) of TriBITS (minimally) requires that the automated
test suite run with ``ctest`` pass 100%.  To add new features (in most cases)
new automated tests must be added to define and protect those features (again,
see `Contributing to TriBITS`_).

.. References:

.. _Contributing to TriBITS: https://github.com/TriBITSPub/TriBITS/wiki/Contributing-to-TriBITS

.. _Report them: https://github.com/TriBITSPub/TriBITS/issues

.. _TriBITS System Developer: https://tribits.org/doc/TribitsMaintainersGuide.html#tribits-system-developer
