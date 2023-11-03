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

# Compile the MPI program (assuming your program is named ping_pong_latency.c)
mpicc ping_pong_latency.c -o ping_pong_latency

# Define the message sizes to test
MESSAGE_SIZES="1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576"

for size in $MESSAGE_SIZES
do
    # Execute on one node
    echo "Running on one node with message size $size bytes"
    srun --ntasks=2 ./ping_pong_latency $size
done

## Resource Request for Two Nodes
#SBATCH --job-name=PingPong_TwoNodes
#SBATCH --partition=exercise_hpc   # partition (queue)
#SBATCH -t 0-2:00              # time limit: (D-HH:MM) 
#SBATCH --nodes=2              # number of nodes
#SBATCH --ntasks-per-node=1    # one process per node for two nodes
#SBATCH --output=two_nodes.out     # file to collect standard output
#SBATCH --error=two_nodes.err      # file to collect standard errors

# Define the message sizes to test (you can modify or reuse MESSAGE_SIZES)

for size in $MESSAGE_SIZES
do
    # Execute on two nodes
    echo "Running on two nodes with message size $size bytes"
    srun --ntasks=2 -w ceg-brook01,ceg-brook02 ./ping_pong_latency $size
done
