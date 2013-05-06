#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct 
{
	char *codser; //3 DIGITOS codigo de servicio
	int numtran; //6 DIGITOS numero de transaccion
	char *fechahora; //14 DIGITOS fecha y hora de transaccion
	char *comprobante; //11 DIGITOS numero de comprobante
	int monto; //12 DIGITOS monto de la factura
	char *vencimiento; //8 DIGITOS fecha de vencimiento
	int verificador; //1 DIGITOS digito verificador
	int prefijo; //4 DIGITOS prefijo
	int numero; //7 DIGITOS numero telefonico
	int nummed; //15 DIGITOS numero de medidor
	char *abonado; //9 DIGITOS numero de abonado
}SERVICIO;


void col_parser (SERVICIO servicio);
