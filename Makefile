CC=gcc
CFLAGS=-Wall -I$(INCLUDE) -lm -lSDL2

INCLUDE=include/

SRC := $(shell find . -name *.c)

OUT=framework-test

PACKAGEDIR=bin/
PACKAGENAME=engine.zip

all:
	clear
	$(CC) -o $(OUT) $(SRC) $(CFLAGS)

package: all
	cp $(OUT) $(PACKAGEDIR)
	zip -r $(PACKAGENAME) $(PACKAGEDIR)
