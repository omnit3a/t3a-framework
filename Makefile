CC=gcc
CFLAGS=-Wall -I$(INCLUDE) -lm -lSDL2 -lSDL2_ttf

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
	cp -r $(ASSETDIR) $(PACKAGEDIR)
	cp LICENSE $(PACKAGEDIR)
	cp *.txt $(PACKAGEDIR)
	zip -r $(PACKAGENAME) $(PACKAGEDIR)
