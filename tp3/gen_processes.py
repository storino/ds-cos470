import subprocess
cmd = 'python3 processo.py'
for i in range(2):
    p = subprocess.Popen(cmd, shell=True)
#out, err = p.communicate() 
#result = out.split('\n')
#for lin in result:
#    if not lin.startswith('#'):
#        print(lin)
