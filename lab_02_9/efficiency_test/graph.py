import matplotlib.pyplot as plt

N = range(50, 2050, 50)
datas = list(zip(*(map(float, line.split()) for line in open("res.txt"))))
titles = ["вставками (без ключей)", "вставками (с ключами)", "слиянием (без ключей)", "слияние (с ключами)"]

for title, data in zip(titles, datas):
    plt.plot(N, data, label=title)

percent_a = (1 - datas[2][-1] / datas[0][-1]) * 100
print(f"Эффективность вставками/слиянием по времени (без ключей): {percent_a:.2f}%",)

percent_b = (1 - datas[3][-1] / datas[1][-1]) * 100
print(f"Эффективность вставками/слиянием по времени (с ключами): {percent_b:.2f}%",)

percent_b = (1 - datas[3][-1] / datas[2][-1]) * 100
print(f"Эффективность по времени с/без ключей (слияние): {percent_b:.2f}%",)

plt.legend()
plt.show()
