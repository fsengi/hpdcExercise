#!/bin/sh

#SBATCH --job-name=nBody
#SBATCH --partition=exercise_hpc   # partition (queue)
#SBATCH -t 0-1:00             # time limit: (D-HH:MM)
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=12
#SBATCH --output=nBody.out     # file to collect standard output
#SBATCH --error=nBody.err      # file to collect standard errors


# 2 4 8 12 24 48 64
nodes=(1 1 1 1 2 3 4 5 6)
tasks_per_node=(2 4 8 12 24 36 48 60 72)

rm finaldata.txt
rm data1.txt

length=${#nodes[@]}
# module load devtoolset/10 mpi/open-mpi-4.1.6
mpicc -o bin/nBody nBody.c -lm

for (( j = 0; j < length; j++ )) ; do
        for (( i = 128; i <= 32768; i*=2 )) ; do
                echo "${tasks_per_node[$j]} Processes,${nodes[$j]} Nodes, $i Bodys" >> data1.txt
                srun --partition=exercise_hpc --nodes=${nodes[$j]} --ntasks=${tasks_per_node[$j]} ./bin/nBody $i 100 >> finaldata.txt
        done
done 
