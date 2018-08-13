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
	char flagA [9];
	char flagB [9];
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
	  	MPI_Send(matB,(NROWS*NCOLS),MPI_INT,1,0,MPI_COMM_WORLD);
	  	MPI_Send(matB,(NROWS*NCOLS),MPI_INT,2,0,MPI_COMM_WORLD);
	  	int j = 1;
	  	for (int i = 0; i < NROWS; i+=2, j++)
	  	{
	  		MPI_Send(&matA[i],NCOLS,MPI_INT,1,j,MPI_COMM_WORLD);
	  		MPI_Send(&matA[i+1],NCOLS,MPI_INT,2,j,MPI_COMM_WORLD);
	  		MPI_Recv(&flagA, 9, MPI_CHAR, 1,j, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	  		MPI_Recv(&flagB, 9, MPI_CHAR, 2,j, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	  	}
		gettimeofday(&end, NULL);
		printf("Rank1:%s\n",flagA);
		printf("Rank2:%s\n",flagB);
		printf("Tempo total:%lu\n",((end.tv_sec * 1000000 + end.tv_usec)
			  - (start.tv_sec * 1000000 + start.tv_usec)));
			
	}
	else if (rank == 1)
	{
		int *matA = malloc(NCOLS * sizeof(int));
		int *matB = malloc(NROWS * NCOLS * sizeof(int));
		char message [] = "Finished";
		MPI_Recv(matB, (NROWS*NCOLS), MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		for (int i = 1; i <= (NROWS/2);i++)
		{
			MPI_Recv(matA, NCOLS, MPI_INT, 0, i, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			mult_matrix(matA,matB);
			MPI_Send(&message,9,MPI_CHAR,0,i,MPI_COMM_WORLD);
		}	
	}
	else if (rank == 2)
	{
		int *matA = malloc(NCOLS * sizeof(int));
		int *matB = malloc(NROWS * NCOLS * sizeof(int));
		char message [] = "Finished";
		MPI_Recv(matB, (NROWS*NCOLS), MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		for (int i = 1; i <= (NROWS/2); i++)
		{
			MPI_Recv(matA, NCOLS, MPI_INT, 0, i, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			mult_matrix(matA,matB);
			MPI_Send(&message,9,MPI_CHAR,0,i,MPI_COMM_WORLD);	
		}
	}
	MPI_Finalize();
	return 0;
}


void mult_matrix(int *matA, int *matB){
	int *result = malloc(NROWS * NCOLS * sizeof(int));
	int count = 0;


	for (int j = 0; j < NROWS; j++)
	{
		count = 0;
		for (int k = 0; k < NCOLS ; k++)
		{									
			count += matA[k]*matB[(k*NROWS)+j];	
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
 