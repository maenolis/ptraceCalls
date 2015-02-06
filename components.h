/*Filename : components.h*/
#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_

/*ptrace args*/
#ifndef __x86_64__
#	define RAX_OR_EAX EAX
#	define RBX_OR_EBX EBX
#	define RCX_OR_ECX ECX
#	define RDX_OR_EDX EDX
#	define MYORIG ORIG_EAX
#	define MULTIPLIER 4
#else
#	define RAX_OR_EAX RAX
#	define RBX_OR_EBX RBX
#	define RCX_OR_ECX RCX
#	define RDX_OR_EDX RDX
#	define MYORIG ORIG_RAX
#	define MULTIPLIER 8
#endif

#define PROCESS_CONTROL_ON 1
#define PROCESS_CONTROL_OFF 0
#define FILE_MANAGEMENT_ON 1
#define FILE_MANAGEMENT_OFF 0

#define B_MODE_ON 1
#define B_MODE_OFF 0

void cli(int* _limitTraceNumber, int* _processControl, int* _fileManagement, char*** _streams, int* _blockingMode, int* _arguments, char*** _newArgv, int* quited);
void getStringInPieces(char* _string,char*** _stringInPieces, int* _pieces);
void emptyStringInPieces(char*** _stringInPieces, int* _pieces);
int properCommand(int _pieces, int rightNumber, const char* str);
void printHelp();
int handleTrace(const long _ptraceReturnValue,const int limit, const int _processControl, const int _fileManagement, int* _processControlTraced, int* _fileManagementTraced, int flag);
int handleTraceFileManagement(const long _ptraceReturnValue);
int handleTraceProcessControl(const long _ptraceReturnValue);
void fromFileToPipe(const int fd, char* filename);
void fromPipeToFile(const int fd, char* filename);

#endif
