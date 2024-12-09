import os
import re
import matplotlib.pyplot as plt

def read_numbers_from_file(file_path):
    with open(file_path, 'r') as file:
        numbers = [int(line.strip()) for line in file if int(line.strip()) <= 300]
    return numbers

def plot_data(file_name, numbers):
    plt.figure(figsize=(10, 8))
#   for i, num in enumerate(numbers):
#        if num < 90:
#            plt.scatter(i, num, color='red', marker='o')  # Plot in red for values below 90
#        else:
    plt.plot(numbers , color='blue', marker='o')  # Plot in blue for values >= 90
    plt.title(f'Latency of redundant instructions', fontsize=20)
    plt.xlabel('Measurement index', fontsize=18)
    plt.ylabel('Latency in cycles', fontsize=18)
    plt.grid(True)
    
    # 设置横坐标刻度，每1000一个刻度
    plt.xticks(range(0, len(numbers), 1000), fontsize=16)
    plt.yticks(range(0, 600, 30), fontsize=16)
    plt.show()

def extract_number(file_name):
    match = re.search(r'res_(\d+)\.txt', file_name)
    return int(match.group(1)) if match else float('inf')

def main():
    directory = './dco_re_200'
    if not os.path.exists(directory):
        print(f'Directory {directory} does not exist.')
        return
    
    files = [f for f in os.listdir(directory) if f.endswith('.txt')]

    for file_name in files:
        file_path = os.path.join(directory, file_name)
        numbers = read_numbers_from_file(file_path)
        plot_data(file_name, numbers)

if __name__ == "__main__":
    main()

