#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <syslog.h>
#include <signal.h>
#include <pthread.h> 
// Codigo del infierno


/*

	Todas las variables en minúsculas, se usa guion bajo si es necesario
	ej; var, var_muy_larga
	Todos los define en mayúsculas
	ej: #define MACRO valor_macro
	todos los nombres de función en minúsculas
	





*/

// Biblioteca de Funciones de:
// Hash, postgres, semaforos

#define ARGUMENTOS_INVALIDOS 1
#define NO_CONFIG_FILE 2
#define INVALID_CONFIG_fILE 3
 

/*
	acl_file es un puntero al nombre del fichero que tiene los datos de autenticación
	usuario es un puntero al nombre usuario
	clave es un puntero a la clave
	la función retorna 0 si la autenticación es exitosa y se debe definir códigos 
	de error para cada caso de error
*/
char authentication(char * acl_file, char * usuario, char * clave);


/*
	config_parser lee un archivo de configuración y establece los parámetros del demonio a partir del mismo
	retorna 0 si no hubo errores, establecer sus códigos de error en otro caso
	config_file, puntero al nombre del archivo de configuración
	puerto, threads, timeout, logpath, logfile, parámetros del demonio.
*/
char config_parser(char * config_file, int * puerto, char * threads, char * timeout, char * logpath, char * logfile); 

