#! /bin/bash

#SBATCH --job-name=test_run     # job name, "OMP_run"
#SBATCH --partition=exercise_hpc   # partition (queue)
#SBATCH -t 0-0:10              # time limit: (D-HH:MM) 
#SBATCH --nodes=4              # number of nodes
#SBATCH --ntasks-per-node=6 --ntasks=24
#SBATCH -o out/%A_%a
#SBATCH --array 2-24:2
#SBATCH --output=slurm.out     # file to collect standard output
#SBATCH --error=slurm.err      # file to collect standard errors




for nmsg in 100000, 1000000, 10000000
do
    module load mpi/open-mpi-4.0.5
    mpirun -host octane001, octane002, octane003, octane004 -np ${SLURM_ARRAY_TASK_ID} ex02/message_send_receive_time.out -n $nmsg
done