#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int n_buckets;
int contador = 0;
pthread_mutex_t lock;

typedef struct {
  int *values;
  int tamanho;
}bucket;


void bubble_sort(int *v, int tam){
  int i,j,temp,trocou;
  for(j=0; j<tam-1; j++){
	  trocou = 0;
	  for(i = 0; i < tam -1; i++){
	    if(v[i+1] < v[i]){
		    temp = v[i];
		    v[i] = v[i + 1];
		    v[i + 1] = temp;
		    trocou = 1;
	    }
	  }
	   if(!trocou) break;
  }
}

int incrementar_contador() {

    contador++;

    return contador - 1;
}

void* thread(void *param) {
    bucket *p = (bucket *) param;
    while (1) {
    	pthread_mutex_lock(&lock);
        int c = incrementar_contador();
        pthread_mutex_unlock(&lock);
        if (c >= n_buckets)
            break;
        bubble_sort(p[c].values, p[c].tamanho);
        printf("Thread %d processando balde %d\n ", (int)pthread_self() , c);
    }
    pthread_exit(NULL);
}


int main(int argc, char **argv) {

  int tamanho = atoi(argv[1]);
  int n_threads = atoi(argv[2]);
  n_buckets = atoi(argv[3]);

  // ---------------Cria vetor original--------------------
  int i;
  int *vetor = (int *) malloc(sizeof(int)*tamanho);
  srand((unsigned)time(NULL));
  for (i = 0; i < tamanho; i++) {
    vetor[i] = rand() % (tamanho - 1);
    printf("vetor[%d] = %d \n", i, vetor[i]);
  }

  //------------Cria e organiza os baldes----------------

  bucket *balde = (bucket *) malloc(sizeof(bucket) * n_buckets);
  int minRange = 0;
  int maxRange = tamanho/n_buckets;
  int resto = tamanho%n_buckets;
  int j;
  int qntElemBalde;

  for (i = 0; i<n_buckets; i++) {

      //alocando posição desnecessária (nao tem como o balde ter + que tamanho)
      balde[i].values = (int *) malloc(sizeof(int)*tamanho);
      qntElemBalde = 0;
      for (j = 0; j<tamanho; j++) {
          if(vetor[j] >= minRange && vetor[j] <= maxRange) {
              balde[i].values[qntElemBalde] = vetor[j];
              printf("balde[%d] = %d \n", i, balde[i].values[qntElemBalde]);
              qntElemBalde++;
          }
      }
      balde[i].tamanho = qntElemBalde;
      printf("\nMinRange = %d\nMaxRange = %d\nTamanho = %d \n", minRange, maxRange,qntElemBalde);
      minRange = maxRange + 1;
      //até o valor do resto ele coloca +1 no range do bucket
      if (i < resto) {
          maxRange = minRange + (tamanho/n_buckets);
      }
      else{
      maxRange = minRange + (tamanho/n_buckets) - 1;
      }
  }

  //-------------Criação e chamada das threads--------------------

  pthread_t *threads = (pthread_t *) malloc(sizeof(pthread_t) * n_threads);
  for (i=0; i<n_threads; i++) {
      pthread_create(&threads[i], NULL, thread, (void *) balde);
  }


  for(i = 0; i < n_threads; i++)
      pthread_join(threads[i], NULL);

  //------------Reordenação-------
  int cont = 0;
  for (i = 0; i<n_buckets; i++) {
      for (j = 0; j<balde[i].tamanho; j++) {
        vetor[cont] = balde[i].values[j];
        printf(" %d, ", vetor[cont]);
        cont++;
      }
  }

  free(threads);
  free(balde);

  return 0;

}
