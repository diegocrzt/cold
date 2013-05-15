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
#include "config_parser.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
//#include <netbd.h>

/*

	Todas las variables en minúsculas, se usa guion bajo si es necesario
	ej; var, var_muy_larga
	Todos los define en mayúsculas
	ej: #define MACRO valor_macro
	todos los nombres de función en minúsculas
	
*/

//Estructuras de datos
typedef struct
{
	int thread_index;
	int address_size; //Socket Address Structure size
	int socket_descriptor;
	struct sockaddr_in socket;
	int log_fd;
	char * acl_file;
} thread_arg;

char * ready;

// Biblioteca de Funciones de:
// Hash, postgres, semaforos

#define OK 0
#define ARGUMENTOS_INVALIDOS 1
#define NO_CONFIG_FILE 2
#define INVALID_CONFIG_FILE 3
#define SOCK_DESCRIPTOR_ERROR 4
#define BINDING_ERROR 5 
#define LISTENNING_ERROR 6
#define ACCEPT_CONNECTION_ERROR 7
#define CANT_FORK 8
#define SESSION_ERROR 9
#define CHDIR_ERROR 10
#define LOG_ERROR 11

/*
	acl_file es un puntero al nombre del fichero que tiene los datos de autenticación
	usuario es un puntero al nombre usuario
	clave es un puntero a la clave
	la función retorna 0 si la autenticación es exitosa y se debe definir códigos 
	de error para cada caso de error
*/
//char authentication(char * acl_file, char * usuario, char * clave);


/*
	config_parser lee un archivo de configuración y establece los parámetros del demonio a partir del mismo
	retorna 0 si no hubo errores, establecer sus códigos de error en otro caso
	config_file, puntero al nombre del archivo de configuración
	puerto, threads, timeout, logpath, logfile, parámetros del demonio.
*/
int config_parser (char * config_file, int * puerto, int * threads, int * timeout, char ** logpath, char ** logfile);


/*
	EL CORE DAEMON
*/
void * coredaemon(void * argumento);

