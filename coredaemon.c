#include "coldaemon.h"
#include <string.h>

void * coredaemon(void * argumento)
{
	thread_arg arg = *((thread_arg * ) argumento);
	char print_buffer[512];


	sprintf(print_buffer,"hilo %d durmiendo por 20 segundos\n", arg.thread_index);
	write(arg.log_fd,print_buffer,strlen(print_buffer));

	system("sleep 20");
	
	
	ready[arg.thread_index] = 1;
	return;
}
