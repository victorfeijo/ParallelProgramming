#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>

#define BUFFER_SIZE 1000  // Buffer size
#define N_PRODUCER 10    // Number of producers
#define N_CONSUMER 10    // Number of consumers

sem_t empty;
sem_t full;
sem_t lock_prod;
sem_t lock_cons;

int buffer[BUFFER_SIZE];
int i = 0;
int f = 0;

void *Consumer (void *arg) {
    while (1) {
        sem_wait(&full);
        sem_wait(&lock_cons);
        i = (i+1) % BUFFER_SIZE;
        buffer[i] = 0;
        printf ("Consuming the %d index\n", i);
        sem_post(&lock_cons);
        sem_post(&empty);
    }
}

void *Producer (void *arg) {
    while (1) {
        sem_wait(&empty);
        sem_wait(&lock_prod);
        f = (f+1) % BUFFER_SIZE;
        buffer[f] = f;
        printf("Producing the %d index\n", f);
        sem_post(&lock_prod);
        sem_post(&full);
    }
}

int main() {

    pthread_t consumer_thread[N_CONSUMER];
    pthread_t producer_thread[N_PRODUCER];

    sem_init(&empty, 0, BUFFER_SIZE);      // empty = BUFFER_SIZE
    sem_init(&full, 0, 0);                 // full = 0
    sem_init(&lock_prod, 0, 1);            // lock_prod = 1
    sem_init(&lock_cons, 0, 1);            // lock_cons = 1

    int j;

    // Create the N_CONSUMER threads and N_PRODUCER threads
    for (j=0; j<N_CONSUMER; j++) {
        pthread_create(&consumer_thread[j], NULL, Consumer, NULL);
    }

    for (j=0; j<N_PRODUCER; j++) {
        pthread_create(&producer_thread[j], NULL, Producer, NULL);
    }

    // Join the N_CONSUMER threads and N_PRODUCER threads
    for (j=0; j<N_CONSUMER; j++) {
        pthread_join(consumer_thread[j], NULL);
    }

    for (j=0; j<N_PRODUCER; j++) {
        pthread_join(producer_thread[i], NULL);
    }

    sem_destroy(&full);
    sem_destroy(&empty);
    sem_destroy(&lock_prod);
    sem_destroy(&lock_cons);

    pthread_exit(NULL);

    return 0;
}
