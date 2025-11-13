#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <unistd.h>
using namespace std;

#define MAX_BUFFER 3
#define TOTAL_ITEMS 5
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2

queue<int> buffer;
sem_t empty_slots;
sem_t full_slots;
pthread_mutex_t mutex_lock;

void* producer(void* arg)
{
    int id = *((int*)arg);
    for (int item = 1; item <= TOTAL_ITEMS; item++)
    {
        sem_wait(&empty_slots);
        pthread_mutex_lock(&mutex_lock);
        
        buffer.push(item);
        cout << "Producer " << id << " produced item " << item << endl;
        
        pthread_mutex_unlock(&mutex_lock);
        sem_post(&full_slots);
        sleep(1);
    }
    pthread_exit(NULL);
    return nullptr;
}

void* consumer(void* arg)
{
    int id = *((int*)arg);
    for (int i = 1; i <= TOTAL_ITEMS; i++)
    {
        sem_wait(&full_slots);
        pthread_mutex_lock(&mutex_lock);
        
        int item = buffer.front();
        buffer.pop();
        cout << "Consumer " << id << " consumed item " << item << endl;
        
        pthread_mutex_unlock(&mutex_lock);
        sem_post(&empty_slots);
        sleep(3);
    }
    pthread_exit(NULL);
    return nullptr;
}

int main()
{
    pthread_t prodThreads[NUM_PRODUCERS], consThreads[NUM_CONSUMERS];
    int pids[NUM_PRODUCERS], cids[NUM_CONSUMERS];

    pthread_mutex_init(&mutex_lock, NULL);
    sem_init(&empty_slots, 0, MAX_BUFFER);
    sem_init(&full_slots, 0, 0);

    // create producer threads
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pids[i] = i + 1;
        pthread_create(&prodThreads[i], NULL, producer, &pids[i]);
    }

    // create consumer threads
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        cids[i] = i + 1;
        pthread_create(&consThreads[i], NULL, consumer, &cids[i]);
    }

    // join producer threads
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(prodThreads[i], NULL);
    }

    // join consumer threads
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consThreads[i], NULL);
    }

    pthread_mutex_destroy(&mutex_lock);
    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);

    cout << "\nAll items produced and consumed successfully ✅" << endl;
    return 0;
}
