#define _GNU_SOURCE
#include <pthread.h>
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

#define LOCALIP "0.0.0.0"
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

#define NBRE_RESSOURCES 7
pthread_mutex_t modifier_etat;
int ressources[NBRE_RESSOURCES] = {0};

char header_mot[] = {0x37, 0x06, 0x68, 0x07};
struct XwayAddr localXway;

void* client_thread(void* arg) {
  int diag = *(int*)arg;
  struct XwayAddr remoteXway;
  char buff_rx[MAXCAR + 1];
  int nbCar, res;
  char OK = 0xFE;
  char KO = 0xFD;
  char * datas;

  printf("debut dialogue with %d\n", diag);

  while (1) {
    nbCar = recvfrom(diag, buff_rx, MAXCAR + 1, 0, NULL, NULL);
    if (nbCar == -1 || nbCar == 0)
      continue;
#ifdef _DEBUG_
    PRINT("recue : ");
    affiche_trame(buff_rx);
#endif
#ifdef _DEBUG_TRAMES
    printf("nbcar = %d\n", nbCar);
#endif
    res = check_trame(buff_rx);
    switch (res){
      case REQUEST_TYPE:
        PRINT("requete refusée car mauvais type de requete\n");
        send_response(diag, &KO, 2, localXway, remoteXway, buff_rx[13]);
        break;
      case WORD_DATA:
        PRINT("données malformées\n");
        send_response(diag, &KO, 2, localXway, remoteXway, buff_rx[13]);
        break;
      case 0:
        remoteXway.network = buff_rx[9];
        remoteXway.addr = buff_rx[8];
        send_response(diag, &OK, 1, localXway, remoteXway, buff_rx[13]);
        datas = malloc(100 * sizeof(char));
        memcpy(datas, buff_rx + 20, buff_rx[20]*2 + 2);
#ifdef _DEBUG_TRAMES
        printf("buffrx : \n");
        printf("%d\t", buff_rx[20]);
        printf("%d\t", buff_rx[21]);
        printf("%d\t", buff_rx[22]);
        printf("%d\t\n", buff_rx[23]);
        printf("données données moi : \n");
        printf("%d\t", datas[0]);
        printf("%d\t", datas[1]);
        printf("%d\t", datas[2]);
        printf("%d\t\n", datas[3]);
#endif
        traitement(diag, datas, remoteXway);
    }
#ifdef _DEBUG_
    PRINT("recue : ");
    affiche_trame(buff_rx);
#endif
#ifdef _DEBUG_TRAMES
    printf("nbcar = %d\n", nbCar);
#endif
  }
  close(diag);
  pthread_exit(NULL);
}

int main(){
  struct sockaddr_in addrSrv, peer_addr;
  socklen_t peer_addr_size;
  char buff_rx[MAXCAR + 1];
  int sd1;
  int res;
  int nbCar;

  for (int i = 0; i <NBRE_RESSOURCES; i++)
    printf("%d ", ressources[i]);
  printf("\n");

  localXway.network = 0x10;
  localXway.addr = 0x42;

  struct XwayAddr remoteXway;

  localXway.network = 0x10;
  localXway.addr = 0x14;

  //libération de toute les mutex
  pthread_mutex_unlock(&screen);
  pthread_mutex_unlock(&modifier_etat);


  // Creation de la socket
  CHECK(sd1 = socket(AF_INET, SOCK_STREAM, 0), "creation sd1");

  // Adressage de la socket
  addrSrv.sin_family = AF_INET;
  addrSrv.sin_addr.s_addr = INADDR_ANY;

  //definition port
  addrSrv.sin_port = htons(LOCAL_PORT);

  //affectation de l'addresse a la socket
  CHECK(bind(sd1, (struct sockaddr *)&addrSrv, sizeof(addrSrv)),
        "erreur bind");

  CHECK(listen(sd1, 50), "listen");
  printf("OK listen\n");

  printf("debut\n");
  peer_addr_size = sizeof(peer_addr);
  while(1){
    int diag;
    CHECK(diag =accept(sd1, (struct sockaddr *) &peer_addr, &peer_addr_size), "accept");
     pthread_t tid;
     CHECK_T(pthread_create(&tid, NULL, client_thread, &diag), "problème création thread");
     pthread_detach(tid);
  }
  // communication

}

int check_trame(char* buff_rx){
  for (int i = 0; i < 4; i++){
    if (buff_rx[i + 14] != header_mot[i]){
      return REQUEST_TYPE;
    }
  }

  int nbreMots = buff_rx[20];
  int sizeData = buff_rx[5] - 16;
#ifdef _DEBUG_TRAMES
  printf("nbreMots = %d, sizeData = %d\n", nbreMots, sizeData);
#endif

  if (nbreMots*2 != sizeData){
    return WORD_DATA;
  }

  return 0;
}

int traitement(int sock, char * datas, struct XwayAddr remoteAddr){
  struct param_thread * param = (struct param_thread *) malloc(sizeof(struct param_thread));
  pthread_t tid;
  pthread_attr_t attr;

  //attributs des threads
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  //paramètres
  param->datas = malloc(100*sizeof(char));
  memcpy(param->datas, datas, datas[0]*2 + 2);
  param->remote = remoteAddr;
  param->sock = sock;

  CHECK_T(pthread_create(&tid, &attr, (void*(*)(void*)) thread_traitement, param), "pb creation thread\n");
  //sleep(1);
  return 0;
}

void * thread_traitement(struct param_thread * param){
  int nbreMot = param->datas[0];
  if (param->datas[3] == -1){
    // on bloque des ressources

    // on vérifie que toutes les mutex dont on à besoin sont libres
    printf("take by %d\n", param->sock);
    int res = 0;
    while (res == 0){
      pthread_mutex_lock(&modifier_etat);
      res = 1;
      for (int i= 0;  i<nbreMot; i++){
        if (ressources[((int) param->datas[2*i + 2])-1] != 0){
          res = 0;
          pthread_mutex_unlock(&modifier_etat);
          break;
        }
      }
    }

    // on prend les ressources
    printf("taken by %d\n", param->sock);
    for (int i= 0;  i<nbreMot; i++){
      ressources[((int) param->datas[2*i + 2])-1] = 1;
    }

    pthread_mutex_unlock(&modifier_etat);

  } else {
    // on libère des ressources sans vérifier quelles sont prises YOLO
    pthread_mutex_lock(&modifier_etat);
    printf("free by %d\n", param->sock);
    for (int i= 0;  i<nbreMot; i++){
      ressources[((int) param->datas[2*i + 2])-1] = 0;
    }
    pthread_mutex_unlock(&modifier_etat);
  }

  free(param->datas);
  char K = 0x4b;
  pthread_mutex_lock(&modifier_etat);
  printf("état des ressources : \n");
  for (int u = 0; u <NBRE_RESSOURCES; u++)
    printf("%d ", ressources[u]);
  printf("\n");
  pthread_mutex_unlock(&modifier_etat);

  send_trame(param->sock, &K, 1, localXway, param->remote, NULL, 0);
  return NULL;
};
