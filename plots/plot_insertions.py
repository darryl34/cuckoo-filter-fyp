import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

from io import StringIO

# read data
def process_file(filename):
    with open(filename, "r") as f:
        next(f)
        data = f.read().split("Done\n")
        arrs = []
        for i in range(1, len(data)):
            if len(data[i]) > 0:
                arrs.append(pd.read_csv(StringIO(data[i]), sep=",", header=None).to_numpy())
        min_cols = min([arr.shape[0] for arr in arrs])
        arrs = [arr[:min_cols, :] for arr in arrs]
        return np.mean(arrs, axis=0)

mean_2d = process_file("results/test_insert_speed_2d.txt")
mean_4d = process_file("results/test_insert_speed_4d.txt")
mean_8d = process_file("results/test_insert_speed_8d.txt")

plt.plot(mean_2d[:, 0], mean_2d[:, 1], label="d=2", marker="", linestyle="-")
plt.plot(mean_4d[:, 0], mean_4d[:, 1], label="d=4", marker="", linestyle="--")
plt.plot(mean_8d[:, 0], mean_8d[:, 1], label="d=8", marker="", linestyle=":")
plt.xlabel("Load factor", fontsize=12)
plt.ylabel("Insertions Throughput (thousands per ms)", fontsize=12)
# plt.title("Throughput")
plt.legend(fontsize=11)
plt.show()
