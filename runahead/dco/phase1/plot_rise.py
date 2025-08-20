import os
import numpy as np
import matplotlib.pyplot as plt

# Define the directory containing the text files
directory = './output'

# Function to read numbers from a file
def read_numbers_from_file(file_path):
    """Reads numbers from a file and returns them as a list of floats."""
    with open(file_path, 'r') as file:
        return [float(line.strip()) for line in file if line.strip()]

# Check if the directory exists
if os.path.exists(directory):
    # Get the list of .txt files in the directory
    txt_files = [f for f in os.listdir(directory) if f.endswith('.txt')]

    # Initialize storage for statistics and data
    stats = {}
    data = []

    # Define colors for the box plots
    colors = ['lightblue', 'lightgreen', 'lightpink', 'lightcoral']

    # Calculate statistics for each file
    for file_name in txt_files:
        file_path = os.path.join(directory, file_name)
        numbers = read_numbers_from_file(file_path)
        if numbers:
            # Calculate statistics
            mean = np.mean(numbers)
            variance = np.var(numbers)
            quartiles = np.percentile(numbers, [25, 50, 75])

            # Store statistics
            stats[file_name] = {
                'mean': mean,
                'variance': variance,
                'quartiles': quartiles
            }

            # Append data for plotting
            data.append(numbers)

    # Display the calculated statistics
    for file_name, stat in stats.items():
        print(f"File: {file_name}")
        print(f"  Mean: {stat['mean']:.2f}")
        print(f"  Variance: {stat['variance']:.2f}")
        print(f"  Quartiles (Q1, Median, Q3): {stat['quartiles']}")

    # Plot a box plot for the data from all files
    plt.figure(figsize=(10, 8))
    boxprops = dict(linewidth=1.5)
    medianprops = dict(linewidth=2.5, color='black')
    flierprops = dict(marker='o', color='black', alpha=0)  # Suppress default outlier markers

    # Create the box plot with custom colors
    bplot = plt.boxplot(data, patch_artist=True, labels=['100', '200', '300', '400'],
                        boxprops=boxprops, medianprops=medianprops, flierprops=flierprops)

    # Apply colors to boxes and make outliers match the box color
    for patch, color, flier in zip(bplot['boxes'], colors[:len(data)], bplot['fliers']):
        patch.set_facecolor(color)
        flier.set_markerfacecolor(color)
        flier.set_marker('o')  # Set marker for outliers to filled circles

    # Set title and labels
    plt.title('Box Plot of four experiments', fontsize=18)
    plt.xlabel('Instruction Count (mov)', fontsize=16)
    plt.ylabel('Duration of phrase one (iterations)', fontsize=16)
    plt.ylim(1000, 3000)  # Set y-axis limit
    plt.grid(axis='y', linestyle='--', alpha=0.7)

    # Show the plot
    plt.show()
else:
    print(f"Directory {directory} does not exist.")

