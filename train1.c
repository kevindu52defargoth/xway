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


#define TRONCON(no, mot, capt) vht[2] = (char) no; write_words(sd1, vht, mot, 3, addrLocal, addrAPI, NULL, NULL); wait_ack(sd1, addrLocal, addrAPI, capt);
#define AIGUILLAGE(no, mot) vha[4] = (char) no; write_words(sd1, vha, mot, 3, addrLocal, addrAPI, NULL, NULL); wait_ack(sd1, addrLocal, addrAPI, -1);
#define DEMANDE_RESSOURCE(no) res[0] = no; res[1] = -1; write_words(sd1, res, motRes, 1, addrLocal, addrAPI, NULL, NULL); wait_ressource(sd1, addrLocal, addrGeste);
#define LIBERE_RESSOURCE(no) res[0] = no; res[1] = 0; write_words(sd1, res, motRes, 1, addrLocal, addrGeste, NULL, NULL);

int main() {
  int sd1; // descripteur de socket de dialogue
  struct sockaddr_in addrServ, addrCli;

  int nbcar;
  int adrlg = sizeof(struct sockaddr_in);

  struct XwayAddr addrLocal;
  struct XwayAddr addrAPI;
  struct XwayAddr addrGeste;
 
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
  addrLocal.network = addrAPI.network = addrGeste.network = 0x10;
  addrLocal.addr = 0x0A;
  addrAPI.addr = 0x14;
  addrGeste.addr = 0x42;

  
  char train1[] = {0x01, 0x00};
  char vha[] = {0x0A, 0x00, 0xFF, 0xFF, 0x32, 0x00};
  
  char vht[] = {0x0A, 0x00, 0x2c, 0x00, 0xFF, 0xFF};

  char res[] = {0x00, 0x00};
  char motRes[] = {0x01, 0x00};
  
	while(1) {
  AIGUILLAGE(31, train1);
  
  TRONCON(3, train1, 24);

  //R1
  DEMANDE_RESSOURCE(1)

  AIGUILLAGE(52, train1);
  
  TRONCON(23, train1, 58);
  
  //-R1, R2+R3
  LIBERE_RESSOURCE(1)
  
  AIGUILLAGE(33, train1);
  
  TRONCON(10, train1, 60);
  
  //-R2, R4
  
  AIGUILLAGE(63, train1);
  
  TRONCON(29, train1, 34);
  
  //-R4
    
  TRONCON(19, train1, 23);	
  }
}
