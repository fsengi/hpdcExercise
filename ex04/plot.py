import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from PIL import Image
import os

# Function to parse the data and create a heatmap
def plot_heatmap(ax, data, index):
    ax.clear()
    ax.imshow(data, cmap='hot', interpolation='nearest', origin='upper')
    ax.set_title(f"Heatmap {index}")
    ax.set_xlabel('Column')
    ax.set_ylabel('Row')

# Read data from the file
with open('data.txt', 'r') as file:
    # Split the file into blocks based on empty lines
    blocks = file.read().split('\n\n')

    # Create the figure and axis
    fig, ax = plt.subplots()

    # Function to update the plot in the animation
    def update(frame):
        if frame < len(blocks):
            block = blocks[frame]
            lines = block.strip().split('\n')
            data = [[float(value) for value in line.split(';') if value] for line in lines]
            data_array = np.array(data)
            plot_heatmap(ax, data_array, frame)

            # Save each frame as an image
            plt.savefig(f'frame_{frame:03d}.png', bbox_inches='tight', pad_inches=0.1)

    # Create the animation
    animation = FuncAnimation(fig, update, frames=len(blocks), interval=10, repeat=False)

    # Save animation as GIF using Pillow
    animation.save('heatmap_animation.gif', writer='pillow', fps=30)

# Clean up: remove temporary image files
for frame in range(len(blocks)):
    filename = f'frame_{frame:03d}.png'
    try:
        # Attempt to remove the file
        os.remove(filename)
    except OSError:
        pass
