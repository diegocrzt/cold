#include "coldaemon.h"
#include <string.h>

void * coredaemon(void * argumento)
{
	thread_arg arg = *((thread_arg * ) argumento);
	char print_buffer[512];
	char buffer[16384];
	char resp[16384];
	int len;

	sprintf(resp,"I'm the thread %d, of cold\n",arg.thread_index);

	if(send(arg.socket_descriptor, resp, strlen(resp),0) == -1)
	{
		syslog(LOG_ERR,"No se puede enviar\n");
	}

	if( ( len = recv(arg.socket_descriptor, buffer, 16384,0) ) == -1 ) 
	{
		syslog(LOG_ERR, "no se puede recibir\n");
	}

	while( len > 0)
	{
		buffer[len-1]='\0';
		sprintf(resp,"Su mensaje fue '%s'\n", buffer);
		
		if(send(arg.socket_descriptor, resp,strlen(resp),0) == -1)
        	{
                	syslog(LOG_ERR,"No se puede enviar\n");
        	}

		if( ( len = recv(arg.socket_descriptor, buffer, 16384,0) ) == -1 )
        	{
                	syslog(LOG_ERR, "no se puede recibir\n");
        	}

	}

	sprintf(print_buffer,"Hilo %d cerrando la conexión\n", arg.thread_index);
	
	write(arg.log_fd,print_buffer,strlen(print_buffer));
	close(arg.socket_descriptor);

	ready[arg.thread_index] = 1;
	return;
}
