import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("results/test_critical_load.txt")

grouped = df.groupby(['num_buckets', 'poss_buckets', 'bucket_size'])

# Calculate the mean and standard deviation for each group
mean = grouped['avg_throughput'].mean()
std = grouped['avg_throughput'].std()

# Combine the results into a new DataFrame
results = pd.DataFrame({'mean_throughput': mean, 'std_dev': std})

# Reset the index to make the DataFrame easier to work with
results = results.reset_index()

for i, b in enumerate(results["bucket_size"].unique()):
    plt.figure(figsize=(12, 8))
    subset = results[results["bucket_size"] == b]
    for poss_b in subset["poss_buckets"].unique():
        subsubset = subset[subset["poss_buckets"] == poss_b]
        plt.errorbar(subsubset["num_buckets"]*b, subsubset["mean_throughput"]*1000, marker='o', linestyle='--', label=f'd={poss_b}')
    plt.xscale('log', base=2)
    plt.legend()
    plt.ylabel('Critical Load (Insertions/ms)', fontsize=13)
    plt.xlabel('Filter size', fontsize=13)
    plt.title(f"Critical Load vs Filter Size (bucket_size={b})", fontsize=14)
    plt.show()