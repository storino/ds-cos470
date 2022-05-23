import matplotlib.pyplot as plt
import csv
import os


for N_arg, N_label in N_values:
    for K_arg in [str(2**k) for k in range(3)]:
        for k in range(10):
            os.system(f"./adder {i} {j} >> dados_{i}{j}.csv")
        with open(f"dados_{i}{j}.csv", 'r') as csv_file:
            dataset = csv.reader(csv_file, delimiter=';')
            l = []
            for line in dataset:
                _, runtime = line
                l.append(float(runtime))
            media = sum(l)/10
        os.system(f"echo {j};{media} >> dados_{n}.csv")


files = ["dados_10^7.csv", "dados_10^8.csv", "dados_10^9.csv"]
legends = ["N=10^7","N=10^8","N=10^9"]

for i in range(3):
    x = []
    y = []
    with open(files[i], 'r') as csv_file:
        dataset = csv.reader(csv_file, delimiter=';')
        next(dataset)
        for line in dataset:
            number, runtime = line
            x.append(int(number.strip()))
            y.append(float(runtime.strip()))
        plt.plot(x,y, label=legends[i])

os.system("rm *.csv")
plt.ylabel("Runtime (s)")
plt.xlabel("Tamanho de N")
plt.title(f"Número de Threads vs Tempo")
plt.legend(loc="upper left")
plt.savefig("graph")
