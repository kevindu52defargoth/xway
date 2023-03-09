#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXCAR 80

struct XwayAddr {
    char network;
    char addr;
};

void affiche_trame(char * buff){
    for (int i =0; i < 6 + buff[5]; i++)
        printf("%x\t", buff[i]);
    printf("\n");
}

void send_trame(int socket, char * msg, int lenMsg, struct XwayAddr src, struct XwayAddr dest, char * response, int * lenResponse){
  char buff_tx[MAXCAR + 1];
  char buff_rx[MAXCAR + 1];
  int nbcar;

  // Ecriture de la trame
  //header Modbus/TCP/IP
  char header[] = {0x00, 0x00,  0x00, 0x01, 0x00, 0x0A, 0x00};
  int length = lenMsg + 8;
  header[5] = length;
  memcpy(buff_tx,  header, 7);

  buff_tx[7] = 0xF1;  // type de donnee

  // on ajoute les addresses
  buff_tx[8] = src.addr;
  buff_tx[9] = src.network;
  buff_tx[10] = dest.addr;
  buff_tx[11] = dest.network;

  buff_tx[12] = 0x09;   // On transmet
  buff_tx[13] = 0x42;   // ID aleatoire

  memcpy(buff_tx + 14, msg, lenMsg);

  //Envoie
#ifdef _DEBUG_
  printf("\nenvoie : ");
  affiche_trame(buff_tx);
#endif

  nbcar = send(socket, buff_tx, length+6, 0);

#ifdef _DEBUG_
  printf("envoye : %d\n", nbcar);
#endif

  nbcar = recvfrom(socket, buff_rx, 15, 0, NULL, NULL);

#ifdef _DEBUG_
  printf("recu : ");
  affiche_trame(buff_rx);
#endif

  if (response != NULL && lenResponse != NULL) {
    memcpy(response, buff_rx + 12, buff_rx[5] - 6);
    *lenResponse = buff_rx[5] - 6;
  }
};

void write_words(int socket, char * msg, char * addresse_mot, int nbreMot, struct XwayAddr src, struct XwayAddr dest, char * response, int * lenResponse){
    int length;

    char buff_tx[MAXCAR + 1] = {0};
    char header_write[] = {0x37, 0x06, 0x68, 0x07};
    memcpy(buff_tx,  header_write, 4);

    memcpy(buff_tx + 4, addresse_mot, 2);

    buff_tx[6] = nbreMot;
    buff_tx[7] = 0;

    memcpy(buff_tx + 8, msg, nbreMot*2);


    length = nbreMot*2 + 8;
    send_trame(socket, buff_tx, length, src, dest, response, lenResponse);
}

void wait_ack(int socket, struct XwayAddr src, struct XwayAddr dest, int capteur) {
	int nbcar;
	char buff_rx[MAXCAR + 1];
	
	
	int att = -1;
	do {
		printf("On attend %d\n", capteur);
		nbcar = recvfrom(socket, buff_rx, 30, 0, NULL, NULL);
#ifdef _DEBUG_
  printf("recu : ");
  affiche_trame(buff_rx);
#endif
		if (capteur != -1) att = (int) buff_rx[22];
		
		printf("On a %d\n", att);
		
		char buff_tx[] = {0x00, 0x00,  0x00, 0x01, 0x00, 0x09, 0x00, buff_rx[7], buff_rx[10], buff_rx[11], buff_rx[8], buff_rx[9], 0x19, buff_rx[13], 0xFE};
	
#ifdef _DEBUG_
  	printf("\nenvoie : ");
  	affiche_trame(buff_tx);
#endif

		send(socket, buff_tx, buff_tx[5]+6, 0);
	} while (att != capteur);
}

void send_response(int socket, char * msg, int lenMsg, struct XwayAddr src, struct XwayAddr dest, char id){
  char buff_tx[MAXCAR + 1];
  char buff_rx[MAXCAR + 1];
  int nbcar;

  // Ecriture de la trame
  //header Modbus/TCP/IP
  char header[] = {0x00, 0x00,  0x00, 0x01, 0x00, 0x0A, 0x00};
  int length = lenMsg + 8;
  header[5] = length;
  memcpy(buff_tx,  header, 7);

  buff_tx[7] = 0xF1;  // type de donnee

  // on ajoute les addresses
  buff_tx[8] = src.addr;
  buff_tx[9] = src.network;
  buff_tx[10] = dest.addr;
  buff_tx[11] = dest.network;

  buff_tx[12] = 0x19;   // On répond
  buff_tx[13] = id;   // ID aleatoire

  memcpy(buff_tx + 14, msg, lenMsg);

  //Envoie
  #ifdef _DEBUG_
  printf("\nenvoie : ");
  affiche_trame(buff_tx);
  #endif

  nbcar = send(socket, buff_tx, length+6, 0);

  #ifdef _DEBUG_
  printf("envoye : %d\n", nbcar);
  #endif
};

void wait_ressource(int socket, struct XwayAddr src, struct XwayAddr dest) {
  int nbcar;
  char buff_rx[MAXCAR + 1];


  int att = -1;
  printf("On attend les ressources\n");
  nbcar = recvfrom(socket, buff_rx, 30, 0, NULL, NULL);
#ifdef _DEBUG_
  printf("recu : ");
  affiche_trame(buff_rx);
#endif

  printf("On a %d\n", att);

  char buff_tx[] = {0x00, 0x00,  0x00, 0x01, 0x00, 0x09, 0x00, buff_rx[7], buff_rx[10], buff_rx[11], buff_rx[8], buff_rx[9], 0x19, buff_rx[13], 0xFE};

}
