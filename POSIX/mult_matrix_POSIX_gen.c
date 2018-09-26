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
#include <omp.h>
#include <sys/wait.h>
#define NROWS  5000
#define NCOLS  5000
#define MEM_SIZE 1024

/*
 *Multiplicação de matrizes
 */ 
void mult_matrix(int *matA, int *matB, int *matC, int id, int subsizepart);



int main(int argc, char **argv){	
	//printf("Iniciou o programa\n");
	
	const char *name_object1 = "matA";
	const char *name_object2 = "matB";
	const char *name_object3 = "matResult";
	//Declaring pointers for memory
	int *matA;
	int *matB;
	int *matC;
        //Opening first memShared
	int shm_fd = shm_open(name_object1, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, NROWS * NROWS * sizeof(int));
	matA = mmap(0,NROWS * NCOLS * sizeof(int), PROT_READ | PROT_WRITE,MAP_SHARED,shm_fd,0);
	//Opening second memShared
	shm_fd = shm_open(name_object2, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, NROWS * NCOLS * sizeof(int));
	matB = mmap(0,NROWS * NCOLS * sizeof(int), PROT_READ | PROT_WRITE,MAP_SHARED,shm_fd,0);
	//Opening Third memShared
	shm_fd = shm_open(name_object3, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, NROWS * NCOLS * sizeof(int));
	matC = mmap(0,NROWS * NCOLS * sizeof(int), PROT_READ | PROT_WRITE,MAP_SHARED,shm_fd,0);
	//Populate the matrixs
	//printf("Criou as mems compart\n");
	srand(time(NULL));
	for (int i = 0; i < NROWS; i++)
	{
		for (int j = 0; j < NCOLS; j++)
		{
			matA[(i*NROWS)+j] = rand() % 100;
			matB[(i*NROWS)+j] = rand() % 100;
		}
	}

	int sizepart = NROWS/(omp_get_max_threads());
        int subsizepart = (sizepart *  sizeof(int))/MEM_SIZE;
	printf("sizepart:%d\n",sizepart);
	printf("subsizepart:%d\n",(subsizepart));
	int id = 0;	
	pid_t child_pid;
	for(int i = 1; i < omp_get_max_threads();i++){
		child_pid = fork();
		if(child_pid < 0){
			perror("fork");
			exit(0);
		}
		else if(child_pid == 0){
			 id = i;
			 break;
		}	

	}	
	//printf("Criou os processos\n");
	struct timeval start, end;
	gettimeofday(&start, NULL);
       	printf("ID:%d\n",id);
	mult_matrix(matA,matB,matC,id,subsizepart);	
	gettimeofday(&end, NULL);
	if(child_pid == 0)exit(id);
	else{
		for(int i = 1;i < omp_get_max_threads();i++){
			pid_t son_pid = waitpid(child_pid,NULL,WUNTRACED); 
			printf("ChildPID:%d\ti:%d\n",son_pid,i);
		}
	}	
	printf("Tempo total:%lu\n",((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
	return 0;
}

void mult_matrix(int *matA, int *matB, int *matC, int id, int subsizepart){
	printf("Id:%d and subsizepart:%d\n",id,subsizepart);
	int count;	
	for (int iM = id * subsizepart ; iM < NCOLS; iM += (subsizepart * omp_get_max_threads()))
	{	
		for (int jM = id * subsizepart; jM < NCOLS ; jM += (subsizepart * omp_get_max_threads()))
		{
			//printf("jM:%d\n",jM);
			for (int kM = id * subsizepart ; kM < NCOLS ; kM += (subsizepart * omp_get_max_threads()))
			{
				//printf("kM:%d\n",kM);
				#pragma omp for schedule(static) private(count)	
				for(int i = iM ; i < (subsizepart + iM ); i++)
				{
				       for(int j = jM; j < (subsizepart  + jM); j++)
				       {
						count = 0;
						for(int k = kM; k < (subsizepart + kM); k++)
						{
							count += (matA[(i*NCOLS)+k] * matB[(k*NCOLS)+j]);
						}
						matC[(i*NROWS)+j] = count;
					}	
				}	
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
	





