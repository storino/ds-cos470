// atomic_flag as a spinning lock
#include <iostream>       // std::cout
#include <atomic>         // std::atomic_flag
#include <thread>         // std::thread
#include <vector>         // std::vector
#include <sstream>        // std::stringstream
#include <unistd.h>
#include <ctime>

#define SIZE 10000000
#define K 256

std::atomic_flag lock_acumulator = ATOMIC_FLAG_INIT;
signed char vector[SIZE];
double result = 0;

void vector_init(){
    srand(time(NULL));
    int x;
    for(int i=0; i < SIZE; i++){
        vector[i] = (signed char)(rand() % 201) - 100;
    }
}

void add(int sec)
{
    int inicio, fim;

    inicio = sec * (SIZE / K);
    fim = ( (sec+1) * (SIZE / K) ) -1;

    for (int i = inicio; i <= fim; i++)
    {
        while (lock_acumulator.test_and_set())
        result += vector[i];
        lock_acumulator.clear();
    }
}

int main(int argc, char* argv[])
{
    
    vector_init();
    std::vector<std::thread> threads;

    int antes, depois;

    antes = clock();
    for (int i=0; i < K; ++i)threads.push_back(std::thread(add,i));
    for (auto& th : threads) th.join();
    depois = clock();

    std::cout << (double)(depois - antes) / CLOCKS_PER_SEC << '\n';

    return 0;
}