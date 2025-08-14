#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define SIZE 1000

long int M[SIZE][SIZE];
long int v1[SIZE], v2[SIZE];

int thread_count; 

void *minha_thread(void* arg)
{
  long my_id = (long)arg;
  int meu_trabalho = SIZE/thread_count;

  int i_inicial, i_final, j;
  i_inicial = my_id*meu_trabalho;
  i_final = i_inicial + meu_trabalho;

  if(my_id == thread_count-1){
    i_final = SIZE;
  }
  for (int i=i_inicial; i<i_final; i++) {
    v2[i] = 0;
    for(j=0; j<SIZE; j++) {
      v2[i] += M[i][j]*v1[j];
    }
  }
  return NULL;
}


int main(int argc, char *argv[])
{
  pthread_t *thread_handles;
  struct timeval start, stop;

  thread_count = strtol(argv[1], NULL, 10);
  thread_handles = malloc(thread_count*sizeof(pthread_t));

  int i,j;
  for (i=0; i<SIZE; i++) {
    v1[i] = i;
    for (j=0; j<SIZE; j++)
      M[i][j] = i+j;
  }


  gettimeofday(&start, NULL);
  /* FORK Cria as threads informadas na linha de comando */
  for (i=0; i<thread_count; i++) {
    if (pthread_create(&thread_handles[i], NULL, minha_thread, (void *)i) != 0) {
      fprintf(stderr, "Nao consegui criar a thread\n");
      exit(-1);
    }
  }

  /* JOIN */
  for (i=0; i<thread_count; i++)
  {
    pthread_join(thread_handles[i], NULL);
  }
  gettimeofday(&stop, NULL);


  double temp = \
    (((double)(stop.tv_sec)*1000.0 + (double)(stop.tv_usec/1000.0)) - \
    ((double)(start.tv_sec)*1000.0 + (double)(start.tv_usec/1000.0)));

  fprintf(stdout, "Time elapsed = %g ms\n", temp);

  for (i=0; i<SIZE; i++)
    fprintf(stdout, "v[%d]=%ld\n", i, v2[i]);

	return 0;
}
