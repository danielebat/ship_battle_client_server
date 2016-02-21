// SEZIONE FUNZIONI CON IMPLEMENTAZIONE

#include "client.h"
	
void set_match(){ //prepara il campo di battaglia
	int i,j;
	int num_ships=1;
	for(i=0; i<10;i++){
		for(j=0;j<10;j++){
			match_map[i][j].t = ACQUA;
			match_map[i][j].punt = NULL;
			match_map[i][j].water_hit = 0;
			enemy_map[i][j].t = ACQUA;
			enemy_map[i][j].punt = NULL;
			enemy_map[i][j].water_hit = 0;
			}
		}
	printf("\nLa mappa e' un quadrato che ha 10 elementi per lato, le righe sono numerate da 1 a 10 mentre le colonne vanno da A a J.\n\n");
	
	i=6;
	show_my_map();
	printf("\n");
	while(i!=0){
		place_ship(i,num_ships);
		show_my_map();
		printf("\n");
		switch(i){
		case 6: i = i-2; break;
		case 4: 
		if(num_ships == 2){
			i--; num_ships = 1;
			}
		else{	num_ships++;}
		break;
		case 3:
		if(num_ships == 3){ i--; num_ships = 1;}
		else{	num_ships++;}
		break;
		case 2:
		if(num_ships == 4){ i = i-2; num_ships = 1;}
		else{	num_ships++;}
		break;
		default: break;
		}
	}
printf("Navi posizionate in questo modo:\n");
show_my_map();
return;
}

void place_ship(int dim, int number){ //piazza le navi
	int i;
	char work[5];
	int translation;
	int column, line;
	int pos_error;
	do{
		pos_error = 0;
		printf("Inserisci le coordinate per la nave %d di dimensione %d...\n", number,dim);
		do{
			printf("Inserisci ora l'orientamento (V = VERTICALE - O = ORIZZONTALE):\n");
			fgets(work,3,stdin);
			if(work[strlen(work)-1] != '\n') fflush(stdin);
			translation = (int)work[0];
			if(!(translation == 86 || translation == 79)){
				printf("\n");
				printf("Non hai inserito correttamente l'orientamento...riprova\n");}
			}while(!((translation == 86) || (translation == 79))) ;
		do{
			printf("Inserisci ora la casella da cui vuoi far partire il posizionamento della nave (ESEMPIO: B3):\n");
			fgets(work,5,stdin);
			column = (int)work[0] - 65;
			if(strlen(work) < 4) line = (atoi(work+1)-1);
			else line = 9;
			if(column<0 || column>9 || line<0 || line>9){
				pos_error = 1;
				printf("Hai inserito coordinate errate!\n");
				}
			else pos_error = 0;
			}while(pos_error == 1);
			printf("Ora controlliamo se è possibile piazzare la nave...\n");
			if(translation == 79){
				for(i=0;i<dim;i++){
					if(match_map[line][column+i].t != ACQUA || ((column+i) >9))
					pos_error = 1;
					}
				if(pos_error == 1) printf("Non è possibile inserire la nave: finisce fuori mappa o occupa un posto già occupato\n");
				}
			else if(translation == 86){
				for(i=0;i<dim;i++){
					if(match_map[line+i][column].t != ACQUA || ((line+i) > 9))
					pos_error = 1;
					}
				if(pos_error == 1) printf("Non è possibile inserire la nave: finisce fuori mappa o occupa un posto già occupato\n");
				}
			}while(pos_error == 1) ;
		if(translation == 86){
			for(i=0;i<dim;i++){
				match_map[line+i][column].t = NAVE;
				match_map[line+i][column].punt = (struct ship*)malloc(ship_len);
				match_map[line+i][column].punt->length=dim;
				match_map[line+i][column].punt->coord[0]=line;
				match_map[line+i][column].punt->coord[1]=column;
				match_map[line+i][column].punt->hit = 0;
				match_map[line+i][column].punt->hit_number = 0;
				match_map[line+i][column].punt->destroyed = 0;
				}
			}
		else{
			for(i=0;i<dim;i++){
				match_map[line][column+i].t = NAVE;
				match_map[line][column+i].punt = (struct ship*)malloc(ship_len);
				match_map[line][column+i].punt->length=dim;
				match_map[line][column+i].punt->coord[0]=line;
				match_map[line][column+i].punt->coord[1]=column;
				match_map[line][column+i].punt->hit = 0;
				match_map[line][column+i].punt->hit_number = 0;
				match_map[line][column+i].punt->destroyed = 0;
				}
			printf("Inserimento nave completato con successo.\n");
			}
  	return;
	}

void show_my_map(){
	int i, j;
	printf("Legenda della mappa: _=ACQUA   B=COLPO A VUOTO   O=COLPO A SEGNO   X=NAVE AFFONDATA   N=NAVE\n");
	printf("\n");
	printf("  ");
	for(i=1;i<11;i++){
		i=i+64;
		if(i == 65) printf(" %s",((char*)&i));
		else printf("%s",((char*)&i));
		printf(" ");
		i=i-64;
		}
	printf("\n");
	for(i=1;i<11;i++){
		if(i!=10) printf(" %d",i);
		else printf("%d",i);	
		for(j=0;j<10;j++){
			if(j == 0 && i!= 10){
				if(match_map[i-1][j].water_hit == 1) printf(" B");
				else if(match_map[i-1][j].t == ACQUA) printf(" _");
				else if(match_map[match_map[i-1][j].punt->coord[0]][match_map[i-1][j].punt->coord[1]].punt->destroyed == 1) printf(" X");
				else if(match_map[i-1][j].punt->hit == 1) printf(" O");
				else if(match_map[i-1][j].punt != NULL) printf(" N");
				}
			else{
				if(match_map[i-1][j].water_hit == 1) printf(" B");
				else if(match_map[i-1][j].t == ACQUA) printf(" _");
				else if(match_map[match_map[i-1][j].punt->coord[0]][match_map[i-1][j].punt->coord[1]].punt->destroyed == 1) printf(" X");
				else if(match_map[i-1][j].punt->hit == 1) printf(" O");
				else if(match_map[i-1][j].punt != NULL) printf(" N");
				}
			}
		printf("\n");
		}
	printf("\n");
	return;
	}

void show_my_enemy_map(){
   	int i,j;
	printf("Legenda della mappa: _=ACQUA   B=COLPO A VUOTO   O=COLPO A SEGNO\n");
   	printf("\n");
   	printf("  ");
   	for(i=1;i<11;i++){
		i=i+64;
		if(i == 65) printf(" %s",((char*)&i));
		else printf("%s",((char*)&i));
		printf(" ");
		i=i-64;
		}
	printf("\n");
   	for(i=1;i<11;i++){
     		if(i!=10) printf(" %d",i);
		else printf("%d",i);
    		for(j=0;j<10;j++){
			if(j == 0 && i!= 10){
	    			if(enemy_map[i-1][j].water_hit==1) printf(" B");	
        			else if(enemy_map[i-1][j].t==ACQUA) printf(" _");
				else if(enemy_map[i-1][j].punt->hit==1) printf(" O");
        			}
			else{
				if(enemy_map[i-1][j].water_hit==1) printf(" B");	
        			else if(enemy_map[i-1][j].t==ACQUA) printf(" _");
				else if(enemy_map[i-1][j].punt->hit==1) printf(" O");
				}	 
    			}
		printf("\n");
  		fflush(stdout);
 		}
	printf("\n");
	}
	
int hit_ship(int x,int y){
	if(x<0 || x>9 || y <0 || y>9){ 
	    	printf("Coordinata non valida: fuori dalla mappa!\n");
		return -1;
	  	}
	if(enemy_map[x][y].water_hit==1 || enemy_map[x][y].t==NAVE){
	      	printf("Coordinata non valida: zona già colpita!\n");
		return -1;
	    	}
    	hit_coord[0]=x;
    	hit_coord[1]=y;	 
	return 1;	 
	}

int enemy_hit(int x,int y){ //ricezione colpo avversario, ritorno di codici in base all'avvenimento
    	if(match_map[x][y].t==ACQUA)
       	return 1; //colpo a vuoto
    	else{
       		match_map[x][y].punt->hit=1;
	   	match_map[match_map[x][y].punt->coord[0]][match_map[x][y].punt->coord[1]].punt->hit_number++;
	  	if(match_map[match_map[x][y].punt->coord[0]][match_map[x][y].punt->coord[1]].punt->hit_number==match_map[x][y].punt->length){
	      		remained_ships--;
		 	match_map[match_map[x][y].punt->coord[0]][match_map[x][y].punt->coord[1]].punt->destroyed=1;
                  	match_map[x][y].punt->destroyed=1;
		  	}
	    	if(remained_ships==0)
	     	return 2; //mi hanno affondato l'ultima nave, ho perso
	    	if(match_map[match_map[x][y].punt->coord[0]][match_map[x][y].punt->coord[1]].punt->hit_number==match_map[x][y].punt->length)
	     	return 3; //mi hanno affondato una nave ma non ho ancora perso
		return 4; // ho colpito una nave
		}
	}


void show_commands(){
 	printf("Sono disponibili i seguenti comandi:\n");
 	printf("  *!help --> mostra l'elenco dei comandi disponibili\n");
 	printf("  *!who --> mostra l'elenco dei client connessi al server\n");
 	printf("  *!connect nome_client --> avvia una partita con l'utente nome_client");
 	if(busy==1)	printf("(non utilizzabile al momento)\n");
 	else printf("\n");
 	printf("  *!disconnect -->disconnette il client dall'attuale partita intrapresa con un altro peer");
 	if(started_match==0)	printf("(non utilizzabile al momento)\n");
 	else printf("\n");
 	printf("  *!quit --> disconnette il client dal server\n");
 	printf("  *!show_my_map --> mostra la tua mappa\n");
 	printf("  *!show_my_enemy_map --> mostra la mappa dell'avversario");
 	if(busy==0)	printf("(non utilizzabile al momento)\n");
 	else printf("\n");
 	printf("  *!hit coordinates --> colpisce le coordinate coordinates (EX: B5)");
 	if(started_match==0 ||(started_match==1 && own_turn==0))	printf("(non utilizzabile al momento)\n");
 	else printf("\n");
 	return;
	}

int record_name(char * client_name){
   	int error;
   	int dim_name;
      	do{
        	printf("Prego, inserisci il tuo nome : ");
		fgets(client_name,30,stdin);
		if(client_name[strlen(client_name)-1]!='\n') 	fflush(stdin);
		else client_name[strlen(client_name)-1]='\0';
       	 	msg_buffer=malloc(INTEGER);
		*(int*)msg_buffer=4+strlen(client_name)+1;
		error=send(tcp_socket,msg_buffer,INTEGER,0);
		if(error==-1 || error<INTEGER){
		   	printf("Impossibile inviare al server la dimensione del pacchetto contenente il nome.\n");
		   	return -1;
			}
		free(msg_buffer);
		dim_name=strlen(client_name)+1;
		msg_buffer=malloc(4+dim_name);
		*(int*)msg_buffer=1; //CODICE SEGNALAZIONE COMUNICAZIONE NOME
		strcpy((char*)msg_buffer+4,client_name);
		error=send(tcp_socket,msg_buffer,dim_name+4,0);
		if(error==-1 || error<(dim_name+4)){
		    	printf("Errore: Nome non comunicato al Server.\n");
			return -1;
			}
		// altrimenti attendo la risposta del server
		msg_buffer=malloc(INTEGER);
		error=recv(tcp_socket,msg_buffer,INTEGER,0); //PRIMA SEND, RICEZIONE DIMENSIONE
		if(error==0){
		   	printf("Errore: Comunicazione con Server chiusa.\n");
		   	return -1;
			}
		if(error==-1 || error<INTEGER){
		       	printf("Errore: Dimensione Esito Registrazione non ricevuto.\n");
			return -1;		
			}
		error=recv(tcp_socket,msg_buffer,4,0); //SECONDA SEND, RICEZIONE ESITO (SO GIÀ CHE È UN INTERO)
		if(error==0){
		   	printf("Errore: Comunicazione con Server chiusa.\n");
		   	return -1;
			}
		if(error==-1 || error<4){
		       	printf("Errore: Esito Registrazione non ricevuto.\n");
			return -1;		
			}
		error=*(int*)msg_buffer;
		if(error==18){
		   	printf("Registrazione avvenuta con successo!\n");
		   	return 1;
			}
		if(error==19){
		  	printf("Nome già presente!\n");
		  	return -1;
			}
		else{
		   	printf("Codice di ritorno dal Server sconosciuto.\n");
		   	return -1;
			}
    		}while(1);	
	}

int record_udp_port(u_int16_t porta_udp_x){
    	int error;
	msg_buffer=malloc(INTEGER);
	*(int*)msg_buffer=sizeof(u_int16_t)+4;
	error=send(tcp_socket,msg_buffer,INTEGER,0);
	if(error==-1 || error<INTEGER){
	   	printf("Registrazione porta udp non avvenuta: errore invio dimensione!\n");
	   	return -1;
		}
	msg_buffer=malloc(sizeof(u_int16_t)+4);
	*(int*)msg_buffer=2; // codice numero porta udp da registrare
	*(u_int16_t*)(msg_buffer+4)=porta_udp_x;
	error=send(tcp_socket,msg_buffer,sizeof(u_int16_t)+4,0);
	if(error==-1 || error<(sizeof(u_int16_t)+4)){
	  	printf("Registrazione porta udp non avvenuta: errore invio dati!\n");
	  	return -1; 
		}
	// aspetto la risposta 
	msg_buffer=malloc(INTEGER);
	error=recv(tcp_socket,msg_buffer,INTEGER,0);
	if(error==0){
	  	printf("Errore: Comunicazione con Server chiusa.\n");
	  	return -1;
		}
	if(error==-1 || error<INTEGER){
	  	printf("Errore: Dimensione Esito Registrazione Porta_udp non ricevuto.\n");
	  	return -1;
		}
	error=recv(tcp_socket,msg_buffer,4,0);
	if(error==0){
	   	printf("Errore: Comunicazione con Server chiusa.\n");
	   	return -1;
		}
	if(error==-1 || error<4){
	  	printf("Errore: Esito Registrazione Porta_udp non ricevuto.\n");
	  	return -1;
		}
	error=*(int*)msg_buffer;
	if(error==20){
	  // è ok presso il server, controllo se lo è anche qui sul client e eventualmente
          // la confermo!
	  	return 1;
		}
	else{
		printf("Impossibile registrare porta udp.\n");
		return -1;
		}
	}

int manage_tcp_command(void* buffer){
   	int command=*(int*)buffer;
   	char * receive=NULL;
   	char answer[3];
   	fd_set client_set;
   	FD_ZERO(&client_set);
   	FD_SET(0, &client_set);
   	switch(command){
    		case 10: 
	   		printf("Nessun altro cliente connesso al server.\n");
	   		break;
		case 11: 
	   		printf("Client connessi al Server:\n");
	   		receive=(char*)malloc(tcp_recvdim-4);
	   		strcpy(receive,(char*)(buffer+4));
      			strcat(receive,"\n\0");
	   		printf("%s",receive);
       			free(receive);
	   		break;
		case 12: 
	   		printf("Client richiesto non presente.\n");
	   		busy=0;
      	 		started_match=0;
	   		break;
		case 13:
	  		printf("Il Client %s ti ha sfidato, la accetti? [Y,N]\n ", (char*)(buffer+4+2+INET_ADDRSTRLEN));
	  		fflush(stdout);
	  		select(1,&client_set,NULL,NULL,NULL);
	  		fgets(answer,3,stdin);
	  		fflush(stdin);
	  		if(answer[0]!='Y'){
	     			printf("Hai rifiutato la partita.\n");
		 		tcp_message=malloc(4);
		 		*(int*)tcp_message=8;
		 		tcp_senddim=4;
		 		FD_CLR(tcp_socket, &master_read_set);
		 		FD_SET(tcp_socket, &master_write_set);
		 		break;
	    			}
	  		printf("Hai appena accettato di giocare con %s.\n",(char*)buffer+4+2+INET_ADDRSTRLEN);
	  		// salvo tutti i dati del giocatore
	  		enemy.sin_port=*(u_int16_t*)(buffer+4);
	  		inet_pton(AF_INET, (char*)(buffer+6), &(enemy.sin_addr));
	  		strcpy(enemy_name, (char*)(buffer+2+4+INET_ADDRSTRLEN));
	  		busy=1;
			if(first_match==0){
				printf("\nDevi prima riposizionare le navi!\n");
				remained_ships=10;
				set_match();
				}
			first_match=0;
      			started_match=1;
	  		own_turn=0;
	  		tcp_message=malloc(4);
	  		*(int*)tcp_message=7;
	  		tcp_senddim=4;
      			timeout.tv_sec=60;
      			timeout.tv_usec=0;
	  		FD_CLR(tcp_socket, &master_read_set);
	  		FD_SET(tcp_socket, &master_write_set);
	  		break;
		case 14:
	  		printf("Client già occupato in un altra partita!\n");
	  		busy=0;
      			started_match=0;
	  		break;
		case 15:
	  		printf("Il Client %s ha appena accettato la tua sfida.\n", enemy_name);
	  		enemy.sin_port=*(u_int16_t*)(buffer+4);
	  		inet_pton(AF_INET, (char*)(buffer+6), (&enemy.sin_addr));
	  		udp_message[0]=6;
	  		own_turn=0;
	  		started_match=1;
			if(first_match==0){ 
				printf("\n Devi prima riposizionare le navi!\n");
				remained_ships=10;
				set_match();
				}
			first_match=0;
			printf("Tocca a lui la prima mossa!\n");
      			timeout.tv_sec=60;
	  		FD_SET(udp_socket, &master_write_set);
	  		break;
		case 16:
	  		printf("Il Client ha rifiutato la tua sfida.\n");
	  		busy=0;
      			started_match=0;
	 	 	break;
		case 17:
	  		printf("Avversario disconnesso dal server!\n");
	  		printf("HAI VINTO!\n");
	  		busy=0;
	  		started_match=0;
	  		shell=0;
			break;  
   		default: 
			return -1;
   		}
   	return 1;
	}

void manage_stdin_command(char* command){
 	int x,y;
 	if(strcmp("!disconnect",command)==0){
       		if(started_match==0){
	     		printf("Nessuna partita in corso!\n");
		 	return;
	    		}
	   	shell=1;
	   	printf("Chiusura partita con il Client %s.\n", enemy_name);
	   	busy=0;
	   	started_match=0;
	   	tcp_senddim=4; // lo comunico al server
	   	tcp_message=malloc(4);
	   	*(int*)tcp_message=4; //CODICE SEGNALAZIONE "DISCONNECT"
	   	udp_message[0]=7;//CODICE SEGNALAZIONE "DISCONNECT" AVVERSARIO
	   	FD_SET(tcp_socket,&master_write_set);
	   	FD_SET(udp_socket,&master_write_set);
	   	FD_CLR(tcp_socket,&master_read_set);
	   	printf("TI SEI ARRESO.\n");
	   	shell=0;
	   	return;
		}
 	if(strcmp("!quit",command)==0){
      		printf("Chiusura client in corso...\n");
	  	tcp_message=malloc(4);
	  	*(int*)tcp_message=5;//CODICE SEGNALAZIONE "QUIT"
            	if(started_match==1){ 
            		udp_message[0]=7;//CODICE SEGNALAZIONE "QUIT" AVVERSARIO
            		FD_SET(udp_socket,&master_write_set);
          		}
	  	FD_SET(tcp_socket,&master_write_set);
	  	FD_CLR(tcp_socket,&master_read_set);
      		close_conn=1;
	  	return;
		}
 	if(strcmp("!show_my_map",command)==0){
		show_my_map();
		return;
		}	
	if(strcmp("!show_my_enemy_map",command)==0){
		if(started_match==0){
		     	printf("Nessuna partita in corso!\n");
			return;
		  	}
    		show_my_enemy_map();
    		return;
    		}
 	if(strcmp("!help",command)==0){
    		show_commands();
		return;
   		}
 	if(strcmp("!who",command)==0){
    		printf("Richiesta di altri Client connessi.\n");
    		tcp_senddim=4;
    		tcp_message=malloc(4);
    		*(int*)tcp_message=3;//CODICE SEGNALAZIONE "WHO"
    		FD_SET(tcp_socket,&master_write_set);
    		FD_CLR(tcp_socket,&master_read_set);
    		return;   
    		}
 	if(strncmp("!hit ",command,5)==0){
    		if(started_match==0 || own_turn==0){
	  		printf("Comando non utilizzabile ora!\n");
	  		return;
	  		}
	// altrimenti puoi provare a sparare
    		y=(int)command[5]-65;
    		if((strlen(command)-1)==6)
	 	x=atoi((char*)command+6)-1;
    		else
     		x=9; 
		int error=hit_ship(x,y);
		if(error==-1)
	 	return; //Errore riportato dalla funzione
		udp_message[0]=8;//CODICE SEGNALAZIONE "HIT"
		udp_message[1]=x;
		udp_message[2]=y;
		hit_coord[0]=x;
		hit_coord[1]=y;  
    		own_turn=0;
		FD_SET(udp_socket,&master_write_set);
		return;
    		}
 	if(strncmp("!connect ",command,9)==0){
    		if(busy==1){
	   		printf("Stai giocando con il CLient %s,per cambiare avversario devi disconnetterti!\n",enemy_name);
	   		return;
			}
    		printf("Comunico al server l'avvio di una partita.\n");
    		busy=1;
    		strcpy(enemy_name,command+9);
    		int dimension=strlen(enemy_name);
    		tcp_senddim=4+dimension+1;
    		tcp_message=malloc(tcp_senddim);
    		*(int*)tcp_message=6;//CODICE SEGNALAZIONE "CONNECT"
    		strcpy((char*)(tcp_message+4),enemy_name);
    		FD_SET(tcp_socket,&master_write_set);
    		FD_CLR(tcp_socket,&master_read_set);
    		return;
   		}
   	else
   	printf("Comando non riconosciuto!\n");
   	return;
	}

int manage_udp_command(void* buffer){
   	int line, column;
   	char x='0', y='A';
   	int result;
   	int cmd=*(int*)buffer;
   	switch(cmd){
    		case 6:
	  		printf("La partita e' cominciata, fai la tua mossa!\n");
	  		own_turn=1;
	  		started_match=1;
      			timeout.tv_sec=60;
	  		break;
		case 7:
	   		printf("Il tuo avversario, il Client %s ha appena abbandonato la partita, hai vinto!\n",enemy_name);
	   		busy=0;
	   		started_match=0;
	   		shell=0;
	   		break;
    		case 8:
	  		line=*(int*)(buffer+4);
	  		column=*(int*)(buffer+8);
      			if(line==9)
        		printf("Il CLient %s ha appena sparato alla coordinata %c%s .\n",enemy_name, (char)(y+column), "10");
      			else
        		printf("Il Client %s ha appena sparato alla coordinata %c%c .\n",enemy_name, (char)(y+column),(char)(x+line+1));
	  		result=enemy_hit(line,column);
	  		switch(result){
				case 1:
	    				printf("L'avversario ti ha colpito a vuoto!\n");
					udp_message[0]=9;
					break;
	  			case 2:
	      				printf("L'avversario ti ha colpito l'ultima nave! HAI PERSO!\n");
	      				busy=0;
		  			started_match=0;
		  			shell=1;
		  			udp_message[0]=10; //CODICE SEGNALAZIONE SCONFITTA
		  			tcp_message=malloc(4);
		  			tcp_senddim=4;
		  			*(int*)tcp_message=4; //CODICE COMANDO QUIT
		  			FD_SET(tcp_socket,&master_write_set);
		  			FD_CLR(tcp_socket,&master_read_set);
		  			FD_SET(udp_socket,&master_write_set);
		  			shell=0;
		  			return 1;
      				case 3:
	      				printf("L'avversario ti ha colpito e ha affondato una nave!\n");
		  			udp_message[0]=11; //CODICE SEGNALAZIONE NAVE AFFONDATA
	    				break;
	  			case 4:
	      				printf("L'avversario ti ha colpito.\n");
	      				udp_message[0]=12; //CODICE SEGNALAZIONE COLPO RICEVUTO
	    				break;
				}
       			own_turn=1;
	   		FD_SET(udp_socket,&master_write_set);
	   		printf("Avanti, e' il tuo turno.\n");
	   		break;
		case 9:
	   		printf("Hai colpito a vuoto!\n");
	   		enemy_map[hit_coord[0]][hit_coord[1]].water_hit=1;
       			show_my_enemy_map();
			printf("E' il turno del Client %s \n", enemy_name);
       			timeout.tv_sec=60;
	   		break;
		case 10:
			enemy_map[hit_coord[0]][hit_coord[1]].punt=(struct ship*)malloc(ship_len);
	   		enemy_map[hit_coord[0]][hit_coord[1]].punt->hit=1;
	   		enemy_map[hit_coord[0]][hit_coord[1]].punt->destroyed=1;
			enemy_map[hit_coord[0]][hit_coord[1]].t=NAVE;
			show_my_enemy_map();
	   		printf("Hai appena affondato l'ultima nave avversaria, hai vinto!\n");
	   		fflush(stdout);
			busy=0;
	   		started_match=0;
	   		shell=0;
	   		break;
		case 11:
	   		enemy_map[hit_coord[0]][hit_coord[1]].punt=(struct ship*)malloc(ship_len);
	   		enemy_map[hit_coord[0]][hit_coord[1]].punt->hit=1;
       			enemy_map[hit_coord[0]][hit_coord[1]].t=NAVE;
       			show_my_enemy_map();
			printf("Hai colpito e affondato una nave nemica! E' ora il turno del Client %s\n", enemy_name);
       			timeout.tv_sec=60;
	   		break;
		case 12:
	   		enemy_map[hit_coord[0]][hit_coord[1]].punt=(struct ship*)malloc(ship_len);
	   		enemy_map[hit_coord[0]][hit_coord[1]].punt->hit=1;
       			enemy_map[hit_coord[0]][hit_coord[1]].t=NAVE;
       			printf("Hai appena colpito una nave nemica!\n");
	   		show_my_enemy_map();
           		timeout.tv_sec=60;
	   		printf("E' il turno del Client %s\n", enemy_name);
	   		break;
    		default: 
			return -1;
  		}
 	return 1;
	}
