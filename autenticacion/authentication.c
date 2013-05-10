#include "hash.h"
int main () 
{
    unsigned long hash_pass; 
    char user[50];
    char pass[50];
    char user_buscado[50];
    char pass_buscado[50];
    unsigned long hash_pass_buscado;  
    FILE * acl;
 
    //activar si queremos guardar mas usuarios
//para escribir en el archivo acl ..hice para guardar en el archivo las contrasenhas 
    acl=fopen("cold.users.acl.txt", "at");//abre el archivo para agregar/escribir lineas al archivo
    printf("Usuario: ");
    scanf("%s", user);
    printf ("Password: ");
    scanf ("%s", pass);

   hash_pass= hash(pass);//funcion que convierte a hash la contrasenha devuelve un float
   fprintf(acl,"%s::%ld \n", user, hash_pass);//imprime en el archivo acl el usuario::contrasenha hash
// fin de escribir en el archivo acl  
   fclose(acl);//cierra el archivo de escritura acl



//Aca empieza de verdad :O
//AUTENTICACION DE USER Y PASS 
	printf("Ingrese usuario buscar: ");
   	scanf("%s", user_buscado);
   	printf ("Password: ");
   	scanf ("%s", pass_buscado);
	 
  	hash_pass_buscado= hash(pass_buscado);//hashea el pass a buscar
   	acl=fopen("cold.users.acl.txt", "r");//abre el archivo con permiso de lectura.

   if (acl==NULL)
   {
      printf("%p ERROR NO ABRE EL ARCHIVO ",acl);
   }

   authentication (acl, user_buscado, hash_pass_buscado);/* llama a la funcion authentication enviando el archivo acl, el user a buscar, y el pass hasheado */
//FIN DE AUTENTICACION

    
}
