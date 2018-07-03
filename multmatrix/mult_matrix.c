#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define NROWS  5000
#define NCOLS  5000

/*
 *Multiplicação de matrizes
 */ 
void mult_matrix(int *matA, int *matB);



int main(int argc, char **argv){
	int *matA = malloc(NROWS * NCOLS * sizeof(int));
	int *matB = malloc(NROWS * NCOLS * sizeof(int));
	int numThread = 4;
	srand(time(NULL));
	for (int i = 0; i < NROWS; i++)
	{
		for (int j = 0; j < NCOLS; j++)
		{
			matA[(i*NROWS)+j] = rand() % 100;
			matB[(i*NROWS)+j] = rand() % 100;
		}
	}
	struct timeval start, end;
  	gettimeofday(&start, NULL);
	mult_matrix(matA,matB);
	gettimeofday(&end, NULL);
	printf("Tempo total:%lu\n",((end.tv_sec * 1000000 + end.tv_usec)
		  - (start.tv_sec * 1000000 + start.tv_usec)));
	return 0;
}

void mult_matrix(int *matA, int *matB){
	int *result = malloc(NROWS * NCOLS * sizeof(int));
	int count = 0;

	#pragma omp parallel firstprivate(result) 
	{
	#pragma omp for reduction(+:count) schedule(static)
	for (int i = 0; i < NROWS; i++)
	{
		for (int j = 0; j < NROWS; j++)
		{
			count = 0;
			for (int k = 0; k < NCOLS ; k++)
			{
				
				count += (matA[(i*NROWS)+k]*matB[(k*NROWS)+j]); 
			}
			result[(i*NROWS)+j] = count;
		}
			
	}
		//printf("Número de Threads:%d\n",omp_get_num_threads());
	}
	/*
	for (int i = 0; i < 100; i++)
	{
		printf("|");
		for (int j = 0; j < 100; j++)
		{
			printf("%d\t", result[i][j]);
		}
		printf("|\n");
	}
	*/
}
	





