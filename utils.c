#include "coldaemon.h"

void writelog(int log_fd, const char * mensaje)
{
	pthread_mutex_lock(&lock);
	write(log_fd, mensaje, strlen(mensaje));
	pthread_mutex_unlock(&lock);
	return;
}

void thread_add(struct thread_list **lista, int index)
{
	syslog(LOG_ERR,"Añadir un thread a la lista");
	if(*lista == NULL)
	{
		syslog(LOG_ERR,"Lista vacía añadiendo hilo %d\n",index);
		
		*lista = (struct thread_list *)malloc(sizeof(struct thread_list));
		(*lista)->thread_index = index;
		(*lista)->siguiente = NULL;
	}else{
		struct thread_list * temp  = *lista;
		while(temp->siguiente != NULL)
		{
			temp = temp->siguiente;
		}
		temp->siguiente = (struct thread_list *)malloc(sizeof(struct thread_list));
		temp = temp->siguiente;
		temp->thread_index = index;
		temp->siguiente = NULL;
	}
	return;
}

pthread_t * thread_get(struct thread_list *lista, int index)
{
	syslog(LOG_DEBUG,"Obteniendo un pthread_t para el hilo %d",index);
	if(lista == NULL)
	{
		syslog(LOG_ERR,"[CRITICAL ERROR]thread_list llega nulo!\n");
		exit(NULL_THREAD);
	}
	do
	{
		if(index == lista->thread_index)
		{
			return &(lista->hilo);
		}
	}while(lista->siguiente != NULL && (lista = lista->siguiente) );

	syslog(LOG_ERR,"No existe el hilo buscado");
	return NULL;
}

void thread_del(struct thread_list **lista, int index)
{
	struct thread_list * temp = *lista;
	struct thread_list * anterior = NULL;
	if(*lista == NULL)
	{
		syslog(LOG_ERR,"No se pueden borrar los hilos");
		exit(NULL_THREAD);
	}
	do{
		if(index == temp->thread_index)
		{
			if(anterior != NULL)
			{
				anterior->siguiente = temp->siguiente;
				
			}else
			{
				*lista = temp->siguiente;
			}
			free(temp);
			return;
		}
		anterior = temp;
	}while(temp->siguiente != NULL && (temp = temp->siguiente)); // ACA ME QUEDÉ!
	
	syslog(LOG_ERR,"No hay hilos ejecutandose\n");
	return;
}
