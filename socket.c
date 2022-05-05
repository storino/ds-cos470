#include <stdio.h>
#include <netdb.h>

int main(){

    int my_socket, connection;

    struct sockaddr_in saddr =
    {
        .sin_family = AF_INET,
        .sin_port = htons(5000),
        .sin_addr.s_addr = htonl(INADDR_ANY)
    };
    
    my_socket = socket(AF_INET, SOCK_STREAM, 0);

    connection = connect(my_socket, (struct sockaddr *)&saddr, sizeof saddr);

    return 0;
}