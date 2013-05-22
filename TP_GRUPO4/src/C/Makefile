cold: coldaemon.o config_parser.o coredaemon.o utils.o transa_parser.o db_module.o thread_manager.o config_module.o coldaemon.h
	cc coldaemon.o config_parser.o coredaemon.o transa_parser.o utils.o db_module.o thread_manager.o config_module.o -o cold -lpthread -lpq

coldaemon.o: coldaemon.c coldaemon.h
	cc -c coldaemon.c -o coldaemon.o

config_parser.o: config_parser.c coldaemon.h
	cc -c config_parser.c -o config_parser.o

coredaemon.o: coredaemon.c coldaemon.h
	cc -c coredaemon.c -o coredaemon.o

transa_parser.o: transa_parser.c coldaemon.h
	cc -c transa_parser.c -o transa_parser.o

utils.o: utils.c coldaemon.h
	cc -c utils.c -o utils.o

db_module.o: db_module.c coldaemon.h
	cc -c db_module.c -o db_module.o	

thread_manager.o: thread_manager.c coldaemon.h
	cc -c thread_manager.c -o thread_manager.o

config_module.o: config_module.c coldaemon.h
	cc -c config_module.c -o config_module.o

.PHONY: clean install uninstall

clean:
	rm -rf cold *.o

install:
	install cold /bin/cold
	install -m 644 cold.users.acl /etc/cold.users.acl
	install -m 644 cold.properties /etc/cold.properties
	install -m 755 coldaemon /etc/init.d/coldaemon
	update-rc.d coldaemon defaults 80

uninstall:
	rm -f /bin/cold
	rm -f /etc/cold.user.acl
	rm -f /etc/cold.properties
	rm -f /etc/init.d/coldaemon

