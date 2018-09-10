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
	

	else{
		printf("HelloWord from process:%d\n",getpid());
	}	

}	
