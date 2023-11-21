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
output_file="centralizedout_3_5.txt"

for size_exponent in {7..13}; do
    problem_size=$((2**size_exponent))
    echo "Running for problem size: $problem_size"
    srun ./test $problem_size $problem_size >> $output_file
done
