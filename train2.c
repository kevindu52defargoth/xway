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
  addrLocal.addr = 0x0B;
  addrAPI.addr = 0x14;
  addrGeste.addr = 0x42;
  
  char train2[] = {0x04, 0x00};
  char vha[] = {0x0A, 0x00, 0xFF, 0xFF, 0x32, 0x00};
  
  char vht[] = {0x0A, 0x00, 0x2c, 0x00, 0xFF, 0xFF};
  
  char res[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  char noRes[3] = {0, 0, 0};
  char motRes[] = {0x01, 0x00};
  
	while(1) {

  TRONCON(4, train2, 27);
  
  //R4+R6
  noRes[0] = 4;
  noRes[1] = 6;
  DEMANDE_RESSOURCE(2)

  AIGUILLAGE(7, train2);
  
  TRONCON(22, train2, 48);
  
  //-R4
  noRes[0] = 4;
  LIBERE_RESSOURCE(1)
  
  TRONCON(27, train2, 51);
  
  //-R6, R5
  noRes[0] = 6;
  LIBERE_RESSOURCE(1)
  noRes[0] = 5;
  DEMANDE_RESSOURCE(1)
  
  AIGUILLAGE(13, train2);
  
  TRONCON(28, train2, 54);
  
  AIGUILLAGE(12, train2);
  
  TRONCON(9, train2, 50);
  
  //-R5
  noRes[0] = 5;
  LIBERE_RESSOURCE(1)
  
  AIGUILLAGE(50, train2);
  
  TRONCON(24, train2, 26);  
  }
}
