#!/bin/bash

## Resource Request for One Node
#SBATCH --job-name=PingPong_OneNode
#SBATCH --partition=exercise_hpc   # partition (queue)
#SBATCH -t 0-1:00              # time limit: (D-HH:MM) 
#SBATCH --nodes=1              # number of nodes
#SBATCH --ntasks-per-node=2    # number of processes
#SBATCH --output=one_node.out     # file to collect standard output
#SBATCH --error=one_node.err      # file to collect standard errors

module load devtoolset/10 mpi/open-mpi-4.0.5

# Define the message sizes to test
MESSAGE_SIZES="1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576"

for size in $MESSAGE_SIZES
do
    # Execute on one node
    echo "Running on one node with message size $size bytes"
    srun --ntasks=2 ./latency_test $size
done