#include <iostream>       
#include <atomic>         
#include <thread>         
#include <bits/stdc++.h>

using namespace std;


typedef struct spinlock 
{
    atomic<bool> held = {false};
} spinlock;

void acquire(spinlock *lock)
{
    while(lock->held.exchange(true, memory_order_acquire));
}

void release(spinlock *lock) 
{
    lock->held.store(false, memory_order_release);
}


spinlock thread_lock;

int accumulator = 0;


signed char *vec_init(int N)
{
    srand(time(NULL));
    signed char* vec = (signed char*)malloc((N)*sizeof(char));

    for(int i=0; i < N; i++) vec[i] = (signed char)(rand() % 201) - 100;

    return vec;
}


void add(signed char *vec, int start, int end)
{
    int result;
    
    for (int i = start; i <= end; i++) result += vec[i];

    acquire(&thread_lock);
    accumulator += result;
    release(&thread_lock);
}


int main(int argc, char* argv[])
{
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
        threads.push_back(thread(add, vec, start, end));
    }

    for (auto& th : threads) th.join();

    auto program_end = chrono::high_resolution_clock::now();

    double time_taken = 
        chrono::duration_cast<chrono::nanoseconds>(program_end - program_start).count();

    time_taken *= 1e-6;
    
    cout << K << ';' << time_taken << setprecision(9) << '\n';

    return 0;
}