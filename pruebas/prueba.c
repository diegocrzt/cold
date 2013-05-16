#include <stdio.h>
#include <stdlib.h>
#include "transa_parser.h"

int main (void)
{
	SERVICIO servicio;
	SERVICIO reversa;

	//col_parser(&servicio);
	/*printf("Codigo:%s\n",servicio.codser);
	printf("Transaccion:%d\n",servicio.numtran);
	printf("FechaHora:%s\n",servicio.fechahora);
	printf("TipoFactura:%s\n",servicio.tipofact);
	printf("Comprobante:%s\n",servicio.comprobante);
	printf("Monto:%d\n",servicio.monto);
	printf("Vencimiento:%s\n",servicio.vencimiento);
	printf("Verificador:%d\n",servicio.verificador);
	printf("Prefijo:%s\n",servicio.prefijo);
	printf("Numero:%s\n",servicio.numero);
	printf("Medidor:%d\n",servicio.nummed);
	printf("Abonado:%s\n",servicio.abonado);*/
	rev_parser(&reversa);
}
