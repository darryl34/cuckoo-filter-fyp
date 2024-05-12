import random
import matplotlib.pyplot as plt
import time

from cuckoo import CuckooFilter

results = {}

num_buckets = [2**n for n in range(10, 15, 2)]
bucket_size = [2, 4, 8]
possible_buckets = [2, 4, 8]
random.seed(0)

for n in num_buckets:
    for b in bucket_size:
        for p in possible_buckets:
            print(f"Testing {n} buckets, {b} bucket size, {p} possible buckets")
            cf = CuckooFilter(n, b, p)
            start = time.time()
            for i in range(n * b):
                try:
                    cf.insert(i)
                except Exception as e:
                    # print(f"Failed to insert {i}: {e}")
                    # print("Time taken:", time.time() - start)
                    break
            lf = cf.load_factor()
            results[(n, b, p)] = lf
            print(f"Load factor: {lf}")

# Plot the results
fig, ax = plt.subplots()
for b in bucket_size:
    for p in possible_buckets:
        x = [n for n in num_buckets]
        y = [results[(n, b, p)] for n in num_buckets]
        ax.plot(x, y, label=f"{b} bucket size, {p} possible buckets")
ax.set_xlabel("Number of Buckets")
ax.set_ylabel("Load Factor")
ax.set_xscale('log')
ax.legend()

plt.show()