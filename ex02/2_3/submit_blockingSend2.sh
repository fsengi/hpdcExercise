#!/bin/bash

#SBATCH --job-name=twoNodes_Blocking
#SBATCH --partition=exercise   # partition (queue)
#SBATCH -t 0-2:00              # time limit: (D-HH:MM) 
#SBATCH --nodes=2              # number of nodes
#SBATCH --ntasks-per-node=1    # one process per node for two nodes
#SBATCH --output=two_nodes_blocking.out     # file to collect standard output
#SBATCH --error=two_nodes_blocking.err      # file to collect standard errors
module load devtoolset/10 mpi/open-mpi-4.1.6

for ((i=1;i<=1;i++));
do
    # Execute on one node
    echo "Running iteration $i"
    srun -w ceg-brook01,ceg-brook02 ./blockingSend
done
