CC = gcc

CPPFLAGS =
ifeq ($(DEBUG),on)
CPPFLAGS += -D_DEBUG
endif
ifeq ($(TEST_BASIQUE), on)
CPPFLAGS += -D_TEST_BASIQUE
endif
ifneq ($(NBMAX),)
CPPFLAGS += -DNBMAX=$(NBMAX)
endif

CFLAGS  = -Wall
CFLAGS += -Werror
CFLAGS += -std=c99
CFLAGS += -pedantic

