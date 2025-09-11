#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define N 15000

//at.bocces@unesp.br

double f(int i)
{
  int j, start = i*(i+1)/2, finish = start+i;
  double return_val = 0.0;
  
  for (j=start; j<=finish; j++)
    return_val += sin(j);

  return return_val;
}

int main(int argc, char *argv[])
{
  FILE* file = fopen("arq.csv", "w"); 
  if (file == NULL) {
       perror("Error opening file");
       return 1;
   }
  
  char* modes[] = {"static", "dynamic", "guided"};
  fprintf(file, "Modo,Tempo_ms,Soma,Total Threads\n");

  int thread_count = strtol(argv[1], NULL, 10);

  for(int m=0; m<3; m++){

    struct timeval start, stop;
    double sum = 0.0;

    setenv("OMP_SCHEDULE", modes[m], 1);
    gettimeofday(&start, NULL);

    #pragma omp parallel for reduction(+:sum) schedule(runtime) num_threads(thread_count)
    for (int i=0; i<N; i++)
        sum += f(i);

    gettimeofday(&stop, NULL);

    double tempo = \
        (((double)(stop.tv_sec)*1000.0 + (double)(stop.tv_usec/1000.0)) - \
        ((double)(start.tv_sec)*1000.0 + (double)(start.tv_usec/1000.0)));

     fprintf(file, "%s,%.2f,%g,%d\n", modes[m], tempo, sum, thread_count);
    
  }
  return 0;
}