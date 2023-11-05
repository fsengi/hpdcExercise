#!/bin/bash

#SBATCH --job-name=NonBlockingSend1
#SBATCH --partition=exercise_hpc   # partition (queue)
#SBATCH -t 0-1:00              # time limit: (D-HH:MM) 
#SBATCH --nodes=1              # number of nodes
#SBATCH --ntasks-per-node=2    # number of processes
#SBATCH --output=one_node_blocking.out     # file to collect standard output
#SBATCH --error=one_node_blocking.err      # file to collect standard errors

module load devtoolset/10 mpi/open-mpi-4.1.6

for ((i=1;i<=1;i++));
do
    # Execute on one node
    echo "Running iteration $i"
    srun --ntasks=2 ./blockingSend $size
done