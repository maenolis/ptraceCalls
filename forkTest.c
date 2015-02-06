/*Filename : forkTest.c*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){

	char temp = 'c';
	
	while((temp = getc(stdin)) != EOF){
		fputc(temp, stdout);
		fputc(temp, stderr);
	}
	
	printf("forkTest just finished!\n");
	
	int i, child;
	for(i = 0; i < 10; i++){
		child = fork();
		if(child < 0){
			perror("fork");
		}
		else if(child == 0){
			printf("getpid = %d\n", getpid());
			exit(0);
		}
		else{
			printf("getpid = %d\n", getpid());
			waitpid(child, NULL, 0);
		}
	}
	
	return 0;
}
