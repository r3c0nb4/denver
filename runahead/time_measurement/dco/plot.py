import os
import re
import matplotlib.pyplot as plt

def read_numbers_from_file(file_path):
    with open(file_path, 'r') as file:
        numbers = [int(line.strip()) for line in file if int(line.strip()) <= 5000]
    return numbers

def plot_data(file_name, numbers):
    plt.figure(figsize=(10, 5))
    plt.plot(numbers, marker='o')
    plt.title(f'Data from {file_name}')
    plt.xlabel('Index')
    plt.ylabel('Value')
    plt.grid(True)
    plt.show()

def extract_number(file_name):
    match = re.search(r'res_(\d+)\.txt', file_name)
    return int(match.group(1)) if match else float('inf')

def main():
    directory = './optpos'
    if not os.path.exists(directory):
        print(f'Directory {directory} does not exist.')
        return
    
    files = [f for f in os.listdir(directory) if f.endswith('.txt')]
    files.sort(key=extract_number)

    for file_name in files:
        number = extract_number(file_name)
        if number % 10 == 0:
            file_path = os.path.join(directory, file_name)
            numbers = read_numbers_from_file(file_path)
            plot_data(file_name, numbers)

if __name__ == "__main__":
    main()
