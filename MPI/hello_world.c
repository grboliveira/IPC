#include "mpi.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
	MPI_Init(&argc,&argv);
	printf("Hello World");
	MPI_Finalize();
	return 0;
}