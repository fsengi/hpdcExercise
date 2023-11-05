import numpy as np
import matplotlib.pyplot as plt

# Load the data from the file
data_file = "D:\Master\HPDC\hpdcExercise\ex02\Florian\data.txt"
with open(data_file, "r") as file:
    data = file.readlines()

# Initialize lists to store data
n_values_10000 = []
own_barrier_runtimes_10000 = []
mpi_barrier_runtimes_10000 = []

n_values_100000 = []
own_barrier_runtimes_100000 = []
mpi_barrier_runtimes_100000 = []

n_values_1000000 = []
own_barrier_runtimes_1000000 = []
mpi_barrier_runtimes_1000000 = []

# Parse the data from the file
for line in data:
    parts = line.strip().split(";")
    if len(parts) < 8:
        continue
    n = int(parts[1])
    own_barrier_runtime = float(parts[3])
    mpi_barrier_runtime = float(parts[6]) 
    iterations = int(parts[9])
    
    if iterations == 10000:
        n_values_10000.append(n)
        own_barrier_runtimes_10000.append(own_barrier_runtime)
        mpi_barrier_runtimes_10000.append(mpi_barrier_runtime)
    elif iterations == 100000:
        n_values_100000.append(n)
        own_barrier_runtimes_100000.append(own_barrier_runtime)
        mpi_barrier_runtimes_100000.append(mpi_barrier_runtime)
    elif iterations == 1000000:
        n_values_1000000.append(n)
        own_barrier_runtimes_1000000.append(own_barrier_runtime)
        mpi_barrier_runtimes_1000000.append(mpi_barrier_runtime)

# Plot data
plt.figure(figsize=(10, 6))

plt.scatter(n_values_10000, own_barrier_runtimes_10000, label='own Barrier (10000)', color='blue', marker='o')
plt.scatter(n_values_10000, mpi_barrier_runtimes_10000, label='MPI Barrier (10000)', color='red', marker='x')

plt.scatter(n_values_100000, own_barrier_runtimes_100000, label='own Barrier (100000)', color='green', marker='o')
plt.scatter(n_values_100000, mpi_barrier_runtimes_100000, label='MPI Barrier (100000)', color='yellow', marker='x')

plt.scatter(n_values_1000000, own_barrier_runtimes_1000000, label='own Barrier (1000000)', color='black', marker='o')
plt.scatter(n_values_1000000, mpi_barrier_runtimes_1000000, label='MPI Barrier (1000000)', color='pink', marker='x')

plt.xlabel("N Values")
plt.ylabel("Runtimes (us)")
plt.title("Runtimes for Barrier and own Barrier at Different Iterations")
plt.legend()
plt.grid(True)

plt.show()
