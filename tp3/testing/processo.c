#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define PORT 8080

int main(int argc , char *argv[])
{
    int sock = 0;
    char buffer[10] = {0};
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
        <= 0)
    {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr,
                sizeof(serv_addr))
        < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    
    for(int j = 0; j < 3; j++)
    {   
        char msg[] = "John China";
        send(sock, msg, sizeof(msg), 0);
    }

    return 0;
}