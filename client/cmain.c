// SEZIONE MAIN

#include "client.h"

int remained_ships=10;
int ship_len=sizeof(struct ship);
unsigned int addrlen=sizeof(struct sockaddr_in);
int udp_socket;
int tcp_socket;
int tcp_recv1=1;
int tcp_send1=1;
int tcp_recvdim;
int tcp_senddim;
int hit_coord[2];
struct box match_map[10][10];
struct box enemy_map[10][10];
struct sockaddr_in server;
struct sockaddr_in client;
struct sockaddr_in enemy;
struct sockaddr_in work;
u_int16_t udp_port;
fd_set master_read_set;
fd_set master_write_set;
struct timeval timeout;
int busy=0;
int started_match=0;
int own_turn=0; // comincia l'avversario
int close_conn=0;
int shell=0;
char client_name[30];
char enemy_name[30];
int  udp_message[3];
void* tcp_message;
void* msg_buffer;
int first_match=1;


int main(int argc, char* argv[]){

fd_set read_temp_set;
fd_set write_temp_set;
int error;
int max_fd_set;
timeout.tv_sec=60;
timeout.tv_usec=0;
char port[6];
char command [30];
int right=0;
int max_try=3;


// fase iniziale : connessione al server

if(argc!=3){
  	printf("Parametri non inseriti correttamente, chiusura in corso...\n");
  	exit(1);
	}

printf("Connessione al server in corso (indirizzo di ascolto %s, porta di ascolto %s)\n",argv[1],argv[2]);

//creazione socket

tcp_socket=socket(AF_INET,SOCK_STREAM,0);
if(tcp_socket==-1){
  	perror("Errore creazione socket tcp");
  	exit(1);
	}
udp_socket=socket(AF_INET,SOCK_DGRAM,0);
if(udp_socket==-1){
 	perror("Errore creazione socket udp");
 	close(tcp_socket);
 	exit(1);
	}
	
max_fd_set=(tcp_socket>udp_socket)?tcp_socket:udp_socket;
memset(&server,0, sizeof(server));
memset(&client,0, sizeof(client));
memset(&enemy, 0, sizeof(enemy));
memset(&work, 0, sizeof(work));
enemy.sin_family=AF_INET;
work.sin_family=AF_INET;
client.sin_family=AF_INET;
server.sin_family=AF_INET;
// per il server
server.sin_port=htons(atoi(argv[2]));
inet_pton(AF_INET, argv[1], &server.sin_addr.s_addr);

//Connessione socket

error=connect(tcp_socket,(struct sockaddr*)&server,addrlen);
if(error==-1){
 	perror("Impossibile connettersi al server");
 	close(tcp_socket);
 	close(udp_socket);
 	exit(1); 
	}

printf("Connessione con il server %s sulla porta %s avvenuta con successo.\n", argv[1], argv[2]);
//Inseriamo nome e porta udp
error=record_name(client_name);
while(error==-1){
	if(max_try!=0){
		printf("Riproviamo...\n");
		error=record_name(client_name);
		max_try--;
		}
	else if(max_try==0){
		printf("Problemi nell'inserimento del nome. Disconnessione...\n");
		close(tcp_socket);
		close(udp_socket);
		exit(1);
	    	}
	}
do{
  	printf("Per favore inserisci il numero di porta udp per il gioco:\n");
	fgets(port,6,stdin);
	if(port[strlen(port)-1]!='\n')
	fflush(stdin);
    	udp_port=atoi(port);
	if(udp_port<1024 || udp_port>=65535)
	printf("Porta upd inserita non valida...\n");
	else right=1;		
	}while(right==0);
udp_port=htons(udp_port);
error=record_udp_port(udp_port);	
if(error==-1){
	printf("Problemi nell'inserimento della porta udp. Disconnessione...\n");
	close(tcp_socket);
	close(udp_socket);
	exit(1);   
	}
//Esecuzione Bind socket udp
client.sin_port=udp_port;
client.sin_addr.s_addr = INADDR_ANY; // usa il mio indirizzo IP
error=bind(udp_socket, (struct sockaddr*)&client, addrlen);
if(error==-1){
	perror("Errore nella bind della socket udp in ricezione(porta occupata)");
        printf("Server in chiusura...\n");
	close(tcp_socket);
	close(udp_socket);
	exit(1); 
	}
else
printf("Porta udp registrata correttamente presso il server.\n");

//Posizionamento mappa

set_match();
show_commands();
			 
// inizializzo i descrittori

FD_ZERO(&master_read_set);
FD_ZERO(&master_write_set);
FD_SET(0, &master_read_set);//stdin
FD_SET(tcp_socket, &master_read_set);
FD_SET(udp_socket, &master_read_set);
		  
// comincia il corpo del client

do{
	read_temp_set=master_read_set;
	write_temp_set=master_write_set;
	if(shell==0){
	   	if(started_match==0)
		printf(">");
	   	else
		printf("#");
	  	shell=1;
	 	}
	fflush(stdout);
	error=select(max_fd_set+1, &read_temp_set, &write_temp_set, NULL, &timeout);
	if(error==-1){
	   	perror("Errore nella select");
	   	close(tcp_socket);
	   	close(udp_socket);
	   	exit(1);
		}
	else if((error==0) && (started_match!=0)){ // non e' successo niente per 60s durante una partita
	   	printf("Inattivita' durante una partita.\n");
	   	printf("Chiudo la partita con il CLient %s. \n",enemy_name);
	   	busy=0;
	   	started_match=0;
	   	tcp_senddim=4;
	   	tcp_message=malloc(4);
	  	*(int*)tcp_message=4; //CODICE SEGNALAZIONE COMANDO "DISCONNECT"
	   	udp_message[0]=7;// CODICE SEGNALAZIONE "QUIT" AVVERSARIO
		shell=0;
	   	FD_SET(tcp_socket, &master_write_set);
	   	FD_SET(udp_socket, &master_write_set);
	   	FD_CLR(tcp_socket, &master_read_set);
		}
  	// altrimenti controlliamo i socket in lettura
    	if(FD_ISSET(0, &read_temp_set)){
  		// leggo un comando da tastiera
	   	shell = 0;
	   	fgets(command,30, stdin);
	   	if(command[strlen(command)-1]!='\n') fflush(stdin);
	   	else command[strlen(command)-1]='\0';
	   	manage_stdin_command(command);
    		}
    
	if(FD_ISSET(tcp_socket, &read_temp_set)){
		if(tcp_recv1==1){
	   		msg_buffer=malloc(INTEGER);
	   		error=recv(tcp_socket,msg_buffer,INTEGER,0);
	   		if(error==0){
		  		printf("Errore: Comunicazione con Server chiusa.");
		  		close_conn=1;
		  		break;
        			}
	   		if(error==-1 || error<INTEGER){
		  		perror("Errore in ricezione!");
		  		close_conn=1;
		  		break;
				}
			tcp_recvdim=*(int*)msg_buffer;
			free(msg_buffer);
			tcp_recv1=0;
			}
		else{
	   		msg_buffer=malloc(tcp_recvdim);
	   		error=recv(tcp_socket,msg_buffer,tcp_recvdim,0);
	   		if(error==0){
		  		printf("Errore: Comunicazione con Server chiusa.");
		  		close_conn=1;
		  		break;
				}
	   		if(error==-1 || error<tcp_recvdim){
		  		perror("Errore in ricezione!");
		  		close_conn=1;
		  		break;
				}
			error=manage_tcp_command(msg_buffer);
			free(msg_buffer);
	   		if(error==-1){
		  		printf("Errore in ricezione!");
		  		close_conn=1;
		  		break;
				}
			tcp_recv1=1;
			shell=0;
			}
  		}
  	if(FD_ISSET(udp_socket, &read_temp_set)){
	 	msg_buffer=malloc(12);
	 	error=recvfrom(udp_socket,msg_buffer,12,0,(struct sockaddr*)&work,&addrlen);
		if(error==-1){
			perror("Errore in ricezione! (recvfrom)");
			close_conn=1;
			break;
			}
    		if((busy==1) && work.sin_addr.s_addr==enemy.sin_addr.s_addr){
			shell=0;
			error=manage_udp_command((int*)msg_buffer);
			if(error==-1){
				printf("Errore: Comando dell'altro giocatore non gestito correttamente.\n");
				close_conn=1;
				break;
				}
			free(msg_buffer);
			}
		// altrimenti ho ricevuto un pacchetto da qualcun'altro
		}
  
  	// controlliamo i socket in scrittura
  
  	if(FD_ISSET(tcp_socket,&write_temp_set)){
	 	if(tcp_send1==1){
			msg_buffer=malloc(INTEGER);
			*(int*)msg_buffer=tcp_senddim;
			error=send(tcp_socket,msg_buffer,INTEGER,0);
			if(error==-1){
		  		perror("Errore nella send!");
		  		close_conn=1;
		  		break;
	    			}
			free(msg_buffer);
			tcp_send1=0;			
	  		}
	 	else{
			error=send(tcp_socket,tcp_message,tcp_senddim,0);
			if(error==-1){
		  		perror("Errore nella send!");
		  		close_conn=1;
		  		break;
				}
			if(*(int*)tcp_message==5)
			close_conn=1;
			free(tcp_message);
			tcp_send1=1;
			FD_CLR(tcp_socket, &master_write_set);
			FD_SET(tcp_socket, &master_read_set);
			}
		}
  	if(FD_ISSET(udp_socket,&write_temp_set)){
	 	error=sendto(udp_socket,(void*)&udp_message,12,0,(struct sockaddr*)&enemy,addrlen);
	 	if(error==-1){
		 	perror("Errore nella sendto!");
		 	close_conn=1;
		 	break;
	  		}
		FD_CLR(udp_socket, &master_write_set);
		}
	}while(close_conn==0);
close(tcp_socket);
close(udp_socket);
printf("\nArrivederci!\n");
return 0;	  
}

