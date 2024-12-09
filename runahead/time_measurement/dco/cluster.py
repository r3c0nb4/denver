import os

# Read data from file (no longer ignoring the first ten lines)
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
    drop_positions = []  # Store all detected drop positions
    for i in range(len(numbers) - group_size + 1):
        group = numbers[i:i + group_size]
        count_less_than_threshold = sum(1 for num in group if num < threshold)
        
        if count_less_than_threshold >= group_size * percentage and group[0] < threshold:
            drop_positions.append(i)  # Store the index of the first number in the group
    
    return drop_positions  # Return list of drop positions

# Process a single file
def process_file(file_path):
    file_name = os.path.basename(file_path)
    numbers = read_data(file_path)
    
    if not numbers:
        print(f"{file_name}: No valid data.")
        return None
    
    drop_positions = find_first_group_with_condition(numbers)
    
    if drop_positions:
        # Only return the first drop position
        return drop_positions[0]
    else:
        return None

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

# Count drop positions in specific ranges
def count_drop_position_ranges(drop_positions):
    ranges = {
        "0-1000": 0,
        "1000-2000": 0,
        "2000-3000": 0,
        "3000-4000": 0,
        "4000-5000": 0,
        "5000-6000": 0,
        "6000-7000": 0,
        "7000-8000": 0,
        "8000-9000": 0,
        "9000-10000": 0,
    }
    
    for drop_pos in drop_positions:
        if 0 <= drop_pos < 1000:
            ranges["0-1000"] += 1
        elif 1000 <= drop_pos < 2000:
            ranges["1000-2000"] += 1
        elif 2000 <= drop_pos < 3000:
            ranges["2000-3000"] += 1
        elif 3000 <= drop_pos < 4000:
            ranges["3000-4000"] += 1
        elif 4000 <= drop_pos < 5000:
            ranges["4000-5000"] += 1
        elif 5000 <= drop_pos < 6000:
            ranges["5000-6000"] += 1
        elif 6000 <= drop_pos < 7000:
            ranges["6000-7000"] += 1
        elif 7000 <= drop_pos < 8000:
            ranges["7000-8000"] += 1
        elif 8000 <= drop_pos < 9000:
            ranges["8000-9000"] += 1
        elif 9000 <= drop_pos < 10000:
            ranges["9000-10000"] += 1
    
    return ranges

# Print the drop positions count for each range
def print_drop_position_counts(ranges):
    print("Drop Position Distribution Across All Files:")
    for range_label, count in ranges.items():
        print(f"{range_label}: {count}")

# Main function
def main():
    directory = './dco_re'  # Specify the directory
    drop_positions = process_directory(directory)
    
    if drop_positions:
        # Count the drop positions in specified ranges
        ranges = count_drop_position_ranges(drop_positions)
        print_drop_position_counts(ranges)
    else:
        print("No drop positions found.")

if __name__ == "__main__":
    main()

