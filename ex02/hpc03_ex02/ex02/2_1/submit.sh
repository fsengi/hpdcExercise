#!/bin/bash

## Resource Request
#SBATCH --job-name=HPDC03_job
#SBATCH --partition=exercise_hpc   # partition (queue)
#SBATCH -t 0-0:10              # time limit: (D-HH:MM) 
#SBATCH --nodes=1              # number of nodes
#SBATCH --ntasks-per-node=2    # number of cores
#SBATCH --output=slurm.out     # file to collect standard output
#SBATCH --error=slurm.err      # file to collect standard errors


module load devtoolset/10 mpi/open-mpi-4.0.5
mpirun ./test
# srun --ntasks=1 --nodelist= octane05,octane06,octane07,octane08 ./test
# mpirun -host octane05,octane06,octane07,octane08  -np 4 ./test
