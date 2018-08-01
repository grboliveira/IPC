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
	  	for (int i = 1; i < numtasks; i++)
	  	{
	  		MPI_Send(matB,(NROWS*NCOLS),MPI_INT,i,0,MPI_COMM_WORLD);
	  	}	
	  	int i = 0;
	  	int rank_recv;
	  	while(i < NROWS){
	  		MPI_Recv(&rank_recv, 1, MPI_INT, MPI_ANY_SOURCE,MPI_ANY_TAG, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	  		MPI_Send(&matA[i],NCOLS,MPI_INT,rank_recv,MPI_ANY_TAG,MPI_COMM_WORLD);
	  		MPI_Send(&i,1,MPI_INT,rank_recv,MPI_ANY_TAG,MPI_COMM_WORLD);
	  		i++;
	  	}
		gettimeofday(&end, NULL);
		for (int i = 1; i < numtasks; i++)
	  	{
	  		MPI_Send(&i,(NROWS*NCOLS),MPI_INT,i,MPI_ANY_TAG,MPI_COMM_WORLD);
	  	}	
		printf("Finished\n");
		printf("Tempo total:%lu\n",((end.tv_sec * 1000000 + end.tv_usec)
			  - (start.tv_sec * 1000000 + start.tv_usec)));
			
	}
	else if (rank != 0)
	{
		int i = 0;
		int *matA = malloc(NCOLS * sizeof(int));
		int *matB = malloc(NROWS * NCOLS * sizeof(int));
		MPI_Recv(matB, (NROWS*NCOLS), MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		while(i < NCOLS){
			MPI_Send(&rank,1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD);
			MPI_Recv(matA, NCOLS, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			mult_matrix(matA,matB);
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
