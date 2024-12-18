import os
import matplotlib.pyplot as plt
import numpy as np

# Function to read data from a file
def read_data(file_path):
    """Read numbers from a file."""
    with open(file_path, 'r') as file:
        data = [int(line.strip()) for line in file if line.strip().isdigit()]
    return data

# Function to plot box plots for multiple files with different colors and labels
def plot_box(directory, file_colors, labels):
    """Plot box plots for multiple files with specified colors and labels."""
    data = []
    colors = []
    xticks_labels = []

    # Read data from files and prepare for plotting
    for file_name, color in file_colors.items():
        file_path = os.path.join(directory, file_name)
        if not os.path.exists(file_path):
            print(f"File {file_name} does not exist.")
            continue

        file_data = read_data(file_path)
        data.append(file_data)
        colors.append(color)
        xticks_labels.append(labels.get(file_name, file_name))  # Get label or default to file name

    # Create the box plot
    plt.figure(figsize=(10, 8))
    box = plt.boxplot(data, patch_artist=True, labels=xticks_labels, notch=False, vert=True, showfliers=False)

    # Apply colors to each box
    for patch, color in zip(box['boxes'], colors):
        patch.set_facecolor(color)

    # Set plot details
    plt.title("Box Plot of Iteration Counts for Latency Jump", fontsize=20)
    plt.xlabel("Number of mov Instructions", fontsize=18)
    plt.ylabel("Iteration Count for Latency Jump", fontsize=18)
    
    # Increase axis tick size
    plt.xticks(fontsize=14)
    plt.yticks(fontsize=14)

    # Optional: Adjust gridlines if needed
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.show()

# Main function
def main():
    directory = './stage'  # Directory containing the files
    file_colors = {
        '100.txt': 'lightblue',
        '200.txt': 'lightgreen',
        '300.txt': 'lightpink',
        '400.txt': 'lightcoral'
    }  # File names and their corresponding colors

    labels = {
        '100.txt': '100 movs',
        '200.txt': '200 movs',
        '300.txt': '300 movs',
        '400.txt': '400 movs'
    }  # Custom labels for the files

    plot_box(directory, file_colors, labels)

if __name__ == "__main__":
    main()

