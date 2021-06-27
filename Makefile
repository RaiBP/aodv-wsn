CONTIKI_PROJECT = main
all: $(CONTIKI_PROJECT)

CONTIKI = $(HOME)/contiki

PROJECT_SOURCEFILES += print_func.c
PROJECT_SOURCEFILES += struct.c
PROJECT_SOURCEFILES += print_func.c
PROJECT_SOURCEFILES += send_func.c

CONTIKI_WITH_RIME = 1

include $(CONTIKI)/Makefile.include
