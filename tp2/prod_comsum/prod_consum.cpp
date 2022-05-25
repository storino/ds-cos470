#include <iostream>                      
#include <bits/stdc++.h>
#include <semaphore.h>
#include <thread>
#include <queue>
#include <time.h>

using namespace std;


sem_t full;
sem_t empty;
int num_val_processar = 10000;
int semente_produtor = time(NULL)%1000;
int tamanho_fila;
sem_t mutex_fila;
sem_t mutex_encerramento;

queue<int> fila;


int gerador_aleatorio(int semente) 
{
    int valor_max = 10000000;
    srand(semente);
    int x = rand() % valor_max;
    return x;
}


int is_prime(int n)
{
    if (n % 2 == 0 || n <= 1) return 0;

    for (int i=3; i<n/2; i=i+2)
    {
        if (n % i == 0) return 0;
    }
    return 1;
}


void produtor()
{
    int valor_aleatorio;
    while(num_val_processar > 0)
    {

        valor_aleatorio = gerador_aleatorio(semente_produtor);

        sem_wait(&empty);
        sem_wait(&mutex_fila);
        
        
        fila.push(valor_aleatorio);
        if(fila.size() > tamanho_fila) {
        //    cout << "estourou a fila\n";
        }

        semente_produtor--;

        sem_post(&mutex_fila);
        sem_post(&full);
    }
    sem_post(&full);
    sem_post(&empty);
}


void consumidor()
{
    int valor_recolhido;
    while(num_val_processar > 0) {

        sem_wait(&full);
        sem_wait(&mutex_fila);
        
            valor_recolhido = fila.front();
            fila.pop();

        sem_post(&mutex_fila);
        sem_post(&empty);

        if(is_prime(valor_recolhido)) 
        //    cout << valor_recolhido << " é primo\n";

        sem_wait(&mutex_encerramento);
            num_val_processar--;
        sem_post(&mutex_encerramento);
    }
    sem_post(&full);
    sem_post(&empty);
}


int main(int argc, char* argv[])
{
    int num_th_prod = atoi(argv[1]);
    int num_th_consum = atoi(argv[2]);
    tamanho_fila = atoi(argv[3]);
    
    //int num_th_prod = 2;
    //int num_th_consum = 2;

    sem_init(&full,0,0);
    sem_init(&empty,0,tamanho_fila);
    sem_init(&mutex_fila,0,1);
    sem_init(&mutex_encerramento,0,1);

    vector<thread> threads;

    auto program_start = chrono::high_resolution_clock::now();

    for (int i = 0; i < num_th_prod; i++) threads.push_back(thread(produtor));
    for (int i = 0; i < num_th_consum; i++) threads.push_back(thread(consumidor));

    for (auto& th : threads) th.join();

    auto program_end = chrono::high_resolution_clock::now();

    double time_taken = 
        chrono::duration_cast<chrono::nanoseconds>(program_end - program_start).count();

    time_taken *= 1e-6;
    
    cout << tamanho_fila << ';' << time_taken << setprecision(9) << '\n';

    return 0;
}