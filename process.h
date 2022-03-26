#ifndef PROCESS_H
#define PROCESS_H

typedef struct _process
{
	unsigned time;
	unsigned pid;
	unsigned run;
	unsigned io;
	unsigned repeat;

	// process status
	unsigned b;
	unsigned g;
	unsigned remainingTicks;
} Process;

typedef struct _processes
{
	Process *processList; 
	size_t processesNum;
} Processes;

Processes GetProcesses(FILE *in);
void CloseProcesses(Processes processes);

#endif
