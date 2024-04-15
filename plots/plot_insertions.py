import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("results/test_insert_speed.txt")
fig, ax = plt.subplots(2, 1, figsize=(12, 14), sharey=True)
axes = ax.flatten()
i = 0

for i, b in enumerate(df["bucket_size"].unique()):
    subset = df[df["bucket_size"] == b]
    axes2 = axes[i].twinx()
    for poss_b in subset["poss_buckets"].unique():
        subsubset = subset[subset["poss_buckets"] == poss_b]
        axes[i].plot(subsubset["num_buckets"]*b, subsubset["avg_throughput"]*1000, 
                     marker='o', linestyle='--', label=f'possible_buckets={poss_b}')
        axes2.plot(subsubset["num_buckets"]*b, subsubset["avg_lf"], 
                     marker='.', linestyle='-.')
    axes[i].set_xscale('log', base=2)
    axes[i].set_title(f'bucket_size={b}')
    axes[i].legend()
    axes[i].set_ylabel('Insertions/ms')
    axes2.set_ylabel('Load Factor')
axes[i].set_xlabel('Filter size')

fig.suptitle('Throughput (Insertions/ms)')
# plt.savefig('insert_speed.png')
plt.show()