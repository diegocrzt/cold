#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
//unsigned long hash( char * str);
//void authentication (FILE *acl, char * user, unsigned long  pass);


/*generador de hash de contrasenha recibe el char ingresado por teclado de la contrasenha y lo hashea retornando el long hash del pass*/
unsigned long hash( char * str)
    {         
        //printf ("lk%s", &str);      
        uint32_t hash = 5381;
        int c;

         while (c = *str++)
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
            
        // printf ("asdn%s", hash);
         return hash;
    }
    
   


/*recibe el puntero al archivo, el puntero al usuario y contrasenha a verificar*/
char authentication (char * acl_file, char * user, unsigned long  pass_buscado)
{
               char user1[50];
               char *pass_file;
               uint32_t pass1=0;
		FILE * acl; 

		if( ( acl = fopen(acl_file,"r") ) == NULL)
		{
			//syslog(LOG_ERR,"No se pudo abrir %s\n",acl_file);
			//printf("No se pudo abrir %s\n",acl_file);
			return -1; //Código de error para "No se puede leer ACL"
		}

               strcpy(user1, "");//vacia la variable user1
               fscanf(acl, "%s", user1);//lee una linea del archivo
               pass_file=(strpbrk(user1, ":")+2);//extrae lo que encuentra depues de '::' pass
               pass1 = atoi(pass_file);//convierte el pass string leido de archivo a long
  
               while (memcmp(user1,user,strlen(user))!=0 && pass1!=pass_buscado && !feof(acl))
               {/*si el usuario y la contrasenha no son iguales, y no es fin de archivo leer la siguiente linea del archivo acl*/
                     fscanf(acl, "%s", user1);
                     pass_file=(strpbrk(user1, ":")+2);//extrae de la linea user::pass lo que esta despues del :: "pass"
                     pass1 = atoi(pass_file);//convierte la cadena leida a long para luego comparar
               }
               /*int memcmp(const void *s1, const void *s2, size_t n);
Compara los primeros n caracteres del objeto apuntado por s1 (interpretado como
unsigned char) con los primeros n caracteres del objeto apuntado por s2 (interpretado
como unsigned char).Devuelve 0 en caso que sean iguales*/

               if(pass_buscado==pass1 && memcmp(user1,user,strlen(user))==0)
               {/*verifica si la variable registrada el final de recorrer la lista de archivos es igual al usuario y contrasenha buscados para la autenticacion*/
                        printf("Usuario # %s # Valido\n", user);
			return 0;
               }     
               else//si no es por que recorrio todo el archivo y no encontro coincidencias de user y pass
               {
                   printf(" #######ERROR########\n ==Usuario %s INVALIDO==\n", user);
			return -1;//DEFINIR ERROR
               }
                   
}//fin autenticacion

