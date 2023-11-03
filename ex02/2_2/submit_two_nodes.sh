#!/bin/bash

## Resource Request for Two Nodes
#SBATCH --job-name=PingPong_TwoNodes
#SBATCH --partition=exercise_hpc   # partition (queue)
#SBATCH -t 0-2:00              # time limit: (D-HH:MM) 
#SBATCH --nodes=2              # number of nodes
#SBATCH --ntasks-per-node=1    # one process per node for two nodes
#SBATCH --output=two_nodes.out     # file to collect standard output
#SBATCH --error=two_nodes.err      # file to collect standard errors

module load devtoolset/10 mpi/open-mpi-4.0.5

# Define the message sizes to test
MESSAGE_SIZES="1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576"

for size in $MESSAGE_SIZES
do
    # Execute on two nodes
    echo "Running on two nodes with message size $size bytes"
    srun -p=exercise_hpc -w=octane006,octane007 ./latency_test $size
done
