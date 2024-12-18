import os
import re
import matplotlib.pyplot as plt

def read_numbers_from_file(file_path):
    """Read numbers from the file, filtering values <= 300."""
    with open(file_path, 'r') as file:
        numbers = [int(line.strip()) for line in file if int(line.strip()) <= 300]
    return numbers

def plot_data(file_name, numbers):
    """Plot the data points with specific colors based on latency ranges."""
    plt.figure(figsize=(10, 8))

    # Separate values into categories and plot them with specific colors
    x_values = range(len(numbers))  # x coordinates as indices

#    for x, y in zip(x_values, numbers):
#        if x < 2000:  # All points for x < 2000 are blue
#            plt.scatter(x, y, color='blue', marker='o')
#        else:
#            if y < 100:  # Red for values < 100
#                plt.scatter(x, y, color='red', marker='o')
#            elif 150 <= y <= 180:  # Blue for 150-180
#                plt.scatter(x, y, color='blue', marker='o')
#            elif y > 190:  # Green for > 190
#                plt.scatter(x, y, color='green', marker='o')
    plt.scatter(range(len(numbers)), numbers, color="blue", marker = 'o', s=3)
    # Chart title and labels
    plt.title(f'Latency of Redundant Instructions ({file_name})', fontsize=20)
    plt.xlabel('Measurement Index', fontsize=18)
    plt.ylabel('Latency in Cycles', fontsize=18)
    plt.grid(True)

    # Set axis ticks
    plt.xticks(range(0, len(numbers), 1000), fontsize=16)
    plt.yticks(range(0, 300, 50), fontsize=16)

    # Show the plot
    plt.show()

def extract_number(file_name):
    """Extract a numeric identifier from the file name."""
    match = re.search(r'res_(\d+)\.txt', file_name)
    return int(match.group(1)) if match else float('inf')

def main():
    """Main function to read and plot data from all text files in a directory."""
    directory = './dco_re_anti'
    if not os.path.exists(directory):
        print(f'Directory {directory} does not exist.')
        return

    # Get a list of .txt files in the directory
    files = [f for f in os.listdir(directory) if f.endswith('.txt')]

    # Sort the files based on the numeric part of the filename (e.g., res_1.txt, res_2.txt, ...)
    files.sort(key=extract_number)

    # Process each file in sorted order
    for file_name in files:
        file_path = os.path.join(directory, file_name)
        numbers = read_numbers_from_file(file_path)
        plot_data(file_name, numbers)

if __name__ == "__main__":
    main()

