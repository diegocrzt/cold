#include "coldaemon.h"

int config_parser(char * config_file, int * puerto, int * threads, int *timeout, char ** logpath, char ** logfile)
{
	char *tokenPtr; //puntero para los tokens
	char string[100]; //string donde se almacenan las lineas de configuracion
	int i; //variable para el for
	FILE *ficheroPtr;

	if(( ficheroPtr = fopen( config_file, "r")) == NULL)
		return INVALID_CONFIG_FILE;
	/* Comparacion de string con cada argumento de configuracion */
	for(i=0; i<=4;i++)
	{
	/* Lectura de los argumentos de configuración */
		fscanf( ficheroPtr, "%s", string);

		tokenPtr= strtok( string, "="); //se extrae el argumento hasta =    
                if(strstr(tokenPtr,"port")) //si es port
                {   
                        tokenPtr = strtok(NULL," "); //se extrae la configuracion
			sscanf(tokenPtr,"%d", puerto); //se almacena
						
//free(tokenPtr); //se libera la memoria
		}
		if(strstr(tokenPtr,"threads")) //si es threads
		{
			tokenPtr = strtok(NULL," ");
			sscanf(tokenPtr,"%d", threads); //se almacena
		}
		if(strstr(tokenPtr,"timeout"))//si es timeout
		{
			tokenPtr = strtok(NULL," ");
			sscanf(tokenPtr,"%d", timeout); //se almacena
		}
		if(strstr(tokenPtr,"logpath"))//si es logpath
		{
			tokenPtr = strtok(NULL," ");
			*logpath = (char *) malloc(sizeof(char)*strlen(tokenPtr));
			strcpy(*logpath, tokenPtr);
		}
		if(strstr(tokenPtr,"logfile"))//si es logfile
		{
			tokenPtr = strtok(NULL," ");
			*logfile = (char *) malloc(sizeof(char)*strlen(tokenPtr));
			strcpy(*logfile, tokenPtr);
		}
	}
	fclose(ficheroPtr);
	return OK;
}
