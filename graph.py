import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("data_out.txt")
df["b_creation_io"] = df["b_creation_read"] + df["b_creation_write"]
df["bp_creation_io"] = df["bp_creation_read"] + df["bp_creation_write"]

df["b_avg_search_io"] = df["b_avg_search_read"] + df["b_avg_search_write"]
df["bp_avg_search_io"] = df["bp_avg_search_read"] + df["bp_avg_search_write"]

creation_time_df = df[["n", "b_creation_time", "bp_creation_time"]]
creation_time_df = creation_time_df.set_index("n")
creation_time_df.plot()
plt.savefig("plots/creation_time.png")

creation_io_df = df[["n", "b_creation_io", "bp_creation_io"]]
creation_io_df = creation_io_df.set_index("n")
creation_io_df.plot()
plt.savefig("plots/creation_ios.png")

size_df = df[["n", "b_size", "bp_size"]]
size_df = size_df.set_index("n")
size_df.plot()
plt.savefig("plots/size.png")

avg_search_time_df = df[["n", "b_avg_search_time", "bp_avg_search_time"]]
avg_search_time_df = avg_search_time_df.set_index("n")
avg_search_time_df.plot()
plt.savefig("plots/avg_search_time.png")


avg_search_io_df = df[["n", "b_avg_search_io", "bp_avg_search_io"]]
avg_search_io_df = avg_search_io_df.set_index("n")
avg_search_io_df.plot()
plt.savefig("plots/avg_search_ios.png")

search_16_df = pd.read_csv("search_data_16.txt")
search_16_df.plot.scatter(x="llave",y="valor", s=0.3)
plt.title("Search n=16")
plt.savefig("plots/search_16.png")

search_22_df = pd.read_csv("search_data_22.txt")
search_22_df.plot.scatter(x="llave",y="valor", s=0.3)
plt.title("Search n=22")
plt.savefig("plots/search_22.png")
