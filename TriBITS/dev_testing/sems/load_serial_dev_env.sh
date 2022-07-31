# This script can be sourced to load a simple env on any SEMS machine in order
# to do a basic configuration and testing of TriBITS without MPI.  For some
# reason the SEMS-built env causes a bunch of TriBITS tests to fail.

module load sems-env
module load sems-git/2.10.1
module load atdm-env
module load atdm-cmake/3.11.1
module load atdm-ninja_fortran/1.7.2
