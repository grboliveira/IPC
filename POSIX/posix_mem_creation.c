#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#define SIZE 268435456

int main (int argc, char **argv){
	const char *name_object = "Reserved Memory";
	int *ptr;
	
	//create a shared memory object
	int shm_fd = shm_open(name_object,O_CREAT | O_RDWR, 0666);
	//set the size of the object
	ftruncate(shm_fd,SIZE);
	//map the memory object
	ptr = mmap(0,SIZE,PROT_READ | PROT_WRITE,MAP_SHARED,shm_fd,0);
	
	*ptr = 1234;
	
	pid_t child_pid;

	child_pid = fork();

	if(child_pid < 0){
		perror("fork");
	}

	else if(child_pid == 0){
		int resp;
		resp = *ptr;
		printf("Valor de resp:%d\n",resp);
	}	
		
	return 0;	
}	
