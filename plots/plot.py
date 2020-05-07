import numpy as np
import matplotlib.pyplot as plt

n = np.array([10000, 50000, 100000, 500000, 1000000])
normal_kruskal = np.array([3.91e-02, 2.23e-01, 4.79e-01, 2.66e+00, 5.55e+00])
filter_kruskal_one = np.array([1.38e-02, 7.26e-02, 1.50e-01, 9.80e-01, 1.99e+00])
filter_kruskal_two = np.array([2.60e-02, 1.33e-01, 2.78e-01, 1.51e+00, 2.92e+00])
filter_kruskal_four = np.array([2.29e-02, 1.19e-01, 2.47e-01, 1.28e+00, 2.49e+00])
filter_kruskal_six = np.array([2.25e-02, 1.13e-01, 2.29e-01, 1.20e+00, 2.31e+00])

#normal_kruskal = np.array([3.91e-02, 2.23e-01, 4.79e-01, 2.66e+00, 5.55e+00])
#filter_kruskal_one = np.array([1.18e-02, 7.75e-02, 1.57e-01, 9.84e-01, 2.12e+00])
#filter_kruskal_two = np.array([2.72e-02, 1.44e-01, 2.84e-01, 1.41e+00, 2.93e+00])
#filter_kruskal_four = np.array([2.53e-02, 1.33e-01, 2.85e-01, 1.32e+00, 2.80e+00])
#filter_kruskal_six = np.array([2.90e-02, 1.31e-01, 2.70e-01, 1.30e+00, 2.62e+00])

#plt.title("Estimated latency per byte vs number of bytes sent")
#plt.xlabel("Estimated latency per byte / s")
#plt.ylabel("Number of bytes sent")
#latency = []
#for (num_bytes, send_time) in data.items():
#	if (num_bytes != 1):
#		latency.append(np.mean((send_time[2:6] - send_one[2:6]) / (num_bytes - 1)))
#x_pos = np.arange(len(latency))
#plt.bar(x_pos, latency)
#plt.yscale('log')
#plt.xticks(x_pos, ['100', '1000', '10000', '100000'])
#plt.show()

#plt.title("Estimated startup latency vs rank")
#plt.xlabel("Estimated startup latency / s")
#plt.ylabel("Rank")
#x_pos = np.arange(len(send_one))
#plt.bar(x_pos, send_one)
#plt.xticks(x_pos, ['0', '1', '2', '3', '4', '5'])
#plt.yscale('log')
#plt.show()

plt.title("Runtime vs Number of graph nodes (50 edges per node)")
plt.xlabel("Number of graph nodes (n)")
plt.ylabel("Runtime / s")
runtime = filter_kruskal_one / filter_kruskal_six
print(runtime)
x_pos = np.arange(len(runtime))
plt.bar(x_pos, runtime)
plt.xticks(x_pos, ['10000', '50000', '100000', '500000', '1000000'])
plt.show()