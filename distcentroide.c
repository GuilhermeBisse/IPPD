#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <mpi.h>

int main(int argc, char **argv) {
    int i, j;
    struct timeval start, stop;

    if (argc < 2) {
        printf("Necessário informar o número de pontos.\n");
        exit(-1);
    }
    int npontos = atoi(argv[1]);

    // aloca memória para os pontos (x,y) - i-ésimo ponto está na posição i*2 e
    // i*2+1 do vetor
    int *pvetor = (int *)malloc(npontos*2*sizeof(int));

    // aloca vetor de saída para as distâncias
    double *dist = (double *)malloc(npontos*sizeof(double));

    // gera os pontos de forma aleatória
    srand(0);
    for (i=0; i < npontos; i++) {
        pvetor[i*2] = rand() % 2048;
        pvetor[(i*2)+1] = rand() % 2048;
    }

    int size, rank;
    int *rcvbuffer = (int *)malloc(npontos*2*sizeof(int));  // Buffer para os pontos recebidos
    double *distlocal = (double *)malloc(npontos*sizeof(double));  // Distâncias locais

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int tamanho = npontos * 2 / size;  // Cada processo vai receber tamanho pontos (x, y)

    gettimeofday(&start, NULL);

    // Distribui os pontos entre os processos
    MPI_Scatter(pvetor, tamanho, MPI_INT, rcvbuffer, tamanho, MPI_INT, 0, MPI_COMM_WORLD);

    // Cálculo da centróide em paralelo
    double x_local = 0, y_local = 0;
    int pontos_por_processos = npontos / size;
    
    // Cada processo soma os valores dos pontos que recebeu
    for (i = 0; i < pontos_por_processos; i++) {
        x_local += rcvbuffer[i*2];
        y_local += rcvbuffer[(i*2)+1];
    }

    // Reduz os resultados de cada processo para o processo 0
    double x_total = 0, y_total = 0;
    MPI_Reduce(&x_local, &x_total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&y_local, &y_total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Broadcast da centróide calculada para todos os processos
    if (rank == 0) {
        x_total /= npontos;  // Média total de x
        y_total /= npontos;  // Média total de y
    }

    MPI_Bcast(&x_total, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&y_total, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Agora cada processo calcula a distância de seus pontos para a centróide
    for (i = 0; i < pontos_por_processos; i++) {
        double a = rcvbuffer[i*2], b = rcvbuffer[(i*2)+1];
        distlocal[i] = sqrt((x_total - a) * (x_total - a) + (y_total - b) * (y_total - b));
    }

    // Coleta os resultados (distâncias) no processo 0
    MPI_Gather(distlocal, pontos_por_processos, MPI_DOUBLE, dist, pontos_por_processos, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    gettimeofday(&stop, NULL);

    if (rank == 0) {
      double t = (((double)(stop.tv_sec)*1000.0  + (double)(stop.tv_usec / 1000.0)) - \
              ((double)(start.tv_sec)*1000.0 + (double)(start.tv_usec / 1000.0)));

      fprintf(stdout, "Tempo decorrido = %g ms\n", t);

      printf("Vetor com as distâncias para a centroide (%g,%g):\n", x_total, y_total);
      
      // imprime o vetor com as distâncias
      // for (i = 0; i < npontos; i++) {
      //     printf("%.3f ", dist[i]);
      // }
      // printf("\n");
    }

    MPI_Finalize();

    free(pvetor);
    free(dist);
    free(rcvbuffer);
    free(distlocal);

    return 0;
}

