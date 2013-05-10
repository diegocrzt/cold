#include "coldaemon.h"
#include <syslog.h>
#include <pthread.h>

int main(int argc, char * argv[])
{
	char * config_file;
	pid_t pid, sid;
	// Valores por defecto solo para pruebas
	int puerto;
	int threads;
	int timeout;
	char * logpath;
	char * logfile;
	char log[512];
	// Recuerda quitar los valores por defecto
	struct sockaddr_in sin;
	struct sockaddr_in pin;
	int sock_descriptor;
	int temp_sock_descriptor;
	int address_size;
	char buf[16384];
	char printBuffer[512];
	int i, len, log_fd, ret, j;
	pthread_t * hilo;
	thread_arg  * argumento;
	int create_thread_value;
	char flag_asignado_hilo;
	FILE * acl_file;

	if(argc != 2)
	{
		// Rutina para loggear errores
		syslog(LOG_ERR,"Se requiere un fichero de configuración\n");
		return ARGUMENTOS_INVALIDOS;
	}

	config_file = argv[1];

	// Implentar parser de archivo de configuración
	// y configurar los parámetros del demonio

	if( ret = config_parser(config_file, &puerto, &threads, &timeout, &logpath, &logfile)){
		syslog(LOG_ERR,"Fichero de configuración inválido\n");
		return ret;
	}
	// REMEBER TO FREE MEMORY OF POINTER POINTER
	syslog(LOG_DEBUG,"%d\n%d\n%d\n%s\n%s\n",puerto, threads, timeout, logpath, logfile);

	pid = fork();
	
	if(pid < 0)
	{
		// log erros 
		syslog(LOG_ERR,"No se puede crear proceso hijo\n");
		return CANT_FORK;
	}

	if(pid > 0)
	{
		syslog(LOG_DEBUG,"Este proceso ha creado un zombie\n");
		return OK;
	}

	sid = setsid();

	if(sid < 0)
	{
		syslog(LOG_ERR,"No se puede crear la sesión\n");
		return SESSION_ERROR;
	}
	
	if( ( acl_file = fopen("cold.users.acl", "r") ) == NULL )
	{
		syslog(LOG_ERR,"No existe el archivo cold.users.acl\n");
		return -1;// CODIGO DE ERROR, DEFINIR
	}

	if( (chdir("/")) < 0)
	{
		syslog(LOG_ERR,"No se puede cambiar de directorio\n");
		return CHDIR_ERROR;
	}

	umask(0);

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	openlog("cold", LOG_PID, LOG_DAEMON);
	syslog(LOG_INFO,"Cobros On Line\n");

	hilo = (pthread_t *) malloc(sizeof(pthread_t) * threads);
	argumento = (thread_arg *)malloc(sizeof(thread_arg) * threads);
	ready = (char *)calloc(threads, sizeof(char));
	for(i = 0; i < threads; i++)
	{
		ready[i] = 1;
	}
	
	strcpy(log,logpath);
	strcat(log,logfile);
	free(logpath);//HERE FREE THE POINTER POINTER
	free(logfile);
	syslog(LOG_DEBUG,"Abriendo el log %s\n", log);
	if( (log_fd = open(log, O_CREAT | O_WRONLY | O_APPEND, 0666)) < 0 )
	{
		syslog(LOG_ERR,"No se puede abrir el fichero %s (%d)\n",log, log_fd);
		return LOG_ERROR;
	}
	strcpy(printBuffer,"coldaemon become a full daemon\n");
	write(log_fd, printBuffer, strlen(printBuffer));

	// Implementar Sockets

	sock_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if( sock_descriptor == -1)
	{
		// log erros
		syslog(LOG_ERR,"No se puede crear el descriptor del socket\n");
		exit(SOCK_DESCRIPTOR_ERROR);
	}

	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(puerto);

	if( bind(sock_descriptor, (struct sockaddr *)&sin, sizeof(sin)) == -1 )
	{
		// log errors
		syslog(LOG_ERR,"No se puede abrir el puerto %d\n",puerto);
		exit(BINDING_ERROR);
	}

	if( listen(sock_descriptor, threads) == -1 )
	{
		// log error
		syslog(LOG_ERR,"No se puede escuchar en el puerto %d\n",puerto);
		exit(LISTENNING_ERROR);
	}
	
	// registrar que el servidor ya puede aceptar conexiones
	
	i = 0;

	while(1)
	{
		temp_sock_descriptor = accept(sock_descriptor, (struct sockaddr *)& pin, &address_size);
		if( temp_sock_descriptor == -1 )
		{
			// log error
			syslog(LOG_ERR,"No se puede aceptar la conexión\n");

			exit(ACCEPT_CONNECTION_ERROR);
		}
		strcpy(printBuffer,"Conexión Entrante\n");
		write(log_fd, printBuffer, strlen(printBuffer));

		syslog(LOG_DEBUG,"threads = %d\n",threads);
		for(; i < threads; i++)
		{
			syslog(LOG_DEBUG,"Verificando Hilo  = %d\n",i);
			if( ready[i] )
			{
				syslog(LOG_DEBUG,"hilo %d disponible\n",i);
				flag_asignado_hilo = 1;
				ready[i] = 0;
				argumento[i].thread_index = i;
				argumento[i].address_size = address_size;
				argumento[i].socket_descriptor = temp_sock_descriptor;	
				argumento[i].socket = pin;
				argumento[i].log_fd = log_fd;
				argumento[i].acl_file = acl_file;
				create_thread_value = pthread_create(&hilo[i],NULL, coredaemon, (void *) &argumento[i]); 
				syslog(LOG_DEBUG,"create_thread_value = %d\n",create_thread_value);
				syslog(LOG_DEBUG,"Usando Hilo %d\n",i);
				break;
			}
			for(j = 0; j < threads; j++)
			{
				sprintf(printBuffer,"%d ",ready[j]);
				write(log_fd, printBuffer, strlen(printBuffer));
			}
			sprintf(printBuffer,"\n");
			write(log_fd, printBuffer, strlen(printBuffer));
			
		}

		if(!flag_asignado_hilo)
		{
			// Rechazar la conexión
			strcpy(printBuffer, "No hay hilos disponibles\n");
			write(log_fd, printBuffer, strlen(printBuffer));
		}
		flag_asignado_hilo = 0;
		if( i == threads)
		{
			i = 0;
		}
	}
	/*
		Incomplete Implementation
	*/
	
		

	// Implementar Hilos

	// Implementar las Señales
	
	return 0;
}
