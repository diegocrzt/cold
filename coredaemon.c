#include "coldaemon.h"
#include <string.h>
#include "hash.h"
#include <postgresql/libpq-fe.h>

void fin_hilo(thread_arg arg)
{
	close(arg.socket_descriptor);
	ready[arg.thread_index] = 1;
	return;
}

int recvtimeout(int socket, char *buffer, int len, int timeout)
{
	fd_set fds;
	int n;
	struct timeval tv;
	FD_ZERO(&fds);
	FD_SET(socket, &fds);
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	n = select(socket+1, &fds, NULL, NULL, &tv);
	if (n == 0)
	{
		return -2; // timeout!
	}
	if (n == -1)
	{
		return -1; // error
	}

	return recv(socket, buffer, len, 0);
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
	SERVICIO serv;

	writelog(arg.log_fd,"Iniciando Autenticación\n");
	sprintf(resp,"Usuario: ");

	if(send(arg.socket_descriptor, resp, strlen(resp),0) == -1)
	{
		writelog(arg.log_fd,"No se puede enviar\n");
		fin_hilo(arg);
		return;
	}
	if( ( len = recvtimeout(arg.socket_descriptor, buffer, 16384,arg.timeout) ) < 0 ) 
	{
		writelog(arg.log_fd, "No se puede recibir: ");
		if(len == -2)
		{
			writelog(arg.log_fd, "Timeout\n");
		}else{
			writelog(arg.log_fd, "Error de I/O\n");
		}
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
	if( ( len = recvtimeout(arg.socket_descriptor, buffer, 16384,arg.timeout) ) < 0 ) 
	{
		writelog(arg.log_fd, "No se puede recibir\n");
		if(len == -2)
		{
			writelog(arg.log_fd, "Timeout\n");
		}else{
			writelog(arg.log_fd, "Error de I/O\n");
		}
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
	if( ( len = recvtimeout(arg.socket_descriptor, buffer, 16384,arg.timeout) ) < 0 ) 
	{
		writelog(arg.log_fd, "No se puede recibir\n");
		if(len == -2)
		{
			writelog(arg.log_fd, "Timeout\n");
		}else{
			writelog(arg.log_fd, "Error de I/O\n");
		}
		fin_hilo(arg);
		return;
	}

	while( len > 0)
	{
		buffer[len-1]='\0';
		
		//sprintf(resp,"Su mensaje fue '%s'\n$ ",buffer);
		sprintf(temp,"[DEBUG]Hilo %2d: '%s'\n",arg.thread_index,buffer);
		writelog(arg.log_fd, temp);

		// Motor de Inferencia
		if(strcmp(buffer,"exit") == 0)
		{
			sprintf(temp,"El usuario %s ha cerrado sesión en el hilo %d\n",usuario,arg.thread_index);
			writelog(arg.log_fd, temp);
			fin_hilo(arg);
			return;
		}
		if(strcmp(buffer,"help") == 0)
		{
			sprintf(temp,"IMPRIMIR LA AYUDA EN PANTALLA\n");
			writelog(arg.log_fd, temp);
		}
		if(strcmp(buffer,"lastrx") == 0)
		{
			sprintf(temp,"IMPRIMIR LAS ÚLTIMAS 3 TRANSACCIONES\n");
			writelog(arg.log_fd, temp);
			if( db_module("lastrx", serv,usuario,arg.log_fd,resp) != 0)
			{
	               		writelog(arg.log_fd,"EXPLOTO BD Intentando mostrar las útlimas 3 transacciones\n");
			}else{
				if(send(arg.socket_descriptor, resp,strlen(resp),0) == -1)
        			{
                			writelog(arg.log_fd,"No se puede enviar\n");
					fin_hilo(arg);
					return;
        			}
			}
		}
		if(strncmp(buffer,"col ",4) == 0)
		{
			sprintf(temp,"PARSEANDO MENSAJE DE COBRO EN LINEA\n");
			writelog(arg.log_fd, temp);
			buffer[len-1] = '\n';
			if( col_parser(&serv,buffer+4) != 0)
			{
				sprintf(temp,"Patron inválido\n");
				writelog(arg.log_fd, temp);
				sprintf(resp,"Patron inválido. Verifique y reintente\n$ ");
				if(send(arg.socket_descriptor, resp,strlen(resp),0) == -1)
        			{
                			writelog(arg.log_fd,"No se puede enviar\n");
					fin_hilo(arg);
					return;
        			}
			}else{
				//MAGIA
                		writelog(arg.log_fd,"MAGIA DE BD\n");
				//IMPRIMIR LOS RESULTADOS BLAH BLHA BLHA Y LOGGEAR
				if( db_module("col",serv,usuario,arg.log_fd,NULL) != 0)
	                		writelog(arg.log_fd,"CoL EXPLOTO BD\n");
			}
		}
		if(strncmp(buffer,"rev ",4) == 0)
		{
			sprintf(temp,"IMPRIMIR LA AYUDA EN PANTALLA\n");
			writelog(arg.log_fd, temp);
			buffer[len-1] = '\n';
			if( rev_parser(&serv,buffer+4) != 0)
			{
				sprintf(temp,"Patron inválido\n");
				writelog(arg.log_fd, temp);
				sprintf(resp,"Patron inválido. Verifique y reintente\n$ ");
				if(send(arg.socket_descriptor, resp,strlen(resp),0) == -1)
        			{
                			writelog(arg.log_fd,"No se puede enviar\n");
					fin_hilo(arg);
					return;
        			}
			}else{
				//MAGIA
                		writelog(arg.log_fd,"MAGIA DE BD\n");
				//IMPRIMIR LOS RESULTADOS BLAH BLHA BLHA Y LOGGEAR
				if( db_module("rev",serv,usuario,arg.log_fd,NULL) != 0)
	                		writelog(arg.log_fd,"EXPLOTO BD\n");
			}
		}

		sprintf(resp,"$ ");
		if(send(arg.socket_descriptor, resp,strlen(resp),0) == -1)
        	{
                	writelog(arg.log_fd,"No se puede enviar\n");
			fin_hilo(arg);
			return;
        	}

		if( ( len = recvtimeout(arg.socket_descriptor, buffer, 16384,arg.timeout) ) < 0 )
        	{
                	writelog(arg.log_fd, "no se puede recibir\n");
			if(len == -2)
			{
				writelog(arg.log_fd, "Timeout\n");
			}else{
				writelog(arg.log_fd, "Error de I/O\n");
			}
			fin_hilo(arg);
			return;
        	}

	}

	sprintf(print_buffer,"Hilo %d cerrando la conexión\n", arg.thread_index);
	
	write(arg.log_fd,print_buffer,strlen(print_buffer));
	
	fin_hilo(arg);
	return;
}

