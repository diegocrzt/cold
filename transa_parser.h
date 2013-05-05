void lectura (int ** transaccion, int * tipo);


struct AGUA
{
	char *codser; //codigo de servicio
	int numtran; //numero de transaccion
	char *fechahora; //fecha y hora de transaccion
	char *comprobante; //numero de comprobante
	int monto; //monto de la factura
	char *vencimiento; //fecha de vencimiento
	int verificador; //digito verificador
};

struct FIJO //Telefono fijo
{
	char *codser; //codigo de servicio
	int numtran; //numero de transaccion
	char *fechahora; //fecha y hora de transaccion
	int prefijo; //prefijo
	int numero; //numero telefonico
	int monto; //monto
};

struct ELEC //Consumo electrico
{
	char *codser; //codigo de servicio
	int numtran; //numero de transaccion
	char *fechahora; //fecha y hora de transaccion
	int nummed; //numero de medidor
	int monto; //monto
};

struct MOVIL //Telefono Movil
{
	char *codser; //codigo de servicio
	int numtran; //numero de transaccion
	char *fechahora; //fecha y hora de transaccion
	int prefijo; //prefijo
	int numero; //numero
	int monto; //monto
};

struct TV //Cable TV
{
	char *codser; //codigo de servicio
	int numtran; //numero de transaccion
	char *fechahora; //fecha y hora
	char *abonado; //numero de abonado
	int monto; //monto
};
