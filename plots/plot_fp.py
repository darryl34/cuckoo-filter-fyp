import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("results/test_fp.txt")
plt.figure(figsize=(8, 6))

markers = ['o', '^', '.']
linestyles = ['-', '--', ':']

for i, poss_b in enumerate(df["possible_buckets"].unique()):
    subset = df[df["possible_buckets"] == poss_b]
    plt.plot(subset["fp_size"], subset["load_factor"], marker=markers[i], linestyle=linestyles[i], label=f'poss_buckets={poss_b}')

plt.xlabel('Fingerprint Size (bits)')
plt.xscale('log', base=2)
plt.xticks([8, 16, 32, 64], [8, 16, 32, 64])
plt.ylabel('Load Factor')
# plt.title('Load Factor vs Filter Size (bucket_size=2)')
plt.legend()
plt.show()