cold: coldaemon.o config_parser.o coredaemon.o config_parser.h
	cc coldaemon.o config_parser.o coredaemon.o -o cold -lpthread

coldaemon.o: coldaemon.c coldaemon.h
	cc -c coldaemon.c -o coldaemon.o

config_parser.o: config_parser.c config_parser.h
	cc -c config_parser.c -o config_parser.o

coredaemon.o: coredaemon.c coldaemon.h
	cc -c coredaemon.c -o coredaemon.o

.PHONY: clean

clean:
	rm -rf cold *.o

