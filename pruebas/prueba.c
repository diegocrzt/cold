#include <stdio.h>
#include <stdlib.h>
#include "transa_parser.h"

int main (void)
{
	SERVICIO servicio;

	col_parser(servicio);
	printf("Codigo de Servicio:%s\nCodigo de transaccion:%d\nFecha-Hora:%s\n", servicio.codser, servicio.numtran, servicio.fechahora);
}
