struct XwayAddr {
    char network;
    char addr;
};

void affiche_trame(char * buff);

void send_trame(int socket, char * msg, int lenMsg, struct XwayAddr src, struct XwayAddr dest, char * response, int * lenResponse);

void write_words(int socket, char * msg, int nbreMot, struct XwayAddr src, struct XwayAddr dest, char * response, int * lenResponse);
