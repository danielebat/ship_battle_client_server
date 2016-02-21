battaglia_navale_server : sfunctions.o smain.o
	gcc sfunctions.o smain.o -o battaglia_navale_server

smain.o : server.h smain.c
	gcc -c -Wall smain.c

sfunctions.o : server.h sfunctions.c
	gcc -c -Wall sfunctions.c
