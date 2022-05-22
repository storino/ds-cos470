#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <signal.h>
#include<sys/wait.h>
using namespace std;

int pid = {};
int signal_to_send;

void signal_handler(int sig) {
    if (sig == SIGUSR1){
        cout << "Busque conhecimento\n";
    }
    if (sig == SIGPROF){
        cout << "Qual a pergunta?\n";
    }
    if (sig == SIGTERM){
        cout << "42\n";

        kill(getpid(),6);

    }
}

int main(){
    int mypid = getpid();
    cout << "O meu codigo e " << mypid << endl;
    bool busy_wait;
    int entrada;
    cout << "Escreva 0 caso queira busy_wait outra coisa caso queira blocking_wait ";
    cin >> entrada;
    busy_wait = (int(entrada) == 0);

    int id = fork();
    int numero;

    if(id != 0) {
        if (entrada == 0){
            while (true) {
                if (signal(SIGUSR1, signal_handler) == SIG_ERR) {
                    cout << "SIGUSR1 não foi capturado \n";
                }  
                if (signal(SIGPROF, signal_handler) == SIG_ERR) {
                    cout << "SIPROF não foi capturado \n";
                }
                if (signal(SIGTERM, signal_handler) == SIG_ERR) {
                    cout << "SIGTERM não foi capturado \n";
                }  
                sleep(5);
                cout << "estou aqi ";
            }
            
        }
        else {
            while (true)
            {
            if (signal(SIGUSR1, signal_handler) == SIG_ERR) {
                cout << "SIGUSR1 não foi capturado \n";
            } 
            if (signal(SIGPROF, signal_handler) == SIG_ERR) {
                cout << "SIPROF não foi capturado \n";
            }
            if (signal(SIGTERM, signal_handler) == SIG_ERR) {
                cout << "SIGTERM não foi capturado \n";
            }
            wait(NULL);
            cout << "isso nunca e impresso na tela";
            
            }     
        }

        
    
    }
    else {
        
        while (true) {
        cout << "Escreva o codigo do processo ";
        cin >> pid;
        cout << "Escreva o sinal que sera enviado ";
        cin >> signal_to_send;
        if (kill(pid,signal_to_send)) {
            cout << "O processo de codigo " << pid << " nao existe" <<endl;
            return 1;
        }
        cout << "sinal enviado com sucesso meu rapaz" <<endl;
    }

    }
}