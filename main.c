#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXCAR 80
#define LOCALIP "10.98.14.73"
#define REMOTEIP "10.31.125.14"
#define REMOTEPORT 502

#define CHECK(sts, msg)                                                        \
  if ((sts) == -1) {                                                           \
    perror(msg);                                                               \
    exit(-1);                                                                  \
  }

int main(int argc, char *argv[]) {
  int sd1; // descripteur de socket de dialogue
  struct sockaddr_in addrServ, addrCli;
  char buff_tx[MAXCAR + 1];
  char buff_rx[MAXCAR + 1];
  int nbcar;
  int adrlg = sizeof(struct sockaddr_in);

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


  // Etape 4 - Ecriture de la trame
  //header Modbus/TCP/IP
  char header[] = {0x00, 0x00,  0x00, 0x01, 0x00, 0x0A, 0x00};
  memcpy(buff_tx,  header, 7);

  buff_tx[7] = 0xF1;  // type de donnee

  // on ajoute les addresses
  char src_addr[] = {0x20, 0x10};  // notre addresse est 1.32
  char dest_addr[] = {0x14, 0x10};
  memcpy(buff_tx + 8, src_addr, 2);
  memcpy(buff_tx + 10, dest_addr, 2);

  buff_tx[12] = 0x09;   // On transmet
  buff_tx[13] = 0x42;   // ID aleatoire

  buff_tx[14] = 0x24;  //message pour allumer l'API
  buff_tx[15] = 0x06;

  // Etape 5 - Envoie
  nbcar = send(sd1, buff_tx, 16, 0);
  printf("envoye : %d\n", nbcar);
  nbcar = recvfrom(sd1, buff_rx, 15, 0, NULL, NULL);

  printf("\nrecu : ");
  for (int i =0; i < 6 + buff_rx[5]; i++)
    printf("%c\t", buff_rx[i]);

  // Etape6 - on ferme la socket ce qui va liberer le port
  close(sd1);

  return EXIT_SUCCESS;
}
