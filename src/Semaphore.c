#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#define MAX_THREADS 128
int var_global = 0;
sem_t semaphore;

void *PrintHello(void *arg) {
    //Mutex da lock na incrementação do var_global, garantindo que
    //apenas uma thread por vez consiga encrementar o valor de var_global
    int j;
    for (j = 0; j<100; j++) {
        sem_wait(&semaphore);
        var_global++;
        sem_post(&semaphore);
    }
}

int main() {

    //Cria um vetor de threads
    pthread_t thread[MAX_THREADS];
    //Inicializa o mutex
    sem_init(&semaphore, 0, 1);
    int i;

    //Cria MAX_THREADS e manda executar o método PrintHello
    for (i = 0; i < MAX_THREADS; i++) {
        pthread_create(&thread[i], NULL, PrintHello, NULL);
    }
    for (i = 0; i < MAX_THREADS; i++) {
        pthread_join(thread[i], NULL);
    }

    printf("%d", var_global);
    sem_destroy(&semaphore);
    pthread_exit(NULL);
    return 0;
}
