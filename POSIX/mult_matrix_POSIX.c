#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#define NROWS  1000
#define NCOLS  1000

/*
 *Multiplicação de matrizes
 */ 
void mult_matrix(int *matA, int *matB);



int main(int argc, char **argv){
	const char *name_object1 = "matA";
	const char *name_object2 = "matB";
	//Declaring pointers for memory
	int *matA;
	int *matB;
        //Opening first memShared
	int shm_fd = shm_open(name_object1, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, NROWS * NROWS * sizeof(int));
	matA = mmap(0,NROWS * NCOLS * sizeof(int), PROT_READ | PROT_WRITE,MAP_SHARED,shm_fd,0);
	//Opening second memShared
	shm_fd = shm_open(name_object2, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, NROWS * NCOLS * sizeof(int));
	matB = mmap(0,NROWS * NCOLS * sizeof(int), PROT_READ | PROT_WRITE,MAP_SHARED,shm_fd,0);
	//Populate the matrixs
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
	const char *name_result = "matC";
	int *result;
	int shm_fd = shm_open(name_result, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, NROWS * NCOLS * sizeof(int));
	result = mmap(0,NROWS * NCOLS * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd,0);

	int count = 0; 
	
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
	





