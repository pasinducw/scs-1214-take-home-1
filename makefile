CC=gcc

VERSION=v1
SRC_DIR=src

HEADERS=$(SRC_DIR)/my_lib.h
SOURCES=$(SRC_DIR)/test.c $(SRC_DIR)/$(VERSION)/my_lib.c

application: $(SOURCES)
	$(CC) -g -o $@.out $(SOURCES)

.PHONY: clean

clean:
	rm -f ./application.out