#include <pthread.h>
#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <semaphore.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "request.h"
#include "gestionnaire.h"

#define LOCALIP "10.98.14.73"
#define LOCAL_PORT 7867

#define MAXCAR 80

// macros de check
#define CHECK_T(status, msg)                        \
if (0 != (status))   {                            \
  fprintf(stderr, "pthread erreur : %s\n", msg);  \
  exit (EXIT_FAILURE);                            \
}

// macros pour l'affichage à l'écran
pthread_mutex_t screen;
#define PRINT(msg) { \
pthread_mutex_lock(&screen);   \
printf("\n%s\n", msg);   \
pthread_mutex_unlock(&screen);   \
}

#define NBRE_RESSOURCES 4
pthread_mutex_t modifier_etat;
int ressources[NBRE_RESSOURCES];

char header_mot[] = {0x37, 0x06, 0x68, 0x07};
int sd1; // descripteur de socket de dialogue
struct XwayAddr localXway;

int main(){
  struct sockaddr_in addrlect, addrecr;
  char buff_rx[MAXCAR + 1];
  int adrlg; // longueur de l'addresse
  int res;

  localXway.network = 0x10;
  localXway.addr = 0x42;

  struct XwayAddr remoteXway;

  localXway.network = 0x10;
  localXway.addr = 0x14;

  //libération de toute les mutex
  pthread_mutex_unlock(&screen);
  pthread_mutex_unlock(&modifier_etat);


  // Creation de la socket
  CHECK(sd1 = socket(AF_INET, SOCK_DGRAM, 0), "creation sd1");

  // Adressage de la socket
  addrlect.sin_family = AF_INET;
  addrlect.sin_addr.s_addr = INADDR_ANY;

  //definition port
  addrlect.sin_port = htons(LOCAL_PORT);

  //affectation de l'addresse a la socket
  CHECK(bind(sd1, (struct sockaddr *)&addrlect, sizeof(addrlect)),
        "erreur bind");

  printf("debut\n");
  // communication
  while(1){
    recvfrom(sd1, buff_rx, MAXCAR + 1, 0, NULL, NULL);
#ifdef _DEBUG_
    PRINT("\recue : ");
    affiche_trame(buff_rx);
#endif

    res = check_trame(buff_rx);
    switch (res){
      case REQUEST_TYPE:
        PRINT("requete refusée car mauvais type de requete\n");
        break;
      case WORD_DATA:
        PRINT("données malformées\n");
        break;
    }

    remoteXway.network = buff_rx[9];
    remoteXway.addr = buff_rx[8];

    if (res == 0){
      traitement(buff_rx + 19, remoteXway);
      send_response(sd1, "FE", 1, localXway, remoteXway, buff_rx[13]);
    } else {
      send_response(sd1, "FE", 1, localXway, remoteXway, buff_rx[13]);
    }
  }
}

int check_trame(char* buff_rx){
  for (int i = 0; i < 4; i++){
    if (buff_rx[i + 13] != header_mot[i]){
      return REQUEST_TYPE;
    }
  }

  int nbreMots = buff_rx[20];
  int sizeData = buff_rx[5] - 12;

  if (nbreMots*2 != sizeData){
    return WORD_DATA;
  }

  return 0;
}

int traitement(char * datas, struct XwayAddr remoteAddr){
  struct param_thread param;
  pthread_t tid;
  pthread_attr_t attr;

  //attributs des threads
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  //paramètres
  param.datas = datas;
  param.remote = remoteAddr;

  CHECK_T(pthread_create(&tid, &attr, (void*(*)(void*)) thread_traitement, &param), "pb creation thread\n");
  return 0;
}

void * thread_traitement(struct param_thread * param){
  char datas[MAXCAR];
  int nbreMot = param->datas[0];
  memcpy(datas, param->datas, nbreMot);

  if (datas[2] == 0xf){
    // on bloque des ressources

    // on vérifie que toutes les mutex dont on à besoin sont libres
    int res = 0;
    while (res == 0){
      pthread_mutex_lock(&modifier_etat);
      res = 1;
      for (int i= 0;  i<nbreMot; i++){
        if (ressources[datas[2*i + 1]] != 0){
          res = 0;
          pthread_mutex_unlock(&modifier_etat);
          break;
        }
      }
    }

    // on prend les ressources
    for (int i= 0;  i<nbreMot; i++){
      ressources[datas[2*i + 1]] = 1;
    }

    pthread_mutex_unlock(&modifier_etat);

  } else {
    // on libère des ressources sans vérifier quelles sont prises YOLO
    pthread_mutex_lock(&modifier_etat);
    for (int i= 0;  i<nbreMot; i++){
      ressources[datas[2*i + 1]] = 0;
    }
    pthread_mutex_lock(&modifier_etat);
  }

  send_trame(sd1, "OK", 2, localXway, param->remote, NULL, 0);

  return NULL;
};