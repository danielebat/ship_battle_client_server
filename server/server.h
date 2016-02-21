// CODICE SERVER
// SEZIONE HEADER
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <arpa/inet.h>
#define INTEGER 4

struct client{
char name[30];
char IP[INET_ADDRSTRLEN];
u_int16_t udp_port_client;// i tre precedenti sono richiesti esplicitamente
int enemy_id; // se non sto giocando enemy_id == -1
int id;
int busy;// 0=false, 1=true
int recv1, send1;
int dim_recv, dim_send;
void* buffer_client; // da buffer
struct  client* next; // puntatore per la lista
};
	
// variabili globali

extern struct client* waiting_list; // puntatore alla lista dei client connessi al server   
extern fd_set master_read_set;
extern fd_set master_write_set;
extern unsigned int addrlen;

// funzioni

struct client* find_client(int id);
struct client* find_client_by_name(char* name);
void rmv_client(int id);
extern struct client* add_client(int id, struct sockaddr_in clientaddr);
int manage_command(void* buffer, struct client* client_to_serve);

