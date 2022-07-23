import socket
#import sys
import os
from time import sleep

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print ("Socket successfully created")
except socket.error as err:
    print ("socket creation failed with error %s" %(err))
 
# connecting to the server
s.connect(("127.0.0.1", 8080))

pid = os.getpid()

for i in range(2):
    sleep(2)
    s.send(f"{pid} | {i}\n".encode())
