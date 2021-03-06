#include "coldaemon.h"

static void
exit_nicely(PGconn *conn)
{
    PQfinish(conn);
    return;
}

int existe_factura(const char * cod_serv, const char * compr,const char * medidor, const char * prefijo, const char * numero, const char * abonado,const char * monto, const char * vencimiento, PGconn * conn, PGresult * res,int log_fd,char * resp){
	
	const char *parametros[8];
	parametros[0] = compr;
	parametros[1] = medidor;
	parametros[2] = abonado;
	parametros[3] = prefijo;
	parametros[4] = numero;
	parametros[5] = monto;
	parametros[6] = vencimiento;
	parametros[7] = cod_serv;
	int entero = 0;
	char temp[512];
	res = PQexecParams(conn,"SELECT * FROM pendientes WHERE (compr=$1 OR medidor=$2 OR abonado=$3 OR (numero=$4 AND prefijo=$5)) AND (monto=$6 AND vencimiento>=$7 AND cod_serv=$8);",
						8,
						NULL,
						parametros,
						NULL,
						NULL,
						0);
	
	if (PQntuples(res) == 0)
    	{
		sprintf(resp,"Factura invalida\n");
    		sprintf(temp,"Factura inválida\n");
		writelog(log_fd,temp);
       		entero = 1;
    	}
	
    	PQclear(res);
	return entero;
}

int existe_trx(const char * transaccion, PGconn * conn, PGresult * res, int log_fd, char * resp){
	const char *parametros[1];
	parametros[0] = transaccion;
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
		sprintf(resp,"No existe transaccion a reversar\n");
		sprintf(temp,"No existe transaccion a reversar\n");
		writelog(log_fd,temp);
		entero = 1;
	}
	PQclear(res);
	return entero;
}

int db_module(char * operacion, SERVICIO serv, char * usuario, int log_fd, char * resp)
{
	time_t current_time;
   	char* c_time_string;
 
	//Se obtiene el tiempo en segundos 
	current_time = time(NULL);

	//Se convierte al formato de hora local
	c_time_string = ctime(&current_time);
	limpiar_linea(c_time_string);
 
	char temp[512] = {0};
	const char 	*conninfo;  
	PGconn     	*conn;
	PGresult   	*res;
    
  	const char *paramValues[15];
  	const char *paramValues2[14]; //utilizado para comandos de eliminacion
	const char *paramRev[14];
	int         paramLengths[14];
	int         paramFormats[14];
	int		t,f,tuples;              
	char aux_monto[512];
	char aux_nummed[512];
	char aux_numtran[512];
	char aux_venc[512];
	char aux_verificador[2];
	
	conninfo = "dbname = coldaemon";
	
	paramValues[0] = serv.codser;
	paramValues[1] = serv.tipofact;
	paramValues[2] = serv.comprobante;
	sprintf(aux_monto,"%lu",serv.monto);
	paramValues[3] = aux_monto;
	sprintf(temp,"Monto = %s\n",paramValues[3]);
	sprintf(aux_verificador,"%d",serv.verificador);
	paramValues[4] = aux_verificador;
	paramValues[5] = serv.prefijo;
	paramValues[6] = serv.numero;
	paramValues[7] = serv.nummed;
	paramValues[8] = serv.abonado;
	sprintf(aux_numtran,"%d",serv.numtran);
	paramValues[9] = aux_numtran;
	paramValues[10] = serv.fechahora;
	paramValues[11] = usuario;
	paramValues[12] = "20131212235959";
	paramValues[13] = operacion;
	paramValues[14] = serv.mensaje;
	/*	
	strcpy(aux_venc,serv.vencimiento);
	strcat(aux_venc,"235959");
	paramValues[12] = aux_venc;
	*/	
	paramRev[0]=operacion;
	paramRev[1]=serv.codser;
	paramRev[2]=serv.fechahora;
	paramRev[3]=usuario; 
	paramRev[4]=serv.mensaje;
	paramRev[5]=serv.tipofact;
	paramRev[6]=serv.comprobante;
	paramRev[7]=aux_monto; paramRev[8]=aux_verificador;
	paramRev[9]=serv.prefijo;
	paramRev[10]=serv.numero;
	paramRev[11]=serv.nummed;
	paramRev[12]=serv.abonado;
	paramRev[13]=aux_numtran;
	
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
	//INICIO DE OPERACIONES EN LA BASE DE DATOS
	if(strcmp(operacion, "col") == 0){
		sprintf(temp,"[%s::%s::%s::Peticion de cobro]\n",c_time_string,usuario,operacion);
		writelog(log_fd,temp);
		/*
		*	se envia serv.fecha hora a existe_factura y no serv.vencimiento
		*	se cambia paramValues[10] por paramValues[12]
		*/
		if(existe_factura(paramValues[0],paramValues[2],paramValues[7],paramValues[5],paramValues[6],paramValues[8],paramValues[3],paramValues[10],conn,res,log_fd,resp) == 0){
			//insertar registro en la tabla de pagadas
			res = PQexecParams(conn,
                       "INSERT INTO pagadas VALUES ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13);",
                       13,       // 13 parametros
                       NULL,    // let the backend deduce param type
                       paramValues,
                       NULL,    // don't need param lengths since text
                       NULL,    // default to all text params
                       0);      // ask for non binary results
            if (PQresultStatus(res) != PGRES_COMMAND_OK)
    		{
			//SE GENERA EL MENSAJE DE RETORNO
			sprintf(resp,"%s%s%s001Fallo el cobro\n",				serv.codser,aux_numtran,serv.fechahora);
        		sprintf(temp,"insert command failed: %s", PQerrorMessage(conn));
			writelog(log_fd,temp);
        		PQclear(res);
        		exit_nicely(conn);
    		}     
    		
			PQclear(res);
			//eliminar registro de la tabla pendientes
			//asignar valores a paramValues (si! hace falta)
			if (strcmp(serv.codser,"001") == 0)
			{
				//eliminar aguas
				paramValues2[0] = serv.comprobante;
				res = PQexecParams(conn,
						"DELETE FROM pendientes WHERE compr=$1;",
						1,
						NULL,
						paramValues2,
						NULL,
						NULL,
						0);
			}else if(strcmp(serv.codser,"002") == 0){
				//eliminar fijo
				paramValues2[0] = serv.prefijo;
				paramValues2[1] = serv.numero;
				
				res = PQexecParams(conn,
						"DELETE FROM pendientes WHERE numero=$2 AND prefijo=$1;",
						2,
						NULL,
						paramValues2,
						NULL,
						NULL,
						0);
			}else if(strcmp(serv.codser,"003") == 0){
				//eliminar electricidad
				paramValues2[0] = serv.nummed;
				
				res = PQexecParams(conn,
						"DELETE FROM pendientes WHERE medidor=$1;",
						1,
						NULL,
						paramValues2,
						NULL,
						NULL,
						0);
				
			}else if(strcmp(serv.codser,"004") == 0){
				//eliminar movil
				paramValues2[0] = serv.prefijo;
				paramValues2[1] = serv.numero;
				res = PQexecParams(conn,
						"DELETE FROM pendientes WHERE (prefijo=$1 AND numero=$2);",
						2,
						NULL,
						paramValues2,
						NULL,
						NULL,
						0);
			}else if(strcmp(serv.codser,"005") == 0){
				//eliminar cable
				paramValues2[0] = serv.abonado;
				res = PQexecParams(conn,
						"DELETE FROM pendientes WHERE abonado=$1;",
						1,
						NULL,
						paramValues2,
						NULL,
						NULL,
						0);
			}
			if (PQresultStatus(res) != PGRES_COMMAND_OK)
    		{
			//SE GENERA EL MENSAJE DE RETORNO
			sprintf(resp,"%s%s%s001Fallo el cobro\n",serv.codser,aux_numtran,serv.fechahora);
        		sprintf(temp,"delete command failed: %s", PQerrorMessage(conn));
				writelog(log_fd,temp);
        		PQclear(res);
        		exit_nicely(conn);
    		}
    		
			PQclear(res);
			//agregar a transacciones
    		res = PQexecParams(conn,
        	"INSERT INTO transacciones VALUES ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14);",
           	14,       // 13 parametros
            NULL,    // let the backend deduce param type
            paramRev,
            NULL,    // don't need param lengths since text
            NULL,    // default to all text params
            0);      // ask for non binary results
			
			if (PQresultStatus(res) != PGRES_COMMAND_OK)
    		{
			//SE GENERA EL MENSAJE DE RETORNO
			sprintf(resp,"%s%s%s001Fallo el cobro\n",serv.codser,aux_numtran,serv.fechahora);
        		sprintf(temp,"fallo de envio a transaccion: %s", PQerrorMessage(conn));
				writelog(log_fd,temp);
        		PQclear(res);
        		exit_nicely(conn);
    		}
		sprintf(resp,"%s%s%s000Cobro Exitoso\n",serv.codser,aux_numtran,serv.fechahora);
		writelog(log_fd,resp);
    		PQclear(res);
		}
	}else if(strcmp(operacion, "rev") == 0){
		sprintf(temp,"[%s::%s::%s::Peticion de reversa]\n", 					c_time_string,usuario,operacion);
		writelog(log_fd,temp);
		if(existe_trx(paramValues[9],conn,res,log_fd,resp) == 0){
			sprintf(temp,"Existe la transaccion\nMoviendo a pendientes\n");
			writelog(log_fd,temp);
			
			//mover a pendientes
			res = PQexec(conn,"INSERT INTO pendientes 		(cod_serv,tipo,compr,monto,vencimiento,dig_verif,prefijo,numero,medidor,abonado) SELECT cod_serv,tipo,compr,monto,vencimiento,dig_verif,prefijo,numero,medidor,abonado	FROM pagadas;");
			
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
			paramValues2[0] = aux_numtran;
			res = PQexecParams(conn,
                       "DELETE FROM pagadas WHERE transaccion=$1;",
                       1,       // 1 parametro
                       NULL,    // let the backend deduce param type
                       paramValues2,
                       NULL,    // don't need param lengths since text
                       NULL,    // default to all text params
                       0);      // ask for non binary results
                       
            		if (PQresultStatus(res) != PGRES_COMMAND_OK)
    			{
				sprintf(temp,"delete rev command failed: %s",PQerrorMessage(conn));
				writelog(log_fd,temp);
				PQclear(res);
				exit_nicely(conn);
    			}
    		
    		//agregar a transacciones
    		res = PQexecParams(conn,
        	"INSERT INTO transacciones VALUES ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14);",
           	14,       // 13 parametros
            NULL,    // let the backend deduce param type
            paramRev,
            NULL,    // don't need param lengths since text
            NULL,    // default to all text params
            0);      // ask for non binary results
			
		if (PQresultStatus(res) != PGRES_COMMAND_OK)
    		{
        		sprintf(temp,"fallo de envio a transaccion: %s", PQerrorMessage(conn));
				writelog(log_fd,temp);
        		PQclear(res);
        		exit_nicely(conn);
    		}
		
    		PQclear(res);
		}
	}else if(strcmp(operacion, "lastrx") == 0){
		sprintf(temp,"[%s::%s::%s::Peticion de listado]\n",c_time_string,usuario,operacion);
		writelog(log_fd,temp);
		paramValues2[0] = usuario;
		int retorno = 0;
		res = PQexecParams(conn, "SELECT * FROM transacciones WHERE usuario=$1;",
						1,
						NULL,
						paramValues2,
						NULL,
						NULL,
						0);
		if (PQntuples(res) == 0)
		{
			sprintf(temp,"No hay transacciones recientes\n");
			writelog(log_fd,temp);
			sprintf(resp,"No hay transacciones recientes\n");
			retorno = 1;
		}else{
			resp[0] = '\0';
			tuples = PQntuples(res) - 3;
			if (PQntuples(res) < 3)
				tuples = 0;
			for(t = tuples; t < PQntuples(res); t++)
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
		//agregar a transacciones
		/*
		*	Se podria guardar la fecha/hora del sistema
		*/
		paramRev[2] = c_time_string;
    		res = PQexecParams(conn,"INSERT INTO transacciones VALUES ($1,$2,$3,$4,'0',0,0,0,0,0,0,0,0,0);",
		4,       // 14 parametros
            NULL,    // let the backend deduce param type
            paramRev,
            NULL,    // don't need param lengths since text
            NULL,    // default to all text params
            0);      // ask for non binary results
			
			if (PQresultStatus(res) != PGRES_COMMAND_OK)
    		{
			
        		sprintf(temp,"fallo de envio a transaccion: %s", PQerrorMessage(conn));
				writelog(log_fd,temp);
        		PQclear(res);
        		exit_nicely(conn);
    		}
    		PQclear(res);
	
	}
	//Se cierra la conexión a la base de datos
    PQfinish(conn);
	return OK;
}
