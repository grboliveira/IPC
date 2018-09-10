#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main (int argc, char **argv){
	pid_t pro1_pid;
	pro1_pid = fork();

	if(pro1_pid < 0){
		perror("fork");
	}

	else if(pro1_pid == 0){
		printf("Child process\n");
		execve("./print2",argv,NULL);
	}

	else{
		printf("Parent process\n");
		execve("./print1",argv,NULL);
	}
	
	return 0;
}	
