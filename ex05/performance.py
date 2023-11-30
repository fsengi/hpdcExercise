import matplotlib.pyplot as plt

# Read data from data.txt
with open('data.txt', 'r') as file:
    lines = file.readlines()

# Extract data
grid_sizes = []
gflops = []

for line in lines[1:]:  # Skip the header line
    parts = line.split()
    grid_size = parts[0]
    gflop_per_s = float(parts[-1])
    grid_sizes.append(grid_size)
    gflops.append(gflop_per_s)

# Create a line chart with points marked as 'o'
plt.plot(grid_sizes, gflops, marker='o', color='blue', label='GFLOP/s')
plt.xlabel('Grid Size')
plt.ylabel('GFLOP/s')
plt.title('Comparison of GFLOP/s for Different Grid Sizes')
plt.grid(True)
plt.legend()

# Display the values next to the points
for i, (size, gflop) in enumerate(zip(grid_sizes, gflops)):
    plt.text(size, gflop, f'{gflop:.3f}', ha='left', va='bottom', fontsize=8)

# Show the plot
plt.show()

plt.savefig("performance.png")
