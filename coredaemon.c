#include "coldaemon.h"
#include <string.h>
#include "hash.h"
#include <postgresql/libpq-fe.h>

void fin_hilo(thread_arg arg)
{
	close(arg.socket_descriptor);
	--ready;
	thread_del(&(arg.lista_hilo), arg.thread_index);
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

	n = recv(socket, buffer, len, 0);
	if( n > 0 )
	{
		buffer[n - 1] = '\0';
		limpiar_telnet(buffer);
		--n;
	}

	return n;
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
	int log_fd;
	SERVICIO serv;

	if( (log_fd = open(arg.log, O_CREAT | O_WRONLY | O_APPEND, 0666)) < 0 )
	{ 
		syslog(LOG_ERR,"No se puede abrir el fichero %s (%d)\n",arg.log, log_fd);
		exit(LOG_ERROR);
	}

	writelog(log_fd,"Iniciando Autenticación\n");
	sprintf(resp,"Usuario: ");

	if(send(arg.socket_descriptor, resp, strlen(resp),0) == -1)
	{
		writelog(log_fd,"No se puede enviar\n");
		fin_hilo(arg);
		return;
	}
	if( ( len = recvtimeout(arg.socket_descriptor, buffer, 16384,arg.timeout) ) < 0 ) 
	{
		writelog(log_fd, "No se puede recibir: ");
		if(len == -2)
		{
			writelog(log_fd, "Timeout\n");
		}else{
			writelog(log_fd, "Error de I/O\n");
		}
		fin_hilo(arg);
		return;
	}
	if(len == 0)
	{
		writelog(log_fd, "Conexión abortada\n");
		fin_hilo(arg);
	}
	strcpy(usuario,buffer);

	sprintf(resp,"Clave: ");
	if(send(arg.socket_descriptor, resp, strlen(resp),0) == -1)
	{
		writelog(log_fd,"No se puede enviar\n");
		fin_hilo(arg);
		return;
	}
	if( ( len = recvtimeout(arg.socket_descriptor, buffer, 16384,arg.timeout) ) < 0 ) 
	{
		writelog(log_fd, "No se puede recibir\n");
		if(len == -2)
		{
			writelog(log_fd, "Timeout\n");
		}else{
			writelog(log_fd, "Error de I/O\n");
		}
		fin_hilo(arg);
		return;
	}
	if(len == 0)
	{
		writelog(log_fd, "Conexión abortada\n");
		fin_hilo(arg);
		return;
	}
	strcpy(clave,buffer);

	// AUTENTICACIÓN
	syslog(LOG_ERR,"user '%s' pass = '%s'\n",usuario,clave);
	if(authentication (arg.acl, usuario, hash(clave)) != 0)
	{
		writelog(log_fd,"Fallo de autenticación\n");
		sprintf(resp,"Credenciales inválidas\n");
		if(send(arg.socket_descriptor, resp, strlen(resp),0) == -1)
		{
			writelog(log_fd,"No se puede enviar\n");
		}
		sprintf(temp,"[Hilo %d] Cerrando la conexión\n",arg.thread_index);
		writelog(log_fd,temp);
		fin_hilo(arg);
		return;
	}
	sprintf(temp,"Se autenticó exitosamente al usuario %s\n",usuario);
	writelog(log_fd, temp);

	sprintf(resp,"Bienvenido al sistema de Cobros en Linea\n$ ");
	if(send(arg.socket_descriptor, resp, strlen(resp),0) == -1)
	{
		writelog(log_fd,"No se puede enviar\n");
		fin_hilo(arg);
		return;
	}
	if( ( len = recvtimeout(arg.socket_descriptor, buffer, 16384,arg.timeout) ) < 0 ) 
	{
		writelog(log_fd, "No se puede recibir\n");
		if(len == -2)
		{
			writelog(log_fd, "Timeout\n");
		}else{
			writelog(log_fd, "Error de I/O\n");
		}
		fin_hilo(arg);
		return;
	}

	while( len > 0)
	{
		
		//sprintf(resp,"Su mensaje fue '%s'\n$ ",buffer);
		sprintf(temp,"[DEBUG]Hilo %2d: '%s'\n",arg.thread_index,buffer);
		writelog(log_fd, temp);

		// Motor de Inferencia
		if(strcmp(buffer,"exit") == 0)
		{
			sprintf(temp,"El usuario %s ha cerrado sesión en el hilo %d\n",usuario,arg.thread_index);
			writelog(log_fd, temp);
			fin_hilo(arg);
			return;
		}
		if(strcmp(buffer,"help") == 0)
		{
			sprintf(temp,"IMPRIMIR LA AYUDA EN PANTALLA\n");
			writelog(log_fd, temp);
		}
		if(strcmp(buffer,"lastrx") == 0)
		{
			sprintf(temp,"IMPRIMIR LAS ÚLTIMAS 3 TRANSACCIONES\n");
			writelog(log_fd, temp);
			if( db_module("lastrx", serv,usuario,log_fd,resp) != 0)
			{
	               		writelog(log_fd,"EXPLOTO BD Intentando mostrar las útlimas 3 transacciones\n");
			}else{
				if(send(arg.socket_descriptor, resp,strlen(resp),0) == -1)
        			{
                			writelog(log_fd,"No se puede enviar\n");
					fin_hilo(arg);
					return;
        			}
			}
		}
		if(strncmp(buffer,"col ",4) == 0)
		{
			sprintf(temp,"PARSEANDO MENSAJE DE COBRO EN LINEA\n");
			writelog(log_fd, temp);
			buffer[len-1] = '\n';
			if( col_parser(&serv,buffer+4,log_fd) != 0)
			{
				sprintf(temp,"Patron inválido\n");
				writelog(log_fd, temp);
				sprintf(resp,"Patron inválido. Verifique y reintente\n$ ");
				if(send(arg.socket_descriptor, resp,strlen(resp),0) == -1)
        			{
                			writelog(log_fd,"No se puede enviar\n");
					fin_hilo(arg);
					return;
        			}
			}else{
				//MAGIA
                		writelog(log_fd,"MAGIA DE BD\n");
				//IMPRIMIR LOS RESULTADOS BLAH BLHA BLHA Y LOGGEAR
				if( db_module("rev",serv,usuario,log_fd,resp) != 0)
	                		writelog(log_fd,"CoL EXPLOTO BD\n");
			}
		}
		if(strncmp(buffer,"rev ",4) == 0)
		{
			sprintf(temp,"IMPRIMIR LA AYUDA EN PANTALLA\n");
			writelog(log_fd, temp);
			buffer[len-1] = '\n';
			if( rev_parser(&serv,buffer+4) != 0)
			{
				sprintf(temp,"Patron inválido\n");
				writelog(log_fd, temp);
				sprintf(resp,"Patron inválido. Verifique y reintente\n$ ");
				if(send(arg.socket_descriptor, resp,strlen(resp),0) == -1)
        			{
                			writelog(log_fd,"No se puede enviar\n");
					fin_hilo(arg);
					return;
        			}
			}else{
				//MAGIA
                		writelog(log_fd,"MAGIA DE BD\n");
				//IMPRIMIR LOS RESULTADOS BLAH BLHA BLHA Y LOGGEAR
				if( db_module("rev",serv,usuario,log_fd,NULL) != 0)
	                		writelog(log_fd,"EXPLOTO BD\n");
			}
		}

		sprintf(resp,"$ ");
		if(send(arg.socket_descriptor, resp,strlen(resp),0) == -1)
        	{
                	writelog(log_fd,"No se puede enviar\n");
			fin_hilo(arg);
			return;
        	}

		if( ( len = recvtimeout(arg.socket_descriptor, buffer, 16384,arg.timeout) ) < 0 )
        	{
                	writelog(log_fd, "no se puede recibir\n");
			if(len == -2)
			{
				writelog(log_fd, "Timeout\n");
			}else{
				writelog(log_fd, "Error de I/O\n");
			}
			fin_hilo(arg);
			return;
        	}

	}

	sprintf(print_buffer,"Hilo %d cerrando la conexión\n", arg.thread_index);
	writelog(log_fd,print_buffer);
	
	fin_hilo(arg);
	return;
}

void limpiar_telnet(char * cadena)
{
	int i = 0;
	
	while(cadena[i] != '\0')
	{
		if(cadena[i] == '\015')
		{
			cadena[i] = '\0';
			break;
		} 
		i++;
	}

	return;
}
