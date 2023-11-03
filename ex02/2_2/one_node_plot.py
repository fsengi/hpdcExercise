import matplotlib.pyplot as plt

# Data
message_sizes = [
    1024,
    2048,
    4096,
    8192,
    16384,
    32768,
    65536,
    131072,
    262144,
    524288,
    1048576,
]
latencies = [
    0.752662,
    0.887380,
    1.877909,
    2.387698,
    4.290904,
    7.044059,
    10.914113,
    18.529811,
    34.439753,
    66.797563,
    129.968422,
]

# Create the plot
plt.figure(figsize=(10, 5))
plt.plot(
    message_sizes, latencies, marker="o", color="b", label="Latency (microseconds)"
)
plt.title("Latency vs. Message Size for one node with two processes")
plt.xlabel("Message Size (bytes)")
plt.ylabel("Latency (microseconds)")
plt.xscale("log")  # Set x-axis to logarithmic scale for better visualization
plt.grid(True, which="both", ls="--", c="gray")  # Add gridlines
plt.legend()
plt.savefig("one_node_plot.png")
