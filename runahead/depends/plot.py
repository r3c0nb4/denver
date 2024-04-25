import matplotlib.pyplot as plt
import numpy as np
column_x88_flush = []
column_x11_flush = []
column_x88_nonflush = []
column_x11_nonflush = []

with open("output_flush", "r") as f:
    for line in f.readlines():
        columns = line.strip().split("\t")
        column_x88_flush.append(columns[1])
        column_x11_flush.append(columns[2])


with open("output_nonflush", "r") as f:
    for line in f.readlines():
        columns = line.strip().split("\t")
        column_x88_nonflush.append(columns[1])
        column_x11_nonflush.append(columns[2])

print(column_x88_flush, column_x11_flush)
print(column_x88_nonflush, column_x11_nonflush)

x = np.arange(10, 81)

# 绘制散点图
plt.figure(figsize=(10, 6))

# 绘制 x88 flush 数据
plt.scatter(x, np.histogram(column_x88_flush, bins=range(10, 82))[0], color='blue', marker='o', label='x88 flush')

# 绘制 x11 flush 数据
plt.scatter(x, np.histogram(column_x11_flush, bins=range(10, 82))[0], color='red', marker='s', label='x11 flush')

# 绘制 x88 nonflush 数据
plt.scatter(x, np.histogram(column_x88_nonflush, bins=range(10, 82))[0], color='green', marker='^', label='x88 nonflush')

# 绘制 x11 nonflush 数据
plt.scatter(x, np.histogram(column_x11_nonflush, bins=range(10, 82))[0], color='purple', marker='x', label='x11 nonflush')

plt.xlabel('Value')
plt.ylabel('Data')
plt.title('Scatter Plot of x88 and x11 Data')
plt.legend()

plt.tight_layout()
plt.show()
