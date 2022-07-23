import socket
#import sys
import os
from time import sleep
from datetime import datetime

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print ("Socket successfully created")
except socket.error as err:
    print ("socket creation failed with error %s" %(err))
 
# connecting to the server
s.connect(("127.0.0.1", 4242))

pid = os.getpid()
r = 20


for i in range(r):

    request_msg = f"0|{pid}|{i}"
    s.send(request_msg.encode())
    grant_msg = s.recv(1)
    print("valor da mensagem é " + str())
    if(grant_msg.decode() == "7"):
        print("aribaa")
        continue

    if grant_msg.decode() == '1':
        hora_atual = datetime.now()
        resultado = open("resultado.txt", "a")
        resultado.write(f"{hora_atual} | {pid} | {i}\n")
        resultado.close()
        release_msg = f"2|{pid}|{i}"
        s.send(release_msg.encode())

    else:
        print("Sem permissão para executar o processo")

    sleep(1)
s.close()
