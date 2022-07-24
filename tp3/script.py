import subprocess
from time import time, sleep
import matplotlib.pyplot as plt
import numpy as np

values_of_n_plot = [2,4,8,16,32,64,128]
br = np.arange(len(values_of_n_plot))
bar_width = 0.5
RUNTIMES = []

for test_number in range(1,4):

    if test_number == 1:
        values_of_n = [2,4,8,16,32]
        r=10
        k=2

    elif test_number == 2:
        values_of_n = [2,4,8,16,32,64]
        r=5
        k=1

    elif test_number == 3:
        values_of_n = [2,4,8,16,32,64,128]
        r=3
        k=0

    runtimes = []
    cmd = f'python3 process.py {r} {k}'
    for n in values_of_n:
        s = time()
        for i in range(int(n)-1):
            subprocess.Popen(cmd, shell=True)
        subprocess.run(cmd, shell=True)
        e = time()

        sleep(2)

        runtimes.append((e - s)*1000)

    while (len(runtimes) != len(values_of_n_plot)):
        runtimes.append(0)
    
    RUNTIMES.append(runtimes)

"""
plt.bar(br + bar_width*(test_number - 1), a, label=f'r=10, k=2')
plt.bar(br + bar_width*(test_number - 1), b, label=f'r=5, k=1')
plt.bar(br + bar_width*(test_number - 1), c, label=f'r=3, k=0')

plt.xticks(br + bar_width, values_of_n_plot)
plt.xlabel("N")
plt.ylabel("Tempo (ms)")
plt.title("Tempo de execução do processo")
plt.legend()
plt.savefig("runtime.png")
"""
