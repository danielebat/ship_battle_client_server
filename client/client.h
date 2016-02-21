//CODICE CLIENT
//SEZIONE HEADER
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

typedef enum tipo {ACQUA,NAVE}type;

struct ship{
 	int length;
 	int hit_number;//utilizzato per memorizzare colpi ricevuti, utile per confronto con lenght
 	int coord[2];//coordinate iniziali
 	int hit;
 	int destroyed; //utilizzato per visualizzare mappe
	};

struct box{
 	type t;
 	struct ship* punt;
 	int water_hit;//utilizzato per colpo a vuoto
	};

extern int remained_ships;
extern int ship_len;
extern unsigned int addrlen;
extern int udp_socket, tcp_socket;
extern int tcp_recv1, tcp_send1;
extern int tcp_recvdim, tcp_senddim;
extern int hit_coord[2];
extern struct box match_map[10][10]; //mappa client
extern struct box enemy_map[10][10]; //mappa enemy
extern struct sockaddr_in server;
extern struct sockaddr_in client;
extern struct sockaddr_in enemy;
extern struct sockaddr_in work;//controllo originalità client avversario
extern u_int16_t udp_port;
extern fd_set master_read_set;
extern fd_set master_write_set;
extern int busy;
extern int started_match;
extern int own_turn;
extern int close_conn;//utilizzato chiusura while client
extern int shell;
extern struct timeval timeout;
extern char client_name[30];
extern char enemy_name[30];
extern int  udp_message[3];//struttuta comando [0]-comando, [1]-coord x, [2]-coord y
extern void* tcp_message;
extern void* msg_buffer;
extern int first_match;

void set_match();
void place_ship(int dim, int number);
void show_my_map();
void show_my_enemy_map();
int hit_ship(int x,int y);
int enemy_hit(int x,int y);
void show_commands();
int record_name(char* client_name);
extern int record_udp_port(u_int16_t udp_port_x);
int manage_tcp_command(void* buffer);
void manage_stdin_command(char* command);
int manage_udp_command(void* buffer);
