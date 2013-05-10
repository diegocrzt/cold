#include "coldaemon.h"
#include <syslog.h>


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
	int i, len, log_fd, ret;
	pthread_t * hilo;

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
	strcpy(printBuffer,"Demonio\n");
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

	while(1)
	{
		temp_sock_descriptor = accept(sock_descriptor, (struct sockaddr *)& pin, &address_size);
		if( temp_sock_descriptor == -1 )
		{
			// log error
			syslog(LOG_ERR,"No se puede aceptar la conexión\n");

			exit(ACCEPT_CONNECTION_ERROR);
		}
		write(log_fd, "Paquete kachiai\n", strlen("Paquete kachiai\n"));
	}
	/*
		Incomplete Implementation
	*/
	
		

	// Implementar Hilos

	// Implementar las Señales
	
	return 0;
}
