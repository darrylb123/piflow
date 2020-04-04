LD_FLAGS = -L./libpifacedigital -lpifacedigital -L./libmcp23s17 -lmcp23s17
INCLUDE = -Ilibpifacedigital/src 
CFLAGS += -O2 -Wall
CC= gcc
all: piflow

SUBDIRS = libpifacedigital libmcp23s17

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

piflow: piflow.c libpifacedigital libmcp23s17
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE) $(LD_FLAGS) 


clean:
	rm -f piflow

