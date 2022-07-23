import subprocess

from time import time

s = time()

cmd = 'python3 processo.py'
for i in range(1):
    p = subprocess.Popen(cmd, shell=True)

e = time()

print(e - s)

#out, err = p.communicate() 
#result = out.split('\n')
#for lin in result:
#    if not lin.startswith('#'):
#        print(lin)
