#!/bin/bash

## Resource Request for One Node
#SBATCH --job-name=mmul_sequential
#SBATCH --partition=exercise_hpc   # partition (queue)
#SBATCH -t 0-01:00              # time limit: (D-HH:MM) 
#SBATCH --nodes=1              # number of nodes
#SBATCH --ntasks-per-node=2
#SBATCH --output=mmul.out     # file to collect standard output
#SBATCH --error=mmul.err      # file to collect standard errors


module load devtoolset/10 mpi/open-mpi-4.0.5

srun ./test 2000