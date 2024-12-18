import os
import matplotlib.pyplot as plt
import numpy as np

# Read data from file
def read_data(file_path):
    with open(file_path, 'r') as file:
        data = []
        for line in file:
            line = line.strip()
            if line.isdigit():
                num = int(line)
                data.append(num)
    return data

# Find the first group where 90% of the numbers are less than 100 and the first number is less than 100
def find_first_group_with_condition(numbers, threshold=100, percentage=0.9, group_size=100):
    for i in range(len(numbers) - group_size + 1):
        group = numbers[i:i + group_size]
        count_less_than_threshold = sum(1 for num in group if num < threshold)
        if count_less_than_threshold >= group_size * percentage and group[0] < threshold:
            return i  # Return the index of the first number in the group
    return None

# Process a single file
def process_file(file_path):
    numbers = read_data(file_path)
    if not numbers:
        return None
    return find_first_group_with_condition(numbers)

# Process all txt files in a directory and collect all drop positions
def process_directory(directory):
    if not os.path.exists(directory):
        print(f"Directory {directory} does not exist.")
        return []
    
    txt_files = [f for f in os.listdir(directory) if f.endswith('.txt')]
    if not txt_files:
        print(f"No .txt files found in the directory {directory}.")
        return []
    
    all_drop_positions = []
    for file_name in txt_files:
        file_path = os.path.join(directory, file_name)
        drop_position = process_file(file_path)
        if drop_position is not None:
            all_drop_positions.append(drop_position)
    return all_drop_positions

# Plot drop positions with annotations for dense y-axis regions
def plot_drop_positions_with_density(drop_positions):
    # Define y-axis bins: [0, 200), [200, 400), ..., [800, 1000), [1000, 2000), ...
    y_bins = np.concatenate((np.arange(0, 1000, 200), np.arange(2000, 3001, 1000)))
    density, _ = np.histogram(drop_positions, bins=y_bins)

    # Identify the three densest regions for bins 0-1000
    density_0_1000 = density[:5]  # Focus on bins within 0-1000
    densest_bin_indices = np.argsort(density_0_1000)[-3:][::-1]  # Indices of the three most dense bins
    
    densest_ranges = [(y_bins[i], y_bins[i + 1]) for i in densest_bin_indices]
    densest_counts = [density[i] for i in densest_bin_indices]

    # Define colors for the densest regions
    colors = ["red", "orange", "yellow"]

    # Create scatter plot
    plt.figure(figsize=(10, 6))
    plt.scatter(range(len(drop_positions)), drop_positions, color='blue', s=3, label='Drop Positions')

    # Highlight the densest regions
    for i, (densest_range, densest_count) in enumerate(zip(densest_ranges, densest_counts)):
        plt.axhspan(densest_range[0], densest_range[1], color=colors[i], alpha=0.3, label=f"Region {i + 1}: {int(densest_range[0])}-{int(densest_range[1])}, Count: {densest_count}")

    # Configure plot appearance
    plt.title('Scatter Plot of Drop Positions', fontsize=20)
    plt.xlabel('Experiment index', fontsize=18)
    plt.ylabel('Drop Position (drop after y iterations)', fontsize=18)
    plt.ylim(0, 2000)  # Set y-axis range to 0-3000
    plt.yticks(np.concatenate((np.arange(0, 1000, 200), np.arange(1000, 2000, 1000))), fontsize=16)
    plt.grid(True)
    plt.legend()
    plt.show()

# Main function
def main():
    directory = './dco_re_store'  # Specify the directory
    drop_positions = process_directory(directory)
    
    if drop_positions:
        # Plot the drop positions with density annotation
        plot_drop_positions_with_density(drop_positions)
    else:
        print("No drop positions found.")

if __name__ == "__main__":
    main()

