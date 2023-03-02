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
  
  char train4[] = {0x0a, 0x00};
  char vha[] = {0x0A, 0x00, 0xFF, 0xFF, 0x32, 0x00};
  
  char vht[] = {0x0A, 0x00, 0x2c, 0x00, 0xFF, 0xFF};
  
	while(1) {
		
  //R4 + R3
		
  AIGUILLAGE(10, train4);

  TRONCON(7, train4, 61);
  
  //-R4 + R2 + R5

  AIGUILLAGE(43, train4);
  
  TRONCON(29, train4, 54);
  
  //-R2
  
  TRONCON(49, train4, -1);
  
  AIGUILLAGE(13, train4);
      
  TRONCON(9, train4, 55);
  
  //-R5
  
  TRONCON(28, train4, 51);
  
  //R4
  
  AIGUILLAGE(53, train4);
  
  TRONCON(27, train4, 32	);
  
  //-R4
  
  TRONCON(7, train4, 40);  
  
  TRONCON(47, train4, -1); 
  }
}
