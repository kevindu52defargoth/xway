#ifndef IMPORT_REQUEST

struct XwayAddr {
    char network;
    char addr;
};

#define CHECK(sts, msg)                                                        \
  if ((sts) == -1) {                                                           \
    perror(msg);                                                               \
    exit(-1);                                                                  \
  }


  /* macros automate */

  #define TRONCON(no, mot, capt) vht[2] = (char) no; write_words(sd1, vht, mot, 3, addrLocal, addrAPI, NULL, NULL); wait_cap(sd1, addrLocal, addrAPI, capt);
  #define AIGUILLAGE(no, mot) vha[4] = (char) no; write_words(sd1, vha, mot, 3, addrLocal, addrAPI, NULL, NULL); wait_cap(sd1, addrLocal, addrAPI, -1);
void affiche_trame(char * buff);

void send_trame(int socket, char * msg, int lenMsg, struct XwayAddr src, struct XwayAddr dest, char * response, int * lenResponse);

void write_words(int socket, char * msg, char * addresse_mot, int nbreMot, struct XwayAddr src, struct XwayAddr dest, char * response, int * lenResponse);

void wait_cap(int socket, struct XwayAddr src, struct XwayAddr dest, int capteur);

void wait_acknowledge(int socket, struct XwayAddr src, struct XwayAddr dest);

void send_response(int socket, char * msg, int lenMsg, struct XwayAddr src, struct XwayAddr dest, char id);

void wait_ressource(int socket, struct XwayAddr src, struct XwayAddr dest);

#define IMPORT_REQUEST

#endif
