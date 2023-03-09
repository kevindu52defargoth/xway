CC = gcc

CFLAGS  = -Wall
CFLAGS += -std=c99

ifeq ($(DEBUG),on)
CFLAGS += -D_DEBUG
endif

all: train1 train2 train3 train4 gestionnaire request.o

gestionnaire : request.o gestionnaire.o
	$(CC) gestionnaire.c request.o $(CFLAGS) -lpthread -o gestionnaire

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

train%: train%.o request.o
	$(CC) $(CFLAGS) $< request.o -o $@
