import os
import matplotlib.pyplot as plt

def read_data(file_path):
    """读取文件中的数据，并返回去除超出2000的数的列表"""
    data = []
    with open(file_path, 'r') as file:
        for line in file:
            try:
                number = int(line.strip())
                if number <= 2000:
                    data.append(number)
            except ValueError:
                # 忽略非整数行
                pass
    return data

def plot_data(depend_data, independ_data):
    """绘制折线图"""
    plt.figure(figsize=(10, 6))
    
    # 绘制 depend 数据
    plt.plot(depend_data, label='Depend', color='blue', marker='o')
    
    # 绘制 independ 数据
    plt.plot(independ_data, label='Independ', color='red', marker='x')
    
    # 添加标题和标签
    plt.title('Depend vs Independ Data')
    plt.xlabel('Index')
    plt.ylabel('Value')
    plt.legend()
    plt.grid(True)
    
    # 显示图形
    plt.show()

def main():
    # 设置文件路径
    depend_file = './depend/output'
    independ_file = './independ/output'
    
    # 读取数据
    depend_data = read_data(depend_file)
    independ_data = read_data(independ_file)
    
    # 绘制折线图
    plot_data(depend_data, independ_data)

if __name__ == "__main__":
    main()
