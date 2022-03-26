#ifndef MLFQ_H
#define MLFQ_H

#include "prioque.h"

void read_process_description(void);
void init_all_queues(void);
void do_IO(void);
void execute_highest_priority_process(void);
void queue_new_arrivals(void);
void final_report(void);
int processes_exist(void);
void init_process(Process *p);

typedef struct _process_behavior
{
        unsigned long ioburst;
        unsigned cpuburst;
        int repeat;
} ProcessBehavior;

typedef struct _process
{
        int arrival_time;
        int pid;
        int b;
        int g;
        Queue behaviors;
} Process;

#endif
