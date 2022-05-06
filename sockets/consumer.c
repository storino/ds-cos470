#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int is_prime(int n){
    if (n % 2 == 0 || n <= 1) return 0;
    for (int i=3; i<n/2; i=i+2){
        if (n % i == 0){
            return 0;
        }
    }
    return 1;
}

int main(int argc, char const* argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    char* hello = "Hello from server";
 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0))
        == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
 
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
 
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket
        = accept(server_fd, (struct sockaddr*)&address,
                (socklen_t*)&addrlen))
        < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    int num;
    const char * msg;

    while (1) {

        read(new_socket, buffer, 1024);
        printf(buffer);
        num = atoi(buffer);
        if (num == 0) {
            printf("Terminando processo\n");
            return 0;
        }
        memset(buffer, 0, sizeof(buffer));

        if(is_prime(num)) {
            msg = "1";
            send(new_socket, msg, strlen(msg), 0);
    
        } else {
            msg = "0";
            send(new_socket, msg, strlen(msg), 0);

        }
        printf("\nNúmero %d recebido.\n", num);
    }

    return 0;
}