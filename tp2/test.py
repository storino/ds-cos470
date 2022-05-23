import os
import matplotlib.pyplot as plt


N_values = [("1"+"0"*i, "10^"+str(i)) for i in range(7,10)]

K_values = [2**k for k in range(3)]


for N_arg, N_label in N_values:

    K_runtimes = []

    for K_arg in K_values:

        results = []

        it = 2

        for k in range(it):
            
            results.append(float(os.system(f"./adder {N_arg} {K_arg}")))
        
        media = sum(results) / it

        K_runtimes.append(media)

    print(K_values, K_runtimes)
    plt.plot(K_values, K_runtimes, label=N_label)


plt.ylabel("Runtime (s)")

plt.xlabel("Tamanho de N")

plt.title("Número de Threads vs Tempo")

plt.legend(loc="upper left")

plt.savefig("graph")