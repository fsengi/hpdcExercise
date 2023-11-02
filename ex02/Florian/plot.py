import numpy as np
import matplotlib.pyplot as plt

# Load the data from the file
data_file = "data.txt"
with open(data_file, "r") as file:
    data = file.readlines()

# Initialize lists to store data
n_values = []
own_barrier_runtimes = []
mpi_barrier_runtimes = []

# Parse the data from the file
for line in data:
    parts = line.strip().split(";")
    if len(parts) < 8:
        continue
    n = int(parts[1])
    own_barrier_runtime = float(parts[3])
    mpi_barrier_runtime = float(parts[5][:-2])  # Remove "us"
    iterations = int(parts[7])
    
    if iterations == 10000 or iterations == 100000 or iterations == 1000000:
        n_values.append(n)
        own_barrier_runtimes.append(own_barrier_runtime)
        mpi_barrier_runtimes.append(mpi_barrier_runtime)

# Separate data for different iterations
iteration_10000 = [i for i, it in enumerate(iterations) if it == 10000]
iteration_100000 = [i for i, it in enumerate(iterations) if it == 100000]
iteration_1000000 = [i for i, it in enumerate(iterations) if it == 1000000]

# Plot data
plt.figure(figsize=(10, 6))

plt.scatter(np.array(n_values)[iteration_10000], np.array(own_barrier_runtimes)[iteration_10000], label="own Barrier (10000)")
plt.scatter(np.array(n_values)[iteration_10000], np.array(mpi_barrier_runtimes)[iteration_10000], label="MPI Barrier (10000)")

plt.scatter(np.array(n_values)[iteration_100000], np.array(own_barrier_runtimes)[iteration_100000], label="own Barrier (100000)")
plt.scatter(np.array(n_values)[iteration_100000], np.array(mpi_barrier_runtimes)[iteration_100000], label="MPI Barrier (100000)")

plt.scatter(np.array(n_values)[iteration_1000000], np.array(own_barrier_runtimes)[iteration_1000000], label="own Barrier (1000000)")
plt.scatter(np.array(n_values)[iteration_1000000], np.array(mpi_barrier_runtimes)[iteration_1000000], label="MPI Barrier (1000000)")

plt.xlabel("N Values")
plt.ylabel("Runtimes (us)")
plt.title("Runtimes for Barrier and own Barrier at Different Iterations")
plt.legend()
plt.grid(True)

plt.show()
