import os
import matplotlib.pyplot as plt
import csv


def read_csv(file):

    y = []

    with open(file, 'r') as csv_file:

        dataset = csv.reader(csv_file, delimiter=';')
        next(dataset)
        
        for line in dataset:

            _, runtime = line
            y.append(float(runtime.strip()))

    return y


N_values = [("1"+"0"*i, "10^"+str(i)) for i in range(7,10)]
K_values = [2**k for k in range(9)]


for N_arg, N_label in N_values:

    K_results = []

    for K_arg in K_values:

        it = 10

        for i in range(it):
            
            new_file = f"dado_{N_arg}_{K_arg}.csv"
            os.system(f"./adder {N_arg} {K_arg} >> {new_file}")
        
        results = read_csv(new_file)
        media = sum(results) / it
        K_results.append(media)
    
    plt.plot(K_values, K_results, label=N_label)


plt.ylabel("Runtime (ms)")
plt.xlabel("Quantidade de Threads")
plt.title("Número de Threads vs Tempo")
plt.legend(loc="upper left")
plt.xscale("log",base=2)
plt.savefig("graph")

os.system("rm *.csv")
