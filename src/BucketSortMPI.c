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

  if (rank == 0) {
      //------Send and Receive--------
      //------buffer[0]=tamanhoBalde-------
      //------buffer[1]=idBucket-------
      //------buffer[2]=-1->Finish------
  	  int rankEnvio;
  	  int buffer[3];
      int buffer2[3];
      int *aux;
      for (i=0; i<n_buckets; i++) {
          //Recebe o rank do processo que está livre
          MPI_Recv(&rankEnvio, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &st);
          printf("Enviando balde %d para escravo %d\n", i, rankEnvio);
          buffer[0] = balde[i].tamanho;
          buffer[1] = i;
          //Se já estiver no ultimo bucket, seta o buffer[2] que finalizou
	      if (i+1 == n_buckets){
	  	      buffer[2] = -1;
          }
          //Envia primeiramente o Buffer, contendo o tamanho,id e finish
          MPI_Send(&buffer, 3, MPI_INT, rankEnvio, 0, MPI_COMM_WORLD);
          //Envia o vetor para ser ordenado pelos escravos
          MPI_Send(balde[i].values, balde[i].tamanho, MPI_INT, rankEnvio, 0, MPI_COMM_WORLD);


          //O mestre recebe o buffer que foi recebido anteriormente-------------
          //------buffer[0]=tamanho do balde ordenado pelo escravo--------------
          //------buffer[1]= id do balde ordenado pelo escravo------------------
          //------buffer[2]= rank do escravo que enviou o balde ordenado--------
          MPI_Wait();
          MPI_Recv(&buffer2, 3, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &st);
          printf("Recebendo balde ORDENADO %d do escravo %d \n", buffer2[1], buffer2[2]);
          //O mestre recebe o vetor ordenado, com os atributos do recebidor do
          //buffer--------------------------------------------------------------
          aux = malloc(sizeof(int)*buffer[0]);
          MPI_Recv(aux, buffer2[0], MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &st);
          balde[buffer2[1]].tamanho = buffer2[0];
          balde[buffer2[1]].values = aux;
      }
  }
  else {
      //------Send and Receive--------------------------------------------------
      //------buffer[0]=tamanhoBalde--------------------------------------------
      //------buffer[1]=idBucket------------------------------------------------
      //------buffer[2]=-1->Finish-----------------------------------------------
      int buffer[3];
      int *valuesBalde;
      buffer[2] = 0;
      while(buffer[2] != -1){
          //Envia para o mestre o rank do escravo, dizendo que está livre para
          //ordenar um vetor de um balde nao ordenado---------------------------
          MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
          //Recebe o buffer contendo informações sobre o bucket-----------------
    	  MPI_Recv(&buffer, 3, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &st);
    	  valuesBalde = malloc(sizeof(int)*buffer[0]);
          //Recebe o vetor de tamanho buffer[0], armazenando na variável--------
          //valuesBalde---------------------------------------------------------
    	  MPI_Recv(valuesBalde, buffer[0], MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &st);
    	  bubble_sort(valuesBalde, buffer[0]);
          printf("Ordenando bucket...");
          //Envia devolta o buffer para o mestre, com informaçoes do bucket já
          //ordenado e seta buffer[2] com o rank--------------------------------
          buffer[2] = rank;
          MPI_Send(&buffer, 3, MPI_INT, 0, 0, MPI_COMM_WORLD);
          //Envia para o mestre o vetor ordenado--------------------------------
          MPI_Send(valuesBalde, buffer[0], MPI_INT, 0, 0, MPI_COMM_WORLD);
      }
  }
  MPI_Finalize();
  //------------Reordenação-------
  int cont = 0;
  for (i = 0; i<n_buckets; i++) {
      for (j = 0; j<balde[i].tamanho; j++) {
        vetor[cont] = balde[i].values[j];
        printf(" %d, ", vetor[cont]);
        cont++;
      }
  }

  free(vetor);
  free(balde);

  return 0;

}
