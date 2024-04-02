import matplotlib.pyplot as plt
import numpy as np

bkt_size = 4
num_buckets = 2**12

# Load data
with open("evictions.txt") as f:
    arr = np.zeros(bkt_size*num_buckets, dtype=int)
    for i, line in enumerate(f):
        arr[i] = int(line.rstrip())
        
# Parse data
plt.plot(arr, marker='', linestyle='--')
plt.title('Evictions')
plt.xlabel('Insertion order')
plt.ylabel('Number of evictions')
plt.show()