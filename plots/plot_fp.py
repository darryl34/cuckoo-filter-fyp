import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("results/test_fp.txt")
plt.figure(figsize=(12, 10))

size2 = df[df["bucket_size"] == 2]

for num_bkt in size2["num_buckets"].unique():
    subset = size2[size2["num_buckets"] == num_bkt]
    for poss_b in subset["possible_buckets"].unique():
        subsubset = subset[subset["possible_buckets"] == poss_b]
        plt.plot(subsubset["fp_size"], subsubset["load_factor"], marker='o', linestyle='--', label=f'poss_buckets={poss_b}')

plt.xlabel('Fingerprint Size (bits)')
plt.xscale('log', base=2)
plt.ylabel('Load Factor')
plt.ylim(top=1)
plt.title('Load Factor vs Filter Size (bucket_size=2)')
plt.legend()
plt.show()