import matplotlib.pyplot as plt

# Data for one node
message_sizes_one_node = [
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
latencies_one_node = [
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

# Data for two nodes
message_sizes_two_nodes = [
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
latencies_two_nodes = [
    104.063767,
    81.249979,
    130.756240,
    229.239296,
    242.995722,
    424.626317,
    916.556483,
    1497.315557,
    2468.346659,
    4761.808799,
    9912.887564,
]

# Create the plot
plt.figure(figsize=(10, 5))

# Plot for one node
plt.plot(
    message_sizes_one_node,
    latencies_one_node,
    marker="o",
    color="b",
    label="One Node Two Processes",
)

# Plot for two nodes
plt.plot(
    message_sizes_two_nodes,
    latencies_two_nodes,
    marker="x",
    color="r",
    label="Two Nodes One Process",
)

plt.title("Latency vs. Message Size")
plt.xlabel("Message Size (bytes)")
plt.ylabel("Latency (microseconds)")
plt.xscale("log")
plt.grid(True, which="both", ls="--", c="gray")
plt.legend()
plt.savefig("latency_test.png")
