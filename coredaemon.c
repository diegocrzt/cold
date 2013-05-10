#include "coldaemon.h"
#include <string.h>
#include "hash.h"

void * coredaemon(void * argumento)
{
	thread_arg arg = *((thread_arg * ) argumento);
	char print_buffer[512];
	char buffer[16384];
	char resp[16384];
	int len;
	char usuario[100];
	char clave[100];

	sprintf(resp,"Usuario: \n");

	if(send(arg.socket_descriptor, resp, strlen(resp),0) == -1)
	{
		syslog(LOG_ERR,"No se puede enviar\n");
	}
	if( ( len = recv(arg.socket_descriptor, buffer, 16384,0) ) == -1 ) 
	{
		syslog(LOG_ERR, "No se puede recibir\n");
	}
	if(len == 0)
	{
		close(arg.socket_descriptor);
		syslog(LOG_ERR, "Conexión abortada\n");
	}
	buffer[len -1] = '\0';
	strcpy(usuario,buffer);

	sprintf(resp,"Clave: \n");
	if(send(arg.socket_descriptor, resp, strlen(resp),0) == -1)
	{
		syslog(LOG_ERR,"No se puede enviar\n");
	}
	if( ( len = recv(arg.socket_descriptor, buffer, 16384,0) ) == -1 ) 
	{
		syslog(LOG_ERR, "No se puede recibir\n");
	}
	if(len == 0)
	{
		close(arg.socket_descriptor);
		syslog(LOG_ERR, "Conexión abortada\n");
		return;
	}
	buffer[len -1] = '\0';
	strcpy(clave,buffer);

	// AUTENTICACIÓN
	/*
	if(authentication (arg.acl_file, usuario, hash(clave)) != 0)
	{
		syslog(LOG_ERR,"Fallo de autenticación\n");
		close(arg.socket_descriptor);
		return;
	}
	*/

	sprintf(resp,"Bienvenido al sistema de Cobros en Linea\n");
	if(send(arg.socket_descriptor, resp, strlen(resp),0) == -1)
	{
		syslog(LOG_ERR,"No se puede enviar\n");
	}
	if( ( len = recv(arg.socket_descriptor, buffer, 16384,0) ) == -1 ) 
	{
		syslog(LOG_ERR, "No se puede recibir\n");
	}

	while( len > 0)
	{
		buffer[len-1]='\0';
		
		sprintf(resp,"Su mensaje fue '%s'\n",buffer);
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
