#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
	MPI_Init(NULL,NULL);
	int numtasks;
	int rank;
	char content[12];
	char message[] = "Hello World1";
	MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	printf("NumTasks:%d\nRank:%d\n",numtasks,rank);

	
  	if (rank == 0){
  		MPI_Send(&message,12,MPI_CHAR,1,0,MPI_COMM_WORLD);	
  	}
  	else if (rank == 1){
  		MPI_Recv(&content, 12, MPI_CHAR, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		printf("Message sent:%s\n",content);
  	}
	MPI_Finalize();	
	return 0;
}
