import os
import matplotlib.pyplot as plt
import csv


def read_csv(file):
    y = []

    with open(file, 'r') as csv_file:

        dataset = csv.reader(csv_file, delimiter=';')
        
        for line in dataset:
            _, runtime = line
            y.append(float(runtime.strip()))

    return y

num_threads = [[1,1],[1,2],[1,4],[1,8],[2,1],[4,1],[8,1]]

tamanhos_fila = [2**k for k in range(6)]

barWidth = 0.15

br = [i for i in range(len(num_threads))]

for tamanho_fila in tamanhos_fila:
    
    y = []

    for num_prod, num_coms in num_threads:

        it = 10

        for i in range(it):
            new_file = f"dado_{num_prod}_{num_coms}_{tamanho_fila}.csv"
            os.system(f"./prod_consum {num_prod} {num_coms} {tamanho_fila} >> {new_file}")

        results = read_csv(new_file)
        media = sum(results) / it
        y.append(media)
    print(br)
    br = [x + barWidth for x in br]

    plt.bar(br, y, edgecolor ='grey', width = barWidth, label = f'{tamanho_fila}')


plt.xlabel('produtor x consumidor', fontweight ='bold', fontsize = 15)
plt.ylabel('Runtime (ms)', fontweight ='bold', fontsize = 15)

num_threads_str = [str(i) for i in num_threads]

plt.xticks([r + barWidth for r in range(len(num_threads))], num_threads_str)

plt.legend(loc="upper left")

plt.title("Análise Produtor-Consumidor")

plt.savefig("graph")

os.system("rm *.csv")