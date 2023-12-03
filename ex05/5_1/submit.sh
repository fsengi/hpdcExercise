#!/bin/bash

## Resource Request for One Node
#SBATCH --job-name=heat_par
#SBATCH --partition=exercise_hpc   # partition (queue)
#SBATCH -t 0-00:20              # time limit: (D-HH:MM) 
#SBATCH --nodes=1              # number of nodes
#SBATCH --ntasks-per-node=12
#SBATCH --output=mmul.out     # file to collect standard output
#SBATCH --error=mmul.err      # file to collect standard errors

if [ -f "data.txt" ]; then
    rm "data.txt"
fi

module load devtoolset/10 mpi/open-mpi-4.0.5

mpicc -o bin/heatparallel heat_parallel.c

tasks_per_node="2 4 6 8 10 12"
sizes="128 512 1024 2000 4000"

for size in $sizes; do
    for nTpN in $tasks_per_node; do
        echo "script size $size tpN $nTpN" >> data.txt
        srun --ntasks=$nTpN ./bin/heatparallel $size 1 0  >> data.txt
    done
done