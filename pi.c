#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h> 

int main(int argc, char* argv[]) {
   long long n;
   struct timeval start, stop;
   double factor = 1.0;
   double sum = 0.0;
   int thread_count=1;

   thread_count = strtol(argv[1], NULL, 10);
   n = strtoll(argv[2], NULL, 10);

   gettimeofday(&start, NULL);
   
   #pragma omp parallel 
   {
    #pragma omp for reduction(+:sum)
    for (long long i = 0; i < n; i++) {
      sum += (i&1) ? -1.0/(2*i+1) : 1.0/(2*i+1);
   }
  }
   

   sum = 4.0*sum;
  
   gettimeofday(&stop, NULL);

   double t = (((double)stop.tv_sec)*1000.0 + ((double)(stop.tv_usec)/1000.0)) - \
             (((double)start.tv_sec)*1000.0 + ((double)(start.tv_usec)/1000.0));

   fprintf(stdout, "Time elapsed: %g ms\n", t);

   printf("With n = %lld terms and %d threads,\n", n, thread_count);
   printf("   Our estimate of pi = %.14f\n", sum);
   printf("                   pi = %.14f\n", 4.0*atan(1.0));
   
   return 0;

}