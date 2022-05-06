#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define PORT 8080

int main(int argc, char const* argv[])
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = { 0 };
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr,
                sizeof(serv_addr))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    int message, old_message;
    old_message = 1;

    for (int i = 0; i < 5; i++) {

        srand(time(NULL));
        message = old_message + (rand() % 100);

        char str[32];
        sprintf(str, "%d", message);

        send(sock, str, strlen(str), 0);
        old_message = message;
        printf("Número %d enviado para o consumidor.\n", message);

        read(sock, buffer, 1024);
        int isprimo = atoi(buffer);

        if (isprimo == 1) {
            printf("Número %d é primo\n", message); 
        } else {
            printf("Número %d não é primo\n", message); 
        }
    }

    char str = '0';
    send(sock, str, 8, 0);
}