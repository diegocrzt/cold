#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct 
{
	char *codser; //3 DIGITOS codigo de servicio
	int numtran; //6 DIGITOS numero de transaccion
	char *fechahora; //14 DIGITOS fecha y hora de transaccion
	char *tipofact; // 3 DIGITOS tipo de factura
	char *comprobante; //11 DIGITOS numero de comprobante
	int monto; //12 DIGITOS monto de la factura
	char *vencimiento; //8 DIGITOS fecha de vencimiento
	int verificador; //1 DIGITOS digito verificador
	char *prefijo; //4 DIGITOS prefijo
	char *numero; //7 DIGITOS numero telefonico
	int nummed; //15 DIGITOS numero de medidor
	char *abonado; //9 DIGITOS numero de abonado
	char *mensaje; //20 CARACTERES DE MENSAJE
}SERVICIO;


void col_parser (SERVICIO *servicio);
void rev_parser (SERVICIO *reversa);
