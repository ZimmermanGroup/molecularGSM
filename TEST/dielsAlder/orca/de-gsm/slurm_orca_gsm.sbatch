#!/bin/bash
#SBATCH --array=1
#SBATCH --job-name=orcagsm
#SBATCH --time=3-00:00:00
#SBATCH --nodes=1 --ntasks=4
#SBATCH -o orca.output -e orac.error -p guest
##SBATCH --mail-type=END --mail-user=

time

. /etc/profile.d/slurm.sh

item=$SLURM_ARRAY_TASK_ID
ID=`printf "%0*d\n" 4 ${item}`
module unload Openmpi
shtcut="/export/applications"

export LD_LIBRARY_PATH=/export/apps/Intel/composer_xe_2013.4.183/compiler/lib/intel64:/export/zimmerman/khyungju/OpenMPI/2.0.2/lib:$LD_LIBRARY_PATH
export PATH=//export/zimmerman/khyungju/OpenMPI/2.0.2/bin:$PATH
export OMP_NUM_THREADS=1
./gsm.orca.exe ${item} 4 > scratch/paragsm$ID


wait






