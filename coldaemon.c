#include "coldaemon.h"
#include <pthread.h>

int main(int argc, char * argv[])
{
	char * config_file;
	pid_t pid, sid;
	char * logpath;
	char * logfile;
	char * aclpath;
	char * aclfile;
	char buf[PKG_LEN];
	char printBuffer[STR_LEN];
	int i, len, log_fd, ret, j;
	pthread_t manager;
	thread_arg  argumento;
	int create_thread_value;
	char flag_asignado_hilo;
	FILE * temp_file;
	ready = 0;

	// Validación de Argumentos, debe haber un argumento que especifique
	// el fichero de configuración del demonio
	if(argc != 2)
	{
		syslog(LOG_ERR,"Se necesita el fichero de configuración\n");
		return ARGUMENTOS_INVALIDOS;
	}

	// fichero de configuración de los parámetros del demonio
	config_file = argv[1];
	// MODULO DE CONFIGURACION	
	if( (ret = config_module(config_file, &argumento)) != 0)
	{
		syslog(LOG_ERR,"Error con los parámetros de configuración config_module\n",ret);
		return CONFIG_ERROR;
	}
	
		
	// Creando el proceso huérfano y terminando el proceso padre
	pid = fork();
	if(pid < 0)
	{
		syslog(LOG_ERR,"No se puede crear proceso hijo\n");
		return CANT_FORK;
	}
	if(pid > 0)
	{
		syslog(LOG_DEBUG,"Terminando el proceso padre, proceso huerfano creado\n");
		return OK;
	}

	// Convirtiendo al huérfano en lider de la sesión
	sid = setsid();
	if(sid < 0)
	{
		syslog(LOG_ERR,"No se puede crear la sesión\n");
		return SESSION_ERROR;
	}

	//Cambiando al directorio / como directorio de trabajo
	if( (chdir("/")) < 0)
	{
		syslog(LOG_ERR,"No se puede cambiar de directorio\n");
		return CHDIR_ERROR;
	}

	// Cambiando Mascara 
	umask(0);
	// Cerrando descriptores estándard
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	openlog("cold", LOG_PID, LOG_DAEMON);
	syslog(LOG_INFO,"Cobros On Line Daemon\n");

	/*
	hilo = (pthread_t *) malloc(sizeof(pthread_t) * threads);
	argumento = (thread_arg *)malloc(sizeof(thread_arg) * threads);
	ready = (char *)calloc(threads, sizeof(char));
	*/
	/*	
	for(i = 0; i < threads; i++)
	{
		ready[i] = 1;
	}
	*/
	i = 0;
	while(1)
	{
		signal_handler();
		pthread_create(&manager,NULL, thread_manager, (void *) &argumento); 
		sleep(1);
	}
	
	return OK;
}

void signal_handler(void)
{
	sigset_t sigset;
	struct sigaction action;

	sigemptyset(&sigset); /* Inicializa el conjunto de señales */
	sigaddset(&sigset, SIGHUP); /* Reload Config file */
	sigaddset(&sigset, SIGTERM); /* Parar el demonio */
	sigprocmask(SIG_BLOCK, &sigset, NULL); /* Bloqueamos las señales */
	sigpending(&sigset); /* Comprueba las señales pendientes */
	if(sigismember(&sigset, SIGHUP)) {
	    syslog(LOG_INFO, "SIGHUP\n");
	    /* Ignora SIGHUP */
	    sigemptyset(&action.sa_mask);
	    action.sa_handler = SIG_IGN;
	    sigaction(SIGHUP, &action, NULL);
	    /*
			MODULO DE CONFIGURACIÓN
		*/
	    /* Desbloquea SIGHUP */
	    sigprocmask(SIG_UNBLOCK, &sigset, NULL);
	}
	if(sigismember(&sigset, SIGTERM)) {
	    syslog(LOG_INFO, "SIGTERM\n");
	    /* Ignora SIGHUP */
	    sigemptyset(&action.sa_mask);
	    action.sa_handler = SIG_IGN;
	    sigaction(SIGTERM, &action, NULL);
	    /*
			Parar el demonio
		*/
	    /* Desbloquea SIGHUP */
	    sigprocmask(SIG_UNBLOCK, &sigset, NULL);
	}
	return;
}
