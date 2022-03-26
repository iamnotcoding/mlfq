#define _GNU_SOURCE

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "print-err.h"
#include "process.h"
#include "queue.h"

int TimeCmp(const void *pA, const void *pB)
{
	int result;

	if (((Process *)pA)->time > ((Process *)pB)->time)
	{
		result = 1;
	}
	else if (((Process *)pA)->time < ((Process *)pB)->time)
	{
		result = -1;
	}
	else
	{
		result = 0;
	}

	return result;
}

void Delay(double s)
{
	usleep(s * 1000000);
}

void MLFQ(FILE *out, Processes processes, double delaySec)
{
	Processes processesCopy;
	Queue *pJobQueue = CreateQueue(processes.processesNum);
	Queue *pReadyQueueHigh = CreateQueue(processes.processesNum);
	Queue *pReadyQueueMid = CreateQueue(processes.processesNum);
	Queue *pReadyQueueLow = CreateQueue(processes.processesNum);
	Queue *pCurReadyQueue = pReadyQueueHigh;
	unsigned clock = 0;
	unsigned quantum;
	unsigned curQueueLev;

	if (pJobQueue == NULL)
	{
		PRINT_ERR("CreateQueue() failed");

		return;
	}

	processesCopy.processList =
		malloc(sizeof *(processes.processList) * processes.processesNum);

	if (processesCopy.processList == NULL)
	{
		char tempStr[100];

		sprintf(tempStr, "processesCopy.processList allocation failed : %s",
				strerror(errno));

		PRINT_ERR(tempStr);

		return;
	}

	memcpy(processesCopy.processList, processes.processList,
		   sizeof *processes.processList * processes.processesNum);

	processesCopy.processesNum = processes.processesNum;

	// going to use processesCopy instead of processes from now on

	// sort process by time
	qsort(processesCopy.processList, processesCopy.processesNum,
		  sizeof *(processesCopy.processList), TimeCmp);

	// enqueues all the processes in the time order
	for (size_t i = 0; i < processesCopy.processesNum; i++)
	{
		EnQueue(pJobQueue, processesCopy.processList[i]);
	}

	// ==================== schduling starts ==================== //

	// if there's at lest one process to execute
	if (processesCopy.processesNum > 0)
	{
		Process curProcess;

		do
		{
			clock++;

			// if a process should enter at current clock value
			if (!IsQueueEmpty(*pJobQueue) && clock >= Peek(*pJobQueue).time)
			{
				EnQueue(pReadyQueueHigh, DeQueue(pJobQueue));

				Delay(delaySec);

				fprintf(
					out,
					"CREATE: Process %u entered the ready queue at time %u\n",
					Peek(*pReadyQueueHigh).pid, clock);
			}

			// if there are proceses to execute in the hightest prioriy queue
			if (!IsQueueEmpty(*pReadyQueueHigh))
			{
				pCurReadyQueue = pReadyQueueHigh;

				curQueueLev = 1;
				quantum = 10;
			}
			// if the hightest prioriy queue is empty and,
			// if there are proceses to execute in the middle priority queue.
			else if (!IsQueueEmpty(*pReadyQueueMid))
			{
				pCurReadyQueue = pReadyQueueMid;

				curQueueLev = 2;
				quantum = 30;
			}
			// if the hightest prioriy queue is empty and,
			// if the middle priority queue is empty and,
			// if there are proceses to execute in the lowest priority queue.
			else if (!IsQueueEmpty(*pReadyQueueLow))
			{
				pCurReadyQueue = pReadyQueueLow;

				curQueueLev = 3;
				quantum = 100;
			}
			// if there are no processes to execute
			else
			{
				// if there are no process to execute and the job queue is empty
				if (IsQueueEmpty(*pJobQueue))
				{
					break;
				}
				else
				{
					// waits for a new process which is in the jobqueue
					continue;
				}
			}

			// gets a process from the readyqueue
			curProcess = DeQueue(pCurReadyQueue);

			// if quantum is samller than or equal to repeat, runs quantum
			// times else if qunatum is bigger than repeat, runs repeat
			// times
			while (curProcess.remainingTicks != 0)
			{
				Delay(delaySec);

				fprintf(out,
						"RUN: Process %u started execution from level %u "
						"at time %u; wants to execute for %u ticks.\n",
						curProcess.pid, curQueueLev, clock,
						curProcess.remainingTicks);

				if (quantum < curProcess.remainingTicks)
				{
					clock += quantum;

					curProcess.g++;
					curProcess.remainingTicks -= quantum;
				}
				else
				{
					clock += curProcess.remainingTicks;

					curProcess.b++;
					curProcess.remainingTicks = 0;
				}

				// GetQueueLevel(curProcess, curQueueLev);
			}

			if (curProcess.repeat > 0)
			{
				Delay(delaySec);

				fprintf(out, "I/O: Process %u blocked for I/O at time %u.\n",
						curProcess.pid, clock);

				clock += curProcess.io;

				curProcess.remainingTicks = curProcess.run;
				curProcess.repeat--;

				EnQueue(pJobQueue, curProcess);
			}
			else
			{
				curProcess.repeat = 0;
				fprintf(out, "FINISHED: Process %u finished at time %u.\n",
						curProcess.pid, clock);
			}
		} while (true);
	}

	// frees all queues
	CloseProcesses(processesCopy);
	DestroyQueue(pJobQueue);

	DestroyQueue(pReadyQueueHigh);
	DestroyQueue(pReadyQueueMid);
	DestroyQueue(pReadyQueueLow);
}
