from matplotlib import pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns

df = pd.read_csv('results/test_spatial.txt')

data = np.array([df['index1'], df['index2']])

plt.hist2d(df['index1'], df['index2'], bins=10, cmap='Greys')
plt.colorbar()
plt.xlabel('Index 1')
plt.ylabel('Index 2')
plt.savefig('plots/figures/index1_vs_index2.png')

plt.hist2d(df['index2'], df['index3'], bins=10, cmap='Greys')
plt.xlabel('Index 2')
plt.ylabel('Index 3')
plt.savefig('plots/figures/index2_vs_index3.png')

plt.hist2d(df['index3'], df['index4'], bins=10, cmap='Greys')
plt.xlabel('Index 3')
plt.ylabel('Index 4')
plt.savefig('plots/figures/index3_vs_index4.png')