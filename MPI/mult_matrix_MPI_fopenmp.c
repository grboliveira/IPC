#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define NROWS  1000
#define NCOLS  1000
#define MEM_SIZE 1073741824

/*
 *Multiplicação de matrizes com MPI
 */ 
int *mult_matrix(int *matA, int *matB, int size);
int *transp_matrix(int *mat);


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
		int *matC = malloc(NROWS * NCOLS * sizeof(int));
		srand(time(NULL));
		for (int i = 0; i < NROWS; i++)
		{
			for (int j = 0; j < NCOLS; j++)
			{
				matA[(i*NROWS)+j] = rand() % 1000;
				matB[(i*NROWS)+j] = rand() % 1000;
			}
		} 
		struct timeval start, end;
	  	gettimeofday(&start, NULL);	
	  	int sizepart = NROWS/(numtasks-1);
	  	int subsizepart = ((sizepart*NCOLS)*sizeof(int))/MEM_SIZE;
	  	for (int i = 1; i < numtasks; i++)
	  	{
	  		MPI_Send(&subsizepart,1,MPI_INT,i,0,MPI_COMM_WORLD);
	  	}
	  	//printf("Saiu do for que envia o tamanho\n");
	  	int abort = 0;
	  	int *matransp = transp_matrix(matB);
	  	for (int i = 0; i < subsizepart; i=i+subsizepart)
	  	{
	  		for (int j = 1 ; j < numtasks;j++)
			{
	 			//printf("Master:NROWS/sizepart:%d\n",NROWS/sizepart);
	 			//printf("sizepart*N:%d\n",sizepart*NCOLS);
	 			MPI_Send(&abort,1,MPI_INT,j,3,MPI_COMM_WORLD);
	  			MPI_Send(&matA[(j-1)*subsizepart],subsizepart,MPI_INT,j,1,MPI_COMM_WORLD);
	  			//printf("Enviou o matA\n"); 		
	  			//printf("(J-1)%d\n", j-1); 		
	  			MPI_Send(&matransp[i*subsizepart],subsizepart,MPI_INT,j,2,MPI_COMM_WORLD);
	  			//printf("Enviou o buffer\n");
	 					
	  		}
	  		for (int j = 1; j < numtasks; j++)
	  		{
	  			//printf("Entrou no for do result\n");
	  			int *resultpart = malloc(subsizepart * subsizepart * sizeof(int));
	  			//printf("Master:Alocou o resultpart\n");
	  			MPI_Recv(resultpart, subsizepart, MPI_INT, j, 4, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	  			//printf("Master:Recebeu o resultado\n");
	  			int pos = i*NCOLS;
	  			for(int k = 0 ;k < (subsizepart/NCOLS); k++)
	  			{
	  				for(int l = 0;l < (subsizepart/NROWS);l++)
	  				{
	  					matC[((i+k)*NCOLS)+l] = resultpart[((i+k)*(subsizepart/NROWS))+l];
	  				}
	  			}
	  			//printf("Master:Recebeu o resultado:j%d\n",j);
	  		}
	  		//printf("Master:Executou a leva%d\n",i );

	  	}	
	  		
	  		
	  			
	  	
	  		
	  	
	  	//printf("Saiu da mult\n");
	  
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
			int *result = mult_matrix(rows,columns,size);
			//printf("Slave%d:Calculou a mult_matrix\n",rank);	
			MPI_Send(&result[0],size*size,MPI_INT,0,4,MPI_COMM_WORLD);	
			//printf("Slave%d:Enviou o result\n",rank);
		}while(1);		
	}
	MPI_Finalize();
	return 0;
}

int *mult_matrix(int *matA, int *matB,int size){
	int *result = malloc(size * sizeof(int));
	int count = 0;

	#pragma omp parallel firstprivate(result) private(count)
	{
	#pragma omp for schedule(static)	
	for (int i = 0; i < (size/NCOLS); i++)
	{

		for (int j = 0; j < (size/NROWS); j++)
		{
			count = 0;
			count += matA[(i*NROWS)+j]*matB[(i*NROWS)+j];		
		}
		result[i] = count;
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

	return result;
	
}

int *transp_matrix(int *mat){
	int *transp = malloc(NROWS * NCOLS * sizeof(int));
	#pragma omp parallel firstprivate(transp)
	{
	#pragma omp for schedule(static)	
	for (int i = 0; i < NROWS ; i++)
	{
		for (int j = 0; i < NCOLS; ++i)
		{
			transp[(i*NCOLS)+j] = mat[(j*NCOLS)+i];	
		}
	}
	}
	return transp;
}
