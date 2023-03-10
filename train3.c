#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "request.h"

#define MAXCAR 80
#define LOCALIP "10.98.14.73"
#define REMOTEIP "10.31.125.14"
#define REMOTEPORT 502

int main() {
  int sd1; // descripteur de socket de dialogue
  struct sockaddr_in addrServ, addrCli;

  int nbcar;
  int adrlg = sizeof(struct sockaddr_in);

  struct XwayAddr addrLocal;
  struct XwayAddr addrAPI;
 
  // Etape 1 - Creation de la socket
  CHECK(sd1 = socket(AF_INET, SOCK_STREAM, 0), "probleme creation socket\n");


  // Etape2 - Adressage du destinataire
  addrServ.sin_family = AF_INET;

  addrServ.sin_port = htons(REMOTEPORT);
  addrServ.sin_addr.s_addr = inet_addr(REMOTEIP);


  // Etape 3 - demande d'ouverture de connexion
  CHECK(connect(sd1, (const struct sockaddr *)&addrServ,
                     sizeof(struct sockaddr_in)), "Probleme connection\n");
  printf("OK connect\n");

  //etape 4 - envoie du message pour faire tourner le train
  addrLocal.network = addrAPI.network = 0x10;
  addrLocal.addr = 0x0A;
  addrAPI.addr = 0x14;
  
  char train3[] = {0x07, 0x00};
  char vha[] = {0x0A, 0x00, 0xFF, 0xFF, 0x32, 0x00};
  
  char vht[] = {0x0A, 0x00, 0x2c, 0x00, 0xFF, 0xFF};
  
  char noRes[3] = {0, 0, 0};
  char motRes[] = {0x01, 0x00};
  
  
	while(1) {
	
  // train 3 
  
  AIGUILLAGE(0, train3);
  
  TRONCON(0, train3, 0);
  
  TRONCON(13, train3, 16);

  AIGUILLAGE(52, train3);
  
  TRONCON(20, train3, 52);
  
  //R1 + R5
  noRes[0] = 1;
  noRes[1] = 5;
  DEMANDE_RESSOURCE(2)


  AIGUILLAGE(34, train3);

  TRONCON(30, train3, 53);

  //-R1, R2
  noRes[0] = 1;
  LIBERE_RESSOURCE(1)
  
  noRes[0] = 2;
  DEMANDE_RESSOURCE(1)

  AIGUILLAGE(33, train3);
  
  TRONCON(9, train3, 65);
  
  //-R5 -R2
  noRes[0] = 2;
  noRes[1] = 5;
  LIBERE_RESSOURCE(2)


  TRONCON(31, train3, 66);

  AIGUILLAGE(51, train3);
  
  TRONCON(26, train3, 14);
  
  AIGUILLAGE(1, train3);
  
  TRONCON(15, train3, 3);

  TRONCON(12, train3, 2);
  }
}
