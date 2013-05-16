#include "transa_parser.h"

void col_parser (SERVICIO *servicio)
{
	int contcar = 0; //contador de caracteres
	char string[58] = {0}; //rubro
	int tipo = 0;
	char *auxiliar; //string auxiliar
	int entero; //entero auxiliar
	int biciesto = 0; //booleano de año biciesto
	//Asignar el tamaño de codigo de servicio
	auxiliar = (char *) malloc(sizeof(char)*3);
	
	//establecer el rubro de transaccion
	for(contcar=0 ; contcar <=2; contcar++)
	{
		string[contcar] = getchar(); //lectura del caracter
		strcat(auxiliar, string+contcar); //concatenacion en el auxiliar
		if((string[contcar] != '0') && contcar <= 1)
			printf("ERROR: el código de servicio no existe\n"); //CODIFICAR ERROR Y SALIR DE LA FUNCION. ERROR EN LOS DOS PRIMEROS DIGITOS DEL SERVICIO.	
	}
	
	//****CONTROLAR QUE TIPO SEA UN NUMERO VALIDO DE SERVICIO***** SI NO ES VALIDO SALIR!!

	tipo = atoi(string+2);	//definir el tipo de servicio
	//printf("Tipo:%d\n",tipo);
	
	servicio->codser = (char *)malloc(sizeof(char)*3); //Asignar tamaño al codigo de servicio
	strcpy(servicio->codser, auxiliar); // Asignar el valor del auxiliar al codigo de servicio
	//printf("Auxiliar: %s\n", auxiliar);
	//printf("codser: %s\n", servicio->codser);
	free(auxiliar); //liberar el auxiliar

	//establecer el tamaño del auxiliar al de transaccion
	auxiliar = (char *)malloc(sizeof(char)*6);
	
	//establecer el numero de transaccion
	for(contcar=0 ; contcar <=5; contcar++)
	{
		string[3+contcar] = getchar(); //lectura del caracter
		strcat(auxiliar,string+3+contcar); //concatenación en el auxiliar
	}
	
	servicio->numtran = atoi(auxiliar); //se convierte el string de transacción a int y se asigna al numero de transaccion del servicio
	//printf("Auxiliar: %s\n", auxiliar);
	free(auxiliar); //se libera el auxiliar
	//printf("numtran: %d\n", servicio->numtran);

	//Asignar el tamaño del auxiliar a fechahora
	auxiliar = (char *)malloc(sizeof(char)*14);

	//Asignar el tamano del fecharhora del Servicio
	servicio->fechahora = (char *)malloc(sizeof(char)*14);

	//establecer la fecha de la transaccion
	for(contcar=0 ; contcar<=13; contcar++)
	{
		string[9+contcar] = getchar();//lectura del caracter
		strcat(auxiliar, string+9+contcar);//concatenacion en el auxiliar
		if(contcar == 3) //ya se leyo el año completo
		{
			entero = atoi(auxiliar); //convertir el char año a entero
			//if(entero <= 2013) ******** VALIDACION DE AÑOS MAYORES AL AÑO VIGENTE ***********
				//SALIR
			if((entero%4 == 0) && (entero%100 != 0) || (entero%400 == 0)) //verificacion de año biciesto
				biciesto = 1; //año biciesto
		}
		if(contcar == 7) //ya se leyo el mes y el dia completo
		{
			entero = atoi(string+13)*10 + atoi(string+14); //asignar el mes a entero
			
			//if(entero > 12 || entero < 1) VERIFICAR SI EL MES ES CORRECTO
				// ************ SALIR SI EL MES NO ES CORRECTO *************
			
			if((entero == 1) || (entero == 3) || (entero == 5) || (entero == 7) || (entero == 8) || (entero == 10) || (entero ==12)) // si el mes tiene 31 dias
			{
				entero = atoi(string+15)*10 + atoi(string+16);
				//if((entero < 1) || (entero > 31)) VERIFICAR SI LA FECHA ES VALIDA
					//****** SALIR LA FECHA NO ES VALIDA *******
			}
			else if((entero == 4) || (entero == 6) || (entero == 9) || (entero == 11)) //si el mes tiene 30 dias
				{	
					entero = atoi(string+15)*10 + atoi(string+16);
					//if((entero < 1) || (entero > 30)) VERIFICAR SI LA FECHA ES VALIDA
						//****** SALIR LA FECHA NO ES VALIDA *******
				}
			else if(biciesto == 0) //si es febrero y el año NO es biciesto
				{
					entero = atoi(string+15)*10 + atoi(string+16);
					//if((entero < 1) || (entero > 28)) VERIFICAR SI LA FECHA ES VALIDA
						//****** SALIR LA FECHA NO ES VALIDA *******
				}
			else //si el año es biciesto y es febrero
			{
				entero = atoi(string+15)*10 + atoi(string+16);
				//if((entero < 1) || (entero > 29)) VERIFICAR SI LA FECHA ES VALIDA
					//****** SALIR LA FECHA NO ES VALIDA *******
			}
		}		
	}
	
	entero = atoi(string+17)*10 + atoi(string+18);
	//if((entero > 24) || (entero < 1)) // verificar si la hora es valida
		//******* SALIR LA HORA NO ES VALIDA *******
	entero = atoi(string+19)*10 + atoi(string+20);
	//if((entero > 59) || (entero < 0)) //verificar si los minutos son validos
		//******* SALIR LA HORA NO ES VALIDA *******
	entero = atoi(string+21)*10 + atoi(string+22);
	//if((entero > 59) || (entero < 0)) //verificar si los segundos son validos
		//******* SALIR LA HORA NO ES VALIDA *******
	
	//asignar la fechahora del servicio
	strcpy(servicio->fechahora, auxiliar);
	//liberar auxiliar
	//printf("Auxiliar: %s\n", auxiliar);
	free(auxiliar);
	//printf("fechahora: %s\n", servicio->fechahora);

	switch (tipo)
	{
			
			case 1:
				//Asignar recursos al tipo de factura
				servicio->tipofact = (char *)malloc(sizeof(char)*3);
				
				//Asignar recursos al auxiliar
				auxiliar = (char *)malloc(sizeof(char)*3);
				
				//printf("Agua\n");
				for(contcar = 0; contcar <= 2; contcar++)
				{
					string[23+contcar] = getchar();//lectura del caracter
					strcat(auxiliar, string+23+contcar);//concatenacion en el auxiliar
				}
				
				//asignar el tipo de factura
				strcpy(servicio->tipofact, auxiliar);
				//liberar el auxiliar
				//printf("Auxiliar: %s\n", auxiliar);
				free(auxiliar);
				//printf("tipofact: %s\n", servicio->tipofact);
				

				//Asignar recursos al comprobante
				servicio->comprobante = (char *)malloc(sizeof(char)*11);
				//Asignar recursos al auxiliar
				auxiliar = (char *)malloc(sizeof(char)*11);

				for(contcar = 0; contcar <= 10; contcar++)
				{
					string[26+contcar] = getchar(); //lectura del caracter
					strcat(auxiliar, string+26+contcar); //concatenacion en el auxiliar
				}
			
				//asignar el comprobante
				strcpy(servicio->comprobante, auxiliar);
				//liberar auxiliar
				//printf("Auxiliar: %s\n", auxiliar);
				free(auxiliar);
				//printf("Comprobante: %s\n", servicio->comprobante);
				
				
				//Asignar recursos al auxiliar
				auxiliar = (char *)malloc(sizeof(char)*12);
				
				for(contcar = 0; contcar <= 11; contcar++)
				{
					string[37+contcar] = getchar();
					strcat(auxiliar, string+37+contcar);
				}
				
				//Asignar el monto
				servicio->monto = atoi(auxiliar);
				//Liberar el auxiliar
				printf("Auxiliar: %s\n", auxiliar);
				//free(auxiliar);
				printf("Monto: %d\n", servicio->monto);
				
				//Asignar recursos a Vencimiento
				servicio->vencimiento = (char *)malloc(sizeof(char)*8);
				//Asignar recursos a Auxiliar
				auxiliar = (char *)malloc(sizeof(char)*8);
				//Establecer que NO es año biciesto
				
				for(contcar = 0; contcar <= 7; contcar++)
				{
					string[49+contcar] = getchar();
					strcat(auxiliar, string+49+contcar);
					//printf("contcar=%d\naux=%s\n", contcar, auxiliar);
					if(contcar == 3) //ya se leyo el año completo
					{
						entero = atoi(auxiliar); //convertir el char año a entero
						//if(entero <= 2013) ******** VALIDACION DE AÑOS MAYORES AL AÑO VIGENTE ***********
						//SALIR
						if((entero%4 == 0) && (entero%100 != 0) || (entero%400 == 0)) //verificacion de año biciesto
							biciesto = 1; //año biciesto
					}
					if(contcar == 7) //ya se leyo el mes y el dia completo
					{
						entero = atoi(string+53)*10 + atoi(string+54); //asignar el mes a entero
		
						//if(entero > 12 || entero < 1) VERIFICAR SI EL MES ES CORRECTO
						// ************ SALIR SI EL MES NO ES CORRECTO *************
			
						if((entero == 1) || (entero == 3) || (entero == 5) || (entero == 7) || (entero == 8) || (entero == 10) || (entero ==12)) // si el mes tiene 31 dias
						{
							entero = atoi(string+55)*10 + atoi(string+56);

							//if((entero < 1) || (entero > 31)) VERIFICAR SI LA FECHA ES VALIDA
							//****** SALIR LA FECHA NO ES VALIDA *******
						}
						else if((entero == 4) || (entero == 6) || (entero == 9) || (entero == 11)) //si el mes tiene 30 dias
						{	
							entero = atoi(string+55)*10 + atoi(string+56);
	
							//if((entero < 1) || (entero > 30)) VERIFICAR SI LA FECHA ES VALIDA
							//****** SALIR LA FECHA NO ES VALIDA *******
						}
						else if(biciesto == 0) //si es febrero y el año NO es biciesto
						{
							entero = atoi(string+55)*10 + atoi(string+56);
		
							//if((entero < 1) || (entero > 28)) VERIFICAR SI LA FECHA ES VALIDA
							//****** SALIR LA FECHA NO ES VALIDA *******
						}
						else //si el año es biciesto y es febrero
						{
							entero = atoi(string+55)*10 + atoi(string+56);
							//if((entero < 1) || (entero > 29)) VERIFICAR SI LA FECHA ES VALIDA
							//****** SALIR LA FECHA NO ES VALIDA *******
						}
					}		
				}		
				//Asignar el Vencimiento
				strcpy(servicio->vencimiento, auxiliar);
				//Liberar el auxiliar
				//printf("Auxiliar: %s\n", auxiliar);
				free(auxiliar);
				//printf("Vencimiento: %s\n", servicio->vencimiento);
				
				//Obtener el digito verificador
				string[57] = getchar();
				servicio->verificador = atoi(string+57);
				//printf("Verificador: %d\n\n", servicio->verificador);
				
				break;
			case 2:
				//printf("Telefono Fijo\n");
			
				//Asignar recursos al prefijo
				servicio->prefijo = (char *)malloc(sizeof(char)*4);

				//Asignar recursos al auxiliar
				auxiliar = (char *)malloc(sizeof(char)*4);

				for(contcar = 0; contcar <= 3; contcar++)
				{
					string[23+contcar] = getchar();
					strcat(auxiliar, string+23+contcar);
				}
				
				//Asignar el prefijo
				strcpy(servicio->prefijo, auxiliar);
				//Liberar el auxiliar
				//printf("Auxiliar: %s\n", auxiliar);
				free(auxiliar);
				//printf("Prefijo: %s\n", servicio->prefijo);

				//Asignar recursos al numero
				servicio->numero = (char *)malloc(sizeof(char)*7);
				//Asignar recursos al auxiliar
				auxiliar = (char *)malloc(sizeof(char)*7);

				for(contcar = 0; contcar <= 6; contcar++)
				{
					string[27+contcar] = getchar();
					strcat(auxiliar, string+27+contcar);
				}
				
				//Asignar el numero
				strcpy(servicio->numero, auxiliar);
				//Liberar el auxiliar
				//printf("Auxiliar: %s\n", auxiliar);
				free(auxiliar);
				//printf("Numero: %s\n", servicio->numero);

				//Asignar recursos al auxiliar
				auxiliar = (char *)malloc(sizeof(char)*12);
				
				for(contcar = 0; contcar <= 11; contcar++)
				{
					string[34+contcar] = getchar();
					strcat(auxiliar, string+34+contcar);
				}
				
				//Asignar el monto
				servicio->monto = atoi(auxiliar);
				//Liberar el auxiliar
				//printf("Auxiliar: %s\n", auxiliar);
				free(auxiliar);
				//printf("Monto: %d\n", servicio->monto);
				break;
			case 3:
				//printf("Suministro Electrico\n");
				//Asignar recursos al auxiliar
				auxiliar = (char *)malloc(sizeof(char)*12);
				
				for(contcar = 0; contcar <= 14; contcar++)
				{
					string[23+contcar] = getchar();
					strcat(auxiliar, string+23+contcar);
				}
				
				//Asignar el numero de medidor
				servicio->nummed = atoi(auxiliar);
				//Liberar el auxiliar
				//printf("Auxiliar: %s\n", auxiliar);
				free(auxiliar);
				//printf("Numero de Medidor: %d\n", servicio->nummed);
				
				//Asignar recursos al auxiliar
				auxiliar = (char *)malloc(sizeof(char)*12);
				
				for(contcar = 0; contcar <= 11; contcar++)
				{
					string[38+contcar] = getchar();
					strcat(auxiliar, string+38+contcar);
				}
				
				//Asignar el monto
				servicio->monto = atoi(auxiliar);
				//Liberar el auxiliar
				//printf("Auxiliar: %s\n", auxiliar);
				free(auxiliar);
				//printf("Monto: %d\n", servicio->monto);
				break;
			case 4:
				//printf("Telefono Movil\n");
				//Asignar recursos al prefijo
				servicio->prefijo = (char *)malloc(sizeof(char)*4);
				//Asignar recursos al auxiliar
				auxiliar = (char *)malloc(sizeof(char)*4);

				for(contcar = 0; contcar <= 3; contcar++)
				{
					string[23+contcar] = getchar();
					strcat(auxiliar, string+23+contcar);
				}
				
				//Asignar el prefijo
				strcpy(servicio->prefijo, auxiliar);
				//Liberar el auxiliar
				//printf("Auxiliar: %s\n", auxiliar);
				free(auxiliar);
				//printf("Prefijo: %s\n", servicio->prefijo);

				//Asignar recursos al numero
				servicio->numero= (char *)malloc(sizeof(char)*6);
				//Asignar recursos al auxiliar
				auxiliar = (char *)malloc(sizeof(char)*6);

				for(contcar = 0; contcar <= 5; contcar++)
				{
					string[27+contcar] = getchar();
					strcat(auxiliar, string+27+contcar);
				}
				
				//Asignar el numero
				strcpy(servicio->numero, auxiliar);
				//Liberar el auxiliar
				//printf("Auxiliar: %s\n", auxiliar);
				free(auxiliar);
				//printf("Numero: %s\n", servicio->numero);

				//Asignar recursos al auxiliar
				auxiliar = (char *)malloc(sizeof(char)*12);
				
				for(contcar = 0; contcar <= 11; contcar++)
				{
					string[33+contcar] = getchar();
					strcat(auxiliar, string+33+contcar);
				}
				
				//Asignar el monto
				servicio->monto = atoi(auxiliar);
				//Liberar el auxiliar
				//printf("Auxiliar: %s\n", auxiliar);
				free(auxiliar);
				//printf("Monto: %d\n", servicio->monto);
				break;
			case 5:
				//printf("Cable TV\n");

				//Asignar recursos al comprobante
				servicio->abonado = (char *)malloc(sizeof(char)*9);
				//Asignar recursos al auxiliar
				auxiliar = (char *)malloc(sizeof(char)*9);

				for(contcar = 0; contcar <= 8; contcar++)
				{
					string[23+contcar] = getchar(); //lectura del caracter
					strcat(auxiliar, string+23+contcar); //concatenacion en el auxiliar
				}
			
				//asignar el comprobante
				strcpy(servicio->abonado, auxiliar);
				//liberar auxiliar
				//printf("Auxiliar: %s\n", auxiliar);
				free(auxiliar);
				//printf("Abonado: %s\n", servicio->abonado);

				//Asignar recursos al auxiliar
				auxiliar = (char *)malloc(sizeof(char)*12);
				
				for(contcar = 0; contcar <= 11; contcar++)
				{
					string[32+contcar] = getchar();
					strcat(auxiliar, string+32+contcar);
				}
				
				//Asignar el monto
				servicio->monto = atoi(auxiliar);
				//Liberar el auxiliar
				//printf("Auxiliar: %s\n", auxiliar);
				free(auxiliar);
				//printf("Monto: %d\n", servicio->monto);
				break;
			default:
				printf("Servicio Inexistente\n"); // CODIFICAR ERROR Y SALIR DE LA FUNCION. ERROR EN EL DIGITO FINAL DEL CODIGO DE TIPO
				break;
	}

}

void rev_parser(SERVICIO *servicio)
{
	int contcar = 0; //contador de caracteres
	char string[23] = {0}; //rubro
	char *auxiliar; //string auxiliar
	int entero; //entero auxiliar
	int biciesto = 0; //booleano de año biciesto
	char *tokenPtr; //puntero para los tokens
	char caracter[1];
	//Asignar el tamaño de codigo de servicio
	auxiliar = (char *) malloc(sizeof(char)*3);
	
	//establecer el rubro de transaccion
	for(contcar=0 ; contcar <=2; contcar++)
	{
		string[contcar] = getchar(); //lectura del caracter
		strcat(auxiliar, string+contcar); //concatenacion en el auxiliar
		if((string[contcar] != '0') && contcar <= 1)
			printf("ERROR: el código de servicio no existe\n"); //CODIFICAR ERROR Y SALIR DE LA FUNCION. ERROR EN LOS DOS PRIMEROS DIGITOS DEL SERVICIO.
	}	
	//****CONTROLAR QUE TIPO SEA UN NUMERO VALIDO DE SERVICIO***** SI NO ES VALIDO SALIR!!

	servicio->codser = (char *)malloc(sizeof(char)*3); //Asignar tamaño al codigo de servicio
	strcpy(servicio->codser, auxiliar); // Asignar el valor del auxiliar al codigo de servicio
	printf("Auxiliar: %s\n", auxiliar);
	printf("codser: %s\n", servicio->codser);
	free(auxiliar); //liberar el auxiliar

	//establecer el tamaño del auxiliar al de transaccion
	auxiliar = (char *)malloc(sizeof(char)*6);
	
	//establecer el numero de transaccion
	for(contcar=0 ; contcar <=5; contcar++)
	{
		string[3+contcar] = getchar(); //lectura del caracter
		strcat(auxiliar,string+3+contcar); //concatenación en el auxiliar
	}
	
	servicio->numtran = atoi(auxiliar); //se convierte el string de transacción a int y se asigna al numero de transaccion del servicio
	printf("Auxiliar: %s\n", auxiliar);
	free(auxiliar); //se libera el auxiliar
	printf("numtran: %d\n", servicio->numtran);

	//Asignar el tamaño del auxiliar a fechahora
	auxiliar = (char *)malloc(sizeof(char)*14);

	//Asignar el tamano del fecharhora del Servicio
	servicio->fechahora = (char *)malloc(sizeof(char)*14);

	//establecer la fecha de la transaccion
	for(contcar=0 ; contcar<=13; contcar++)
	{
		string[9+contcar] = getchar();//lectura del caracter
		strcat(auxiliar, string+9+contcar);//concatenacion en el auxiliar
		if(contcar == 3) //ya se leyo el año completo
		{
			entero = atoi(auxiliar); //convertir el char año a entero
			//if(entero <= 2013) ******** VALIDACION DE AÑOS MAYORES AL AÑO VIGENTE ***********
				//SALIR
			if((entero%4 == 0) && (entero%100 != 0) || (entero%400 == 0)) //verificacion de año biciesto
				biciesto = 1; //año biciesto
		}
		if(contcar == 7) //ya se leyo el mes y el dia completo
		{
			entero = atoi(string+13)*10 + atoi(string+14); //asignar el mes a entero
			
			//if(entero > 12 || entero < 1) VERIFICAR SI EL MES ES CORRECTO
				// ************ SALIR SI EL MES NO ES CORRECTO *************
			
			if((entero == 1) || (entero == 3) || (entero == 5) || (entero == 7) || (entero == 8) || (entero == 10) || (entero ==12)) // si el mes tiene 31 dias
			{
				entero = atoi(string+15)*10 + atoi(string+16);
				//if((entero < 1) || (entero > 31)) VERIFICAR SI LA FECHA ES VALIDA
					//****** SALIR LA FECHA NO ES VALIDA *******
			}
			else if((entero == 4) || (entero == 6) || (entero == 9) || (entero == 11)) //si el mes tiene 30 dias
				{	
					entero = atoi(string+15)*10 + atoi(string+16);
					//if((entero < 1) || (entero > 30)) VERIFICAR SI LA FECHA ES VALIDA
						//****** SALIR LA FECHA NO ES VALIDA *******
				}
			else if(biciesto == 0) //si es febrero y el año NO es biciesto
				{
					entero = atoi(string+15)*10 + atoi(string+16);
					//if((entero < 1) || (entero > 28)) VERIFICAR SI LA FECHA ES VALIDA
						//****** SALIR LA FECHA NO ES VALIDA *******
				}
			else //si el año es biciesto y es febrero
			{
				entero = atoi(string+15)*10 + atoi(string+16);
				//if((entero < 1) || (entero > 29)) VERIFICAR SI LA FECHA ES VALIDA
					//****** SALIR LA FECHA NO ES VALIDA *******
			}
		}		
	}
	
	entero = atoi(string+17)*10 + atoi(string+18);
	//if((entero > 24) || (entero < 1)) // verificar si la hora es valida
		//******* SALIR LA HORA NO ES VALIDA *******
	entero = atoi(string+19)*10 + atoi(string+20);
	//if((entero > 59) || (entero < 0)) //verificar si los minutos son validos
		//******* SALIR LA HORA NO ES VALIDA *******
	entero = atoi(string+21)*10 + atoi(string+22);
	//if((entero > 59) || (entero < 0)) //verificar si los segundos son validos
		//******* SALIR LA HORA NO ES VALIDA *******
	
	//asignar la fechahora del servicio
	strcpy(servicio->fechahora, auxiliar);
	//liberar auxiliar
	printf("Auxiliar: %s\n", auxiliar);
	free(auxiliar);
	printf("fechahora: %s\n", servicio->fechahora);
	
	auxiliar = (char *)malloc(sizeof(char)*20);
	while((caracter[0] = getchar()) != '\n')
	{
		strcat(auxiliar, caracter+0);
	}
	strcat(auxiliar, caracter);
	servicio->mensaje = (char *)malloc(sizeof(char)*20);
	strcpy(servicio->mensaje, auxiliar);
	printf("Auxiliar:%s\n", auxiliar);
	free(auxiliar);
	printf("Mensaje:%s\n", servicio->mensaje);
	
}
