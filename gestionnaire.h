#include "request.h"
#include <pthread.h>

enum retCode {REQUEST_TYPE = 1, WORD_DATA = 2};

struct param_thread{
  char * datas;
  struct XwayAddr remote;
};

int check_trame(char* buff_rx);

int traitement(char * datas, struct XwayAddr remoteAddr);

void * thread_traitement(struct param_thread * param);
