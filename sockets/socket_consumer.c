#include <stdio.h>
#include <netdb.h>

int is_prime(int n){
    if (n % 2 == 0 || n <= 1) return 0;
    for (int i=3; i<n/2; i=i+2){
        if (n % i == 0){
            return 0;
        }
    }
    return 1;
}

int main(){

    int my_socket, connection;

    struct sockaddr_in saddr =
    {
        .sin_family = AF_INET,
        .sin_port = htons(5000),
        .sin_addr.s_addr = htonl(INADDR_ANY)
    };
    
    my_socket = socket(AF_INET, SOCK_STREAM, 0);

    bind(my_socket, (struct sockaddr *)&saddr, sizeof saddr);

    listen(my_socket, 5);

    int new_connection;
    struct sockaddr_in caddr;
    socklen_t t_cliente = sizeof caddr;
    new_connection = accept(my_socket, (struct sockaddr *) &caddr, &t_cliente);

    int num;
    char buffer[32] = {0};
    const char * msg;

    while (1) {

        read(my_socket, buffer, 32);
        num = atoi(buffer);
        if (num == 0) {
            printf("Terminating process\n");
            return 0;
        }
        memset(buffer, 0, sizeof(buffer));

        if(is_prime(num)) {
            
            msg = "1";
            send(my_socket, msg, strlen(msg), 0);
    
        } else {
            
            msg = "0";
            send(my_socket, msg, strlen(msg), 0);

        }
        prinf("Número %d recebido.\nCalculando se é primo... \n", num);
    }

    return 0;
}