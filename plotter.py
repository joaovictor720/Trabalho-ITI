import sys
import numpy as np
import matplotlib.pyplot as plt

def plot_data(file_path, subsample_factor):
    # Read the floating point values from the file
    with open(file_path, 'r') as file:
        data = [float(line.strip()) for line in file]
    
    # Apply subsampling (take every nth point)
    subsampled_data = data[::subsample_factor]
    
    # Plot the subsampled data
    plt.figure(figsize=(10, 5))
    plt.plot(subsampled_data, marker='o', linestyle='-', markersize=2, label=f"Subsample factor: {subsample_factor}")
    plt.title(f"Plot of {file_path}")
    plt.xlabel("Index (Subsampled)")
    plt.ylabel("Mean length (encoded bits / original byte)")
    plt.grid(True)
    plt.legend()
    
    # Save the plot or show it
    # plt.savefig(f"subsampled_plot.png")
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python plot_floats.py <file_path> <subsample_factor>")
    else:
        file_path = sys.argv[1]
        subsample_factor = int(sys.argv[2])
        plot_data(file_path, subsample_factor)
