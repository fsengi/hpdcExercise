#!/bin/bash

#SBATCH --job-name=mmul_parallel
#SBATCH --partition=exercise_hpc
#SBATCH -t 0-00:10
#SBATCH --output=mmul.out
#SBATCH --error=mmul.err
#SBATCH --ntasks=10
#SBATCH --ntasks-per-node=10

# Load necessary modules
module load devtoolset/10 mpi/open-mpi-4.0.5

srun ./test 2000 2000 >> centralizedout_3_4.txt
