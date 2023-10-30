import numpy as np
import matplotlib.pyplot as plt

data = """N;2;Runtime;0.588079;ms;Roundtrip;0.588079;ms;Average;294.039500;us
N;4;Runtime;1.184967;ms;Roundtrip;1.184967;ms;Average;296.241750;us
N;6;Runtime;2.183421;ms;Roundtrip;2.183421;ms;Average;363.903500;us
N;8;Runtime;2.784112;ms;Roundtrip;2.784112;ms;Average;348.014000;us
N;10;Runtime;104.488177;ms;Roundtrip;104.488177;ms;Average;10448.817700;us
N;12;Runtime;97.703638;ms;Roundtrip;97.703638;ms;Average;8141.969833;us
N;14;Runtime;100.221876;ms;Roundtrip;100.221876;ms;Average;7158.705429;us
N;16;Runtime;108.087525;ms;Roundtrip;108.087525;ms;Average;6755.470312;us
N;18;Runtime;150.884749;ms;Roundtrip;150.884749;ms;Average;8382.486056;us
N;20;Runtime;187.796055;ms;Roundtrip;187.796055;ms;Average;9389.802750;us
N;24;Runtime;195.186736;ms;Roundtrip;195.186736;ms;Average;8132.780667;us"""

# Split the data into lines and parse values
lines = data.split('\n')
n_values = [int(line.split(';')[1]) for line in lines]
average_values = [float(line.split(';')[-2][:-3]) for line in lines]

# Create a plot
plt.figure(figsize=(10, 6))
plt.plot(n_values, average_values, marker='o', linestyle='-')
plt.title('Average Execution Time vs. N')
plt.xlabel('N [Threads]')
plt.ylabel('Average Execution Time [us]')
plt.grid(True)

# Show the plot
plt.show()