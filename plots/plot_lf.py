import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv('test_lf_vs_bktsize.txt')
fig, ax = plt.subplots(2, 2, figsize=(12, 10))
axes = ax.flatten()
i = 0

for b in df['bucket_size'].unique():
    subset = df[df['bucket_size'] == b]
    # plt.figure(figsize=(8, 6))
    for poss_b in subset['possible_buckets'].unique():
        subsubset = subset[subset['possible_buckets'] == poss_b]
        axes[i].plot(subsubset['num_buckets']*b, subsubset['load_factor'], marker='o', linestyle='--', label=f'possible_buckets={poss_b}')
    axes[i].set_xscale('log', base=2)
    axes[i].set_title(f'bucket_size={b}')
    axes[i].set_ylim(top=1)
    i += 1

fig.suptitle('Load Factor vs Number of Buckets (max_kicks=500)')
plt.legend()
plt.savefig('lf_vs_bktsize.png')

