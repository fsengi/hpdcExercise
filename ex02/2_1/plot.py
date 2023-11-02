import numpy as np
import matplotlib.pyplot as plt

data_file = "D:\\Master\\HPDC\hpdcExercise\\ex02\\2_1\\data.txt"
with open(data_file, "r") as file:
    data = file.readlines()

# Split the data into lines and parse values
n_values = [int(line.split(';')[1]) for line in data]
average_values = [float(line.split(';')[-2][:-3]) for line in data]

# Create a plot
plt.figure(figsize=(10, 6))
plt.plot(n_values, average_values, marker='o', linestyle='-')
plt.title('Average Execution Time vs. N')
plt.xlabel('N [Threads]')
plt.ylabel('Average Execution Time [us]')
plt.grid(True)

# Show the plot
plt.show()