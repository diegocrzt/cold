#include "coldaemon.h"

void writelog(int log_fd, const char * mensaje)
{
	write(log_fd, mensaje, strlen(mensaje));
	return;
}
