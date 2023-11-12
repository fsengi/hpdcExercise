#!/bin/bash

## Resource Request for One Node
#SBATCH --job-name=mmul_sequential
#SBATCH --partition=exercise_hpc   # partition (queue)
#SBATCH -t 0-1:00              # time limit: (D-HH:MM) 
#SBATCH --nodes=1              # number of nodes
#SBATCH --output=mmul_sequential.out     # file to collect standard output
#SBATCH --error=mmul_sequential.err      # file to collect standard errors

module load devtoolset/10 mpi/open-mpi-4.0.5

# Execute on one node
echo "Running on one node"
mpirun ./matrix_multiply_sequential 2048