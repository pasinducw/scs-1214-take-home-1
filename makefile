CC=gcc

VERSION=v2
SRC_DIR=src

HEADERS=$(SRC_DIR)/$(VERSION)/my_lib.h
SOURCES=$(SRC_DIR)/main.c $(SRC_DIR)/$(VERSION)/my_lib.c

application: $(SOURCES)
	$(CC) -g -o $@ $(SOURCES)

.PHONY: clean

clean:
	rm -f ./application