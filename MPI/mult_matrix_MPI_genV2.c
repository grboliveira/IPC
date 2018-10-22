#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#define NROWS  4096
#define NCOLS  4096
#define MEM_SIZE 64

/*
 *Multiplicação de matrizes com MPI
 */ 

int *mult_matrix(int *matA, int *matB,int size){
	int *result = malloc(size * size * sizeof(int));
	int count = 0;


	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			count = 0;
			count += matA[(i*size)+j]*matB[(i*size)+j];		
		}
		result[i] = count;
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

	return result;
	
}

int *transp_matrix(int *mat){
	int *transp = malloc(NROWS * NCOLS * sizeof(int));
	for (int i = 0; i < NROWS ; i++)
	{
		for (int j = 0; i < NCOLS; ++i)
		{
			transp[(i*NCOLS)+j] = mat[(j*NCOLS)+i];	
		}
	}
	return transp;
}

void populate_matrix(int *mat){
	srand(time(NULL));
	for(int i = 0; i < NROWS; i++){
		for(int j = 0; j < NCOLS; j++){
			mat[(i*NROWS)+j] = rand() % 100;
		}
	}
}	


int rank;
int main(int argc, char **argv){
	MPI_Init(&argc, &argv);
	int numtasks;
	MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	if (rank == 0)
	{
		int *matA = malloc(NROWS * NCOLS * sizeof(int));
		int *matB = malloc(NROWS * NCOLS * sizeof(int));
		int *matC = malloc(NROWS * NCOLS * sizeof(int));	
		struct timeval start, end;	
	  	int sizepart = NROWS/(numtasks-1);
	  	int subsizepart = sizepart/MEM_SIZE;
		int piecemat = subsizepart*subsizepart;
		int num_pieces = NROWS/subsizepart;
		int abort;
		int pos;
		int *matransp;
		populate_matrix(matA);
		populate_matrix(matB);
	  	for (int i = 1; i < numtasks; i++)
	  	{
	  		MPI_Send(&subsizepart,1,MPI_INT,i,0,MPI_COMM_WORLD);
	  	}
	  	abort = 0;
	  	matransp = transp_matrix(matB);
		gettimeofday(&start, NULL);	
	  	for (int iS = 0; iS < num_pieces  ; iS++)
	  	{
	  		for(int i = 0;i < num_pieces; i+=(numtasks-1))
			{
				for (int j = 1 ; j < numtasks;j++)
				{
	 				MPI_Send(&abort,1,MPI_INT,j,3,MPI_COMM_WORLD);
	  				MPI_Send(&matA[(i+(j-1))*piecemat],piecemat,MPI_INT,j,1,MPI_COMM_WORLD);
	  				MPI_Send(&matransp[iS*piecemat],piecemat,MPI_INT,j,2,MPI_COMM_WORLD);
	 					
	  			}
	  			for (int j = 1; j < numtasks; j++)
	  			{
					int *resultpart = malloc(piecemat * sizeof(int));
					int m = 0;
					int count = 0;
	  				MPI_Recv(resultpart, piecemat, MPI_INT, j, 4, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	  				pos = i*NCOLS;
	  				for(int k = 0 ;m < piecemat; k+=(NCOLS-subsizepart))
	  				{
				
						count++;
	  					for(int l = 0;l < subsizepart;l++,m++)
	  					{
							matC[((i+(j-1))*piecemat)+k+l] = resultpart[m];
						}
	  				}
	  			}

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
		int *rows = malloc(size* size * sizeof(int));
		int *columns = malloc(size * size * sizeof(int));
		do{
			MPI_Recv(&abort, 1, MPI_INT, 0, 3, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			if (abort == 1)
				break;
			MPI_Recv(rows, size*size, MPI_INT, 0, 1, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			MPI_Recv(columns, size*size, MPI_INT, 0, 2, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			int *result = mult_matrix(rows,columns,size);
			MPI_Send(&result[0],size*size,MPI_INT,0,4,MPI_COMM_WORLD);	
		}while(1);		
	}
	MPI_Finalize();
	return 0;
}


