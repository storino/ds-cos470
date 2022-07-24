import socket
import sys
import os
from time import sleep
from datetime import datetime

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print ("Socket successfully created")
except socket.error as err:
    print ("socket creation failed with error %s" %(err))
 
# connecting to the server
s.connect(("127.0.0.1", 8000))

pid = os.getpid()

r = int(sys.argv[1])
k = int(sys.argv[2])

i = 0
while i < r:

    i += 1

    request_msg = f"0|{pid}|{i}"
    sleep(0.01)
    s.send(request_msg.encode())
    sleep(0.01)
    grant_msg = s.recv(1).decode()

    if grant_msg == '1':
        hora_atual = datetime.now()
        resultado = open("resultado.txt", "a")
        resultado.write(f"{hora_atual} | {pid} | {i}\n")
        resultado.close()
        release_msg = f"2|{pid}|{i}"
        s.send(release_msg.encode())

    else:
        print("Sem permissão para executar o processo")
    
    sleep(k)

s.close()