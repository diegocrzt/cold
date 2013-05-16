#include "coldaemon.h"

static void
exit_nicely(PGconn *conn)
{
    PQfinish(conn);
    return;
}

int existe_factura(const char * compr,const char * medidor, const char * prefijo, const char * numero, const char * abonado, PGconn * conn, PGresult * res,int log_fd){
	
	const char *parametros[5];
	parametros[0] = compr;
	parametros[1] = medidor;
	parametros[2] = abonado;
	parametros[3] = prefijo;
	parametros[4] = numero;
	/*parametros[0] = "09";
	parametros[1] = "654";
	parametros[2] = "987";
	parametros[3] = "5464";
	parametros[4] = "987";*/
	int entero = 0;
	char temp[512];
	res = PQexecParams(conn,
						"SELECT * FROM pendientes WHERE compr=$1 OR medidor=$2 OR abonado=$3 OR 							(numero=$4 AND prefijo=$5)",
						5,
						NULL,
						parametros,
						NULL,
						NULL,
						0);
	
	if (PQntuples(res) == 0)
    {
    	sprintf(temp,"No se encontro la factura\n");
	writelog(log_fd,temp);
       	entero = 1;
    }
    PQclear(res);
	return entero;
}

int coincide_monto(const char * monto, PGconn * conn, PGresult * res, int log_fd){
	const char *parametros[1];
	parametros[0] = monto;
	//parametros[0] = "125000";
	int entero = 0;
	char temp[512];
	res = PQexecParams(conn, "SELECT * FROM pendientes WHERE monto=$1",
						1,
						NULL,
						parametros,
						NULL,
						NULL,
						0);
	if (PQntuples(res) == 0)
	{
		sprintf(temp,"Los montos no coinciden\n");
		writelog(log_fd,temp);
		entero = 1;
	}
	PQclear(res);
	return entero;
}

int no_vencio(const char * vencimiento, PGconn * conn, PGresult * res, int log_fd){
	const char *parametros[1];
	parametros[0] = vencimiento;
	//parametros[0] = "20131313";
	int entero = 0;
	char temp[512];
	res = PQexecParams(conn, "SELECT * FROM pendientes WHERE vencimiento<=$1",
						1,
						NULL,
						parametros,
						NULL,
						NULL,
						0);
	if (PQntuples(res) == 0)
	{
		sprintf(temp,"Factura Vencida\n");
		writelog(log_fd,temp);
		entero = 1;
	}
	PQclear(res);
	return entero;
}

int existe_trx(const char * transaccion, PGconn * conn, PGresult * res, int log_fd){
	const char *parametros[1];
	parametros[0] = transaccion;
	//parametros[0] = "987";
	int entero = 0;
	char temp[512];
	res = PQexecParams(conn, "SELECT * FROM pagadas WHERE transaccion=$1",
						1,
						NULL,
						parametros,
						NULL,
						NULL,
						0);
	if (PQntuples(res) == 0)
	{
		sprintf(temp,"No existe transaccion a reversar\n");
		writelog(log_fd,temp);
		entero = 1;
	}
	PQclear(res);
	return entero;
}

int db_module(char * operacion, SERVICIO serv, char * usuario, int log_fd, char * resp)
{
	const char 	*conninfo;  
	PGconn     	*conn;
	PGresult   	*res;
    
  	const char *paramValues[14];
    int         paramLengths[14];
    int         paramFormats[14];
    int			t,f;              
    //char		*operacion;            
    char 		*cod_serv = "001";
    char		*transaccion = "65";
    char		*comprobante = "80878"; 
    char		*monto = "12000";
    char		*fecha = "9/9/11";
    char		*vencimiento = "9/9/10";
	char 		*comando;
	char aux_monto[512];
	char aux_nummed[512];
	char aux_numtran[512];
	char aux_verificador[2];
	char temp[512] = {0};
	conninfo = "dbname = coldaemon";
	//operacion = "lastrx";

	// Asignando valores a paramValues

/*
	paramValues[0]="001";				//cod_serv
	paramValues[1]="002";				//tipo
	paramValues[2]="12345678912";		//compr
	paramValues[3]="000000100000"; 		//monto
	paramValues[4]="7";					//dig_verif
	paramValues[5]="0644";   			//prefijo
	paramValues[6]="9876543";			//numero
	paramValues[7]="987654321012345";	//medidor
	paramValues[8]="987654321";			//abonado
	paramValues[9]="123456";			//transaccion
	paramValues[10]="20131212163215";	//fecha_hora
	paramValues[11]="ariel";			//usuario
	paramValues[12]="20131212";			//vencimiento
*/

	paramValues[0] = serv.codser;
	paramValues[1] = serv.tipofact;
	paramValues[2] = serv.comprobante;
	sprintf(aux_monto,"%d",serv.monto);
	//strcpy(paramValues[3],temp);
	paramValues[3] = aux_monto;
	sprintf(temp,"Monto = %s\n",paramValues[3]);
	writelog(log_fd, temp);
	sprintf(aux_verificador,"%d",serv.verificador);
	paramValues[4] = aux_verificador;
	paramValues[5] = serv.prefijo;
	paramValues[6] = serv.numero;
	sprintf(aux_nummed,"%d",serv.nummed);
	//strcpy(paramValues[7],temp);
	paramValues[7] = aux_nummed;
	paramValues[8] = serv.abonado;
	sprintf(aux_numtran,"%d",serv.numtran);
	//strcpy(paramValues[9],temp);
	paramValues[9] = aux_numtran;
	paramValues[10] = serv.fechahora;
	paramValues[11] = usuario;
	paramValues[12] = serv.vencimiento;

	
    // Se realiza la conexión a la base de datos
    conn = PQconnectdb(conninfo);

    // Se chequea si la conexión backend ha sido establecida
    if (PQstatus(conn) != CONNECTION_OK)
    {
        sprintf(temp, "Connection to database failed: %s",
                PQerrorMessage(conn));
	writelog(log_fd,temp);
        exit_nicely(conn);
    }
	
	/* 
	*	- verificar operacion
	*	- si es insercion comprobar que los id de las operaciones no coincidan, generar la cadena, 		*  	insertar y borrar la factura de pendientes
	*	- si es reverso comprobar que exista la transaccion, mover a pendientes y borrar de pagadas
	*	- si es listado listar las 3 ultimas transacciones del user
	*/
	
	if(strcmp(operacion, "col") == 0){
		
		if(existe_factura(paramValues[2],paramValues[7],paramValues[5],paramValues[6],paramValues[8],conn,res,log_fd) == 0 && coincide_monto(paramValues[3],conn,res,log_fd) == 0 && no_vencio(paramValues[12],conn,res,log_fd) == 0){
			//insertar registro en la tabla de pagadas
			res = PQexecParams(conn,
                       "INSERT INTO Pagadas VALUES ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13);",
                       13,       // 13 parametros
                       NULL,    // let the backend deduce param type
                       paramValues,
                       NULL,    // don't need param lengths since text
                       NULL,    // default to all text params
                       0);      // ask for non binary results
                       
              
            if (PQresultStatus(res) != PGRES_COMMAND_OK)
    		{
        		sprintf(temp,"insert command failed: %s", PQerrorMessage(conn));
			writelog(log_fd,temp);
        		PQclear(res);
        		exit_nicely(conn);
    		}     
    		sprintf(temp,"Insert ejecutado\n");
		writelog(log_fd,temp);
			PQclear(res);
			
			//eliminar registro de la tabla pendientes
			//asignar valores a paramValues (si! hace falta)
			
			paramValues[0]="12345678912";		//compr
			paramValues[1]="987654321012345";	//medidor
			paramValues[2]="0644";   			//prefijo
			paramValues[3]="9876543";			//numero
			paramValues[4]="987654321";			//abonado
			
			res = PQexecParams(conn,
						"DELETE FROM pendientes WHERE compr=$1 OR medidor=$2 OR abonado=$5 OR (prefijo=$3 AND numero=$4);",
						5,
						NULL,
						paramValues,
						NULL,
						NULL,
						0);
						
			if (PQresultStatus(res) != PGRES_COMMAND_OK)
    		{
        		sprintf(temp,"delete command failed: %s", PQerrorMessage(conn));
			writelog(log_fd,temp);
        		PQclear(res);
        		exit_nicely(conn);
    		}
    		sprintf(temp,"Delete ejecutado\n");
		writelog(log_fd,temp);
			PQclear(res);
		}
		
	}else if(strcmp(operacion, "rev") == 0){
	
		if(existe_trx(paramValues[9],conn,res,log_fd) == 0){
			sprintf(temp,"existe la transaccion\nMoviendo a pendientes\n");
			writelog(log_fd,temp);
			
			//mover a pendientes
			/*
			*	En caso de error volver a definir id en pendientes y hacer un PQexec
			*	para asignar un id antes del resto
			*/
			
			res = PQexec(conn,"INSERT INTO pendientes 		(cod_serv,tipo,compr,monto,vencimiento,dig_verif,prefijo,numero,medidor,abonado) SELECT cod_serv,tipo,compr,monto,vencimiento,dig_verif,prefijo,numero,medidor,abonado	FROM pagadas");
			
			
			if (PQresultStatus(res) != PGRES_COMMAND_OK)
    		{
        		sprintf(temp,"moving command failed: %s", PQerrorMessage(conn));
			writelog(log_fd,temp);
        		PQclear(res);
        		exit_nicely(conn);
    		}
    		
			//borrar de pagadas
			sprintf(temp,"borrando de pagadas\n");
			writelog(log_fd,temp);
			paramValues[0] = "123456";
			res = PQexecParams(conn,
                       "DELETE FROM pagadas WHERE transaccion=$1;",
                       1,       // 1 parametro
                       NULL,    // let the backend deduce param type
                       paramValues,
                       NULL,    // don't need param lengths since text
                       NULL,    // default to all text params
                       0);      // ask for non binary results
                       
            if (PQresultStatus(res) != PGRES_COMMAND_OK)
    		{
        		sprintf(temp,"delete rev command failed: %s", PQerrorMessage(conn));
			writelog(log_fd,temp);
        		PQclear(res);
        		exit_nicely(conn);
    		}
		}
	
	}else if(strcmp(operacion, "lastrx") == 0){
		paramValues[0] = usuario;
		int retorno = 0;
		res = PQexecParams(conn, "SELECT * FROM pagadas WHERE usuario=$1",
						1,
						NULL,
						paramValues,
						NULL,
						NULL,
						0);
		if (PQntuples(res) == 0)
		{
			sprintf(temp,"No hay transacciones recientes\n");
			writelog(log_fd,temp);
			retorno = 1;
		}else{
			resp[0] = '\0';
			for (t = 0; t < PQntuples(res) && t < 3; t++)
			{
				for (f = 0; f < PQnfields(res); f++)
				{
				    //concatenar resultados de PQgetvalue
				    sprintf(temp,"%s ", PQgetvalue(res, t, f));
					strcat(resp,temp);
					writelog(log_fd,temp);
				}
				sprintf(temp,"\n");
				strcat(resp,temp);
				writelog(log_fd,temp);
			}
		}
		
		PQclear(res);
		
	}else if(strcmp(operacion, "close") == 0){
	
	}else if(strcmp(operacion, "help") == 0){
	
	}
		
	// Se cierra la conexión a la base de datos
    PQfinish(conn);

    return 0;
}
