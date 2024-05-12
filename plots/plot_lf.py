import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv('results/test_lf.txt')
fig, ax = plt.subplots(3, 1, figsize=(10, 9))
axes = ax.flatten()
i = 0

markers = ['o', '^', '.']
linestyles = ['-', '--', ':']
for b in df['bucket_size'].unique():
    subset = df[df['bucket_size'] == b]
    for j, poss_b in enumerate(subset['poss_buckets'].unique()):
        subsubset = subset[subset['poss_buckets'] == poss_b]
        axes[i].plot(subsubset['num_buckets']*b, subsubset['load_factor'], marker=markers[j], linestyle=linestyles[j], label=f'd={poss_b}')
    axes[i].set_xscale('log', base=2)
    axes[i].set_title(f'bucket size={b}', fontsize=10)
    axes[i].set_ylim(top=1)
    axes[i].set_ylabel('Load Factor', fontsize=12)
    i += 1
axes[2].set_xlabel('Filter Size', fontsize=13)

# fig.suptitle('Load Factor vs Filter Size')
plt.legend(fontsize=11)
# plt.savefig('lf_vs_bktsize.png')
plt.show()

