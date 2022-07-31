# Env for testing TriBITS on CEE machines using default GCC and Python on
# RHEL7

module purge
module load sems-git/2.29.0
module load cde/v1/cmake/3.17.1
module load cde/v2/ninja/1.10.1

# Extra stuff for TriBITS

# Make default install permissions 700 so that we can test that TriBITS will
# use recursive chmod to open up permissions.
umask g-rwx,o-rwx

export TribitsExMetaProj_GIT_URL_REPO_BASE=git@github.com:tribits/
