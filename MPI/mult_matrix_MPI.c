#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#define NROWS  1000
#define NCOLS  1000

/*
 *Multiplicação de matrizes com MPI
 */ 
void mult_matrix(int *matA, int *matB,int argc, char **argv);



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
	mult_matrix(matA,matB,argc,argv);
	gettimeofday(&end, NULL);
	printf("Tempo total:%lu\n",((end.tv_sec * 1000000 + end.tv_usec)
		  - (start.tv_sec * 1000000 + start.tv_usec)));
	return 0;
}

void mult_matrix(int *matA, int *matB,int argc, char **argv){
	int *result = malloc(NROWS * NCOLS * sizeof(int));
	int count = 0;
	int numtasks;
	int rank;	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	printf("NumTasks:%d\nRank:%d\n",numtasks,rank);
	if (rank == 0)
	{
		for (int i = rank; i < NROWS; i=i+numtasks)
		{
			for (int j = 0; j < NROWS; j++)
			{
				count = 0;
				for (int k = 0; k < NCOLS ; k++)
				{									
					int msg = matA[(i*NROWS)+k]*matB[(k*NROWS)+j];
					MPI_Send(&msg,1,MPI_INT,1,0,MPI_COMM_WORLD);	 
				}
				result[(i*NROWS)+j] = count;
			}
			
		}
	}

	if (rank == 1)
	{
		int sum;
		MPI_Recv(&sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		count += sum;
		printf("Count:%d\n", count);
	}
	
	
		//printf("Número de Threads:%d\n",omp_get_num_threads());
	
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
	MPI_Finalize();
}
