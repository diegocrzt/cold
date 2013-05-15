#include "coldaemon.h"
#include <string.h>
#include "hash.h"

void writelog(int log_fd, const char * mensaje)
{
	write(log_fd, mensaje, strlen(mensaje));
	return;
}

void fin_hilo(thread_arg arg)
{
	close(arg.socket_descriptor);
	ready[arg.thread_index] = 1;
	return;
}

void * coredaemon(void * argumento)
{
	thread_arg arg = *((thread_arg * ) argumento);
	char print_buffer[512];
	char buffer[16384];
	char resp[16384];
	char temp[16384];
	int len;
	char usuario[100];
	char clave[100];

	writelog(arg.log_fd,"Iniciando Autenticación\n");
	sprintf(resp,"Usuario: ");

	if(send(arg.socket_descriptor, resp, strlen(resp),0) == -1)
	{
		writelog(arg.log_fd,"No se puede enviar\n");
		fin_hilo(arg);
		return;
	}
	if( ( len = recv(arg.socket_descriptor, buffer, 16384,0) ) == -1 ) 
	{
		writelog(arg.log_fd, "No se puede recibir\n");
		fin_hilo(arg);
		return;
	}
	if(len == 0)
	{
		writelog(arg.log_fd, "Conexión abortada\n");
		fin_hilo(arg);
	}
	buffer[len -1] = '\0';
	strcpy(usuario,buffer);

	sprintf(resp,"Clave: ");
	if(send(arg.socket_descriptor, resp, strlen(resp),0) == -1)
	{
		writelog(arg.log_fd,"No se puede enviar\n");
		fin_hilo(arg);
		return;
	}
	if( ( len = recv(arg.socket_descriptor, buffer, 16384,0) ) == -1 ) 
	{
		writelog(arg.log_fd, "No se puede recibir\n");
		fin_hilo(arg);
		return;
	}
	if(len == 0)
	{
		writelog(arg.log_fd, "Conexión abortada\n");
		fin_hilo(arg);
		return;
	}
	buffer[len -1] = '\0';
	strcpy(clave,buffer);

	// AUTENTICACIÓN
	
	if(authentication (arg.acl_file, usuario, hash(clave)) != 0)
	{
		writelog(arg.log_fd,"Fallo de autenticación\n");
		sprintf(resp,"Credenciales inválidas\n");
		if(send(arg.socket_descriptor, resp, strlen(resp),0) == -1)
		{
			writelog(arg.log_fd,"No se puede enviar\n");
		}
		sprintf(temp,"[Hilo %d] Cerrando la conexión\n",arg.thread_index);
		writelog(arg.log_fd,temp);
		fin_hilo(arg);
		return;
	}
	sprintf(temp,"Se autenticó exitosamente al usuario %s\n",usuario);
	writelog(arg.log_fd, temp);

	sprintf(resp,"Bienvenido al sistema de Cobros en Linea\n$ ");
	if(send(arg.socket_descriptor, resp, strlen(resp),0) == -1)
	{
		writelog(arg.log_fd,"No se puede enviar\n");
		fin_hilo(arg);
		return;
	}
	if( ( len = recv(arg.socket_descriptor, buffer, 16384,0) ) == -1 ) 
	{
		writelog(arg.log_fd, "No se puede recibir\n");
		fin_hilo(arg);
		return;
	}

	while( len > 0)
	{
		buffer[len-1]='\0';
		
		sprintf(resp,"Su mensaje fue '%s'\n$ ",buffer);
		sprintf(temp,"[DEBUG]Hilo %2d: '%s'\n",arg.thread_index,buffer);

		// Motor de Inferencia
		if(strcmp(buffer,"exit") == 0)
		{
			sprintf(temp,"El usuario %s ha cerrado sesión en el hilo %d\n",usuario,arg.thread_index);
			writelog(arg.log_fd, temp);
			fin_hilo(arg);
			return;
		}
		writelog(arg.log_fd, temp);
		if(send(arg.socket_descriptor, resp,strlen(resp),0) == -1)
        	{
                	writelog(arg.log_fd,"No se puede enviar\n");
			fin_hilo(arg);
			return;
        	}

		if( ( len = recv(arg.socket_descriptor, buffer, 16384,0) ) == -1 )
        	{
                	writelog(arg.log_fd, "no se puede recibir\n");
			fin_hilo(arg);
			return;
        	}

	}

	sprintf(print_buffer,"Hilo %d cerrando la conexión\n", arg.thread_index);
	
	write(arg.log_fd,print_buffer,strlen(print_buffer));
	
	fin_hilo(arg);
	return;
}
