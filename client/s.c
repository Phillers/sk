#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>
#define SERVER_PORT 1234
#define QUEUE_SIZE 5

char START=32;
char WRONG=33;
char WIN=34;
char LOSE=35;
char ERROR=36;

int n=256;
struct param{
	int socket;
};


void error(char turn, int player[]){
	perror("error");
	printf("Connection from %d lost1\n",turn);
	if(write(player[1-turn], &ERROR, 1)<1){
		printf("Connection from %d lost1\n",1-turn);
	}
	close(player[0]);
	close(player[1]);
	free(player);
	pthread_exit(&turn);
}

void ex(char turn, int* player){
	close(player[0]);
	close(player[1]);
	free(player);
	pthread_exit(&turn);
}


void* dzialanie(void* clientSocket){
	char pionek;
	char pion;
	char pole;
	char pola[4][4];
	bzero(pola,16);
	char wiersze[4];
	bzero(wiersze,4);
	char kolumny[4];
	bzero(kolumny,4);
	char przekatne[2];
	bzero(przekatne,2);
	char wiersz;
	char kolumna;
	int* player=(int*)clientSocket;
	int turn=0;
	int x;
	char and;
	char or;
	
	while(1){
//kolejka gry
//przyjęcie pionka od jednego gracza i pola od drugiego	
		if(x=read(player[turn], &pionek, 1)<1){
			error(turn,player);
		}
		if(pionek<16){
			error(turn,player);
		}
		pion=pionek-16;
		printf("gracz: %d pionek: %d\n",turn+1,pion);

		if(write(player[1-turn], &pionek, 1)<1){
			error(1-turn,player);
		}
	
		if(x=read(player[1-turn], &pole, 1)<1){
			error(1-turn,player);
		}
		if(pole<0){
			error(1-turn,player);
		}
		
		printf("gracz: %d pole: %d\n",1-turn+1,pole);
		
		if(write(player[turn], &pole, 1)<1){
			error(turn,player);
		}
		
//zapisanie pionka na planszy		
		wiersz=pole/4;
		kolumna=pole%4	;
		pola[wiersz][kolumna]=pion;

//sprawdzenie czy ktoś wygrał, iterujemy po wierszu/kolumnie/przekątnej,
//która została zapełniona		
		if(++wiersze[wiersz]==4){
			and=15;
			or=0;
			for(x=0;x<4;x++){
				and&=pola[wiersz][x];	
				or|=pola[wiersz][x];
			}
			if(and>0||or<15){
				printf("Wygrał gracz %d w wierszu %d\n", 1-turn+1, wiersz+1);
				if(write(player[turn], &LOSE, 1)<0)
					error(turn,player);
				if(write(player[1-turn], &WIN, 1)<0)
					error(1-turn,player);
				ex(turn,player);
			}	
		}
	
		if(++kolumny[kolumna]==4){
			and=15;
			or=0;
			for(x=0;x<4;x++){
				and&=pola[x][kolumna];	
				or|=pola[x][kolumna];
			}
			if(and>0||or<15){
				printf("Wygrał gracz %d w kolumnie %d\n", 1-turn+1, kolumna+1);
				if(write(player[turn], &LOSE, 1)<0)
					error(turn,player);
				if(write(player[1-turn], &WIN, 1)<0)
					error(1-turn,player);
				ex(turn,player);
			}	
		}
		
		if(wiersz==kolumna)if(++przekatne[0]==4){
			and=15;
			or=0;
			for(x=0;x<4;x++){
				and&=pola[x][x];	
				or|=pola[x][x];
			}
			if(and>0||or<15){
				printf("Wygrał gracz %d w przekatej w prawo\n", 1-turn+1);
				if(write(player[turn], &LOSE, 1)<0)
					error(turn,player);
				if(write(player[1-turn], &WIN, 1)<0)
					error(1-turn,player);
				ex(turn,player);
			}	
		
		}
		
		if(wiersz+kolumna==3)if(++przekatne[1]==4){
			and=15;
			or=0;
			for(x=0;x<4;x++){
				and&=pola[x][3-x];	
				or|=pola[x][3-x];
			}
			if(and>0||or<15){
				printf("Wygrał gracz %d w przekatej w lewo\n", 1-turn+1);
				if(write(player[turn], &LOSE, 1)<0)
					error(turn,player);
				if(write(player[1-turn], &WIN, 1)<0)
					error(1-turn,player);
				ex(turn,player);
			}	
		
		}
		
		turn = 1-turn;
	}
	char r=1;
	printf("koniec\n");
	ex(r,player);
}





int main(int argc, char* argv[])
{
   int nSocket;
   int nBind, nListen;
   int nFoo = 1;
   socklen_t nTmp;
   struct sockaddr_in stAddr, stClientAddr;
   pthread_t handle1;

   /* address structure */
   memset(&stAddr, 0, sizeof(struct sockaddr));
   stAddr.sin_family = AF_INET;
   stAddr.sin_addr.s_addr = htonl(INADDR_ANY);
   stAddr.sin_port = htons(SERVER_PORT);

   /* create a socket */
   nSocket = socket(AF_INET, SOCK_STREAM, 0);
   if (nSocket < 0)
   {
       fprintf(stderr, "%s: Can't create a socket.\n", argv[0]);
       exit(1);
   }
   setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&nFoo, sizeof(nFoo));

   /* bind a name to a socket */
   nBind = bind(nSocket, (struct sockaddr*)&stAddr, sizeof(struct sockaddr));
   if (nBind < 0)
   {
       fprintf(stderr, "%s: Can't bind a name to a socket.\n", argv[0]);
       exit(1);
   }
   /* specify queue size */
   nListen = listen(nSocket, QUEUE_SIZE);
   if (nListen < 0)
   {
       fprintf(stderr, "%s: Can't set queue size.\n", argv[0]);
   }

   while(1)
   {
       /* block for connection request */
       nTmp = sizeof(struct sockaddr);
       int *nClientSocket=malloc(2*sizeof(int));
       nClientSocket[0] = accept(nSocket, (struct sockaddr*)&stClientAddr, &nTmp);
       if (nClientSocket[0] < 0)
       {
           fprintf(stderr, "%s: Can't create a connection's socket.\n", argv[0]);
           exit(1);
       }
		
       	printf("%s: [connection from %s]\nsocket: %p\n",argv[0], 
       		inet_ntoa((struct in_addr)stClientAddr.sin_addr),&nClientSocket[0]);
       		
       nClientSocket[1] = accept(nSocket, (struct sockaddr*)&stClientAddr, &nTmp);
       if (nClientSocket[1] < 0)
       {
           fprintf(stderr, "%s: Can't create a connection's socket.\n", argv[0]);
           exit(1);
       }
		
       	printf("%s: [connection from %s]\n, soc %p\n",argv[0], 
       		inet_ntoa((struct in_addr)stClientAddr.sin_addr),&nClientSocket[1]);
  
		if(write(nClientSocket[0], &START, 1)<0)
			perror("error");
		pthread_create(&handle1, NULL, dzialanie, nClientSocket);
		
       }

   close(nSocket);
   return(0);
}
