CC=gcc

VERSION=v2
SRC_DIR=src

HEADERS=$(SRC_DIR)/$(VERSION)/my_lib.h
SOURCES=$(SRC_DIR)/main.c $(SRC_DIR)/$(VERSION)/my_lib.c

application: $(SOURCES)
	$(CC) -o $@.out $(SOURCES)

.PHONY: clean

clean:
	rm -f ./application.out