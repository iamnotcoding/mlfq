#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prioque.h"


void mlfq()
{
	int Clock = 0;

	init_all_queues();
	init_process(&IdleProcess);
	read_process_description();

	while (processes.exist())
	{
		Clock++;
		queue_new_arrivals();
		execute_highest_priority_process();
		do_IO();
	}

	Clock++;
	final_report();
}

void init_all_queues(void)
{
	Queue firstQ;
	Queue secondQ;
	Queue thirdQ;
	Queue ioQ;
}

void init_process(Process *IdleProcess)
{
}

void read_process_description(void)
{
	Process p;
	ProcessBehavior b;
	int pid = 0, first = 1;
	unsigned long arrival;

	init_process(&p);
	arrival = 0;

	while (scanf("%lu", &arrival) != EOF)
	{
		scanf("%d %lu %lu %d", &pid, &b.cpuburst, &b.ioburst, &b.repeat);

		if (!first && p.pid != pid)
		{
			add_to_queue(&ArrivalQ, &p, p, arrival_time);
			init_process(&p);
		}

		p.pid = pid;
		p.arrival_time = arrival;
		first = 0;
		add_to_queue(&p.behaviors, &b, 1);
	}

	add_to_queue(&ArrivalQ, &b, p.arrival_time);
}
