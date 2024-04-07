import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("results/test_insert_speed.txt")
fig, ax = plt.subplots(3, 1, figsize=(12, 15), sharey=True)
axes = ax.flatten()
i = 0

for i, b in enumerate(df["bucket_size"].unique()):
    subset = df[df["bucket_size"] == b]
    for poss_b in subset["poss_buckets"].unique():
        subsubset = subset[subset["poss_buckets"] == poss_b]
        axes[i].plot(subsubset["num_buckets"]*b, subsubset["avg_throughput"], 
                     marker='o', linestyle='--', label=f'possible_buckets={poss_b}')
    axes[i].set_xscale('log', base=2)
    axes[i].set_title(f'bucket_size={b}')
    axes[i].legend()
    axes[i].set_ylabel('Insertions/ms')
axes[i].set_xlabel('Filter size')

fig.suptitle('Throughput (Insertions/ms)')
# plt.savefig('insert_speed.png')
plt.show()