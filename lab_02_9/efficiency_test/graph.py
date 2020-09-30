import matplotlib.pyplot as plt

def read_data_file(filename):
    data = []
    for line in open(filename, "rt"):
        N, repeats, time = list(map(int, line.strip().split()))
        data.append([N, time])
    return data

def get_plot_xy(data):
    x = [p[0] for p in data]
    y = [p[1] for p in data]
    return x, y

titles = ["вставками (а)", "вставками (б)", "слиянием (а)", "слияние (б)"]
files = ["a_slow.txt", "b_slow.txt", "a_fast.txt", "b_fast.txt"]
datas = list(map(read_data_file, files))

for title, data in zip(titles, datas):
    plt.plot(*get_plot_xy(data), label=title)

percent_a = (1 - datas[2][-1][1] / datas[0][-1][1]) * 100
print(f"Эффективность по времени (а): {percent_a:.2f}%",)

percent_b = (1 - datas[3][-1][1] / datas[1][-1][1]) * 100
print(f"Эффективность по времени (b): {percent_b:.2f}%",)

percent_b = (1 - datas[3][-1][1] / datas[2][-3][1]) * 100
print(f"Эффективность по времени (слияние): {percent_b:.2f}%",)

plt.legend()
plt.show()
