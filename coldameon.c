#include "coldaemon.h"


int main(int argc, char * argvp[])
{
	FILE * config = NULL;
	pid_t pid, sid;
	int puerto;
	int threads;
	int timeout;
	
	if(argc != 2)
	{
		// Rutina para loggear errores
		printf("Uso: coldaemon CONFIGFILE\n"):
		return ARGUMENTOS_INVALIDOS
	}

	if( (config = fopen(argv[1],"r")) != 0 )
	{
		// log errors
		printf("Archivo Ilegible\n");
		return NO_CONFIG_FILE;
	}

	// Implentar parser de archivo de configuración
	// y configurar los parámetros del demonio

	if( parser(config,&puerto,&thread) != 0)
	{
		// log errors
		printf("Archivo de configuración Inválido\n");
		return INVALID_CONFIG_FILE;
	}


	pid = fork();
	
	if(pid < 0)
	{
		// log erros 
		printf("No se puede crear proceso hijo\n");
		return 1;
	}

	if(pid > 0)
	{
		return 0;
	}

	sid = setsid();

	if(sid < 0)
	{
		// log erros
		printf("No se puede crear la sesión\n");
		exit(1);
	}

	if( (chdir("/")) < 0)
	{
		// log errors
		printf("No se puede cambiar de directorio\n");
		exit(1);
	}


	umask(0);


	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	// Implementar Sockets

	// Implementar Hilos

	// Implementar las Señales
	
	return 0;
}
