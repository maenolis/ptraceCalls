/*Filename : components.c*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include "components.h"

/*function to implement command line interface and set variables to user's commands*/
void cli(int* _limitTraceNumber, int* _processControl, int* _fileManagement, char*** _streams, int* _blockingMode, int* _arguments, char*** _newArgv, int* quited){
	/*flushing all streams*/
	fflush(NULL);
	/*initializing variables*/
	int gone = 0;
	char command[512];
	char** commandInPieces = NULL;
	int i;
	int pieces = 0;
	*_processControl = PROCESS_CONTROL_OFF;
	*_fileManagement = FILE_MANAGEMENT_OFF;
	printf("\n\n\tCli start.\n");
	printf("\tPlease give your commands.\n");
	while(!gone && !(*quited)){
		printf("\t");
		gets(command);
		printf("\t");
		/*split given command*/
		getStringInPieces(command, &commandInPieces, &pieces);
		if((strcmp(commandInPieces[0], "trace") == 0) || (strcmp(commandInPieces[0], "t") == 0)){
			printf("trace command given.\n");
			if(!properCommand(pieces, 2, "trace")){
				continue;
			}
			if(strcmp(commandInPieces[1], "process-control") == 0){
				*_processControl = PROCESS_CONTROL_ON;
				*_fileManagement = FILE_MANAGEMENT_OFF;
			}
			else if(strcmp(commandInPieces[1], "file-management") == 0){
				*_fileManagement = FILE_MANAGEMENT_ON;
				*_processControl = PROCESS_CONTROL_OFF;
			}
			else if(strcmp(commandInPieces[1], "all") == 0){
				*_processControl = PROCESS_CONTROL_ON;
				*_fileManagement = FILE_MANAGEMENT_ON;
			}
			else{
				printf("Try help command.\n");
			}
		}
		else if((strcmp(commandInPieces[0], "redirect") == 0) || (strcmp(commandInPieces[0], "r") == 0)){
			printf("redirect command given.\n");
			if(!properCommand(pieces, 3, "redirect")){
				continue;
			}
			if((*_streams) == NULL){
				(*_streams) = malloc(3*sizeof(char*));
				if((*_streams) == NULL){
					printf("malloc failed.\n");
					return;
				}
				for(i = 0; i < 3; i++){
					(*_streams)[i] = NULL;
				}
			}
			FILE* tempFilePtr;
			if(strcmp(commandInPieces[1], "stdin") == 0){
				if((*_streams)[0] == NULL){
					(*_streams)[0] = malloc(128*sizeof(char));
				}
				if((*_streams)[0] == NULL){
					printf("malloc failed.\n");
					return;
				}
				strcpy((*_streams)[0], commandInPieces[2]);
				tempFilePtr = fopen((*_streams)[0], "r");
				if(tempFilePtr == NULL){
					printf("Wrong filename : %s for stdin.\n", (*_streams)[0]);
					continue;
				}
				fclose(tempFilePtr);
			}
			else if(strcmp(commandInPieces[1], "stdout") == 0){
				if((*_streams)[1] == NULL){
					(*_streams)[1] = malloc(128*sizeof(char));
				}
				if((*_streams)[1] == NULL){
					printf("malloc failed.\n");
					return;
				}
				strcpy((*_streams)[1], commandInPieces[2]);
				tempFilePtr = fopen((*_streams)[1], "w");
				if(tempFilePtr == NULL){
					printf("Something was wrong with filename : %s for stdout.\n", (*_streams)[1]);
					continue;
				}
				fclose(tempFilePtr);
			}
			else if(strcmp(commandInPieces[1], "stderr") == 0){
				if((*_streams)[2] == NULL){
					(*_streams)[2] = malloc(128*sizeof(char));
				}
				if((*_streams)[2] == NULL){
					printf("malloc failed.\n");
					return;
				}
				strcpy((*_streams)[2], commandInPieces[2]);
				tempFilePtr = fopen((*_streams)[2], "w");
				if(tempFilePtr == NULL){
					printf("Something was wrong with filename : %s for stderr.\n", (*_streams)[2]);
					continue;
				}
				fclose(tempFilePtr);
			}
			else{
				printf("Try help command.\n");
			}
		}
		else if((strcmp(commandInPieces[0], "blocking-mode") == 0) || (strcmp(commandInPieces[0], "b") == 0)){
			printf("blocking-mode command given.\n");
			if(!properCommand(pieces, 2, "blocking-mode")){
				continue;
			}
			char mode[5];
			sscanf(commandInPieces[1], "%s", mode);
			if(strcmp(mode, "on") == 0){
				*_blockingMode = B_MODE_ON;
			}
			else if(strcmp(mode, "off") == 0){
				*_blockingMode = B_MODE_OFF;
			}
			else{
				printf("Try help command.\n");
			}
		}
		else if((strcmp(commandInPieces[0], "limit-trace") == 0) || (strcmp(commandInPieces[0], "l") == 0)){
			printf("limit-trace command given.\n");
			if(!properCommand(pieces, 2, "limit-trace")){
				continue;
			}
			int tempLimit;
			if(sscanf(commandInPieces[1], "%d", &tempLimit) == 0){
				printf("Please give a number for trace-limit.\n");
				continue;
			}
			*_limitTraceNumber = tempLimit;
			
		}
		else if((strcmp(commandInPieces[0], "go") == 0) || (strcmp(commandInPieces[0], "g") == 0)){
			printf("go command given.\n");
			*_arguments = pieces - 1;
			printf("*_arguments = %d\n", (*_arguments));
			(*_newArgv) = malloc((*_arguments + 2)*sizeof(char*));
			if((*_newArgv) == NULL){
				printf("malloc failed.\n");
			}
			for(i = 0; i < (*_arguments) + 1; i++){
				(*_newArgv)[i] = malloc(56*sizeof(char));
				if((*_newArgv)[i] == NULL){
					printf("malloc failed.\n");
				}
				if(i == 0){
					continue;
				}
				strcpy((*_newArgv)[i], commandInPieces[i]);
			}
			(*_newArgv)[*_arguments + 1] = NULL;
			gone = 1;
			printf("Cli end.\n");
		}
		else if((strcmp(commandInPieces[0], "quit") == 0) || (strcmp(commandInPieces[0], "q") == 0)){
			printf("quit command given.\n");
			if(!properCommand(pieces, 1, "quit")){
				continue;
			}
			*quited = 1;
		}
		else if((strcmp(commandInPieces[0], "help") == 0) || (strcmp(commandInPieces[0], "h") == 0)){
			printf("help command given.\n");
			if(!properCommand(pieces, 1, "help")){
				continue;
			}
			printHelp();
			continue;
		}
		else{
			printf("Try help command.\n");
		}
	}
	emptyStringInPieces(&commandInPieces, &pieces);
}



/*fuction for spliting a string into pieces*/
void getStringInPieces(char* _string,char*** _stringInPieces, int* _pieces){
	emptyStringInPieces(_stringInPieces, _pieces);
	int count1 = 0, count2 = 0;
	char temp;
	char** moreWords = NULL;
	moreWords = realloc(*_stringInPieces, sizeof(char*));
	if(moreWords == NULL){
		printf("Reallocating memory.\n");
		return;
	}
	*_stringInPieces = moreWords;
	(*_stringInPieces)[*_pieces] = malloc(128 * sizeof(char));
	if((*_stringInPieces)[*_pieces] == NULL){
		printf("Allocating memory.\n");
		return;
	}
	/*split command to words*/
	do{
		temp = _string[count1++];
		if(temp == ' '){
			(*_stringInPieces)[*_pieces][count2] = '\0';
			*_pieces = *_pieces + 1;
			moreWords = realloc(*_stringInPieces, (*_pieces + 1) * sizeof(char*));
			if(moreWords == NULL){
				printf("Reallocating memory.\n");
				return;
			}
			(*_stringInPieces) = moreWords;
			(*_stringInPieces)[*_pieces] = malloc(128 * sizeof(char));
			if((*_stringInPieces)[*_pieces] == NULL){
				printf("Allocating memory.\n");
				return;
			}
			count2 = 0;
			continue;
		}
		(*_stringInPieces)[*_pieces][count2++] = temp;
	}while(temp != '\0');
	*_pieces = *_pieces + 1;
}

/*function for cleaning a string*/
void emptyStringInPieces(char*** _stringInPieces, int* _pieces){
	if(*_pieces == 0){
		return;
	}
	int i;
	if(*_stringInPieces != NULL){
		for(i = 0; i < *_pieces; i++){
			if((*_stringInPieces)[i] != NULL){
				free((*_stringInPieces)[i]);
				(*_stringInPieces)[i] = NULL;
			}
		}
		free(*_stringInPieces);
		*_stringInPieces = NULL;
	}
	*_pieces = 0;
}


/*check if command was right*/
int properCommand(int _pieces, int rightNumber, const char* str){
	if(_pieces != rightNumber){
		printf("Please give a proper %s command.\n", str);
		return 0;
	}
	return 1;
}

/*printing help*/
void printHelp(){
	printf("\n");
	printf("==========================================================================================================================================\n");
	printf("PICODB HELP!!\n");
	printf("\n");
	printf("\ttrace <category> :\n");
	printf("\t\tselect system calls that you want to trace\n");
	printf("\t\toptions are : process-control, file-management, all\n");
	printf("\n");
	printf("\tredirect <stream> <filename> :\n");
	printf("\t\tselect witch stream you want to redirect to witch file\n");
	printf("\t\toptions are : stdin, stdout, stderr\n");
	printf("\t\tfilename can be any name for stdout and stderr. for stdin the file must exist though\n");
	printf("\n");
	printf("\tblocking-mode <mode> :\n");
	printf("\t\tselect for the programm to stop and ask for every system call or not\n");
	printf("\t\toptions : on or off\n");
	printf("\t\tnote that if it is on programm will ask at every system call trace to continue or no\n");
	printf("\t\tpossible answers to this question are y or n\n");
	printf("\n");
	printf("\tlimit-trace <number> :\n");
	printf("\t\tset a maximum number for traced calls for every category\n");
	printf("\n");
	printf("\tgo :\n");
	printf("\t\tthis option makes the picodb programm to start\n");
	printf("\n");
	printf("\tquit :\n");
	printf("\t\tthis option makes the picodb programm to stop\n");
	printf("\n");
	printf("\thelp :\n");
	printf("\t\tthis option makes the picodb programm print command line interface options\n");
	printf("\t\tbut you already knew this. \n\t\tDidn't you?? :)\n");
	printf("\n");
	printf("******************************************************************************************************************************************\n");
	printf("\tNOTE : \n");
	printf("\tOnly your last given option will be used by the programm.\n");
	printf("******************************************************************************************************************************************\n");
	printf("\n");
	printf("==========================================================================================================================================\n");
	printf("END HELP!!\n");
	printf("\n");
}

/*function for handling traces and counting traced calls*/
int handleTrace(const long _ptraceReturnValue,const int limit, const int _processControl, const int _fileManagement, int* _processControlTraced, int* _fileManagementTraced, int flag){
	
	if(_processControl == PROCESS_CONTROL_ON){
		if(limit == 0 || *_processControlTraced < limit){
			if(handleTraceProcessControl(_ptraceReturnValue)){
				if(flag == 1){
					*_processControlTraced = *_processControlTraced + 1;
					printf("processControlTraced = %d\n", *_processControlTraced);
				}
				return 1;
			}
		}
	}
	if(_fileManagement == FILE_MANAGEMENT_ON){
		if(limit == 0 || *_fileManagementTraced < limit){
			if(handleTraceFileManagement(_ptraceReturnValue)){
				if(flag == 1){
					*_fileManagementTraced = *_fileManagementTraced + 1;
					printf("fileManagementTraced = %d\n", *_fileManagementTraced);
				}
				return 1;
			}
		}
	}
	return 0;
}

/*handle file management*/
int handleTraceFileManagement(const long _ptraceReturnValue){
	char str[20];
	strcpy(str, "not traced");
	if(_ptraceReturnValue == SYS_open){
		strcpy(str, "SYS_open");
	}
	else if(_ptraceReturnValue == SYS_close){
		strcpy(str, "SYS_close");
	}
	else if(_ptraceReturnValue == SYS_read){
		strcpy(str, "SYS_read");
	}
	else if(_ptraceReturnValue == SYS_write){
		strcpy(str, "SYS_write");
	}
	if(strcmp(str, "not traced") != 0){
		printf("Executable made a system call for %s, ptrace returned : %ld.\n", str, _ptraceReturnValue);
		return 1;
	}
	return 0;
}

/*handle process control*/
int handleTraceProcessControl(const long _ptraceReturnValue){
	char str[20];
	strcpy(str, "not traced");
	if(_ptraceReturnValue == SYS_clone){
		strcpy(str, "SYS_clone");
	}
	else if(_ptraceReturnValue == SYS_execve){
		strcpy(str, "SYS_execve");
	}
	else if(_ptraceReturnValue == SYS_wait4){
		strcpy(str, "SYS_wait4");
	}
	else if(_ptraceReturnValue == SYS_kill){
		strcpy(str, "SYS_kill");
	}
	if(strcmp(str, "not traced") != 0){
		printf("Executable made a system call for %s, ptrace returned : %ld.\n", str, _ptraceReturnValue);
		return 1;
	}
	return 0;
}

/*write from pipe to file*/
void fromPipeToFile(const int fd, char* filename){
	
	FILE* stream2 = fopen(filename, "wb");
	if(stream2 == NULL){
		printf("fopen failed\n");
		return;
	}
	int bytesRead;
	
	char temp;
	do{
		bytesRead = read(fd, &temp, 1);
		fwrite(&temp, 1, 1, stream2);
	}while(bytesRead > 0);
	
	fclose(stream2);
}

/*write from file to pipe*/
void fromFileToPipe(const int fd, char* filename){
		
	FILE* stream2 = fopen(filename, "rb");
	int bytesRead;
	if(stream2 == NULL){
		printf("fopen failed\n");
		return;
	}
	char temp;
	
	do{
		bytesRead = fread(&temp, 1, 1, stream2);
		write(fd, &temp, 1);
	}while(bytesRead > 0);
	
	fclose(stream2);
}

