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

void affiche_trame(char * buff);

void send_trame(int socket, char * msg, int lenMsg, struct XwayAddr src, struct XwayAddr dest, char * response, int * lenResponse);

void write_words(int socket, char * msg, char * addresse_mot, int nbreMot, struct XwayAddr src, struct XwayAddr dest, char * response, int * lenResponse);

void wait_ack(int socket, struct XwayAddr src, struct XwayAddr dest, int capteur);

void send_response(int socket, char * msg, int lenMsg, struct XwayAddr src, struct XwayAddr dest, char id);

#define IMPORT_REQUEST

#endif
