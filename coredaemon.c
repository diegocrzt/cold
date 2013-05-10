#include "coldaemon.h"

void * coredaemon(void * argumento)
{
	thread_arg arg = *((thread_arg * ) argumento);
	

	ready[arg.thread_index] = 1;
	return;
}
