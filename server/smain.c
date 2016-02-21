//SEZIONE MAIN

#include "server.h" 
 
unsigned int addrlen =sizeof(struct sockaddr_in);
struct client* waiting_list=NULL;   
fd_set master_read_set;
fd_set master_write_set;

int main(int argc, char* argv[]){
 
fd_set read_temp_set;
fd_set write_temp_set;
int port_number;
int max_fd_set;
int end_conn=5; //utilizzato nel finale
int socket_close=0; //utilizzato per chiudere l'i-esimo socket, se settato.
int server_end=0;
struct client* client_to_serve;
struct sockaddr_in client;
struct sockaddr_in server;
void* msg_buffer;
int listener;
int error=1;
int opz=1;
int i;
	
if(argc!=3){
  	printf("Non hai inserito correttamente indirizzo IP o numero di porta!\n");
  	exit(1);
	}
	   
printf ("Avvio del server all'indirizzo: %s (porta : %s)...\n", argv[1],argv[2]);

port_number=atoi(argv[2]);
if(port_number<1024 || port_number>65535){
  	printf("Porta inserita non valida. Chiusura del server in corso...\n");
  	exit(1);
	}
	
printf("Dati inseriti correttamente!\n");
printf("Creazione del socket di ascolto...\n");

// creazione socket

listener=socket(AF_INET,SOCK_STREAM,0);
if(listener==-1){
	perror("Impossibile creare il Socket di ascolto!\n");
	exit(1);
	}

// settaggio socket

error=setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&opz,sizeof(int)); // per effettuare la bind su indirizzi locali gia' in uso
if(error==-1){
	perror("Errore nel settare il Socket!\n");
	close(listener);
	exit(1);
	}

memset(&server, 0, sizeof(server));
server.sin_family=AF_INET;
server.sin_port=htons(port_number);
if(inet_pton(AF_INET, (char*)argv[1],&server.sin_addr.s_addr)<=0){ // errore se <=0
	printf("Indirizzo IP non valido, chiusura del server...\n");
	exit(1);
	}

// esecuzione bind

error=bind(listener,(struct sockaddr*) &server,addrlen);
if(error==-1){
 	perror("Errore nella bind!");
 	close(listener);
 	exit(1);
	}

// esecuzione listen

error=listen(listener, 10);             
if(error==-1){
	perror("Errore nella listen!");
 	close(listener);
 	exit(1);
	}
	
 //  settaggio socket come non bloccante
	
error=fcntl(listener,F_SETFL,O_NONBLOCK);                         
if(error==-1){
 	perror("Errore nel settare come non-bloccante il Socket!\n");
 	close(listener);
 	exit(1);
	}
   
// completiamo l'inizializzazione

FD_ZERO(&master_read_set);
FD_ZERO(&master_write_set);
FD_ZERO(&read_temp_set);
FD_ZERO(&write_temp_set);
max_fd_set=listener; // poniamo il socket come il più alto dei descrittori
FD_SET(listener,&master_read_set);

printf("Server in ascolto all'indirizzo %s, porta %d!\n", (char*)argv[1],port_number);


//inizio controlli

do{
	read_temp_set=master_read_set;
	write_temp_set=master_write_set;
	error=select(max_fd_set+1,&read_temp_set,&write_temp_set,NULL,NULL); // timeout a NULL, attendiamo fino al primo libero
	if(error==-1){
	  	perror("Errore nella select!");
	  	break;
		}		
for(i=0;i<= max_fd_set;i++){
	socket_close=0;

	if(FD_ISSET(i,&read_temp_set)){
		  // esiste qualche socket in lettura
		  
		if(i==listener){ 
		   	int new;
			do{
				new=accept(listener, (struct sockaddr*)&client, &addrlen);
				if (new < 0){
					if (errno != EWOULDBLOCK){
						perror("Errore nella accept!\n");
						server_end=1;
						}
					break;
					}
				FD_SET(new, &master_read_set);
				client_to_serve=add_client(new, client);
				printf("Client%s: appena connesso con id %d.\n", client_to_serve->IP, client_to_serve->id);
				if(new>max_fd_set){
					max_fd_set=new;
					}		      	
				} while (new!=-1); //tutte le connessioni accettate
			}					  
		else{ // qualche client invia dati (dimensione dati)
			client_to_serve=find_client(i);
			do{ 
			 	if(client_to_serve->recv1==1){
					msg_buffer=malloc(INTEGER);
					error=recv(i,msg_buffer,INTEGER,0);
					if(error==0){ // client disconnesso
				  		perror("Errore nella receive: client disconnesso");
				  		socket_close=1;
				  		break;
						}
				
					if ((error <0 )||(error<INTEGER)){	
						socket_close=1;
						perror("Errore nella receive!");
						break;
						}	
			
					client_to_serve->dim_recv=*(int*)msg_buffer;
					free(msg_buffer);
					client_to_serve->recv1=0;		
		   			}
		
			// qualche client invia dati
		   		else{
			   		msg_buffer=malloc(client_to_serve->dim_recv);
			   		error=recv(i,msg_buffer,client_to_serve->dim_recv,0);
			   		if(error==0){
				  		printf("Errore nella receive: client disconnesso.\n");
				  		socket_close=1;
				  		break;
						}
			   
					if (error<0 || error< client_to_serve->dim_recv){
						socket_close=1;
						perror("Errore nella receive!");
						break;
			   			}
			   		client_to_serve->recv1=1;
			   		error=manage_command(msg_buffer,client_to_serve);
			   		free(msg_buffer);
			   
			   		if(error<0){
				  		printf("Comando non gestito correttamente.\n");
				  		socket_close=1;
				  		break;
						}						
					}
		  		break;	
				}while(1);
	   		}
    		}
	if(FD_ISSET(i, &write_temp_set)){
	  // server invia dati a qualche client, quelli con il socket libero, inizio con le dimensioni
		client_to_serve=find_client(i);
	   	do{
	    		if(client_to_serve->send1==1){
		   		msg_buffer=malloc(INTEGER);
		   		*(int*)msg_buffer=client_to_serve->dim_send;
		   		error=send(i,msg_buffer,INTEGER,0);
		   		if(error<0 || error<INTEGER){
			  		perror("Errore nella send!\n");
			  		socket_close=1;
			  		break;				  
		   			}
		   		free(msg_buffer);
		   		client_to_serve->send1=0;		  
				}
			else{
			 	error=send(i,client_to_serve->buffer_client,client_to_serve->dim_send,0);
			 	if(error==-1 || error<client_to_serve->dim_send){
				  	perror("Errore nella send!\n");
				  	socket_close=1;
				  	break;
					}
				client_to_serve->send1=1;
				free(client_to_serve->buffer_client);
				FD_CLR(i,&master_write_set);
				FD_SET(i,&master_read_set);
				}
	  		break;
			}while(1);
		}
	if(socket_close==1){ // quel socket va chiuso
	
	   	FD_CLR(i,&master_write_set);
	   	FD_CLR(i,&master_read_set);
	   	manage_command((void*)&end_conn,client_to_serve);
	   	rmv_client(i);
	   	close(i); // chiude quel socket
	   	if(i==max_fd_set){
		   	while(FD_ISSET(max_fd_set,&master_read_set)==0 && FD_ISSET(max_fd_set,&master_write_set)==0)
		   		max_fd_set--;
				}
			}
 		}
	}while(server_end==0);

// chiusura server => chiusura socket aperti, eliminiamo le varie strutture in memoria
for(i=0;i<=max_fd_set;i++){
	if(FD_ISSET(i,&master_read_set) || FD_ISSET(i,&master_write_set))
	  rmv_client(i);
	  close(i);
 	}
	return 0;
}
