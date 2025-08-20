import os
import argparse

def read_data(file_path):
    """Read numbers from a file."""
    with open(file_path, 'r') as file:
        data = [int(line.strip()) for line in file if line.strip().isdigit()]
    return data

def find_rise_point(numbers, baseline_range=(100, 200), threshold_range=(10, 50), percentage=0.9, group_size=100):
    """Find the rise point based on the baseline average and thresholds."""
    baseline_avg = sum(numbers[baseline_range[0]:baseline_range[1]]) / (baseline_range[1] - baseline_range[0])
    
    for i in range(len(numbers) - group_size + 1):
        if threshold_range[0] <= numbers[i] - baseline_avg <= threshold_range[1]:
            group = numbers[i:i + group_size]
            count_in_range = sum(1 for num in group if threshold_range[0] <= num - baseline_avg <= threshold_range[1])
            if count_in_range >= group_size * percentage:
                return i  # Return the first rise point
    
    return None

def find_first_group_with_condition(numbers, threshold=100, percentage=0.9, group_size=100):
    """Find all groups where 90% of numbers are below the threshold and return their starting indices."""
    drop_positions = []  # Store all detected drop positions
    for i in range(len(numbers) - group_size + 1):
        group = numbers[i:i + group_size]
        count_less_than_threshold = sum(1 for num in group if num < threshold)
        
        if count_less_than_threshold >= group_size * percentage and group[0] < threshold:
            drop_positions.append(i)  # Store the index of the first number in the group
    
    return drop_positions  # Return list of drop positions

def find_drop_position(numbers, threshold=100, percentage=0.9, group_size=100):
    """Find the first drop position using the given condition."""
    drop_positions = find_first_group_with_condition(numbers, threshold, percentage, group_size)
    return drop_positions[0] if drop_positions else None

def process_file(file_path):
    """Process a single file to find rise and drop points."""
    numbers = read_data(file_path)
    
    if not numbers:
        return None, None, None  # No valid data in the file
    
    rise_position = find_rise_point(numbers)
    drop_position = find_drop_position(numbers)
    
    if rise_position is not None and drop_position is not None:
        duration = drop_position - rise_position
        return rise_position, drop_position, duration  # Return positions and duration
    
    return rise_position, drop_position, None  # If rise or drop not found

def process_directory(directory):
    """Process all files in the directory and collect results."""
    if not os.path.exists(directory):
        print(f"Directory {directory} does not exist.")
        return []
    
    txt_files = [f for f in os.listdir(directory) if f.endswith('.txt')]
    if not txt_files:
        print(f"No .txt files found in the directory {directory}.")
        return []
    
    results = []
    for file_name in txt_files:
        file_path = os.path.join(directory, file_name)
        rise, drop, duration = process_file(file_path)
        results.append((file_name, rise, drop, duration))  # Store results for each file
    
    return results

def save_rise_to_file(results, output_file):
    """Save rise values to a file."""
    with open(output_file, 'w') as file:
        for _, rise, _, _ in results:
            if rise is not None:
                file.write(f"{rise}\n")  # Write only valid rise values

def main():
    parser = argparse.ArgumentParser(description="Process files in a directory and save rise values.")
    parser.add_argument(
        "-d", "--directory", 
        type=str, 
        required=True, 
        help="Directory containing input files"
    )
    parser.add_argument(
        "-o", "--output", 
        type=str, 
        required=True, 
        help="Output file to save rise values"
    )
    args = parser.parse_args()

    directory = args.directory
    rise_output_file = args.output
    
    results = process_directory(directory)
    
    for file_name, rise, drop, duration in results:
        print(f"{file_name}: Rise at {rise}, Drop at {drop}, Duration: {duration if duration is not None else 'N/A'}")
    save_rise_to_file(results, rise_output_file)
if __name__ == "__main__":
    main()

