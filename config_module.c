#include "coldaemon.h"

char config_module(char * config_file, thread_arg * argumento)
{
	char * logpath;
	char * logfile;
	char * aclpath;
	char * aclfile;
	int ret;
	int temp_fd;
	
	// Invocación al config parser
	if( (ret = config_parser(config_file, &(argumento->puerto), &(argumento->threads), &(argumento->timeout), &logpath, &logfile, &aclpath, &aclfile)) != OK )
	{
                syslog(LOG_ERR,"Fichero de configuración inválido, config_parser(%d)\n",ret );
                return ret;
        }

	// Estableciendo el nombre completo del fichero acl
	strcpy(argumento->acl,aclpath);
	strcat(argumento->acl,aclfile);	

	// Estableciendo el nombre completo del fichero log
	strcpy(argumento->log,logpath);
	strcat(argumento->log,logfile);

	// Liberando la memoria reservada en config_parser
	free(aclpath);
	free(aclfile);
	free(logpath);
	free(logfile);

	// Hacer validaciones sobre los parámetros acl, y log
	if( ( temp_fd  = open(argumento->acl, O_RDONLY) ) < 0 )
        {
                syslog(LOG_ERR,"No existe el archivo %s o no se puede abrir\n",argumento->acl);
                return CANT_OPEN_ACL;
        }
        close(temp_fd);

	// Just for debugging purposes 
	//dbg_print_thread_arg(argumento);

	return OK;
}

void dbg_print_thread_arg(thread_arg * argumento)
{
	syslog(LOG_DEBUG, "Printing ARG");
	syslog(LOG_DEBUG, "threads = %d\n", argumento->threads);
	syslog(LOG_DEBUG, "puerto = %d\n", argumento->puerto); 
	syslog(LOG_DEBUG, "timeout = %d\n", argumento->timeout); 
	syslog(LOG_DEBUG, "acl = %s\n", argumento->acl); 
	syslog(LOG_DEBUG, "log = %s\n", argumento->log); 
	syslog(LOG_DEBUG, "thread_index = %d\n", argumento->thread_index); 
	syslog(LOG_DEBUG, "socket_descriptor = %d\n", argumento->socket_descriptor); 
	
	return;
}
	
