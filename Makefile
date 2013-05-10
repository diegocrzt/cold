cold: coldaemon.o config_parser.o config_parser.h
	cc coldaemon.o config_parser.o -o cold

coldaemon.o: coldaemon.c coldaemon.h
	cc -c coldaemon.c -o coldaemon.o

config_parser.o: config_parser.c config_parser.h
	cc -c config_parser.c -o config_parser.o

.PHONY: clean

clean:
	rm -rf cold *.o

