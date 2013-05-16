cold: coldaemon.o config_parser.o coredaemon.o utils.o transa_parser.o db_module.o coldaemon.h
	cc coldaemon.o config_parser.o coredaemon.o transa_parser.o utils.o db_module.o -o cold -lpthread -lpq

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

.PHONY: clean

clean:
	rm -rf cold *.o

