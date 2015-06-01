#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

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

int main(int argc, char **argv) {

  int tamanho = atoi(argv[1]);
  int n_buckets = atoi(argv[2]);

  // ---------------Verifica se número Processos < 2 e número bucket < tamanho do vetor-------------
  if (n_processo < 2 || n_buckets < tamanho) {
      printf("Número de processos ou de buckets inválido");
      return 0;
  }

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

  //-------------Criação e chamada dos processos--------------------

  int size, rank;
  MPI_Status st;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int rankEnvio = 1;

  //-------------Send and Receive--------

  if (rank == 0) {
      for (i=0; i<n_buckets; i++) {
          MPI_Send(i, 1, MPI_INT, rankEnvio, NULL, MPI_COMM_WORLD);
          rankEnvio = (rankEnvio%size) + 1;
      }
  }
  else {
      int indice;
      MPI_Status st;
      MPI_Recv(&indice, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &st);
      bubble_sort(balde[indice].values, balde[indice].tamanho);
  }
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
