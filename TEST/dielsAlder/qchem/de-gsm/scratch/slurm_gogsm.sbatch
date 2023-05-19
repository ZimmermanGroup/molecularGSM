#!/bin/bash
#SBATCH --array=1
#SBATCH -N 1 --ntasks-per-node=12 --time=3-00:00:00
#SBATCH -e ssm.qsh.err -o ssm.qsh.out
#SBATCH -A zimmerman
#SBATCH -p zim2080 --job-name=test_gsm

item=$SLURM_ARRAY_TASK_ID
ID=`printf "%0*d\n" 4 ${item}`

hostname
module load qchem/feb2019/env_intel_2018_parallel

export OMP_NUM_THREADS=4

./gsm $item 6 > scratch/paragsm$ID

rm $QCSCRATCH
