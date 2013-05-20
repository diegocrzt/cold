#include "coldaemon.h"
#include <string.h>
#include "hash.h"
#include <postgresql/libpq-fe.h>

void fin_hilo(thread_arg arg)
{
	close(arg.socket_descriptor);
	pthread_mutex_lock(&lock);
	--ready;
	thread_del(&(arg.lista_hilo), arg.thread_index);
	pthread_mutex_unlock(&lock);
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
	time_t current_time;
    char* c_time_string;
 
    /* Obtain current time as seconds elapsed since the Epoch. */
    current_time = time(NULL);
 
    /* Convert to local time format. */
    c_time_string = ctime(&current_time);


	thread_arg arg = *((thread_arg * ) argumento);
	char print_buffer[STR_LEN];
	char buffer[PKG_LEN];
	char resp[PKG_LEN];
	char temp[PKG_LEN];
	int len;
	char usuario[STR_LEN];
	char clave[STR_LEN];
	int log_fd;
	SERVICIO serv;

	free(argumento);

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
	if( ( len = recvtimeout(arg.socket_descriptor, buffer, PKG_LEN,arg.timeout) ) < 0 ) 
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
		// Motor de Inferencia
		if(strcmp(buffer,"close") == 0)
		{
			sprintf(temp,"[%s::%s::close::Conexion terminada]\n",c_time_string,usuario);
			writelog(log_fd, temp);
			fin_hilo(arg);
			return;
		}
		if(strcmp(buffer,"help") == 0)
		{
			//sprintf(temp,"IMPRIMIR LA AYUDA EN PANTALLA\n");
			sprintf(temp,"[%s::%s::help::Ayuda]\n",c_time_string,usuario);
			writelog(log_fd, temp);
			if( db_module("help", serv,usuario,log_fd,resp) != 0)
			{
	               		writelog(log_fd,"Intentando mostrar la ayuda\n");
			}else{
				if(send(arg.socket_descriptor, resp,strlen(resp),0) == -1)
        			{
                			writelog(log_fd,"No se puede enviar\n");
					fin_hilo(arg);
					return;
        			}
			}
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
				//IMPRIMIR LOS RESULTADOS BLAH BLHA BLHA Y LOGGEAR
				if(db_module("col",serv,usuario,log_fd,resp) != 0)
	                		writelog(log_fd,"CoL EXPLOTO BD\n");
				if(send(arg.socket_descriptor, resp,strlen(resp),0) == -1)
        			{
                			writelog(log_fd,"No se puede enviar\n");
					fin_hilo(arg);
					return;
        			}
			}
		}
		if(strncmp(buffer,"rev ",4) == 0)
		{
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
				//IMPRIMIR LOS RESULTADOS BLAH BLHA BLHA Y LOGGEAR
				if(db_module("rev",serv,usuario,log_fd,resp) != 0)
	                		writelog(log_fd,"EXPLOTO BD\n");
				if(send(arg.socket_descriptor, resp,strlen(resp),0) == -1)
        			{
                			writelog(log_fd,"No se puede enviar\n");
					fin_hilo(arg);
					return;
        			}
			}
		}

		sprintf(resp,"$ ");
		if(send(arg.socket_descriptor, resp,strlen(resp),0) == -1)
        	{
                	writelog(log_fd,"No se puede enviar\n");
			fin_hilo(arg);
			return;
        	}

		if( ( len = recvtimeout(arg.socket_descriptor, buffer, PKG_LEN,arg.timeout) ) < 0 )
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
	close(log_fd);
	
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
