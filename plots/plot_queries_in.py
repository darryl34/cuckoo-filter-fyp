import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("results/test_queries_in.txt")

for i, b in enumerate(df["bucket_size"].unique()):
    plt.figure(figsize=(12, 8))
    subset = df[df["bucket_size"] == b]
    for poss_b in subset["poss_buckets"].unique():
        subsubset = subset[subset["poss_buckets"] == poss_b]
        plt.plot(subsubset["num_buckets"]*b, subsubset["avg_query_speed_half"]*1000,
                        marker='o', linestyle='--', label=f'possible_buckets={poss_b}')
        # plt.plot(subsubset["num_buckets"]*b, subsubset["avg_query_speed_critical"]*1000,
                        # marker='^', linestyle='--', label=f'possible_buckets={poss_b}')
    plt.xscale('log', base=2)
    plt.legend()
    plt.ylabel('Queries/ms')
    plt.xlabel('Filter size')
    plt.title(f"Queries/ms vs Filter Size (bucket size={b})")
    plt.show()
        