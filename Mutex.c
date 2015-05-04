#include <pthread.h>
#include <stdio.h>

#define MAX_THREADS 128
int var_global = 0;
pthread_mutex_t mutex;

void *PrintHello(void *arg) {
    //Mutex da lock na incrementação do var_global, garantindo que
    //apenas uma thread por vez consiga encrementar o valor de var_global
    int j;
    for (j = 0; j<100; j++) {
        pthread_mutex_lock(&mutex);
        var_global++;
        pthread_mutex_unlock(&mutex);
    }
}

int main() {

    //Cria um vetor de threads
    pthread_t thread[MAX_THREADS];
    //Inicializa o mutex
    pthread_mutex_init(&mutex, NULL);
    int i;

    //Cria MAX_THREADS e manda executar o método PrintHello
    for (i = 0; i < MAX_THREADS; i++) {
        pthread_create(&thread[i], NULL, PrintHello, NULL);
    }
    for (i = 0; i < MAX_THREADS; i++) {
        pthread_join(thread[i], NULL);
    }

    printf("%d", var_global);
    pthread_mutex_destroy(&mutex);
    pthread_exit(NULL);
    return 0;
}
