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
void mult_matrix(int *matA, int *matB, int size);



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
	  	int sizepart = NROWS/(numtasks-1);
	  	for (int i = 1; i < numtasks; i++)
	  	{
	  		MPI_Send(&sizepart,1,MPI_INT,i,0,MPI_COMM_WORLD);
	  	}
	  	//printf("Saiu do for que envia o tamanho\n");
	  	int *buffer = malloc(sizepart * NROWS * sizeof(int));
	  	int abort = 0;
	  	for (int i = 0; i < NCOLS ; i=i+sizepart)
	  	{
	  		for (int j = 1 ; j < numtasks;j++)
	 		{
	 			//printf("Master:Entrou no for\n");
	 			//printf("sizepart*N:%d\n",sizepart*NCOLS );
	 			MPI_Send(&abort,1,MPI_INT,j,3,MPI_COMM_WORLD);
	  			MPI_Send(&matA[i*sizepart],sizepart*NCOLS,MPI_INT,j,1,MPI_COMM_WORLD);
	  			//printf("Enviou o matA\n"); 		
	  			//printf("(J-1)%d\n", j-1);
	  			int pos = (j-1) * i;
	  			for (int k = 0; k < sizepart*NROWS; k++)
				{	
					//printf("K*NCOLS = %d\n",k*NCOLS);
					buffer[k] = matB[pos*NCOLS];
				}		  			
				//printf("Preencheu o buffer\n"); 		
	  			MPI_Send(&buffer[0],sizepart*NROWS,MPI_INT,j,2,MPI_COMM_WORLD);
	  			//printf("Enviou o buffer\n"); 		
	  		}	
	  	}
	  		
	  	
	  	
	  
		gettimeofday(&end, NULL);
		abort = 1; 
		for (int i = 1; i < numtasks; i++)
		{
			MPI_Send(&abort,1,MPI_INT,i,3,MPI_COMM_WORLD);
		}
			
		printf("Finished\n");
		printf("Tempo total:%lu\n",((end.tv_sec * 1000000 + end.tv_usec)
			  - (start.tv_sec * 1000000 + start.tv_usec)));
			
	}	
	else if (rank != 0)
	{
		int size;
		int abort;
		MPI_Recv(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		int *rows = malloc(NCOLS * size * sizeof(int));
		int *columns = malloc(NROWS * size * sizeof(int));
		do{
			MPI_Recv(&abort, 1, MPI_INT, 0, 3, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			if (abort == 1)
				break;
			//printf("Slave:Entrou no dowhile\n");
			MPI_Recv(rows, size*NCOLS, MPI_INT, 0, 1, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			//printf("Slave:Recebeu as fileiras\n");
			MPI_Recv(columns, size*NROWS, MPI_INT, 0, 2, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			//printf("Slave:Recebeu as linhas e colunas\n");
			mult_matrix(rows,columns,size);	
		}while(1);		
	}
	MPI_Finalize();
	return 0;
}

void mult_matrix(int *matA, int *matB,int size){
	int *result = malloc(NROWS * NCOLS * sizeof(int));
	int count = 0;


	for (int i = 0; i < size; i++)
	{

		for (int j = 0; j < size; j++)
		{
			count = 0;
			for (int k = 0; k < size ; k++)
			{									
				count += matA[(i*NROWS)+k]*matB[(k*NROWS)+j];	
			}
			result[(i*NROWS)+j] = count;
		}
			
	}
	//printf("Slave:Executou a mult_matrix\n");
		
	
	
	
	
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