// SEZIONE FUNZIONI CON IMPLEMENTAZIONE

#include "server.h" 

struct client* find_client(int id){
	if(waiting_list==NULL) 
     	return NULL;
  	struct client * search=waiting_list;
  	while(search!=NULL){
    		if(search->id==id)
	    	return search;
		else
	   	search=search->next;
    		}
 	return NULL;
	}

struct client* find_client_by_name(char* name){// utilizzato per connettersi ad un client specificato
    	if(waiting_list==NULL) 
      	return NULL;
    	struct client * search=waiting_list;
    	while(search!=NULL){
      		if(strcmp(search->name,name)==0)
	    	return search;
	  	else
	   	search=search->next;
    		}
 	return NULL;
	}  


void rmv_client(int id){
	struct client* delete;
    	struct client* work;
    	if(waiting_list==NULL)
        return;
	if(waiting_list->id==id){
        	work=waiting_list->next;
           	delete=waiting_list;
           	waiting_list=work;
        	}
    	else{
        	work=waiting_list;
        	while(work!=NULL && work->id!=id){
              		delete=work;
              		work=work->next;
            		}   
        	if(work==NULL)
           	return;
        	delete->next=work->next;
        	delete=work;
    		}
 	free(delete);
	}

struct client* add_client(int id, struct sockaddr_in clientaddr){
	struct client* new_client;
	unsigned int client_dim=sizeof(struct client);
	if(waiting_list==NULL){
		waiting_list=(struct client*)malloc(client_dim);
        	new_client=waiting_list;
		}
	else{
		new_client=waiting_list;
		while(new_client->next!=NULL)	new_client=new_client->next;
		new_client->next=(struct client*)malloc(client_dim);
		new_client=new_client->next;
		}
	new_client->id=id;
	new_client->enemy_id=-1;
	inet_ntop(AF_INET,&(clientaddr.sin_addr), new_client->IP, INET_ADDRSTRLEN); /*converte l'indirizzo IP da numeric a 
	Presentation e lo mette in IP(array di char) */
	new_client->busy=0;
	new_client->send1=1;
	new_client->recv1=1;
	new_client->name[0]='\0';
	new_client->next=NULL;
	new_client->buffer_client=NULL;
	return new_client;
	}

int manage_command(void* buffer, struct client* client_to_serve){
    	int id_command = *(int*) buffer; /* primi 4 byte indicano l'identificatore del comando, da
	buffer+4 in poi lo utilizziamo per le funzioni*/
    	char work_name[30];   
	work_name[0]='\0';
	char * name_list;
	int length1=0, length2=0; //variabili di supporto alla gestione di comandi come "who"
	struct client * work=waiting_list;
	struct client * search_client;
	switch(id_command){	
		case 1: // CASO: CLIENT COMUNICA NOME
	    		strcpy(work_name,(char*)(buffer+4)); //utilizzo come sopra indicato 
	    		search_client=find_client_by_name(work_name);
			if(search_client==NULL){ //possiamo accettare - univocità del nome
				strcpy(client_to_serve->name,work_name);
				client_to_serve->buffer_client=malloc(4);
				*(int*)client_to_serve->buffer_client=18; //CODICE SEGNALAZIONE CLIENT INSERITO
				client_to_serve->dim_send=4;
				printf("Giocatore con IP %s: identificato correttamente con il nome: %s.\n",client_to_serve->IP,client_to_serve->name);
				}
			else{  //Nome già presente - riportiamo errore
		   		printf("Giocatore con ip %s: inserito un nome gia' esistente.\n",client_to_serve->IP);
		   		client_to_serve->buffer_client=malloc(4);
		   		*(int*)client_to_serve->buffer_client=19; //CODICE SEGNALAZIONE NOME GIÀ PRESENTE
		   		client_to_serve->dim_send=4;
				}	
			FD_CLR(client_to_serve->id, &master_read_set);
			FD_SET(client_to_serve->id, &master_write_set);
			break;
		case 2: //CASO: CLIENT COMUNICA PORTA UDP
			client_to_serve->udp_port_client=*((u_int16_t*)(buffer+4));
			printf("Client %s: inserita %d come porta udp.\n",client_to_serve->name,ntohs(client_to_serve->udp_port_client));
			client_to_serve->buffer_client=malloc(4);
			*(int*)client_to_serve->buffer_client=20; //CODICE SEGNALAZIONE PORTA UDP ACCETTATA
			client_to_serve->dim_send=4;
			FD_CLR(client_to_serve->id, &master_read_set);
			FD_SET(client_to_serve->id, &master_write_set);
			break;
		case 3:	//CASO: CLIENT ESEGUE COMANDO "WHO"
	    		printf("Client %s: chiesti nomi altri client connessi.\n",client_to_serve->name);
			while(work!=NULL){
				length1++;
		  		if(work->id!=client_to_serve->id)
				length2+=strlen(work->name);
				work=work->next;
				}
			if(length1==1){// Sono l'unico client
				client_to_serve->buffer_client=malloc(4);
				client_to_serve->dim_send=4;
				*(int*)client_to_serve->buffer_client=10; //CODICE SEGNALAZIONE PRESENZA UNICO CLIENT
				printf("Client %s: unico client connesso.\n",client_to_serve->name);
				FD_CLR(client_to_serve->id, &master_read_set);
				FD_SET(client_to_serve->id, &master_write_set);
				break;
	    			}
	    		// Presenza di altri client
	  		length1--;
	    		name_list=(char*)malloc(length1+length2+1);
	    		client_to_serve->buffer_client=malloc(4+length1+length2+1);
			client_to_serve->dim_send=4+length1+length2+1;
			name_list[0]='\0';
			work=waiting_list;
			while(work!=NULL){ //Inserisco i nomi e li separo con uno spazio
		  		if(work->id!=client_to_serve->id){ 
		     			strcat(name_list,work->name);
		     			strcat(name_list," ");
		    			}
		 		work=work->next;
				}
			name_list[strlen(name_list)]='\0';
	    		*(int*)client_to_serve->buffer_client=11; // CODICE SEGNALAZIONE WHO CORRETTO
			strcpy((char*)(client_to_serve->buffer_client+4),name_list);
			free(name_list);
			FD_CLR(client_to_serve->id, &master_read_set);
			FD_SET(client_to_serve->id, &master_write_set);
    			break;
		case 4: //CASO: CLIENT ESEGUE COMANDO DISCONNESSIONE
			search_client=find_client(client_to_serve->enemy_id);
			if(search_client==NULL || client_to_serve->enemy_id==-1 || client_to_serve->busy==0){
				printf("Client %s: errore, non sta giocando!\n",client_to_serve->name);
		    		return -1;
				} //Il client non stava giocando
			printf("Client %s: terminata la partita con Client %s.\n",client_to_serve->name, search_client->name);
			client_to_serve->enemy_id=search_client->enemy_id=-1;
			client_to_serve->busy=search_client->busy=0;
			break;
		case 5: //CASO: CLIENT ESEGUE COMANDO QUIT
			printf("Client %s: disconnessione dal server...\n",client_to_serve->name);
			if(client_to_serve->busy==1 && client_to_serve->enemy_id!=-1){
				client_to_serve->busy=0;
		   		search_client=find_client(client_to_serve->enemy_id);
		   		if(search_client==NULL){
			  		printf("Client %s: errore, non stava giocando.\n", client_to_serve->name);
			  		return -1;
					}
				printf("Client %s: terminata la partita con Client %s.\n", client_to_serve->name,search_client->name);
				search_client->busy=0;
				search_client->buffer_client=malloc(4);
				*(int*)search_client->buffer_client=17; //CODICE SEGNALAZIONE QUIT CLIENT
				FD_CLR(search_client->id,&master_read_set);
				FD_SET(search_client->id,&master_write_set);
				}
			printf("Client %s: disconnesso.\n", client_to_serve->name);
			break;
		case 6://CASO: CLIENT ESEGUE COMANDO "CONNECT" nome_client
			search_client=find_client_by_name((char*)(buffer+4));
	   		if(search_client==NULL || search_client->id==client_to_serve->id){
	      			printf("Client %s: errore, non esiste tra i clienti o si sta cercando di giocare con se stessi.\n" , (char*)(buffer+4));
				client_to_serve->buffer_client=malloc(4);
				client_to_serve->dim_send=4;
				*(int*)client_to_serve->buffer_client=12; //CODICE SEGNALAZIONE ERRORE COMANDO CONNECT
				FD_CLR(client_to_serve->id,&master_read_set);
				FD_SET(client_to_serve->id,&master_write_set);
				break;
				}
			//Abbiamo trovato il client selezionato per giocare
	    		printf("Client %s: vuole iniziare una partita con Client %s.\n", client_to_serve->name, search_client->name);
			if(search_client->busy==0){
		  		search_client->busy=client_to_serve->busy=1;
		  		search_client->buffer_client=malloc(4+2+INET_ADDRSTRLEN+strlen(client_to_serve->name)+1);// 16=inet_addrlen
		  		*(int*)search_client->buffer_client=13; //CODICE SEGNALAZIONE CLIENT TROVATO
		  		*(u_int16_t*)(search_client->buffer_client+4)=client_to_serve->udp_port_client;
		  		strcpy((char*)(search_client->buffer_client+6), client_to_serve->IP);
		  		strcpy((char*)(search_client->buffer_client+6+INET_ADDRSTRLEN), client_to_serve->name);
		  		search_client->enemy_id=client_to_serve->id;
		  		client_to_serve->enemy_id=search_client->id;
		  		search_client->dim_send=(4+2+INET_ADDRSTRLEN+strlen(client_to_serve->name)+1);
		  		FD_CLR(search_client->id, &master_read_set);
		  		FD_SET(search_client->id, &master_write_set);
				}
			else{ //Il client scelto è occupato in una partita
		   		client_to_serve->buffer_client=malloc(4);
		   		client_to_serve->dim_send=4;
		   		*(int*)client_to_serve->buffer_client=14;  //CODICE SEGNALAZIONE CLIENT OCCUPATO
		   		FD_CLR(client_to_serve->id, &master_read_set);
		   		FD_SET(client_to_serve->id, &master_write_set);
		   		printf("Client occupato, errore.\n");
				}	   
			break;
		case 7: //CASO: CLIENT ACCETTA LA SFIDA
			search_client=find_client(client_to_serve->enemy_id);
			if(search_client==NULL){
				printf("Client non trovato, errore. \n");
				return -1;
		   		}
			printf("Client %s: accettata la richiesta di gioco da Client: %s.\n", client_to_serve->name,search_client->name);
			search_client->buffer_client=malloc(4+2+INET_ADDRSTRLEN+1);
           		search_client->enemy_id=client_to_serve->id;
			client_to_serve->enemy_id=search_client->id;
			*(int*)search_client->buffer_client=15; //CODICE SEGNALAZIONE SFIDA ACCETTATA
			*(u_int16_t*)(search_client->buffer_client+4)=client_to_serve->udp_port_client;
			strcpy((char*)(search_client->buffer_client+6), client_to_serve->IP);
			search_client->dim_send=(4+2+INET_ADDRSTRLEN+1);
			FD_CLR(search_client->id, &master_read_set);
			FD_SET(search_client->id, &master_write_set);
			break;
		case 8: //CASO: CLIENT RIFIUTA LA SFIDA
			search_client=find_client(client_to_serve->enemy_id);
			if(search_client==NULL){
				printf("Fase scambio dati, errore.\n");
				return -1;
				}
			printf("Client %s: richiesta di gioco rifiutata da Client %s.\n", client_to_serve->name,search_client->name);
			search_client->buffer_client=malloc(4);
			*(int*)search_client->buffer_client=16; //CODICE SEGNALAZIONE SFIDA RIFIUTATA
			search_client->dim_send=4;
			search_client->busy=client_to_serve->busy=0;
            		search_client->enemy_id=client_to_serve->enemy_id=-1;
			FD_CLR(search_client->id,&master_read_set);
			FD_SET(search_client->id,&master_write_set);	       
			break;
		default: printf("Comando non riconosciuto.\n");
		return -1;
		}
	return 0;
	}
