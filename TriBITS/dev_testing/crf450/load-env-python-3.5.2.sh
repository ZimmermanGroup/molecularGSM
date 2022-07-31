# Env for testing TriBITS on crf450 with Python 3.5.2

module purge

export PATH=${PATH_ORIG}

# From ~/load_dev_env.sh

source ~/load_vera_dev_env.gcc-4.8.3.crf450.sh
export PATH=/projects/sems/install/rhel6-x86_64/atdm/utility/ninja_fortran/1.7.2/bin:$PATH
module load sems-env
module load sems-git/2.10.1
module load sems-cmake/3.17.1
module load sems-ninja_fortran/1.10.0

# Load Python 3.5.2
export PATH=$HOME/bin/python-3.5.2:$PATH
# NOTE: The above is the only way that I could figure out how to get TriBITS
# using the right version of Python for all testing.  The file
# ~/bin/python-3.5.2/python is a symlink to
# /projects/sems/install/rhel7-x86_64/sems/compiler/python/3.5.2/bin/python3.5
# This seems to work.

# Extra stuff for TriBITS

#export PATH=/home/vera_env/common_tools/cmake-3.17.0/bin:${PATH}
export PATH=/home/vera_env/common_tools/ccache-3.7.9/bin:${PATH}

# Make default install permissions 700 so that we can test that TriBITS will
# use recursive chmod to open up permissions.
umask g-rwx,o-rwx

export TribitsExMetaProj_GIT_URL_REPO_BASE=git@github.com:tribits/
