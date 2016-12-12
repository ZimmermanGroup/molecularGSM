#!/bin/bash
##This file was created on 12/12/2016
#this is a pbs script for the FLUX HPC, change accordingly


####  PBS preamble
#PBS -N test
#PBS -M craldaz@umich.edu
#PBS -m abe

# Change the number of cores (ppn=1), amount of memory, and walltime:
#PBS -l nodes=1:ppn=1,mem=2000mb,walltime=00:30:20
#PBS -j oe
#PBS -V

# Change "example_flux" to the name of your Flux allocation:
#PBS -A ners590f16_fluxod 
#PBS -q fluxod
#PBS -l qos=flux

####  End PBS preamble

#  Show list of CPUs you ran on, if you're running under PBS
if [ -n "$PBS_NODEFILE" ]; then cat $PBS_NODEFILE; fi

#  Change to the directory you submitted from
if [ -n "$PBS_O_WORKDIR" ]; then cd $PBS_O_WORKDIR; fi

#  Put your job commands here:
echo "Hello, world"
module load intel/14.0.2
#module load cmake
#alias mopac='~/opt/mopac/MOPAC2016.exe'
#export LD_LIBRARY_PATH=~/opt/mopac/
#export MOPAC_LICENSE=~/opt/mopac/
ctest -V

