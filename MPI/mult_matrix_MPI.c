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
void mult_matrix(int *matA, int *matB);



int rank;
int main(int argc, char **argv){
	MPI_Init(&argc, &argv);
	int numtasks;
	char flag [9];
	MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	printf("NumTasks:%d\nRank:%d\n",numtasks,rank);
	if (rank == 0)
	{
		int *matA = malloc(NROWS * NCOLS * sizeof(int));
		int *matB = malloc(NROWS * NCOLS * sizeof(int));
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
	  	MPI_Send(&matA,1,MPI_INT,1,0,MPI_COMM_WORLD);
	  	MPI_Send(&matB,1,MPI_INT,1,1,MPI_COMM_WORLD);
		mult_matrix(matA,matB);
		MPI_Recv(&flag, 12, MPI_CHAR, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		gettimeofday(&end, NULL);
		printf("Tempo total:%lu\n",((end.tv_sec * 1000000 + end.tv_usec)
			  - (start.tv_sec * 1000000 + start.tv_usec)));
			
	}
	if (rank == 1)
	{
		int *matA = malloc(NROWS * NCOLS * sizeof(int));
		int *matB = malloc(NROWS * NCOLS * sizeof(int));
		char message [] = "Finished";
		MPI_Recv(&matA, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Recv(&matB, 1, MPI_INT, 0, 1, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		mult_matrix(matA,matB);
		MPI_Send(&message,9,MPI_CHAR,1,2,MPI_COMM_WORLD);
	}
	MPI_Finalize();
	return 0;
}


void mult_matrix(int *matA, int *matB){
	int *result = malloc(NROWS * NCOLS * sizeof(int));
	int count = 0;

	if (rank == 0)
	{
		for (int i = 0; i < (NROWS/2); i=i++)
		{
			for (int j = 0; j < NROWS; j++)
			{
				count = 0;
				for (int k = 0; k < NCOLS ; k++)
				{									
					count += matA[(i*NROWS)+k]*matB[(k*NROWS)+j];	 
				}
				result[(i*NROWS)+j] = count;
			}
			
		}
	}

	if (rank == 1)
	{
		for (int i = NROWS/2; i < NROWS; i=i++)
		{
			for (int j = 0; j < NROWS; j++)
			{
				count = 0;
				for (int k = 0; k < NCOLS ; k++)
				{									
					count += matA[(i*NROWS)+k]*matB[(k*NROWS)+j];	 
				}
				result[(i*NROWS)+j] = count;
			}
			
		}
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
	
}
