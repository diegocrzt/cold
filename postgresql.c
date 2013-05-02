#include <stdio.h>
#include <postgresql/libpq-fe.h>
#include <string.h>

static void
exit_nicely(PGconn *conn)
{
    PQfinish(conn);
    return;
}

int main(int argc, char **argv)
{
    const char *conninfo;
    PGconn     *conn;
    PGresult   *res;
    int         nFields;
    int         i,
                j;

    /*
     * Se selecciona la base de datos a conectar de acuerdo al parámetro de entrada
     */
    if (argc > 1)
        conninfo = argv[1];
    else
        conninfo = "dbname = coldaemon";

    /* Se realiza la conexión a la base de datos */
    conn = PQconnectdb(conninfo);

    /* Se chequea si la conexión backend ha sido establecida */
    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection to database failed: %s",
                PQerrorMessage(conn));
        exit_nicely(conn);
    }

	/* inicio del bloque de transacciones */
    res = PQexec(conn, "BEGIN");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "BEGIN command failed: %s", PQerrorMessage(conn));
        PQclear(res);
        exit_nicely(conn);
    }

    /*
     * PQclear PGresult siempre que ya no sea necesario para evitar desperdicio de memoria
     * 
     */
    PQclear(res);

    /*
     * se extraen las filas de pg_database, el catalogo de bases de datos del sistema
     * 
     */
    res = PQexec(conn, "DECLARE myportal CURSOR FOR select * from pg_database");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "DECLARE CURSOR failed: %s", PQerrorMessage(conn));
        PQclear(res);
        exit_nicely(conn);
    }
    PQclear(res);

    res = PQexec(conn, "FETCH ALL in myportal");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "FETCH ALL failed: %s", PQerrorMessage(conn));
        PQclear(res);
        exit_nicely(conn);
    }

    /* primero se imprimen los nombres de los atributos */
    nFields = PQnfields(res);
    for (i = 0; i < nFields; i++)
        printf("%-15s", PQfname(res, i));
    printf("\n\n");

    /* luego se imprimen las filas */
    for (i = 0; i < PQntuples(res); i++)
    {
        for (j = 0; j < nFields; j++)
            printf("%-15s", PQgetvalue(res, i, j));
        printf("\n");
    }

    PQclear(res);

    /* se cierra el portal sin chequear errores */
    res = PQexec(conn, "CLOSE myportal");
    PQclear(res);

    /* se termina la transacción */
    res = PQexec(conn, "END");
    PQclear(res);

    /* se cierra la conexión a la base de datos */
    PQfinish(conn);

    return 0;
}


