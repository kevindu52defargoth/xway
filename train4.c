#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "request.h"

#define MAXCAR 80
#define LOCALIP "10.31.125.110"
#define REMOTEIP "10.31.125.14"
#define GESTEIP "10.31.125.110"
#define REMOTEPORT 502

int main() {
  int t = 0;
  int sd1, sd2; // descripteur de socket de dialogue
  struct sockaddr_in addrServ, addrCli;

  int nbcar;
  int adrlg = sizeof(struct sockaddr_in);

  struct XwayAddr addrLocal;
  struct XwayAddr addrAPI;
  struct XwayAddr addrGeste;

  // Etape 1 - Creation des sockets
  CHECK(sd1 = socket(AF_INET, SOCK_STREAM, 0), "probleme creation socket\n");
  CHECK(sd2 = socket(AF_INET, SOCK_STREAM, 0), "probleme creation socket\n");

  // Etape2 - Adressage du destinataire sd1
  addrServ.sin_family = AF_INET;

  addrServ.sin_port = htons(REMOTEPORT);
  addrServ.sin_addr.s_addr = inet_addr(REMOTEIP);


  // Etape 3 - demande d'ouverture de connexion sd1
  CHECK(connect(sd1, (const struct sockaddr *)&addrServ,
                sizeof(struct sockaddr_in)), "Probleme connection\n");

  printf("OK connect sd1\n");
  // Etape2bis - Adressage du destinataire sd2
  addrServ.sin_port = htons(7867);
  addrServ.sin_addr.s_addr = inet_addr(GESTEIP);
  printf("ok addressage\n");


  // Etape 3bis - demande d'ouverture de connexion sd2
  CHECK(connect(sd2, (const struct sockaddr *)&addrServ,
                sizeof(struct sockaddr_in)), "Probleme connection\n");
  printf("OK connect sd2\n");

  //etape 4 - envoie du message pour faire tourner le train
  addrLocal.network = addrAPI.network = addrGeste.network = 0x10;
  addrLocal.addr = 0x0A;
  addrAPI.addr = 0x14;
  addrGeste.addr = 0x42;
  
  char train4[] = {0x0a, 0x00};
  char vha[] = {0x0A, 0x00, 0xFF, 0xFF, 0x32, 0x00};
  
  char vht[] = {0x0A, 0x00, 0x2c, 0x00, 0xFF, 0xFF};
  
  char noRes[3] = {0, 0, 0};
  char motRes[] = {0x01, 0x00};
  
	while(1) {
		
  //R4 + R3
  noRes[0] = 3;
  noRes[1] = 4;
  DEMANDE_RESSOURCE(2)
		
  AIGUILLAGE(10, train4);

  TRONCON(7, train4, 61);
  
  //-R4 + R2 + R5 + R6
  noRes[0] = 4;
  LIBERE_RESSOURCE(1)
  
  noRes[0] = 2;
  noRes[1] = 5;
  noRes[2] = 6;
  DEMANDE_RESSOURCE(3)

  AIGUILLAGE(43, train4);
  
  TRONCON(29, train4, 54);
  
  //-R2-R3
  noRes[0] = 2;
  noRes[1] = 3;
  LIBERE_RESSOURCE(2)
  
  TRONCON(49, train4, -1);
  
  AIGUILLAGE(13, train4);
      
  TRONCON(9, train4, 55);
  
  //-R5
  noRes[0] = 5;
  LIBERE_RESSOURCE(1)
  
  TRONCON(28, train4, 51);
  
  //R4
  noRes[0] = 4;
  DEMANDE_RESSOURCE(1)
  
  AIGUILLAGE(53, train4);
  
  TRONCON(27, train4, 32);
  
  //-R4-R6
  noRes[0] = 4;
  noRes[1] = 6;
  LIBERE_RESSOURCE(2)
  
  TRONCON(7, train4, 40);  
  
  TRONCON(47, train4, -1); 
  }
}
