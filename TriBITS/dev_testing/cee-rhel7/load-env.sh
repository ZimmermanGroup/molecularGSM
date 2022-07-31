# Env for developing and testing TriBITS on CEE RHEL7 machines

module purge

module load sierra-devel/gcc-7.2.0-openmpi-4.0.3

module unload sierra-cmake/3.12.2
module unload sierra-git/2.6.1

module load sems-env
module load sems-ninja_fortran/1.10.0
module load sems-cmake/3.17.1
module load sems-git/2.10.1

# Make default install permissions 700 so that we can test that TriBITS will
# use recursive chmod to open up permissions.
umask g-rwx,o-rwx

export TribitsExMetaProj_GIT_URL_REPO_BASE=git@github.com:tribits/
