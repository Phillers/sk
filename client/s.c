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

char START = 32;
char WRONG = 33;
char WIN = 34;
char LOSE = 35;
char ERROR = 36;
int n = 256;
struct param {
  int socket;
};

void ex(char turn, int * player) {
//zakończenie wątku
  //przyjmuje parametr gracza który wygrał i tablicę socketów
  //zamyka i zwalnia sockety
  close(player[0]);
  close(player[1]);
  free(player);
  //i kończy wątek
  pthread_exit( & turn);
}

void error(char turn, int player[]) {
//błąd w połączeniu
  //przyjmuje parametr gracza który się rozłączył i tablicę socketów
  perror("error");
  printf("Connection from %d lost\n", turn);
  //informuje drugiego gracza o utracie połączenia
  if (write(player[1 - turn], & ERROR, 1) < 1) {
    printf("Connection from %d lost\n", 1 - turn);
  }
  //i przechodzi do zakończenia wątku
  ex(turn, player);
}

char wygrana(char test[]){
//sprawdza zbiór pól
  //przyjmuje w parametrze 4 pola
  //sprawdza czy mają jakiś wspólny bit i jak tak to zwraca 1
      char and = 15;
      char or = 0;
      int i;
      for (i = 0; i < 4; i++) {
        and &= test[i];
        or |= test[i];
      }
      if (and > 0 || or < 15) return 1;
  return 0;
}

char sprawdz_wygrana(char wiersze[], char wiersz, char kolumny[],char kolumna,char przekatne[],char pola[4][4]){
  //zapisuje zmianę stanu planszy i sprawdza ewentualne możliwości wygrania
  //w parametrze dane o planszy i ruchu

  //zwiekszenie licznika liczby pól w rzędach
  wiersze[wiersz]++;
  kolumny[kolumna]++;
  //i przekątnych
  if (wiersz == kolumna)przekatne[0]++;
  if (wiersz + kolumna == 3) przekatne[1]++;

  //sprawdzenie pełnych rzędów
  //jeśli jakiś jest pełny zapisanie do zbioru testowego
  //i wywołanie funkcji wygrana
  char test[4];
  char wygr=0;
  int x;
  if (wiersze[wiersz] == 4) {
    for(x=0;x<4;x++)
      test[x]=pola[wiersz][x];
    if(wygrana(test))wygr=1;
  }
  if (kolumny[kolumna] == 4) {
    for (x = 0; x < 4; x++)
      test[x]= pola[x][kolumna];
    if(wygrana(test))wygr=1;
  }
  if (przekatne[0] == 4) {
    for (x = 0; x < 4; x++)
      test[x]= pola[x][x];
    if(wygrana(test))wygr=1;
  }
  if (przekatne[1] == 4) {
    for (x = 0; x < 4; x++)
      test[x]= pola[x][3 - x];
    if(wygrana(test))wygr=1;
  }
  return wygr;
}


void * dzialanie(void * clientSocket) {
  //w parametrze przyjmuje tablicę z socketami
  //zmienne do obsługi gry
  int * player = (int * ) clientSocket;
  int turn = 0;
  char pionek;
  char pion;
  char pole;
  char pola[4][4];
  bzero(pola, 16);
  //i dane o stanie planszy
  char wiersze[4];
  bzero(wiersze, 4);
  char kolumny[4];
  bzero(kolumny, 4);
  char przekatne[2];
  bzero(przekatne, 2);
  char wiersz;
  char kolumna;

  int x;

  while (1) {
    //tura gry
    //przyjęcie pionka od jednego gracza i pola od drugiego	
    if (x = read(player[turn], &pionek, 1) < 1) {
      error(turn, player);
    }
    if (pionek < 16) {
      error(turn, player);
    }
    pion = pionek - 16;
    printf("gracz: %d pionek: %d\n", turn + 1, pion);

    if (write(player[1 - turn], &pionek, 1) < 1) {
      error(1 - turn, player);
    }

    if (x = read(player[1 - turn], &pole, 1) < 1) {
      error(1 - turn, player);
    }
    if (pole < 0) {
      error(1 - turn, player);
    }

    printf("gracz: %d pole: %d\n", 1 - turn + 1, pole);

    if (write(player[turn], &pole, 1) < 1) {
      error(turn, player);
    }

    //zapisanie pionka na planszy		
    wiersz = pole / 4;
    kolumna = pole % 4;
    pola[wiersz][kolumna] = pion;

    //sprawdzenie czy ktoś wygrał
    char wygr=sprawdz_wygrana(wiersze,wiersz,kolumny,kolumna,przekatne,pola);
    
    //jesli tak
    if (wygr) {
      printf("Wygrał gracz %d\n", 1 - turn + 1);
      //wysłanie do graczy komunikatów o wygranej i przegranej
      if (write(player[turn], &LOSE, 1) < 0)
        error(turn, player);
      if (write(player[1 - turn], &WIN, 1) < 0)
        error(1 - turn, player);
      //zakończenie wątku
      ex(turn, player);
    }
    turn = 1 - turn;
  }
  char r = 1;
  printf("koniec\n");
  ex(r, player);
}

int main(int argc, char * argv[]) {
  int nSocket;
  int nBind, nListen;
  int nFoo = 1;
  socklen_t nTmp;
  struct sockaddr_in stAddr, stClientAddr;
  pthread_t handle1;

  /* address structure */
  memset( &stAddr, 0, sizeof(struct sockaddr));
  stAddr.sin_family = AF_INET;
  stAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  stAddr.sin_port = htons(SERVER_PORT);

  /* create a socket */
  nSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (nSocket < 0) {
    fprintf(stderr, "%s: Can't create a socket.\n", argv[0]);
    exit(1);
  }
  setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, (char * ) &nFoo, sizeof(nFoo));

  /* bind a name to a socket */
  nBind = bind(nSocket, (struct sockaddr * ) &stAddr, sizeof(struct sockaddr));
  if (nBind < 0) {
    fprintf(stderr, "%s: Can't bind a name to a socket.\n", argv[0]);
    exit(1);
  }
  /* specify queue size */
  nListen = listen(nSocket, QUEUE_SIZE);
  if (nListen < 0) {
    fprintf(stderr, "%s: Can't set queue size.\n", argv[0]);
  }

  while (1) {
    /* block for connection request */
    nTmp = sizeof(struct sockaddr);
    int * nClientSocket = malloc(2 * sizeof(int));
    nClientSocket[0] = accept(nSocket, (struct sockaddr * ) &stClientAddr, &nTmp);
    if (nClientSocket[0] < 0) {
      fprintf(stderr, "%s: Can't create a connection's socket.\n", argv[0]);
      exit(1);
    }

    printf("%s: [connection from %s]\nsocket: %p\n", argv[0],
      inet_ntoa((struct in_addr) stClientAddr.sin_addr), &nClientSocket[0]);

    nClientSocket[1] = accept(nSocket, (struct sockaddr * ) &stClientAddr, &nTmp);
    if (nClientSocket[1] < 0) {
      fprintf(stderr, "%s: Can't create a connection's socket.\n", argv[0]);
      exit(1);
    }

    printf("%s: [connection from %s]\n, soc %p\n", argv[0],
      inet_ntoa((struct in_addr) stClientAddr.sin_addr), &nClientSocket[1]);

    if (write(nClientSocket[0], &START, 1) < 0)
      perror("error");
    pthread_create( &handle1, NULL, dzialanie, nClientSocket);

  }

  close(nSocket);
  return (0);
}
