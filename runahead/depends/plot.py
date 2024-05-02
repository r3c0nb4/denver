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

for i in range(len(column_x88_flush)):
    column_x88_flush[i] = int(column_x88_flush[i]) / 1000

for i in range(len(column_x11_flush)):
    column_x11_flush[i] = int(column_x11_flush[i]) / 1000

for i in range(len(column_x88_nonflush)):
    column_x88_nonflush[i] = int(column_x88_nonflush[i]) / 1000

for i in range(len(column_x11_nonflush)):
    column_x11_nonflush[i] = int(column_x11_nonflush[i]) / 1000

print(column_x88_flush, column_x11_flush)
print(column_x88_nonflush, column_x11_nonflush)

flush_data = {
    'non data dependency': column_x88_flush,
    'data dependency': column_x11_flush
}

nonflush_data = {
    'non data dependency': column_x88_nonflush,
    'data dependency': column_x11_nonflush
}

bar_width = 0.35
x = np.arange(10, 81, 1)

fig, axes = plt.subplots(2, 1, figsize=(10, 8))

for i, (label, data) in enumerate(flush_data.items()):
    axes[0].bar(x + i*bar_width*1.5, data, bar_width, label=label)
axes[0].set_title('Flush')
axes[0].legend()
axes[0].set_xlim(9.5, 80.5)

for i, (label, data) in enumerate(nonflush_data.items()):
    axes[1].bar(x + i*bar_width*1.5, data, bar_width, label=label)
axes[1].set_title('Nonflush')
axes[1].legend()
axes[1].set_xlim(9.5, 80.5)
plt.tight_layout()
plt.show()
