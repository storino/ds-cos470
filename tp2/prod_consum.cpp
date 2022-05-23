#include <iostream>               
#include <thread>         
#include <bits/stdc++.h>
#include <semaphore.h>
#include <thread>
using namespace std;


sem_t semaforo;

int accumulator = 0;

int is_prime(int n){
    if (n % 2 == 0 || n <= 1) return 0;
    for (int i=3; i<n/2; i=i+2){
        if (n % i == 0){
            return 0;
        }
    }
    return 1;
}


signed char *vec_init(int N)
{

    srand(time(NULL));
    signed char* vec= (signed char*)malloc((N)*sizeof(char));

    for(int i=0; i < N; i++)
    {

        vec[i] = (signed char)(rand() % 201) - 100;

    }

    return vec;

}

void add(signed char *vec, int start, int end, sem_t sem)
{

    int result;
    
    for (int i = start; i <= end; i++)
    {

        result += vec[i];

    }
    sem_wait(&sem);
    accumulator += result;
    sem_post(&sem);
    

}

int main(int argc, char* argv[])
{

    sem_init(&semaforo,0,3);

    int N = atoi(argv[1]);
    int K = atoi(argv[2]);

    signed char *vec;
    vec = vec_init(N);

    vector<thread> threads;

    int start, end;

    auto program_start = chrono::high_resolution_clock::now();

    for (int i = 0; i < K; ++i)
    {

        start = i * (N / K);
        end = (i == K - 1) ? (i + 1) * (N / K) - 1 : N - 1;

        threads.push_back(thread(add, vec, start, end, semaforo));

    }

    for (auto& th : threads) th.join();

    auto program_end = chrono::high_resolution_clock::now();

    int time_taken = 
      chrono::duration_cast<chrono::nanoseconds>(program_end - program_start).count();

    
    cout << "Time taken by program is : " << fixed 
         << time_taken << setprecision(9);
    cout << " sec" << endl;

    return 0;
}