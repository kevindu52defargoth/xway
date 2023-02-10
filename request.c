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

void send_trame(int socket, char * msg, int lenMsg, struct XwayAddr src, struct XwayAddr dest){
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
  printf("\nenvoie : ");
  affiche_trame(buff_tx);
  nbcar = send(socket, buff_tx, 16, 0);
  printf("envoye : %d\n", nbcar);
  nbcar = recvfrom(socket, buff_rx, 15, 0, NULL, NULL);
  printf("recu : ");
  affiche_trame(buff_rx);
};