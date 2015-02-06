/*Filename : picodb.c*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include "components.h"

/*arrays for arguments and streams*/
char** streams = NULL, ** newArgv = NULL;
int arguments = 1;

/*signal handler function*/
void signal_handler(int signum){
	int i;
	/*free resources*/
	if(streams != NULL){
		for(i = 0; i < 3; i++){
			if(streams[i] != NULL){
				free(streams[i]);
				streams[i] = NULL;
			}
		}
		free(streams);
		streams = NULL;
	}
	if(newArgv != NULL){
		for(i = 0; i < arguments + 1; i++){
			if(newArgv[i] != NULL){
				free(newArgv[i]);
				newArgv[i] = NULL;
			}
		}
		free(newArgv);
		newArgv = NULL;
	}
	kill(0, SIGKILL);
}


int main(int argc, char* argv[]){

	/*Arguments checking*/
	if (argc != 2){
		printf("Wrong arguments. Please give just two.\n");
		printf("picodb and the executable's path.\n");
		return -1;
	}
	
	/*signal handling!*/
	signal(SIGINT, signal_handler);
	signal(SIGHUP, signal_handler);
	signal(SIGTERM, signal_handler);
	
	/*variables for cli*/
	int processControl = PROCESS_CONTROL_OFF, fileManagement = FILE_MANAGEMENT_OFF, limitTraceNumber = 0, blockingMode = B_MODE_OFF, i, quited = 0, callsTraced = 0, insyscall = 0;
	int fileManagementTraced = 0, processControlTraced = 0;
	
	/*variables for ptrace*/
	long ptraceReturnValue, valueEax, valueEbx, valueEcx, valueEdx;
	
	
	
	do{
		/*command line interface call*/
		cli(&limitTraceNumber, &processControl, &fileManagement, &streams, &blockingMode, &arguments, &newArgv, &quited);
		fileManagementTraced = 0;
		processControlTraced = 0;
		if(quited){
			break;
		}
		strcpy(newArgv[0], argv[1]);
		
		/*pipes and pids for redirecting*/
		int pipes[3][2], redirectChildren[3], status;
		
		for(i = 0; i < 3; i++){
			if(pipe(pipes[i]) < 0){
				printf("pipe failed!!\n");
			}
		}
		/*fork to "monitor the child"*/
		pid_t pid = fork();
		
		/*if fork did not succeed*/
		if(pid < 0){
			perror("fork");
			return -1;
		}
		
		/*child case*/
		if(pid == 0){
			/*redirection if necessary and then close the pipes*/
			if(streams != NULL){
				for(i = 0; i < 3; i++){
					if(streams[i] != NULL){
						char str[7];
						if(i == 0){
							strcpy(str, "stdin");
							
							dup2(pipes[i][0], STDIN_FILENO);
							close(pipes[i][0]);
							close(pipes[i][1]);
							
						}
						else if(i == 1){
							strcpy(str, "stdout");
							
							
							dup2(pipes[i][1], STDOUT_FILENO);
							close(pipes[i][0]);
							close(pipes[i][1]);
							
						}
						else{
							strcpy(str, "stderr");
							
							
							dup2(pipes[i][1], STDERR_FILENO);
							close(pipes[i][0]);
							close(pipes[i][1]);
							
						}
						printf("stream %s is redirected to %s\n", str, streams[i]);
					}
					else{
						close(pipes[i][0]);
						close(pipes[i][1]);
					}
				}
			}
			else{
				for(i = 0; i < 3; i++){
					close(pipes[i][0]);
					close(pipes[i][1]);
				}
			}
			/*ptrace call and execv*/
			ptrace(PTRACE_TRACEME, 0, NULL, NULL);
			execv((const char*)argv[1], newArgv);
			perror("execv");
			printf("this will never print!\n");
			return -1;
		}
		/*father case*/
		else{
			if(streams != NULL){
				for(i = 0; i < 3; i++){
					if(streams[i] != NULL){
						/*stdin case*/
						if(i == 0){
							close(pipes[i][0]);
							redirectChildren[i] = fork();
							if(redirectChildren[i] < 0){
								perror("fork");
							}
							/*if child write to pipe and exit*/
							else if(redirectChildren[i] == 0){
								fromFileToPipe(pipes[i][1], streams[i]);
								close(pipes[i][1]);
								exit(0);
							}
							else{
								close(pipes[i][1]);
							}
						}
						/*stdout case*/
						else if(i == 1){
							close(pipes[i][1]);
							redirectChildren[i] = fork();
							if(redirectChildren[i] < 0){
								perror("fork");
							}
							/*if child read from pipe and exit*/
							else if(redirectChildren[i] == 0){
								fromPipeToFile(pipes[i][0], streams[i]);
								close(pipes[i][0]);
								exit(0);
							}
							else{
								close(pipes[i][0]);
							}
						}
						/*stderr case*/
						else{
							close(pipes[i][1]);
							redirectChildren[i] = fork();
							if(redirectChildren[i] < 0){
								perror("fork");
							}
							/*if child read from pipe and exit*/
							else if(redirectChildren[i] == 0){
								fromPipeToFile(pipes[i][0], streams[i]);
								close(pipes[i][0]);
								exit(0);
							}
							else{
								close(pipes[i][0]);
							}
						}
					}
					/*if no redirection close pipes*/
					else{
						close(pipes[i][0]);
						close(pipes[i][1]);
					}
				}
			}
			/*if no redirection close pipes*/
			else{
				for(i = 0; i < 3; i++){
					close(pipes[i][0]);
					close(pipes[i][1]);
				}
			}
			int ptraceArgument = PTRACE_SYSCALL;
			char answer[10];
			/*tracing loop*/
			while(1){
				waitpid(pid, &status, WUNTRACED | WCONTINUED);
				if(WIFSIGNALED(status) || WIFEXITED(status)){
					break;
				}
				/*limit trace checking*/
				if(limitTraceNumber > 0){
					if(processControl == PROCESS_CONTROL_ON && fileManagement == FILE_MANAGEMENT_ON){
						if(processControlTraced >= limitTraceNumber && fileManagementTraced >= limitTraceNumber){
							ptraceArgument = PTRACE_CONT;
						}
					}
					else if(processControl == PROCESS_CONTROL_ON){
						if(processControlTraced >= limitTraceNumber){
							ptraceArgument = PTRACE_CONT;
						}
					}
					else if(fileManagement == FILE_MANAGEMENT_ON){
						if(fileManagementTraced >= limitTraceNumber){
							ptraceArgument = PTRACE_CONT;
						}
					}
				}
				if(WIFSTOPPED(status)){
					ptraceReturnValue = ptrace(PTRACE_PEEKUSER, pid, MYORIG*MULTIPLIER, NULL);
					callsTraced++;
					if(callsTraced != 1){
						if(insyscall == 0){
							/*syscall entry*/
							insyscall = 1;
							if(handleTrace(ptraceReturnValue, limitTraceNumber, processControl, fileManagement, &processControlTraced, &fileManagementTraced, 1) > 0){
								valueEbx = ptrace(PTRACE_PEEKUSER, pid, MULTIPLIER*RBX_OR_EBX, NULL);
								valueEcx = ptrace(PTRACE_PEEKUSER, pid, MULTIPLIER*RCX_OR_ECX, NULL);
								valueEdx = ptrace(PTRACE_PEEKUSER, pid, MULTIPLIER*RDX_OR_EDX, NULL);
								printf("Called with %ld, %ld, %ld\n", valueEbx, valueEcx, valueEdx);
								/*if blocking take answers*/
								if(blockingMode == B_MODE_ON && ptraceArgument != PTRACE_CONT){
									printf("give y for continue tracing or n for stop tracing.\n");
									gets(answer);
									if(!strcmp(answer, "n") || !strcmp(answer, "N")){
										ptraceArgument = PTRACE_CONT;
									}
									else if(strcmp(answer, "y") && strcmp(answer, "Y")){
										printf("Remember only n or y are correct answers.\n");
									}
								}
							}
						}
						else{
							/*syscall exit*/
							insyscall = 0;
							if(handleTrace(ptraceReturnValue, limitTraceNumber, processControl, fileManagement, &processControlTraced, &fileManagementTraced, 0) > 0){
								valueEax = ptrace(PTRACE_PEEKUSER, pid, MULTIPLIER*RAX_OR_EAX, NULL);
								printf("Returned with %ld\n", valueEax);
							}
						}
					}
					/*next ptrace*/
					ptrace(ptraceArgument, pid, 0, 0);
				}
			}
			
		}
		/*free resources*/
		if(streams != NULL){
			for(i = 0; i < 3; i++){
				if(streams[i] != NULL){
					free(streams[i]);
					streams[i] = NULL;
				}
			}
			free(streams);
			streams = NULL;
		}
		if(newArgv != NULL){
			for(i = 0; i < arguments + 1; i++){
				if(newArgv[i] != NULL){
					free(newArgv[i]);
					newArgv[i] = NULL;
				}
			}
			free(newArgv);
			newArgv = NULL;
		}
		printf("This execution is over.\n");
		printf("Total system-calls made : %d\n", processControlTraced + fileManagementTraced);
	}while(!quited);
	printf("picodb Done!!\n");
	
	
	
	return 0;
}
