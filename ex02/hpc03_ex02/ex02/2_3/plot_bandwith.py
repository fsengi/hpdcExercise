import re
import matplotlib.pyplot as plt

def parse_log_file(log_file):
    message_sizes = []
    bandwidths = []
    with open(log_file, 'r') as file:
        lines = file.readlines()
        for line in lines:
            if re.match(r'Message size: (\d+) bytes, Bandwidth: (\d+\.\d+) MB/s', line):
                match = re.match(r'Message size: (\d+) bytes, Bandwidth: (\d+\.\d+) MB/s', line)
                size = int(match.group(1))
                bandwidth = float(match.group(2))
                message_sizes.append(size)
                bandwidths.append(bandwidth)
    return message_sizes, bandwidths

# Create a single figure and axes for all the graphs
fig, ax = plt.subplots()

# Define log file patterns and titles
log_files = [
    "one_node_blocking.out",
    "two_nodes_blocking.out",
    "one_node_nonBlocking.out",
    "two_nodes_nonBlocking.out",
]

titles = [
    "One Node Blocking",
    "Two Nodes Blocking",
    "One Node Non-Blocking",
    "Two Nodes Non-Blocking",
]

# Plot all four graphs on the same set of axes
for log_file, title in zip(log_files, titles):
    message_sizes, bandwidths = parse_log_file(log_file)
    message_sizes = [message_size / 1024 for message_size in message_sizes]
    ax.plot(message_sizes, bandwidths, marker='o', label=title)

# Set labels and title for the single plot
ax.set_xlabel('Message Size (KB)')
ax.set_ylabel('Bandwidth (MB/s)')
ax.set_title('Bandwidth Comparison')
ax.legend()
ax.grid()

# Save and display the single plot
plt.savefig("bandwidth_comparison.png")
plt.show()
