#!/bin/sh
# Script ejemplo para arranque de servicios en /etc/init.d/
#

case "$1" in
start)
echo "Iniciando servicio... "
# Aquí comando a ejecutar para arrancar el servicio
	cold /home/elena/coldaemon/cold.properties
;;
stop)
# Aquí comando a ejecutar para detener el servicio
	pid=`pidof cold`
	if [ -z $pid ]; then        
		echo "No se puede detener el servicio, el demonio no esta corriendo"
	else
		echo "Deteniendo servicio..."
		sudo kill -TERM $pid
	fi
#TERM (15)

;;
restart)
echo "Reiniciando servicio..."
# Aquí comando a ejecutar para reiniciar el servicio
	pid=`pidof cold`
	if [ -z $pid ]; then        
		echo "No se puede reiniciar el servicio, el demonio no esta corriendo"	
	else
		sudo kill -TERM $pid
		cold /home/elena/coldaemon/cold.properties
	fi
#TERM (15)

;;
reload)
# Aquí comando a ejecutar para recargar el servicio
	pid=`pidof cold`
	vacio=" "
	if [ -z $pid ]; then 
		echo "No se puede recargar.. el demonio no esta corriendo.."
	else 
		echo "Recargando archivo de configuracion del servicio..."	
		sudo kill -HUP $pid
	fi
	#HUP (1)

;;
status)
	pid=`pidof cold`
	if [ -z $pid ]; then
		echo "El demonio no esta activo.."
	else
		echo "El demonio esta activo PID= $pid"
	
	fi

;;
*)
echo "Modo de empleo: /etc/init.d/mi_script {start|stop|restart|reload|status}"
exit 1
;;
esac
exit 0
