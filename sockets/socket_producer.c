#include <stdio.h>
#include <netdb.h>

int main(int argc, char *argv[]){

    int my_socket, connection;

    my_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;

    struct hostent *server;
    server = gethostbyname(argv[1]);

    bzero((char *) &saddr, sizeof(saddr));
    bcopy((char *)server->h_addr, (char *)&saddr.sin_addr.s_addr, server->h_length);

    int port;
    port = atoi(argv[2]);
    saddr.sin_port = htons(port);
    connect(my_socket, (struct sockaddr *)&saddr, sizeof(saddr));


    int num, isprimo;
    char buffer [32];

    for (int i = 0; i < 5; i++) {

        //gera número aleatório
        num = ran();
           
        //Transforma o int num em string para poder enviá-lo pelo socket
        const char * msg = (to_string(num)).c_str();

        //Envia número pro cliente
        send(my_socket, msg, strlen(msg), 0);
        printf("Número %d enviado para o consumidor. ")
        cout << "Número " << num << " enviado para o consumidor." << endl;

        //Recebe a resposta do consumidor
        read(my_socket, buffer, 32);
        int isprimo = atoi(buffer);

        if (isprimo == 1) {
            cout << "Número " << num << " é primo!" << endl;
            cout << "-----------------------------------" << endl; 
        } else {
            cout << "Número " << num << " não é primo!" << endl;
            cout << "-----------------------------------" << endl; 
        }

        num = 0;
    }

    cout << "O programa está sendo encerrado. Enviando valor zero para o consumidor..." << endl;
    send(novaconexao, "0", strlen("0"), 0);

    return 0;
}