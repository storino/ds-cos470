#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


int is_prime(int n){
    if (n % 2 == 0 || n <= 1) return 0;
    for (int i=3; i<n/2; i=i+2){
        if (n % i == 0){
            return 0;
        }
    }
    return 1;
}

int main(int argc, char* argv[]){

    int fd[2];
    if (pipe(fd) == -1 ) {
        printf("An error occured when creating the pipe\n");
        return 1;
    }
    int id;

    if ((id = fork()) == 0) { // child process (producer)
        close(fd[0]);
        int total, i, old_message, message;
        old_message = 1;
        printf("Insert the total of numbers to be send: ");
        scanf("%d", &total);
        for(i = 0 ; i < total ; i++){
            srand(time(NULL));
            message = old_message + (rand() % 100);
            write(fd[1], &message, sizeof(message));
            old_message = message;
        }
        message = 0;
        write(fd[1], &message, sizeof(message));
        printf("Producer terminated\n");
        close(fd[1]);

    } else { // father process (consumer)
        close(fd[1]);
        int message_received, i;
        message_received = 1;
        while (message_received != 0){
            if (is_prime(message_received)){
                printf("Prime number received from child process: %d\n", message_received);
            }
            read(fd[0], &message_received, sizeof(message_received));
        }
        printf("Consumer terminated\n");
        close(fd[0]);
    }

    return 0;
}