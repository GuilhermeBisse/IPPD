#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>


double *vetor;
long vsize;
double sum = 0.0;
long thread_count;
pthread_mutex_t mtx;

void *mvthread(void* id){
  long i;
  long thread_id = (long)id;
  long batch = vsize/thread_count;
  long start = batch*thread_id;
  long remainder = (vsize%thread_count);
  long stop = thread_id == thread_count-1 ?  vsize : start+batch;

  long local_sum = 0;
  for (i=start; i<stop; i++) {
    local_sum += vetor[i];
  }
  pthread_mutex_lock(&mtx);
  sum += local_sum;
  pthread_mutex_unlock(&mtx);
}



int main(int argc, char *argv[])
{
  pthread_mutex_init(&mtx, NULL);

  struct timeval start, stop;

  pthread_t *thread_handles;

  thread_count = strtol(argv[1], NULL, 10);
  thread_handles = malloc(thread_count*sizeof(pthread_t));
  
  vsize = strtol(argv[2], NULL, 10);

  vetor = (double *)malloc(vsize*sizeof(double));
  
  long i;
  for (i=0; i<vsize; i++) 
    vetor[i] = (double)i;
  
  gettimeofday(&start, NULL);
  /* FORK Cria as threads informadas na linha de comando */
  for (i=0; i<thread_count; i++) {
    if (pthread_create(&thread_handles[i], NULL, mvthread, (void *)i) != 0) {
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

  free(thread_handles);


  double temp = \
    (((double)(stop.tv_sec)*1000.0 + (double)(stop.tv_usec/1000.0)) - \
    ((double)(start.tv_sec)*1000.0 + (double)(start.tv_usec/1000.0)));

  fprintf(stdout, "Time elapsed = %g ms\n", temp);
  fprintf(stdout, "Sum = %g\n", sum);

  pthread_mutex_destroy(&mtx);

  return 0;
}
