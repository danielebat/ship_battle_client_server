battaglia_navale_client : cfunctions.o cmain.o
	gcc cfunctions.o cmain.o -o battaglia_navale_client

cmain.o : client.h cmain.c
	gcc -c -Wall cmain.c

cfunctions.o : client.h cfunctions.c
	gcc -c -Wall cfunctions.c
