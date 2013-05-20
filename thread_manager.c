#include "coldaemon.h"

void * thread_manager(void * argumento)
{
	thread_arg arg = *((thread_arg *) argumento);
	
	char * log = arg.log;
	int puerto = arg.puerto;
	int threads = arg.threads;
	int log_fd;
	char printBuffer[STR_LEN];
	//SOCKET
	struct sockaddr_in sin;
	struct sockaddr_in pin;
	int sock_descriptor;
	int temp_sock_descriptor;
	int address_size;
	char buf[PKG_LEN];
	int create_thread_value;
	int ret;
	thread_arg * arg_for_thread = NULL;

	syslog(LOG_DEBUG,"Abriendo el fichero %s para bitácora\n", log);
	if( (log_fd = open(log, O_CREAT | O_WRONLY | O_APPEND, 0666)) < 0 )
	{ 
		syslog(LOG_ERR,"No se puede abrir el fichero %s (%d)\n",log, log_fd);
		exit(LOG_ERROR);
	}
	strcpy(printBuffer,"[cold] Cobros On-Line Daemon\n");
	writelog(log_fd,printBuffer);

	// SOCKET PARA ESCUCHAR CONEXIONES
	sock_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if( sock_descriptor == -1)
	{
		syslog(LOG_ERR,"No se puede obtener un descriptor de socket\n");
		exit(SOCK_DESCRIPTOR_ERROR);
	}

	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(puerto);


	if( (ret = bind(sock_descriptor, (struct sockaddr *)&sin, sizeof(sin))) == -1 )
	{
		syslog(LOG_ERR,"No se puede usar el puerto %d\n",puerto);
		exit(BINDING_ERROR);
	}

	if( listen(sock_descriptor, threads) == -1 )
	{
		syslog(LOG_ERR,"No se puede escuchar en el puerto %d\n",puerto);
		exit(LISTENNING_ERROR);
	}
	
	sprintf(printBuffer,"Esperando conexiones en el puerto %d\n",puerto);
	writelog(log_fd,printBuffer);
	
	while(1)
	{
		temp_sock_descriptor = accept(sock_descriptor, (struct sockaddr *)& pin, &address_size);
		if( temp_sock_descriptor == -1 )
		{
			syslog(LOG_ERR,"No se puede aceptar la conexión\n");
			exit(ACCEPT_CONNECTION_ERROR);
		}
		strcpy(printBuffer,"Conexión Entrante\n");
		writelog(log_fd, printBuffer);

		// Asignar argumentos y ejecutar hilos
		if( ready <  arg.threads )
		{
			ready++;
			syslog(LOG_DEBUG,"Ejecutando el hilo %d\n",ready);
			arg_for_thread = (thread_arg *)malloc(sizeof(thread_arg));

			*arg_for_thread = arg;

			arg_for_thread->thread_index = ready;
			arg_for_thread->socket_descriptor = temp_sock_descriptor;
			arg_for_thread->socket = pin;

			thread_add(&(arg_for_thread->lista_hilo), ready);
			create_thread_value = pthread_create(thread_get(arg_for_thread->lista_hilo,ready),NULL, coredaemon, (void *) arg_for_thread); 

			syslog(LOG_DEBUG,"create_thread_value = %d\n",create_thread_value);
			syslog(LOG_DEBUG,"Usando Hilo %d\n",ready);
		}else{
			// Rechazar la conexión
			sprintf(printBuffer,"El servidor no acepta más conexiones en este momento\nPor favor aguarde un momento y reintente conectarse nuevamente\n");
			if(send(temp_sock_descriptor, printBuffer, strlen(printBuffer), 0) == -1)
			{
				syslog(LOG_ERR,"Error al informar que ya no se aceptan más conexiónes\n");
			}
			strcpy(printBuffer, "No hay hilos disponibles\n");
			writelog(log_fd, printBuffer);
			close(temp_sock_descriptor);
		}
	}
	
	return;
}
